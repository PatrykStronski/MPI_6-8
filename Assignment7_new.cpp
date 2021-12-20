#include <iostream>
#include <mpi.h>

using namespace std;

const int VEC_LEN = 1000000;

int calc_dot_product(int* vec1, int* vec2, int start, int end) {
    int res = 0;
    for (int i = start; i < end; i++) {
        res += vec1[i] * vec2[i];
    }
    return res;
}

using namespace std;
int main(int argc, char **argv)
{
	int rank, size, ibuf;
	MPI_Status status;
	float rbuf;
	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    const int batch_size = VEC_LEN/size;

    if (rank == 0) {
        int vec1[1000000];
        int vec2[1000000];

        for (int i = 0; i < VEC_LEN; i++) {
            vec1[i] = 1;
        }

        for (int i = 0; i < VEC_LEN; i++) {
            vec2[i] = 1;
        }

        int res = 0;
        int full_sum = 0;

        for (int dest = 0; dest < size; dest++) {
            if (dest == 0) {
                full_sum += calc_dot_product(vec1, vec2, 0, batch_size);
            } else {
                MPI_Send(&vec1[dest * batch_size], batch_size, MPI_INT, dest, 1, MPI_COMM_WORLD);
                MPI_Send(&vec2[dest * batch_size], batch_size, MPI_INT, dest, 2, MPI_COMM_WORLD);
            }
        }

        for (int dest = 1; dest < size; dest++) {
            MPI_Recv(&res, 1, MPI_INT, dest, 3, MPI_COMM_WORLD, &status);
                
            full_sum += res;
        }

        cout << "Dot product result is: " << full_sum << endl;
    } else {
        int v1[1000000];
        int v2[1000000];

        MPI_Recv(&v1, batch_size, MPI_INT, 0, 1, MPI_COMM_WORLD, &status);
        MPI_Recv(&v2, batch_size, MPI_INT, 0, 2, MPI_COMM_WORLD, &status);

        int dp = calc_dot_product(v1, v2, 0, batch_size);

        MPI_Send(&dp, 1, MPI_INT, 0, 3, MPI_COMM_WORLD);
    }
	MPI_Finalize();
    return 0;
}