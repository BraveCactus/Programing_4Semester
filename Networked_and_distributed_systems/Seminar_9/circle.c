#include <mpi.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <signal.h>
#include <stdio.h>
#include <time.h>
#include <stdbool.h>

const int RANK = 1;
const int TIMEOUT = 2;

volatile sig_atomic_t sigflag = 1;

// Проверка статуса MPI операций
void CheckState(int status) {
    if (status != MPI_SUCCESS) {
        MPI_Abort(MPI_COMM_WORLD, 1);
    }
}

void handler(int sig) {
    sigflag = 0;
}

int main(int argc, char* argv[]) {
    int coordinator = -1;

    int rank, size;
    MPI_Status status;

    int st = MPI_Init(&argc, &argv);
    CheckState(st);
    st = MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    CheckState(st);
    st = MPI_Comm_size(MPI_COMM_WORLD, &size);
    CheckState(st);      

    bool is_alive;

    // Узнаем, кто из процессов остался в живых    
    if (rank != RANK) {
        srandom(rank);
        int rand_num = random();
        is_alive = (rand_num % 2) == 1;
        
        if (!is_alive) {
            printf("Процесс %d: я мертв (случайное число %d)\n", rank, rand_num);
            CheckState(MPI_Finalize());
            return 0;

        }
        printf("Процесс %d: я жив (случайное число: %d)\n", rank, rand_num);
    } else {
        is_alive = true;
        printf("Процесс %d: я жив (являюсь инициализатором)\n", rank);
    }

    // Буфер для сообщений: участники
    int* buffer = (int*)malloc((size) * sizeof(int));

    for (int i = 0; i < size; i++) {
        buffer[i] = 0;
    }

    if(rank == RANK){
        // Логика инициализатора выборов:
        int next_proc = 0;        

        buffer[rank] = 1;
        int msg_check = 0;

        for (next_proc = (rank + 1) % size; next_proc != rank; next_proc = (next_proc + 1) % size) {             
            printf("Процесс %d: отправил буфер процессу %d\n", rank, next_proc);
            
            signal(SIGALRM, handler);  

            CheckState(MPI_Send(buffer, size, MPI_INT, next_proc, 0, MPI_COMM_WORLD));                                  

            sigflag = 1;
            alarm(TIMEOUT);

            while (sigflag) {
                CheckState(MPI_Iprobe(next_proc, 1, MPI_COMM_WORLD, &msg_check, &status));                    

                if (msg_check) {
                    CheckState(MPI_Recv(buffer, size, MPI_INT, next_proc, 1, MPI_COMM_WORLD, &status));                                        

                    printf("Процесс %d: следующий - процесс %d\n", rank, next_proc);
                    alarm(0);
                    break;
                }
            }

            if (!sigflag) {
                printf("Процесс %d: ничего не получил от процесса %d\n", rank, next_proc);                
                continue;
            }

            if (msg_check) break;
            
        }

        if (next_proc == rank) {
            printf("Процесс %d: я единственный процесс\n", rank);
      
            free(buffer);
            MPI_Finalize();
            return 0;
        }

        printf("Процесс %d: ожидаю сообщение \n", rank);
        CheckState(MPI_Recv(buffer, size, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status));                           

        printf("Процесс %d: кольцо замкнуто \n", rank);
        CheckState(MPI_Send(buffer, size, MPI_INT, status.MPI_SOURCE, 1, MPI_COMM_WORLD));

        if (buffer[rank] == 1) {
            for (int i = size - 1; i >= 0; i--)
                if (buffer[i] == 1) {
                    coordinator = i;
                    break;
                }
      
            printf("Процесс %d: текущий лидер - процесс %d\n", rank, coordinator);
            CheckState(MPI_Send(&coordinator, 1, MPI_INT, next_proc, 2, MPI_COMM_WORLD));                
        }

        CheckState(MPI_Recv(&coordinator, 1, MPI_INT, MPI_ANY_SOURCE, 2, MPI_COMM_WORLD, &status));            

        printf("Процесс %d: текущий лидер - процесс %d\n", rank, coordinator);
                           
    } else {
        // Логика остальных процессов:       
        
        int msg_check = 0;
        
        CheckState(MPI_Recv(buffer, size, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status));                          

        printf("Процесс %d: получил буфер от %d\n", rank, status.MPI_SOURCE);

        buffer[rank] = 1;
          
        printf("Процесс %d: ответил процессу %d\n", rank, status.MPI_SOURCE);
        
        CheckState(MPI_Send(buffer, size, MPI_INT, status.MPI_SOURCE, 1, MPI_COMM_WORLD));

        int next_proc = 0;
        for (next_proc = (rank + 1) % size; next_proc != rank; next_proc = (next_proc + 1) % size) {

            printf("Процесс %d: отправил сообщение о голосовании процессу %d\n", rank, next_proc);

            signal(SIGALRM, handler); 

            CheckState(MPI_Send(buffer, size, MPI_INT, next_proc, 0, MPI_COMM_WORLD));                             

            sigflag = 1;
            alarm(TIMEOUT);

            while (sigflag) {
                CheckState(MPI_Iprobe(next_proc, 1, MPI_COMM_WORLD, &msg_check, &status));            

                if (msg_check) {
                    CheckState(MPI_Recv(buffer, size, MPI_INT, next_proc, 1, MPI_COMM_WORLD, &status));                                 

                    printf("Процесс %d: следующий - процесс %d \n", rank, next_proc);
                    alarm(0);
                    break;
                }
            }

            if (msg_check) break;
        }

        CheckState(MPI_Recv(&coordinator, 1, MPI_INT, MPI_ANY_SOURCE, 2, MPI_COMM_WORLD, &status));            

        CheckState(MPI_Send(&coordinator, 1, MPI_INT, next_proc, 2, MPI_COMM_WORLD));

        printf("Процесс %d: текущий лидер - процесс %d\n", rank, coordinator);
                           
    } 
    
    free(buffer);
    CheckState(MPI_Finalize());
    return 0;

}

