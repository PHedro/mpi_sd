#include <stdio.h>
#include <mpi.h>

int numeroDeTarefas = 6;
int matrizVizinhanca[6][6] = {
	{0,1,1,0,0,0},
	{1,0,1,1,1,0},
	{1,1,0,0,0,1},
	{0,1,0,0,0,0},
	{0,1,0,0,0,0},
	{0,0,1,0,0,0}
};

int contaNumeroDeVizinhos(int myRank)
{
	int i;
	int contador = 0;

	for(i=0; i<numeroDeTarefas; i++)
		if(matrizVizinhanca[myRank][i] == 1)
			contador++;
	return contador;
}

int main(int argc, char* argv[])
{
	int i, j, contador;
	int numeroDeVizinhos;
	int myRank;
	int source;
	int tag = 50;
	int pai;
	MPI_Status status;
	int origem;
	int state;
	int dist[numeroDeTarefas];
	int first[numeroDeTarefas];
	int set[numeroDeTarefas];
	int level[numeroDeTarefas];

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &myRank);

	numeroDeVizinhos = contaNumeroDeVizinhos(myRank);
	printf("iniciando...\n");
	fflush(stdout);

	for (i = 0; i < numeroDeTarefas; ++i)
	{
		dist[i] = numeroDeTarefas;
		first[i] = 0;
		set[i] = 0;
		level[i] = 0;
	}
	dist[myRank] = 0;
	set[myRank] = 0;
	state = 0;

	for(i = 0; i < numeroDeTarefas; i++)
	{
		if(matrizVizinhanca[myRank][i] == 1)
		{
			MPI_Send(set, numeroDeTarefas, MPI_INT, i, tag, MPI_COMM_WORLD);
		}
	}
	while(state < numeroDeTarefas)
	{
		MPI_Recv(set, numeroDeTarefas, MPI_INT, MPI_ANY_SOURCE, tag, MPI_COMM_WORLD, &status);
		origem = status.MPI_SOURCE;
		level[origem]++;
		for(i=0; i<numeroDeTarefas; i++)
		{
			if(set[i]==1)
			{
				if(dist[i]>level[origem])
				{
					dist[i]=level[origem];
					first[i]=origem;
				}
				int continua = 1;
				for (j = 0; j < numeroDeTarefas; j++)
				{
					if(matrizVizinhanca[myRank][j]==1 && state>=level[j])
					{
						continua=0;
						break;
					}
					if(continua)
					{
						state++;
						for (j = 0; j < numeroDeTarefas; j++)
						{
							if(dist[j]==state)
								set[j]=1;
						}
						for(j=0; j<numeroDeTarefas; j++)
							if(matrizVizinhanca[myRank][j]==1)
								MPI_Send(set, numeroDeTarefas, MPI_INT, j, tag, MPI_COMM_WORLD);
					}
				}

			}
		}
		printf("processo %d: state=%d\n", myRank, state);
		fflush(stdout);
	}
	printf("processo %d:", myRank);
	for (i = 0; i < numeroDeTarefas; i++)
		printf("dist(%d)=%d, ", i, first[i]);
	printf("\n");
	fflush(stdout);
	MPI_Finalize();
}