#include <mpi.h>
#include <signal.h>
#include <unistd.h>
#include <iostream>
#include <set>
#include <string>
#include <algorithm>

constexpr int CRITICAL_SECTION_TIME = 4;
constexpr int REMAINDER_SECTION_TIME = 9;
constexpr int TIME_INCREMENT = 1;
constexpr int REPEAT_COUNT = 3;

enum class MessageType { REQUEST = 0, REPLY = 1, RELEASE = 2, FIN = 3 };

class Timestamp {
private:
    int time_;
    int process_rank_;

public:
    Timestamp(int time, int rank) : time_(time), process_rank_(rank) {}

    int time() const { return time_; }
    int rank() const { return process_rank_; }

    bool operator<(const Timestamp& other) const {
        return time_ < other.time_ || (time_ == other.time_ && process_rank_ < other.process_rank_);
    }

    friend std::ostream& operator<<(std::ostream& os, const Timestamp& ts) {
        return os << "(" << ts.time_ << ", " << ts.process_rank_ << ")";
    }
};

class RequestQueue {
private:
    std::set<Timestamp> queue_;
    int max_size_;

public:
    RequestQueue(int max_processes) : max_size_(max_processes) {}

    bool add(const Timestamp& ts) {
        if (ts.rank() >= max_size_ || queue_.size() >= max_size_) {
            std::cerr << "Ошибка: невозможно добавить запрос в очередь" << std::endl;
            return false;
        }
        queue_.insert(ts);
        return true;
    }

    bool remove(int rank) {
        if (rank >= max_size_) {
            std::cerr << "Ошибка: неверный ранг для удаления" << std::endl;
            return false;
        }

        auto it = std::find_if(queue_.begin(), queue_.end(),
                             [rank](const Timestamp& ts) { return ts.rank() == rank; });
        
        if (it != queue_.end()) {
            queue_.erase(it);
            return true;
        }
        std::cerr << "Ошибка: запрос с рангом " << rank << " не найден" << std::endl;
        return false;
    }

    bool is_front(int rank) const {
        return !queue_.empty() && queue_.begin()->rank() == rank;
    }

    friend std::ostream& operator<<(std::ostream& os, const RequestQueue& rq) {
        for (const auto& ts : rq.queue_) os << ts << "; ";
        return os;
    }
};

volatile sig_atomic_t alarm_flag = 1;
int local_time = 0;
int finsCount = 0;
int permsCount = 0;

void alarm_handler(int) { alarm_flag = 0; }

void increment_time() { local_time += TIME_INCREMENT; }

void update_time(int received_time) {
    local_time = std::max(local_time, received_time) + TIME_INCREMENT;
}

void set_random_timer(int seconds) {
    alarm(seconds > 0 ? rand() % seconds + 1 : 1);
}

void checkState(int status) {
    if (status != MPI_SUCCESS) {        
        MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
    }
}

void sendToAll(int sender_rank, int processes, MessageType type) {
    for (int i = 0; i < processes; ++i) {
        if (i != sender_rank) {
            checkState(MPI_Send(&local_time, 1, MPI_INT, i, static_cast<int>(type), MPI_COMM_WORLD));
        }
    }

    const char* msg_type_str = "";
    switch(type) {
        case MessageType::REQUEST: 
            msg_type_str = "REQUEST";
            printf("Процесс %d: отправил всем сообщение REQUEST; время: %d\n", sender_rank, local_time);
            break;
        case MessageType::RELEASE: 
            msg_type_str = "RELEASE";
            printf("Процесс %d: отправил всем сообщение RELEASE; время: %d\n", sender_rank, local_time);
            break;
        case MessageType::FIN: 
            msg_type_str = "FIN";
            printf("Процесс %d: отправил всем сообщение FIN; время: %d\n", sender_rank, local_time);
            break;        
    }
    increment_time();
}

void handle_messages(int current_rank, RequestQueue& queue, bool count_replies) {
    MPI_Status status;
    int has_message;

    while (true) {
        checkState(MPI_Iprobe(MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &has_message, &status));
        if (!has_message) break;

        int received_time;
        checkState(MPI_Recv(&received_time, 1, MPI_INT, status.MPI_SOURCE, 
                         status.MPI_TAG, MPI_COMM_WORLD, &status));

        update_time(received_time);
        MessageType msg_type = static_cast<MessageType>(status.MPI_TAG);
        
        switch (msg_type) {
            case MessageType::REQUEST:
                printf("Процесс %d: получил REQUEST от %d; время: %d\n", 
                      current_rank, status.MPI_SOURCE, local_time);
                if (!queue.add(Timestamp(received_time, status.MPI_SOURCE))) {
                    printf("Ошибка: невозможно добавить запрос в очередь\n");
                }
                std::cout << "Процесс " << current_rank << ": очередь (после REQUEST): " << queue << std::endl;
                checkState(MPI_Send(&local_time, 1, MPI_INT, status.MPI_SOURCE, 
                             static_cast<int>(MessageType::REPLY), MPI_COMM_WORLD));
                increment_time();
                printf("Процесс %d: отправил REPLY процессу %d; время: %d\n", 
                      current_rank, status.MPI_SOURCE, local_time);
                break;

            case MessageType::REPLY:
                printf("Процесс %d: получил REPLY от %d; время: %d\n", 
                      current_rank, status.MPI_SOURCE, local_time);
                if (count_replies && local_time != received_time) permsCount++;
                break;

            case MessageType::RELEASE:
                printf("Процесс %d: получил RELEASE от %d; время: %d\n", 
                      current_rank, status.MPI_SOURCE, local_time);
                if (!queue.remove(status.MPI_SOURCE)) {
                    printf("Ошибка: не удалось удалить запрос из очереди\n");
                }
                std::cout << "Процесс " << current_rank << ": очередь (после RELEASE): " << queue << std::endl;
                break;

            case MessageType::FIN:
                printf("Процесс %d: получил FIN от %d; время: %d\n", 
                      current_rank, status.MPI_SOURCE, local_time);
                finsCount++;
                break;
        }
    }
}

void work(int rank, int seconds, RequestQueue& queue) {
    alarm_flag = 1;
    set_random_timer(seconds);

    while (alarm_flag) {
        handle_messages(rank, queue, false);
    }
}

void process_work(int rank, int processes) {
    RequestQueue queue(processes);
    signal(SIGALRM, alarm_handler);
    srand(rank); 

    for (int i = 0; i < REPEAT_COUNT; ++i) {
        printf("\nПроцесс %d: итерация %d\n", rank, i+1);
        printf("Процесс %d: вход в пролог; время: %d\n", rank, local_time);

        if (!queue.add(Timestamp(local_time, rank))) {
            printf("Ошибка: не удалось добавить собственный запрос\n");
        }
        std::cout << "Процесс " << rank << ": очередь (в прологе): " << queue << std::endl;
        sendToAll(rank, processes, MessageType::REQUEST);
        permsCount = 0;

        while (permsCount < processes - 1 || !queue.is_front(rank)) {
            handle_messages(rank, queue, true);
        }
        increment_time();

        printf("Процесс %d: вход в критическую секцию; время: %d\n", rank, local_time);
        work(rank, CRITICAL_SECTION_TIME, queue);
        increment_time();

        printf("Процесс %d: выход из критической секции; время: %d\n", rank, local_time);
        printf("Процесс %d: вход в эпилог; время: %d\n", rank, local_time);
        if (!queue.remove(rank)) {
            printf("Ошибка: не удалось удалить собственный запрос\n");
        }
        std::cout << "Процесс " << rank << ": очередь (в эпилоге): " << queue << std::endl;
        sendToAll(rank, processes, MessageType::RELEASE);

        printf("Процесс %d: вход в remainder секцию; время: %d\n", rank, local_time);
        work(rank, REMAINDER_SECTION_TIME, queue);
        increment_time();
    }

    sendToAll(rank, processes, MessageType::FIN);
    while (finsCount < processes - 1) {
        handle_messages(rank, queue, false);
    }

    printf("Процесс %d: завершил работу; время: %d\n", rank, local_time);
}

int main(int argc, char* argv[]) {
    MPI_Init(&argc, &argv);

    int processes, rank;
    MPI_Comm_size(MPI_COMM_WORLD, &processes);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    process_work(rank, processes);

    MPI_Finalize();
    return 0;
}