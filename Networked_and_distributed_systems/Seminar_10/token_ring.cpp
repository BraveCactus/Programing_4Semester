#include <stdlib.h>
#include <iostream>
#include "mpi.h"
#include <stdio.h>
#include <math.h>
#include <signal.h>
#include "unistd.h"
#include <string>

const int REPEAT = 3;

void CheckState(int status) {
    if (status != MPI_SUCCESS) {
        MPI_Abort(MPI_COMM_WORLD, 1);
    }
}

int sigflag = 1;
void handler(int nsig) {
	sigflag = 0;
}

int main(int argc, char* argv[])
{
	int rank, size, rc;
	MPI_Status status;

	CheckState(MPI_Init(&argc, &argv));
	
	CheckState(MPI_Comm_size(MPI_COMM_WORLD, &size));
	
	CheckState(MPI_Comm_rank(MPI_COMM_WORLD, &rank));
	

	signal(SIGALRM, handler);
	
	int i = 0;
	int flag, msg;

	srand(rank);

	if (rank == 0)
	{
		CheckState(MPI_Send(&msg, 1, MPI_INT, (rank + 1) % size, 0, MPI_COMM_WORLD));
		
		i = 0;
		while (i != REPEAT)
		{						
			printf(
				"Процесс %d:\n"
				"\tожидает маркер\n",				
				rank
			);
			CheckState(MPI_Recv(&msg, 1, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status));			
			
			printf(
				"Процесс %d:\n"
				"\tполучил маркер, отправляется в критическую область\n",				
				rank
			);

			sleep(rand() % 2 + 1);			

			printf(
				"Процесс %d:\n"
				"\tспит\n",				
				rank
			);

			CheckState(MPI_Send(&msg, 1, MPI_INT, (rank + 1) % size, 0, MPI_COMM_WORLD));			

			sigflag = 1;
			alarm(rand() % 5 + 1);
			while (sigflag)
			{
				CheckState(MPI_Iprobe(MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &flag, &status));				
				if (flag)
				{
					CheckState(MPI_Recv(&msg, 1, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status));					

					CheckState(MPI_Send(&msg, 1, MPI_INT, (rank + 1) % size, 0, MPI_COMM_WORLD));					
				}
			}

			i++;
		}		
		printf(
			"Процесс %d:\n"
			"\tпоследняя критическая секция\n",				
			rank
		);

		CheckState(MPI_Send(&msg, 1, MPI_INT, (rank + 1) % size, 1, MPI_COMM_WORLD));		

		sigflag = 1;
		int count = 0;
		while (sigflag)
		{
			CheckState(MPI_Iprobe(MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &flag, &status));
			
			if (flag)
			{
				if (status.MPI_TAG == 0)
				{
					CheckState(MPI_Recv(&msg, 1, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status));					

					CheckState(MPI_Send(&msg, 1, MPI_INT, (rank + 1) % size, 0, MPI_COMM_WORLD));					
				}
				if (status.MPI_TAG == 1)
				{					
					printf(
						"Процесс %d:\n"
						"\tгототов попасть в последнюю критическую секцию\n",				
						rank
					);
					CheckState(MPI_Recv(&msg, 1, MPI_INT, MPI_ANY_SOURCE, 1, MPI_COMM_WORLD, &status));					

					CheckState(MPI_Send(&msg, 1, MPI_INT, (rank + 1) % size, 1, MPI_COMM_WORLD));
					
					printf(
						"Процесс %d:\n"
						"\tотправляет fin следующему процессу\n",				
						rank
					);

					count++;
					if (count == 2)
					{
						break;
					}
				}
			}
		}
		CheckState(MPI_Finalize());		
		return 0;
	}
	else
	{
		i = 0;
		while (i != REPEAT)
		{			
			printf(
				"Процесс %d:\n"
				"\tожидает маркер\n",				
				rank
			);
			CheckState(MPI_Recv(&msg, 1, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status));	

			
			printf(
				"Процесс %d:\n"
				"\tполучил маркер, отправляется в критическую область\n",				
				rank
			);

			sleep(rand() % 2 + 1);
			
			printf(
				"Процесс %d:\n"
				"\tспит\n",				
				rank
			);

			CheckState(MPI_Send(&msg, 1, MPI_INT, (rank + 1) % size, 0, MPI_COMM_WORLD));			

			sigflag = 1;
			alarm(rand() % 5 + 1);
			while (sigflag)
			{
				CheckState(MPI_Iprobe(MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &flag, &status));				
				if (flag)
				{
					CheckState(MPI_Recv(&msg, 1, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status));					

					CheckState(MPI_Send(&msg, 1, MPI_INT, (rank + 1) % size, 0, MPI_COMM_WORLD));					
				}
			}

			i++;
		}
		int count = 0;
		sigflag = 1;
		while (sigflag)
		{
			CheckState(MPI_Iprobe(MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &flag, &status));
			
			if (flag)
			{
				if (status.MPI_TAG == 1)
				{					
					printf(
						"Процесс %d:\n"
						"\tготов принять fin\n",				
						rank
					);
					CheckState(MPI_Recv(&msg, 1, MPI_INT, MPI_ANY_SOURCE, 1, MPI_COMM_WORLD, &status));
					
					CheckState(MPI_Send(&msg, 1, MPI_INT, (rank + 1) % size, 1, MPI_COMM_WORLD));			
					
					printf(
						"Процесс %d:\n"
						"\tотправляет fin следующему процессу\n",				
						rank
					);

				if (status.MPI_TAG == 0)
				{
					CheckState(MPI_Recv(&msg, 1, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status));					

					CheckState(MPI_Send(&msg, 1, MPI_INT, (rank + 1) % size, 0, MPI_COMM_WORLD));					
				}
				
					count++;
					if (count == 2)
					{
						break;
					}
				}
			}
		}
		CheckState(MPI_Finalize());		
		return 0;

	}

	CheckState(MPI_Finalize());
	return 0;
}