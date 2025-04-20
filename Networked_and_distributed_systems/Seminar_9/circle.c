#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>
#include <stdbool.h>


#define RANK 1
#define TIMEOUT 2

#define ELECTION_MSG 1
#define OK_MSG 2
#define COORDINATOR_MSG 3


volatile sig_atomic_t timeout_flag = 1;

void handler(int sig) {
    timeout_flag = 0;
}

// Проверка статуса MPI операций
void CheckState(int status) {
    if (status != MPI_SUCCESS) {
        MPI_Abort(MPI_COMM_WORLD, 1);
    }
}

int main(int argc, char** argv) {
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
   

    // Буфер для сообщений: участники + тип сообщения + отправитель
    int* msg_buffer = (int*)malloc((size + 2) * sizeof(int));

    for (int i = 0; i < size + 2; i++) {
        msg_buffer[i] = 0;
    }
    msg_buffer[size + 1] = rank; // Ранг отправителя

    int next_alive = -1; // Следующий живой процесс
    int current_candidate = (rank + 1) % size; // Кандидат для проверки
    int coordinator = -1;

    if (rank == RANK) {
        // Начинаем выборы
        printf("Процесс %d: начинаю выборы как инициализатор\n", rank);
        
        msg_buffer[rank] = 1; // Отмечаем себя как участника
        msg_buffer[size] = ELECTION_MSG; // Тип сообщения
        
        // Поиск следующего живого процесса
        while (next_alive == -1) {
            printf("Процесс %d: пытаюсь связаться с процессом %d\n", rank, current_candidate);            
            
            CheckState(MPI_Send(msg_buffer, size + 2, MPI_INT, current_candidate, 0, MPI_COMM_WORLD));                              
            
            // Установка таймаута
            timeout_flag = 1;
            signal(SIGALRM, handler);
            alarm(TIMEOUT);
            
            // Ждем ответ
            while (timeout_flag) {
                int has_message;
                CheckState(MPI_Iprobe(MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &has_message, &status));                            
                
                if (has_message) {
                    CheckState(MPI_Recv(msg_buffer, size + 2, MPI_INT,MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status));                             
                    
                    if (msg_buffer[size] == OK_MSG) {
                        next_alive = current_candidate;
                        printf("Процесс %d: Получил ОК от %d\n", rank, next_alive);
                        timeout_flag = 0;
                        alarm(0); // Сбрасываем таймер
                    }
                }
            }
            
            // Переход к следующему кандидату
            current_candidate = (current_candidate + 1) % size;
            
            // Проверка на одиночество
            if (current_candidate == rank) {
                printf("Процесс %d: Я единственный активный процесс, поэтому становлюсь координатором\n", rank);                
                
                // Сообщаем всем процессам, что текущий процесс - координатор
                msg_buffer[size] = COORDINATOR_MSG;
                msg_buffer[size + 1] = rank;
                
                for (int i = 0; i < size; i++) {
                    if (i != rank) {
                        CheckState(MPI_Send(msg_buffer, size + 2, MPI_INT, i, 0, MPI_COMM_WORLD));                                 
                    }
                }
                
                free(msg_buffer);
                MPI_Finalize();
                return 0;
            }
        }
        
        // Ожидание сообщения о координаторе
        while (1) {
            int has_message;
            CheckState(MPI_Iprobe(MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &has_message, &status));                      
            
            if (has_message) {
                CheckState(MPI_Recv(msg_buffer, size + 2, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status));                         
                
                if (msg_buffer[size] == COORDINATOR_MSG) {
                    coordinator = msg_buffer[size + 1];
                    printf("Процесс %d: Выборы закончились. Координатор - %d\n", rank, coordinator);
                    MPI_Finalize();
                    return 0;
                }
                break;
            }
        }
    } 
    else {
        // Логика участников выборов
        printf("Процесс %d: Ожидаю сообшщения о выборах\n", rank);
        
        // Получение первого сообщения
        CheckState(MPI_Recv(msg_buffer, size + 2, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status));                     
        
        if (msg_buffer[size] == ELECTION_MSG) {
            printf("Процесс %d: Получил сообщение о выборах от %d\n", rank, msg_buffer[size + 1]);                  
            
            // Отправка подтверждения
            msg_buffer[rank] = 1; // Участвуем в выборах
            msg_buffer[size] = OK_MSG;
            CheckState(MPI_Send(msg_buffer, size + 2, MPI_INT, status.MPI_SOURCE, 0, MPI_COMM_WORLD));                     
            
            printf("Процесс %d: Отправил ОК %d\n", rank, status.MPI_SOURCE);
            
            // Поиск следующего живого процесса
            msg_buffer[size] = ELECTION_MSG;
            current_candidate = (rank + 1) % size;
            
            while (next_alive == -1) {
                printf("Процесс %d: Пытаюсь связаться с процессом %d\n", rank, current_candidate);
                
                CheckState(MPI_Send(msg_buffer, size + 2, MPI_INT, current_candidate, 0, MPI_COMM_WORLD));                         
                
                // Установка таймаута
                timeout_flag = 1;
                signal(SIGALRM, handler);
                alarm(TIMEOUT);
                
                // Ожидание ответа
                while (timeout_flag) {
                    int has_message;
                    CheckState(MPI_Iprobe(MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &has_message, &status));                              
                    
                    if (has_message) {
                        CheckState(MPI_Recv(msg_buffer, size + 2, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status));                                 
                        
                        if (msg_buffer[size] == OK_MSG) {
                            next_alive = current_candidate;
                            printf("Процесс %d: Получил ОК от %d\n", rank, next_alive);
                            timeout_flag = 0;
                            alarm(0);
                        }
                    }
                }
                
                // Переход к следующему кандидату
                current_candidate = (current_candidate + 1) % size;
                
                // Если прошли полный круг
                if (current_candidate == rank) {
                    // Определяем координатора (процесс с максимальным рангом)
                    for (int i = size - 1; i >= 0; i--) {
                        if (msg_buffer[i] == 1) {
                            coordinator = i;
                            break;
                        }
                    }
                    
                    printf("Процесс %d: Координатором выбран %d\n", rank, coordinator);
                    
                    // Отправка сообщения о координаторе
                    msg_buffer[size] = COORDINATOR_MSG;
                    msg_buffer[size + 1] = coordinator;
                    
                    // Находим предыдущий процесс в кольце
                    int prev = (rank == 0) ? size - 1 : rank - 1;
                    CheckState(MPI_Send(msg_buffer, size + 2, MPI_INT, prev, 0, MPI_COMM_WORLD));                             
                    
                    printf("Процесс %d: Отправил сообщение о координаторе процессу %d\n", rank, prev);
                    break;
                }
            }
        }
        
        // Ожидание/передача сообщения о координаторе
        while (1) {
            int has_message;
            CheckState(MPI_Iprobe(MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &has_message, &status));                      
            
            if (has_message) {
                CheckState(MPI_Recv(msg_buffer, size + 2, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status));                         
                
                if (msg_buffer[size] == COORDINATOR_MSG) {
                    coordinator = msg_buffer[size + 1];
                    
                    if (coordinator == rank) {
                        // Мы уже отправили это сообщение - завершаем работу
                        printf("Процесс %d: Получил сообщение что я координатор\n", rank);
                        MPI_Finalize();
                        return 0;
                        break;
                    } else {
                        printf("Процесс %d: Получил сообщение об избрании координатора. Координатор - процесс %d\n",  rank, coordinator);                             
                        
                        // Пересылаем сообщение следующему процессу
                        int next = (rank + 1) % size;
                        if (next != RANK) { // Не отправляем инициатору, если он уже получил
                            CheckState(MPI_Send(msg_buffer, size + 2, MPI_INT, 
                                         next, 0, MPI_COMM_WORLD));
                            printf("Процесс %d: Отправил сообщение об избрании координатора процессу %d\n", rank, next);
                            MPI_Finalize();
                            return 0;                                  
                        }
                        break;
                    }
                }
            }
        }
    }

    free(msg_buffer);
    MPI_Finalize();
    return 0;
}