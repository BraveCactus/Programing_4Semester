#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <stdbool.h>
#include <time.h>

const int RANK = 1;
const int TIMEOUT = 5;

#define ELECTION_MSG 1
#define OK_MSG 2
#define COORDINATOR_MSG 3

int sigflag = 1;
int coordinator = -1; //здесь будет хранится номер процесса, который станет новым координатором

bool is_alive = false;
bool in_election = false;
bool received_ok = false;

void CheckState(int state) {
    if (state != MPI_SUCCESS) {
        MPI_Abort(MPI_COMM_WORLD, 1);
    } 
}

//Функция отправляет сообщения всем процессам с большим номером
void send_to_higher(int message, int sender_rank, int size) {
    for (int i = sender_rank + 1; i < size; i++) {
        int st = MPI_Send(&message, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
        CheckState(st);
        printf("Процесс %d: отправил %s %d\n", sender_rank, 
               message == ELECTION_MSG ? "ВЫБОРЫ" : 
               (message == OK_MSG ? "OK" : "КООРДИНАТОР"), i);
    }
}

//Функция отправляет сообщения всем процессам
void send_to_all(int message, int sender_rank, int size) {
    for (int i = 0; i < size; i++) {
        if (i != sender_rank) {
            int st = MPI_Send(&message, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
            CheckState(st);
            printf("Процесс %d: отправил КООРДИНАТОР %d \n", sender_rank, i);                    
        }
    }
}

void handler(int sig) {
    sigflag = 0;
}

int main(int argc, char *argv[]) {
    int rank, size;
    int st = MPI_Init(&argc, &argv);
    CheckState(st);
    st = MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    CheckState(st);
    st = MPI_Comm_size(MPI_COMM_WORLD, &size);
    CheckState(st);
    
    srand(rank);

    //Выбираем кто остался в живых
    if (rank != RANK) {
        int rand_num = rand();
        is_alive = (rand_num % 2) == 0;
        
        if (!is_alive) {
            printf("Процесс %d: я мертв (случайное число %d)\n", rank, rand_num);
            MPI_Finalize();
            return 0;
        }
        printf("Процесс %d: я жив (случайное число: %d)\n", rank, rand_num);
    } else {
        is_alive = true;
        printf("Процесс %d: я жив (являюсь инициализатором)\n", rank);
    }

    
    signal(SIGALRM, handler);

    // Начинаем выборы
    if (rank == RANK && is_alive) {
        printf("Процесс %d: начинаю голосование\n", rank);
        in_election = true;
        send_to_higher(ELECTION_MSG, rank, size);
        alarm(TIMEOUT);
    }

    while (sigflag) {
        if (!is_alive) {            
            continue;
        }

        MPI_Status status;
        int flag;
        int message;

        st = MPI_Iprobe(MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &flag, &status);
        CheckState(st);

        if (flag) {
            st = MPI_Recv(&message, 1, MPI_INT, status.MPI_SOURCE, status.MPI_TAG, 
                    MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            CheckState(st);
            
            printf("Процесс %d: получил %s от %d\n", rank, 
                   message == ELECTION_MSG ? "ВЫБОРЫ" : 
                   (message == OK_MSG ? "OK" : "КООРДИНАТОР"), status.MPI_SOURCE);

            switch (message) {
                case 1:
                    if (rank > status.MPI_SOURCE) {
                        int ok_msg = OK_MSG;
                        st = MPI_Send(&ok_msg, 1, MPI_INT, status.MPI_SOURCE, 0, MPI_COMM_WORLD);
                        CheckState(st);
                        printf("Процесс %d: отправил OK %d\n", rank, status.MPI_SOURCE);
                        
                        if (!in_election) {
                            in_election = true;
                            printf("Процесс %d: начинаем новые выборы\n", rank);
                            send_to_higher(ELECTION_MSG, rank, size);
                            alarm(TIMEOUT);
                        }
                    }
                    break;
                    
                case 2:
                    received_ok = true;
                    printf("Процесс %d: сбрасываем таймер\n", rank);
                    alarm(0); //Сбрасываем таймер
                    break;
                    
                case 3:
                    coordinator = status.MPI_SOURCE;
                    printf("Процесс %d: теперь новый координатор %d\n", rank, coordinator);
                    
                    if (rank != coordinator) {
                        st = MPI_Finalize();
                        CheckState(st);
                        return 0;
                    }
                    break;
            }
        }

        if (!sigflag && in_election) {
            sigflag = 0;
            
            if (!received_ok) {
                coordinator = rank;
                printf("Процесс %d: не получил ОК, значит я координатор\n", rank);
                send_to_all(COORDINATOR_MSG, rank, size);
                printf("Процесс %d: я координатор\n", rank);
                st = MPI_Finalize();
                CheckState(st);
                return 0;
            } else {
                received_ok = false;
                printf("Процесс %d: получил ОК, жду сообщения от координатора\n", rank);
                in_election = false;
            }
        }        
    }

    st = MPI_Finalize();
    CheckState(st);
    return 0;
}