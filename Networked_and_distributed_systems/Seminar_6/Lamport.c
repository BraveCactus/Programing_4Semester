#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>


char comClass(int x){
    if(x > 0){
        return '+';
    } else if(x < 0){
        return '-';
    } else{
        return '0';
    }
}

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
    
    if (rank >= 0 && rank < size) {

        int locTime = 0;
        int d = 1;

        // считываем данные в массив        
        char filename[10];
        snprintf(filename, sizeof(filename), "0%d.dat", rank + 1);
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
        if (commands == NULL) {
            printf("Ошибка выделения памяти.\n");
            fclose(file);
            return 1;
        }
        
        for (int i = 0; i < count; i++) {
            if (fscanf(file, "%d", &commands[i]) != 1) {
                printf("Ошибка чтения файла.\n");                
                fclose(file);
                return 1;
            }
        }

        fclose(file);

        for (int j = 0; j < count; j++){
            locTime += d;
            int time_from_proc;
            switch(comClass(commands[j])){
                case '+':
                    st = MPI_Send(&locTime, 1, MPI_INT, commands[j] - 1, 0, MPI_COMM_WORLD);
                    CheckState(st);
                    printf("Процесс %d: Отправка сообщения %d процессу \n", rank + 1, commands[j]);
                    break;
                case '-':                    
                    st = MPI_Recv(&time_from_proc, 1, MPI_INT, commands[j]*(-1) - 1, 0, MPI_COMM_WORLD, &status);
                    CheckState(st);

                    locTime = (locTime > time_from_proc + d) ? locTime: time_from_proc + d;

                    printf("Процесс %d: Прием сообщения  от процесса %d \n", rank + 1, commands[j]*(-1));
                    break;
                case '0':
                    printf("Процесс %d: Внутреннее событие \n", rank + 1);
                    break;
            }
        }       
        
        printf("Процесс %d: Итоговое время %d\n", rank + 1, locTime);        
    }    
    st = MPI_Finalize();
    CheckState(st);
    return 0;
}