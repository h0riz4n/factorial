#include <mpi.h>
#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>

using namespace std;

constexpr int TOTAL_ELEMENTS = 1000000;


int main(int argc, char** argv) {
	MPI_Init(&argc, &argv);
	
	int processID, totalProcesses;
	MPI_Comm_rank(MPI_COMM_WORLD, &processID);
	MPI_Comm_size(MPI_COMM_WORLD, &totalProcesses);
	
	vector<int> mainArray;
	vector<int> subArray(chunkSize);

	int chunkSize = TOTAL_ELEMENTS / totalProcesses;
	
	if (processID == 0) {
		mainArray.resize(TOTAL_ELEMENTS);
		srand(static_cast<unsigned>(time(nullptr)));
		for (int i = 0; i < TOTAL_ELEMENTS; ++i) mainArray[i] = rand() % 100;
	}
	
	double beginTime = MPI_Wtime();
	MPI_Scatter(mainArray.data(), chunkSize, MPI_INT, subArray.data(), chunkSize, MPI_INT, 0, MPI_COMM_WORLD);
	
	int partialSum = 0;
	for (int value : subArray) partialSum += value;
	
	int accumulatedSum = 0;
	MPI_Reduce(&partialSum, &accumulatedSum, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
	
	double finishTime = MPI_Wtime();
	
	if (processID == 0) {
		cout << "Total Sum: " << accumulatedSum << endl;
		cout << "Time Taken: " << (finishTime - beginTime) << " seconds" << endl;
	}
	
	MPI_Finalize();
	return 0;
}
