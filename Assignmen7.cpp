#include <iostream>
#include <mpi.h>

using namespace std;

const int VEC_LEN = 100;

int* generate_vector(int length) {
    int* vec = new int[length];
    for (int i = 0; i < length; i++) {
        vec[i] = 1;
    }
    return vec;
}

int* separate_vector(int* vec, int start, int end) {
    int* vec2 = new int[end - start];
    for (int i = start; i < end; i++) {
        vec2[i - start] = vec[start];
    }
    return vec2;
}

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
        int* vec1 = generate_vector(VEC_LEN);
        int* vec2 = generate_vector(VEC_LEN);
        int* v1_send = new int[batch_size];
        //int* v2_send = int[batch_size];
        int res = 0;
        int full_sum = 0;

        for (int dest = 0; dest < size; dest++) {
            if (dest == 0) {
                full_sum += calc_dot_product(vec1, vec2, 0, batch_size);
            } else {
                v1_send = separate_vector(vec1, dest * batch_size, (dest + 1) * batch_size);
                MPI_Send(v1_send, batch_size, MPI_INT, dest, 1, MPI_COMM_WORLD);
                //MPI_Send(&vec2[dest * batch_size], batch_size, MPI_INT, dest, 2, MPI_COMM_WORLD);
            }
        }

        for (int dest = 1; dest < size; dest++) {
            MPI_Probe(MPI_ANY_SOURCE, 3, MPI_COMM_WORLD, &status);
            MPI_Recv(&res, 1, MPI_INT, dest, 3, MPI_COMM_WORLD, &status);
                
            full_sum += res;
        }

        cout << "Dot product result is: " << full_sum << endl;
    } else {
        int *v1 = new int[batch_size];
        int *v2 = new int[batch_size];

        MPI_Recv(&v1, batch_size, MPI_INT, 0, 1, MPI_COMM_WORLD, &status);
        for (int i = 0; i<batch_size;i++){
            cout<<i<<": "<<v1[i]<<endl;
        }
        //MPI_Recv(&v2, batch_size, MPI_INT, 0, 2, MPI_COMM_WORLD, &status);
        //for (int i = 0; i<batch_size;i++){
        //    cout<<i<<": "<<v2[i]<<endl;
        //}
        cout<<"RECEIVED"<<rank<<endl<<endl;
        int dp = 1;//calc_dot_product(v1, v2, 0, batch_size);

        MPI_Send(&dp, 1, MPI_INT, 0, 3, MPI_COMM_WORLD);
    }
	MPI_Finalize();
    return 0;
}