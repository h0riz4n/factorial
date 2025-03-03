#include <iostream>
#include <vector>
#include <chrono>
#include <omp.h>
#include <cstdlib>
#include <ctime>
#include <windows.h>

using namespace std;

const int arraySize = 100000;

// Заполнение массива
void fillArray(vector<int> &arr) {
	for (auto &el : arr) {
		el = rand() % 1000;
	}
}

// Последовательная сортировка
void oddEvenSortSequential(vector<int> &arr) {
	bool isSorted = false;
	int n = arr.size();
	while (!isSorted) {
		isSorted = true;
		
		for (int i = 1; i < n - 1; i += 2) {
			if (arr[i] > arr[i + 1]) {
				swap(arr[i], arr[i + 1]);
				isSorted = false;
			}
		}

		for (int i = 0; i < n - 1; i += 2) {
			if (arr[i] > arr[i + 1]) {
				swap(arr[i], arr[i + 1]);
				isSorted = false;
			}
		}
	}
}

// Параллельная сортировка
void oddEvenSortParallel(vector<int> &arr) {
	bool isSorted = false;
	int n = arr.size();
	while (!isSorted) {
		isSorted = true;
		
		#pragma omp parallel for shared(arr, isSorted)
		for (int i = 1; i < n - 1; i += 2) {
			if (arr[i] > arr[i + 1]) {
				swap(arr[i], arr[i + 1]);
				#pragma omp critical
				isSorted = false;
			}
		}

		#pragma omp parallel for shared(arr, isSorted)
		for (int i = 0; i < n - 1; i += 2) {
			if (arr[i] > arr[i + 1]) {
				swap(arr[i], arr[i + 1]);
				#pragma omp critical
				isSorted = false;
			}
		}
	}
}

// Измерение времени выполнения функции
double measureExecutionTime(void (*func)(vector<int>&), vector<int> &arr) {
	auto start = chrono::high_resolution_clock::now();
	func(arr);
	auto end = chrono::high_resolution_clock::now();
	return chrono::duration<double>(end - start).count();
}

int main() {
	SetConsoleOutputCP(65001);

	omp_set_num_threads(4);

	vector<int> seqData(arraySize);
	vector<int> parData(arraySize);
	
	fillArray(seqData);
	parData = seqData;

	cout << "Время последовательной сортировки: " << measureExecutionTime(oddEvenSortSequential, seqData) << " секунд" << endl;
	
	cout << "Время параллельной сортировки: " << measureExecutionTime(oddEvenSortParallel, parData) << " секунд" << endl;

	return 0;
}
