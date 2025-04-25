#include <stdlib.h>
#include <stdio.h>
#include <mpi.h>
#include <time.h>

// Проверка статуса MPI операций
void CheckState(int status) {
    if (status != MPI_SUCCESS) {
        MPI_Abort(MPI_COMM_WORLD, 1);
    }
}

int main(int argc, char *argv[])
{
    int rank, size;
    MPI_Status status;

    int st = MPI_Init(&argc, &argv);
    CheckState(st);
    st = MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    CheckState(st);
    st = MPI_Comm_size(MPI_COMM_WORLD, &size);
    CheckState(st);   
    
    srandom(rank);    
    
    int lampTime = 0;
    char name[8];
    sprintf(name, "%02d.dat", rank + 1);

    int k;

    FILE *file;
    file = fopen(name, "r");
    if (file == NULL) {
        printf("Процесс %d: Нет событий в данном процессе\n", rank);
        CheckState(MPI_Finalize());
        return 0;
    }

    while (fscanf(file, "%i", &k) != EOF) {
        if (k == 0) {
            // Внутреннее событие
            lampTime++;
            printf(
                "Процесс %d:\n"
                "\tТип события: внутреннее событие\n"
                "\tВремя Лэмпорта: %d\n",
                rank, lampTime
            );
        }
        else if (k < 0) {
            // Принимаем сообщение от процесса source = -(k+1)
            int source = -(k + 1);            
            int timeToRecv;
            CheckState(MPI_Recv(&timeToRecv, 1, MPI_INT, source, MPI_ANY_TAG, MPI_COMM_WORLD, &status));           

            if (timeToRecv > lampTime) {
                lampTime = timeToRecv + 1;
            } else {
                lampTime = lampTime + 1;
            }         

            printf(
                "Процесс %d:\n"
                "\tТип события: Внешнее сообщение от процесса %d\n"
                "\tВремя Лэмпорта: %d (принятое время = %d)\n",
                rank, source, lampTime, timeToRecv
            );
        }
        else {          
            // Отправляем сообшение процессу target = k-1            
            lampTime++;
            int target = k-1;
            CheckState(MPI_Send(&lampTime, 1, MPI_INT, target, 0, MPI_COMM_WORLD));                       

            printf(
                "Процесс %d:\n"
                "\tТип события: Отправил сообщение процессу %d\n"
                "\tВремя Лэмпорта: %d\n",
                rank, target, lampTime
            );
        }
    }

    fclose(file);
    CheckState(MPI_Finalize());
    return 0;
}