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

enum class MessageType { REQUEST, REPLY, RELEASE, FIN };

class Timestamp {
private:
    int time_;
    int process_rank_;

public:
    Timestamp(int time, int rank) : time_(time), process_rank_(rank) {}    

    bool operator<(const Timestamp& other) const {
        return time_ < other.time_ || (time_ == other.time_ && process_rank_ < other.process_rank_);
    }

    friend std::ostream& operator<<(std::ostream& os, const Timestamp& ts) {
        return os << "{" << ts.time_ << ", " << ts.process_rank_ << "}";
    }

    int time() const { return time_; }
    int rank() const { return process_rank_; }
};

class RequestQueue {
private:
    std::set<Timestamp> queue_;
    int max_size_;

public:
    RequestQueue(int max_processes) : max_size_(max_processes) {}

    void add(const Timestamp& ts) {
        if (ts.rank() >= max_size_ || queue_.size() >= max_size_) {
            printf("Ошибка: невозможно добавить запрос в очередь\n");
            return;
        }
        queue_.insert(ts);
    }

    void remove(int rank) {
        if (rank >= max_size_) {
            printf("Ошибка: неверный ранг процесса\n");
            return;
        }

        auto it = std::find_if(queue_.begin(), queue_.end(),
                              [rank](const Timestamp& ts) { return ts.rank() == rank; });
        
        if (it != queue_.end()) {
            queue_.erase(it);
        } else {
            printf("запрос от процесса %d не найден\n", rank);
        }
    }

    bool is_front(int rank) const {
        return !queue_.empty() && queue_.begin()->rank() == rank;
    }

    void print() const {
        for (const auto& ts : queue_) std::cout << ts << " ";
        printf("\n");
    }
};

volatile sig_atomic_t alarm_flag = 1;
int local_time = 0;
int received_fin = 0;
int received_replies = 0;

void alarm_handler(int) { alarm_flag = 0; }

void increment_time() { local_time += TIME_INCREMENT; }

void update_time(int received_time) {
    local_time = std::max(local_time, received_time) + TIME_INCREMENT;
}

void set_random_timer() {
    alarm(rand() % CRITICAL_SECTION_TIME + 1);
}

void checkState(int status) {
    if (status != MPI_SUCCESS) {
        MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
    }
}

void sendToAll(int sender_rank, int processes, MessageType type) {
    for (int dest = 0; dest < processes; ++dest) {
        if (dest != sender_rank) {
            checkState(MPI_Send(&local_time, 1, MPI_INT, dest, static_cast<int>(type), MPI_COMM_WORLD));
        }
    }

    switch (type) {
        case MessageType::REQUEST:
            printf("Процесс %d: отправил всем сообщение REQUEST\n", sender_rank);
            break;
        case MessageType::RELEASE:
            printf("Процесс %d: отправил всем сообщение RELEASE\n", sender_rank);
            break;
        case MessageType::FIN:
            printf("Процесс %d: отправил всем сообщение FIN\n", sender_rank);
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
                queue.add(Timestamp(received_time, status.MPI_SOURCE));
                checkState(MPI_Send(&local_time, 1, MPI_INT, status.MPI_SOURCE, 
                             static_cast<int>(MessageType::REPLY), MPI_COMM_WORLD));                
                increment_time();
                break;

            case MessageType::REPLY:                
                if (count_replies) received_replies++;
                break;

            case MessageType::RELEASE:                
                queue.remove(status.MPI_SOURCE);
                break;

            case MessageType::FIN:                
                received_fin++;
                break;
        }
    }
}

void critical_section_work(int rank, RequestQueue& queue) {
    alarm_flag = 1;
    set_random_timer();

    while (alarm_flag) {
        handle_messages(rank, queue, false);
    }
}

void process_work(int rank, int processes) {
    RequestQueue queue(processes);
    signal(SIGALRM, alarm_handler);
    srand(rank);

    for (int i = 0; i < REPEAT_COUNT; ++i) {
        printf("\nПроцесс %d: начал итерацию %d, время: %d\n", rank, i+1, local_time);

        // Делаем запрос
        printf("Процесс %d: вход в пролог\n", rank);
        queue.add(Timestamp(local_time, rank));
        sendToAll(rank, processes, MessageType::REQUEST);
        received_replies = 0;

        // Ожидаем разрешение
        while (received_replies < processes - 1 || !queue.is_front(rank)) {
            handle_messages(rank, queue, true);
        }
        increment_time();

        // Критическая секция
        printf("Процесс %d: вход в критическую секцию\n", rank);
        critical_section_work(rank, queue);
        increment_time();

        // Эпилог
        printf("Процесс %d: вход в эпилог\n", rank);
        queue.remove(rank);
        sendToAll(rank, processes, MessageType::RELEASE);

        // Reminder секция
        printf("Процесс %d: вход в reminder секцию\n", rank);
        critical_section_work(rank, queue);
        increment_time();
    }

    // Завершение работы
    sendToAll(rank, processes, MessageType::FIN);
    while (received_fin < processes - 1) {
        handle_messages(rank, queue, false);
    }

    printf("Процесс %d: завершил работу\n", rank);
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