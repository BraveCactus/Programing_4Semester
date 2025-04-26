#include <mpi.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <stdio.h>
#include <stdbool.h>

const int REPEAT = 3;
const int TOKEN_TAG = 0;
const int FIN_TAG = 1;

int sigflag = 1;

void CheckState(int status) {
    if (status != MPI_SUCCESS) {
        MPI_Abort(MPI_COMM_WORLD, 1);
    }
}

void handler(int sig) {
    sigflag = 0;
}

int main(int argc, char* argv[]) {
    int rank, size;
    MPI_Status status;
    
    int st = MPI_Init(&argc, &argv);
    CheckState(st);
    st = MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    CheckState(st);
    st = MPI_Comm_size(MPI_COMM_WORLD, &size);
    CheckState(st);
    
    srand(rank); // Инициализация генератора случайных чисел значением ранга
    
    bool has_token = (rank == 0); // Процесс 0 начинает с маркером
    bool fin_received = false;
    bool fin_sent = false;
    int repeat_count = 0;
    int next_rank = (rank + 1) % size;
    
    signal(SIGALRM, handler);
    
    // Процесс 0 сразу передает маркер для начала работы
    if (rank == 0) {
        CheckState(MPI_Send(NULL, 0, MPI_INT, next_rank, TOKEN_TAG, MPI_COMM_WORLD));
        has_token = false;
        printf("Процесс %d: инициировал передачу маркера процессу %d\n", rank, next_rank);
    }
    
    while (repeat_count < REPEAT || !fin_received) {
        // Remainder section
        printf("Процесс %d: вошел в remainder section\n", rank);
        
        if (!has_token && !fin_received) {
            int msg_available;
            int timeout = (rand() % 5) + 1; // Случайный таймаут 1-5 секунд
            alarm(timeout);
            
            while (sigflag) {
                CheckState(MPI_Iprobe(MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &msg_available, &status));
                
                if (msg_available) {
                    alarm(0); // Отменяем таймаут
                    
                    if (status.MPI_TAG == TOKEN_TAG) {
                        CheckState(MPI_Recv(NULL, 0, MPI_INT, status.MPI_SOURCE, TOKEN_TAG, MPI_COMM_WORLD, &status));
                        has_token = true;
                        printf("Процесс %d: получил маркер от %d\n", rank, status.MPI_SOURCE);
                        break;
                    } 
                    else if (status.MPI_TAG == FIN_TAG) {
                        CheckState(MPI_Recv(NULL, 0, MPI_INT, status.MPI_SOURCE, FIN_TAG, MPI_COMM_WORLD, &status));
                        fin_received = true;
                        printf("Процесс %d: получил FIN от %d\n", rank, status.MPI_SOURCE);
                        break;
                    }
                }
                
                if (!sigflag) {
                    printf("Процесс %d: таймаут истек (%d сек), продолжаю работу\n", rank, timeout);
                    break;
                }
            }
        }
        
        printf("Процесс %d: вышел из remainder section\n", rank);
        
        // Critical section
        if (has_token && repeat_count < REPEAT) {
            int work_time = (rand() % 2) + 1; // 1 или 2 секунды
            printf("Процесс %d: вошел в критическую секцию (итерация %d), будет работать %d сек\n", 
                   rank, repeat_count + 1, work_time);
            
            sleep(work_time); // Имитация работы в критической секции
            
            printf("Процесс %d: вышел из критической секции\n", rank);
            
            repeat_count++;
            
            // Передача маркера следующему процессу
            CheckState(MPI_Send(NULL, 0, MPI_INT, next_rank, TOKEN_TAG, MPI_COMM_WORLD));
            has_token = false;
            
            // Если завершили все итерации, отправляем FIN
            if (repeat_count == REPEAT && !fin_sent) {
                CheckState(MPI_Send(NULL, 0, MPI_INT, next_rank, FIN_TAG, MPI_COMM_WORLD));
                printf("Процесс %d: завершил работу, передал FIN процессу %d\n", rank, next_rank);
                fin_sent = true;
            }
        }
        
        // Передача FIN дальше, если получили его и уже завершили свою работу
        if (fin_received && !fin_sent && repeat_count == REPEAT) {
            CheckState(MPI_Send(NULL, 0, MPI_INT, next_rank, FIN_TAG, MPI_COMM_WORLD));
            printf("Процесс %d: передал FIN дальше процессу %d\n", rank, next_rank);
            fin_sent = true;
        }
    }
    
    printf("Процесс %d: завершает работу\n", rank);
    CheckState(MPI_Finalize());
    return 0;
}