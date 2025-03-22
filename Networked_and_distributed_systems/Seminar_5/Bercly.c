#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

void CheckState(int state) {
    if (state != MPI_SUCCESS){
        MPI_Abort(MPI_COMM_WORLD, 1);
    } 
}

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
        int server_time = random() % 51;       
        

        //переменная для вычисления разницы для времени сервера
        int sum_diff = 0;

        //Массив с расхождениями с временем сервера для каждого клиента
        int arr_clients_diff[size];       
                 
        //Сервер отправляет клиентам свое время
        for (int i = 1; i < size; i++)
        {            
            st = MPI_Send(&server_time, 1, MPI_INT, i, 0, MPI_COMM_WORLD);             
            CheckState(st);

            printf("Сервер %d:\n\tсобственное время: %d\n\tотправил %d клиенту свое время: %d\n", rank, server_time, i, server_time);
        }        
        
        //Сервер получает расхождения по времени от собсвенных времен клиентов
        for (int j = 1; j < size; j++)
        {    
            int diff_from_client = 0;
            st = MPI_Recv(&diff_from_client, 1, MPI_INT, j, 0, MPI_COMM_WORLD, &status);
            CheckState(st);            
            arr_clients_diff[j] = diff_from_client;  
            sum_diff += diff_from_client;

            printf("Сервер %d:\n\tсобственное время: %d\n\tполучил разницу по времени от клиента %d: %d\n", rank, server_time, j, diff_from_client);
        } 

        //Считаем среднее расхождение по времени
        int mean_diff = (sum_diff + size - 1) / size;

        server_time += mean_diff;
        
        //Сервер отправляет клиентам поправки по времени
        for (int k = 1; k < size; k++)
        {
            int client_time_dif = 0;            
            if (arr_clients_diff[k] > 0)
            {
                client_time_dif = -(arr_clients_diff[k] - mean_diff);
            } else if (arr_clients_diff[k] <= 0)
            {
                client_time_dif = arr_clients_diff[k] + mean_diff;
            }
            
            
            st = MPI_Send(&client_time_dif, 1, MPI_INT, k, 0, MPI_COMM_WORLD);
            CheckState(st);

            printf("Сервер %d:\n\tобновленное собственное время: %d\n\tотправил %d клиенту поправку по времени: %d\n", rank, server_time, k, client_time_dif);
        }        
            
        
    } else if (rank >= 1 && rank < size) {
        // Клиент
        srandom(rank); 
        int client_time = random() % 51;      
        
        int time_from_server;
        
        //Клиент получает время от сервера
        st = MPI_Recv(&time_from_server, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
        CheckState(st);   
        
        printf("Клиент %d:\n\tсобственное время: %d\n\tполучил время от сервера: %d\n", rank, client_time ,time_from_server);
        
        int difference = client_time - time_from_server;

        //Клиент отправляет свое расхождение по времени от времени сервера
        st = MPI_Send(&difference, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
        CheckState(st);

        printf("Клиент %d:\n\tсобственное время: %d\n\tотправил разницу по времени: %d\n", rank, client_time ,difference);

        int diff_from_server;
        
        //Клиент получает поправку по времени от сервера
        st = MPI_Recv(&diff_from_server, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
        CheckState(st);

        client_time += diff_from_server;
        
        printf("Клиент %d:\n\tобновленное собственное время: %d\n\tполучил от сервера поправку по времени: %d\n", rank, client_time ,diff_from_server);
       
           
    }    
    st = MPI_Finalize();
    CheckState(st);
    return 0;
}