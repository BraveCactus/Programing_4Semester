#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

void CheckState(int state) {
    if (state != MPI_SUCCESS){
	MPI_Abort(MPI_COMM_WORLD, 1);
    } 
}

const int CLIENTS_NUMBER = 5;

int main(int argc, char *argv[]) {
    int rank, size, st;
    MPI_Status status;

    // Инициализация MPI    
    st = MPI_Init(&argc, &argv);
    CheckState(st);
    st = MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    CheckState(st);
    st = MPI_Comm_size(MPI_COMM_WORLD, &size);
    CheckState(st);
    
    
    if (rank == 0) {
        // Сервер
        srandom(rank); 
        int utc_time_T1 = random() % 21;

        printf("Сервер %d: глобальное время: %d\n", rank, utc_time_T1);
        
        for (int i = 1; i <= CLIENTS_NUMBER; i++) {
            int message;
            st = MPI_Recv(&message, 1, MPI_INT, i, 0, MPI_COMM_WORLD, &status);             
            CheckState(st);

            // Время приема сообщения
            int receive_time = utc_time_T1 + (random() % 21);
            
            // Время обработки сообщения
            int d = random() % 4; 
            int send_time = receive_time + d; // Время отправки сообщения

            utc_time_T1 += send_time;           
            
            printf("Сервер %d: Клиенту %d передается utc_time_T1 = %d, d = %d\n", rank, i, utc_time_T1, d);
            
            //Передаем utc_time_T1 и d
            st = MPI_Send(&utc_time_T1, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
            CheckState(st);
            st = MPI_Send(&d, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
            CheckState(st);
        }     
        
    } else if (rank >= 1 && rank <= CLIENTS_NUMBER) {
        // Клиент
        srandom(rank); 
        int loc_time_T0 = random() % 21;

        printf("Клиент %d: Локальное время до коррекции: %d\n", rank, loc_time_T0);

        int message = 1; // Сообщение для сервера
        st = MPI_Send(&message, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
        CheckState(st);

        int utc_time_T1, d;        
        
        st = MPI_Recv(&utc_time_T1, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
        CheckState(st);
        st = MPI_Recv(&d, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
        CheckState(st);
        
        //Время работы
        int work_time = rand() % (10 - 5 + 1) + 5;        

        int correction_time = round((utc_time_T1 - loc_time_T0 - d) / 2.0) + work_time;

        printf("Клиент %d: коррекция по времени равна %d \n", rank, correction_time);

        loc_time_T0 += correction_time;
        
        printf("Клиент %d: Локальное время после коррекции: %d\n", rank, loc_time_T0);        
    }    
    st = MPI_Finalize();
    CheckState(st);
    return 0;
}
