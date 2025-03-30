#include <mpi.h>
#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>

using namespace std;

constexpr int MATRIX_SIZE = 500;

int main(int argc, char* argv[]) {
	MPI_Init(&argc, &argv);

	int rank, num_processes;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &num_processes);

	int rows_per_process = MATRIX_SIZE / num_processes;
	vector<int> matrixA(MATRIX_SIZE * MATRIX_SIZE), matrixB(MATRIX_SIZE * MATRIX_SIZE), resultMatrix(MATRIX_SIZE * MATRIX_SIZE, 0);

	// Инициализация матриц на процессе 0
	if (rank == 0) {
			srand(time(nullptr));
			for (int i = 0; i < MATRIX_SIZE; ++i) {
					for (int j = 0; j < MATRIX_SIZE; ++j) {
							matrixA[i * MATRIX_SIZE + j] = rand() % 10;
							matrixB[i * MATRIX_SIZE + j] = rand() % 10;
					}
			}
	}

	// Разделение данных
	vector<int> localMatrixA(rows_per_process * MATRIX_SIZE);
	MPI_Scatter(matrixA.data(), rows_per_process * MATRIX_SIZE, MPI_INT, localMatrixA.data(), rows_per_process * MATRIX_SIZE, MPI_INT, 0, MPI_COMM_WORLD);
	MPI_Bcast(matrixB.data(), MATRIX_SIZE * MATRIX_SIZE, MPI_INT, 0, MPI_COMM_WORLD);

	vector<int> localResult(rows_per_process * MATRIX_SIZE, 0);
	for (int i = 0; i < rows_per_process; ++i) {
			for (int j = 0; j < MATRIX_SIZE; ++j) {
					for (int k = 0; k < MATRIX_SIZE; ++k) {
							localResult[i * MATRIX_SIZE + j] += localMatrixA[i * MATRIX_SIZE + k] * matrixB[k * MATRIX_SIZE + j];
					}
			}
	}

	MPI_Gather(localResult.data(), rows_per_process * MATRIX_SIZE, MPI_INT, resultMatrix.data(), rows_per_process * MATRIX_SIZE, MPI_INT, 0, MPI_COMM_WORLD);

	double startTime = MPI_Wtime();
	if (rank == 0) {
			cout << "Result Matrix:" << endl;
			for (int i = 0; i < 5; ++i) {
					for (int j = 0; j < 5; ++j) {
							cout << resultMatrix[i * MATRIX_SIZE + j] << " ";
					}
					cout << endl;
			}
			double endTime = MPI_Wtime();
			cout << "Time Taken: " << (endTime - startTime) << " seconds" << endl;
	}

	MPI_Finalize();
	return 0;
}
