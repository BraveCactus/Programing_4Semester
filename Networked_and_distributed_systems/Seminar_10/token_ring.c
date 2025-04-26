#include <mpi.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <signal.h>
#include <time.h>
#include <stdbool.h>

const int REPEAT = 3;
const int TOKEN_TAG = 0;
const int FIN_TAG = 1;

int sigflag = 1;

// Проверка статуса MPI операций
void CheckState(int status) {
    if (status != MPI_SUCCESS) {
        MPI_Abort(MPI_COMM_WORLD, 1);
    }
}


void handler(int nsig) {
    sigflag = 0;
}

int main(int argc, char* argv[]) {
    int rank, size;
    MPI_Status status;

    int st;
    st = MPI_Init(&argc, &argv);
    CheckState(st);
    st = MPI_Comm_size(MPI_COMM_WORLD, &size);
    CheckState(st);
    st = MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    CheckState(st);

    signal(SIGALRM, handler);

    int i = 0;
    int flag, msg;
    int count;

    srand(rank);

    if (rank == 0) {
        CheckState(MPI_Send(&msg, 1, MPI_INT, (rank + 1) % size, TOKEN_TAG, MPI_COMM_WORLD));        
        
        i = 0;
        while (i < REPEAT) {
            printf("Процесс %d: ожидает маркер\n", rank);
            CheckState((&msg, 1, MPI_INT, MPI_ANY_SOURCE, TOKEN_TAG, MPI_COMM_WORLD, &status));            

            printf("Процесс %d: получил маркер, входит в критическую секцию\n", rank);

            sleep(rand() % 2 + 1);

            printf("Процесс %d: вышел из критической секции\n", rank);

            CheckState((&msg, 1, MPI_INT, (rank + 1) % size, TOKEN_TAG, MPI_COMM_WORLD));            

            sigflag = 1;
            alarm(rand() % 4 + 1);
            while (sigflag) {
                CheckState((MPI_ANY_SOURCE, TOKEN_TAG, MPI_COMM_WORLD, &flag, &status));                
                if (flag) {
                    CheckState(MPI_Recv(&msg, 1, MPI_INT, MPI_ANY_SOURCE, TOKEN_TAG, MPI_COMM_WORLD, &status));                    

                    CheckState(MPI_Send(&msg, 1, MPI_INT, (rank + 1) % size, TOKEN_TAG, MPI_COMM_WORLD));                    
                }
            }

            i++;
        }

        printf("Процесс %d: начал завершение работы\n", rank);
        CheckState(MPI_Send(&msg, 1, MPI_INT, (rank + 1) % size, FIN_TAG, MPI_COMM_WORLD));    

        sigflag = 1;
        count = 0;
        while (sigflag) {
            CheckState(MPI_Iprobe(MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &flag, &status));            
            if (flag) {
                if (status.MPI_TAG == TOKEN_TAG) {
                    rc = MPI_Recv(&msg, 1, MPI_INT, MPI_ANY_SOURCE, TOKEN_TAG, MPI_COMM_WORLD, &status);
                    CheckState(rc, "Receive");

                    rc = MPI_Send(&msg, 1, MPI_INT, (rank + 1) % size, TOKEN_TAG, MPI_COMM_WORLD);
                    CheckState(rc, "Send");
                }
                if (status.MPI_TAG == FIN_TAG) {
                    printf("Процесс %d: готов получить FIN\n", rank);
                    rc = MPI_Recv(&msg, 1, MPI_INT, MPI_ANY_SOURCE, FIN_TAG, MPI_COMM_WORLD, &status);
                    CheckState(rc, "Receive");

                    rc = MPI_Send(&msg, 1, MPI_INT, (rank + 1) % size, FIN_TAG, MPI_COMM_WORLD);
                    CheckState(rc, "Send");
                    printf("Процесс %d: передал FIN следующему процессу\n", rank);

                    count++;
                    if (count == 2) {
                        break;
                    }
                }
            }
        }
    } 
    else {
        i = 0;
        while (i < REPEAT) {
            printf("Процесс %d: ожидает маркер\n", rank);
            rc = MPI_Recv(&msg, 1, MPI_INT, MPI_ANY_SOURCE, TOKEN_TAG, MPI_COMM_WORLD, &status);
            CheckState(rc, "Receive");

            printf("Процесс %d: получил маркер, входит в критическую секцию\n", rank);

            sleep(rand() % 2 + 1);

            printf("Процесс %d: вышел из критической секции\n", rank);

            rc = MPI_Send(&msg, 1, MPI_INT, (rank + 1) % size, TOKEN_TAG, MPI_COMM_WORLD);
            CheckState(rc, "Send");

            sigflag = 1;
            alarm(rand() % 5 + 1);
            while (sigflag) {
                rc = MPI_Iprobe(MPI_ANY_SOURCE, TOKEN_TAG, MPI_COMM_WORLD, &flag, &status);
                CheckState(rc, "Iprobe");
                if (flag) {
                    rc = MPI_Recv(&msg, 1, MPI_INT, MPI_ANY_SOURCE, TOKEN_TAG, MPI_COMM_WORLD, &status);
                    CheckState(rc, "Receive");

                    rc = MPI_Send(&msg, 1, MPI_INT, (rank + 1) % size, TOKEN_TAG, MPI_COMM_WORLD);
                    CheckState(rc, "Send");
                }
            }

            i++;
        }

        count = 0;
        sigflag = 1;
        while (sigflag) {
            rc = MPI_Iprobe(MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &flag, &status);
            CheckState(rc, "Iprobe");
            if (flag) {
                if (status.MPI_TAG == TOKEN_TAG) {
                    rc = MPI_Recv(&msg, 1, MPI_INT, MPI_ANY_SOURCE, TOKEN_TAG, MPI_COMM_WORLD, &status);
                    CheckState(rc, "Receive");

                    rc = MPI_Send(&msg, 1, MPI_INT, (rank + 1) % size, TOKEN_TAG, MPI_COMM_WORLD);
                    CheckState(rc, "Send");
                }
                if (status.MPI_TAG == FIN_TAG) {
                    printf("Процесс %d: готов получить FIN\n", rank);
                    rc = MPI_Recv(&msg, 1, MPI_INT, MPI_ANY_SOURCE, FIN_TAG, MPI_COMM_WORLD, &status);
                    CheckState(rc, "Receive");

                    rc = MPI_Send(&msg, 1, MPI_INT, (rank + 1) % size, FIN_TAG, MPI_COMM_WORLD);
                    CheckState(rc, "Send");
                    printf("Процесс %d: передал FIN следующему процессу\n", rank);

                    count++;
                    if (count == 2) {
                        break;
                    }
                }
            }
        }
    }

    printf("Процесс %d: завершает работу\n", rank);
    rc = MPI_Finalize();
    CheckState(rc, "Finalize");

    return 0;
}