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
            return false;
        }
        queue_.insert(ts);
        return true;
    }

    bool remove(int rank) {
        auto it = std::find_if(queue_.begin(), queue_.end(),
                             [rank](const Timestamp& ts) { return ts.rank() == rank; });
        if (it != queue_.end()) {
            queue_.erase(it);
            return true;
        }
        return false;
    }

    bool is_front(int rank) const {
        return !queue_.empty() && queue_.begin()->rank() == rank;
    }

    bool empty() const {
        return queue_.empty();
    }

    friend std::ostream& operator<<(std::ostream& os, const RequestQueue& rq) {
        if (rq.queue_.empty()) {
            return os << "empty";
        }
        for (const auto& ts : rq.queue_) os << ts << " ";
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
    local_time = std::max(local_time, received_time + TIME_INCREMENT);
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
    increment_time();
    for (int i = 0; i < processes; ++i) {
        if (i != sender_rank) {
            checkState(MPI_Send(&local_time, 1, MPI_INT, i, static_cast<int>(type), MPI_COMM_WORLD));
        }
    }

    switch(type) {
        case MessageType::REQUEST: 
            printf("Node %d send REQUEST to all at time %d\n", sender_rank, local_time);
            break;
        case MessageType::RELEASE: 
            printf("Node %d send RELEASE to all at time %d\n", sender_rank, local_time);
            break;
        case MessageType::FIN: 
            printf("Node %d send FIN to all at time %d\n", sender_rank, local_time);
            break;
        default: break;
    }    
}

void handle_messages(int current_rank, RequestQueue& queue, bool count_replies) {
    MPI_Status status;
    int has_message;

    while (true) {
        checkState(MPI_Iprobe(MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &has_message, &status));
        if (!has_message) break;
        increment_time();
        int received_time;
        checkState(MPI_Recv(&received_time, 1, MPI_INT, status.MPI_SOURCE, 
                         status.MPI_TAG, MPI_COMM_WORLD, &status));

        update_time(received_time);
        MessageType msg_type = static_cast<MessageType>(status.MPI_TAG);
        
        switch (msg_type) {
            case MessageType::REQUEST:
                printf("Node %d receive REQUEST (%d,%d) at time %d\n",
                      current_rank, received_time, status.MPI_SOURCE, local_time);
                if (!queue.add(Timestamp(received_time, status.MPI_SOURCE))) {
                    printf("Error: Failed to add request to queue\n");
                }
                printf("Node %d queue: ", current_rank);
                std::cout << queue << std::endl;
                increment_time();
                checkState(MPI_Send(&local_time, 1, MPI_INT, status.MPI_SOURCE, 
                             static_cast<int>(MessageType::REPLY), MPI_COMM_WORLD));                
                printf("Node %d send REPLY to %d at time %d\n",
                      current_rank, status.MPI_SOURCE, local_time);
                break;

            case MessageType::REPLY:
                printf("Node %d receive REPLY (%d,%d) at time %d",
                      current_rank, received_time, status.MPI_SOURCE, local_time);
                if (count_replies) {
                    permsCount++;
                    printf(" nperms=%d", permsCount);
                }
                printf("\n");
                break;

            case MessageType::RELEASE:
                printf("Node %d receive RELEASE (%d,%d) at time %d\n",
                      current_rank, received_time, status.MPI_SOURCE, local_time);
                if (!queue.remove(status.MPI_SOURCE)) {
                    printf("Error: Failed to remove request from queue\n");
                }
                printf("Node %d queue: ", current_rank);
                std::cout << queue << std::endl;
                break;

            case MessageType::FIN:
                printf("Node %d receive FIN (%d,%d) at time %d\n",
                      current_rank, received_time, status.MPI_SOURCE, local_time);
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
        printf("\nNode %d: ENTER PROLOGUE at time %d\n", rank, local_time);

        sendToAll(rank, processes, MessageType::REQUEST);
        if (!queue.add(Timestamp(local_time, rank))) {
            printf("Error: Failed to add own request\n");
        }
        printf("Node %d queue: ", rank);
        std::cout << queue << std::endl;
        
        permsCount = 0;

        while (permsCount < processes - 1 || !queue.is_front(rank)) {
            handle_messages(rank, queue, true);
        }
        // increment_time();

        printf("Node %d: ENTER CRITICAL SECTION at time %d\n", rank, local_time);
        work(rank, CRITICAL_SECTION_TIME, queue);
        // increment_time();

        printf("Node %d: EXIT CRITICAL SECTION at time %d\n", rank, local_time);
        printf("Node %d: ENTER EPILOGUE at time %d\n", rank, local_time);
        if (!queue.remove(rank)) {
            printf("Error: Failed to remove own request\n");
        }
        printf("Node %d queue: ", rank);
        std::cout << queue << std::endl;
        sendToAll(rank, processes, MessageType::RELEASE);

        printf("Node %d: ENTER REMAINDER SECTION at time %d\n", rank, local_time);
        work(rank, REMAINDER_SECTION_TIME, queue);
        // increment_time();
    }

    sendToAll(rank, processes, MessageType::FIN);
    while (finsCount < processes - 1) {
        handle_messages(rank, queue, false);
    }

    printf("Node %d: FINISH WORK at time %d\n", rank, local_time);
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