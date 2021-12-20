#include <iostream>
#include <mpi.h>

using namespace std;

const int N = 10;

int main(int argc, char **argv)
{
    srand(0);
	int rank, size;
	MPI_Status status;
    double t_start, sum_time;

	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    for (int message_size = 1; message_size <= 1000000; message_size *= 10) {
        int msg[1000000];
        int recv[1000000];
        for (int i = 0; i < 1000000; i++) {
            msg[i] = rand() % 10;
        }
        for (int i = 0; i < 1000000; i++) {
            recv[i] = rand() % 10;
        }

        sum_time = 0.0;
        
        t_start = MPI_Wtime();

        for (int trial = 0; trial < N; trial++) {
            if (rank == 0) {

                MPI_Send(&msg[0], message_size, MPI_INT, 1, 0, MPI_COMM_WORLD);
                MPI_Recv(recv, message_size, MPI_INT, 1, 0, MPI_COMM_WORLD, &status);

            } else if(rank == 1) {                               
                MPI_Recv(recv, message_size, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
                MPI_Send(recv, message_size, MPI_INT, 0, 0, MPI_COMM_WORLD);
            }
        }
        if (rank == 0) { 
            sum_time = MPI_Wtime() - t_start;

            cout<<"The time was "<<sum_time/(double)N<<" for message size: "<<message_size * sizeof(int)<<"B"<<endl;

            double L = message_size * sizeof(int) / 1048576.0;
            double bandwidth2 = 2.0 * L * (double)N / sum_time;
            double latency = sum_time/(double)N/2.0;
            cout << "The bandwidth is: " << bandwidth2 << " and latency: " << latency <<endl << endl;
        }
    }

	MPI_Finalize();
    return 0;
}