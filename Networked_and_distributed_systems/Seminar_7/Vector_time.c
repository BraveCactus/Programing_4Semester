#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

char comClass(int x) {
    if (x > 0) {
        return '+';
    } else if (x < 0) {
        return '-';
    } else {
        return '0';
    }
}

void print_vector(int vec[], int size) { 
    for (int i = 0; i < size; ++i) {
        printf("%d ", vec[i]);
    }
    printf("\n");
}

void CheckState(int state) {
    if (state != MPI_SUCCESS) {
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
    
    int locTime = 0;
    int d = 1;

    // считываем данные в массив        
    char filename[10];
    snprintf(filename, sizeof(filename), "v0%d.dat", rank + 1);
    FILE *file = fopen(filename, "r");

    if (file == NULL) {
        printf("Процесс %d: для текущего процесса нет команд!\n", rank + 1);
        MPI_Finalize();
        return 0;
    }
    
    int count = 0; // длина массива
    int temp;
    while (fscanf(file, "%d", &temp) == 1) {
        count++;
    }
    
    rewind(file);
   
    int commands[10];
    if (count > 10) {
        printf("Too many commands for process %d (max 10 allowed)\n", rank);
        fclose(file);
        MPI_Abort(MPI_COMM_WORLD, 1);
    }
    
    for (int i = 0; i < count; i++) {
        if (fscanf(file, "%d", &commands[i]) != 1) {
            printf("Ошибка чтения файла.\n");                
            fclose(file);
            MPI_Abort(MPI_COMM_WORLD, 1);
        }
    }

    fclose(file);

    int loc_time[size];
    int vector_from_proc[size]; 

    for (int k = 0; k < size; ++k) {
        loc_time[k] = 0;
    }

    for (int j = 0; j < count; j++) {
 
        loc_time[rank] += d;
        
        switch(comClass(commands[j])) {
            case '+':
                st = MPI_Send(loc_time, size, MPI_INT, commands[j] - 1, 0, MPI_COMM_WORLD);  
                CheckState(st);
                printf("Процесс %d: Отправка сообщения %d процессу \n", rank + 1, commands[j]);
                print_vector(loc_time, size);
                break;
            case '-':                    
                st = MPI_Recv(vector_from_proc, size, MPI_INT, abs(commands[j]) - 1, 0, MPI_COMM_WORLD, &status); 
                CheckState(st);

                for (int i = 0; i < size; ++i) {
                    if(i == rank){
                        continue;
                    }                        
                    loc_time[i] = (loc_time[i] > vector_from_proc[i]) ? loc_time[i] : vector_from_proc[i];
                }                    

                printf("Процесс %d: Прием сообщения от процесса %d \n", rank + 1, abs(commands[j]));
                // print_vector(vector_from_proc, size);
                print_vector(loc_time, size);                    
                break;
            case '0':
                printf("Процесс %d: Внутреннее событие \n", rank + 1);                    
                print_vector(loc_time, size);                    
                break;
        }
    }       
    
    st = MPI_Finalize();
    CheckState(st);
    return 0;
}