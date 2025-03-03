#include <iostream>
#include <vector>
#include <chrono>
#include <omp.h>
#include <cstdlib>
#include <ctime>
#include <windows.h>

using namespace std;

const size_t arraySize = 10000000;

// Параллельное вычисление суммы
long long parSum(vector<int>& array) {
	long long sum = 0;
	#pragma omp parallel for reduction(+:sum)
	for (int i = 0; i < array.size(); ++i) {
		sum += array[i];
	}
	return sum;
}

// Последовательное вычисление суммы
long long seqSum(vector<int>& array) {
	long long sum = 0;
	for (int i = 0; i < array.size(); ++i) {
		sum += array[i];
	}

	return sum;
}

// Измерение времени выполнения функции
double measureExecutionTime(long long (*func)(vector<int>&), vector<int> &array) {
	auto start = chrono::high_resolution_clock::now();
	func(array);
	auto end = chrono::high_resolution_clock::now();
	return chrono::duration<double>(end - start).count();
}

int main() {
	SetConsoleOutputCP(65001);

	vector<int> array(arraySize);

	// Заполнение массива случайными числами
	for (auto &el : array) {
		el = rand() % 1000;
	}

	omp_set_num_threads(4);

	// Параллельное вычисление
	cout << "Параллельное вычисление: " << measureExecutionTime(parSum, array) << "s" << endl;

	// Последовательное вычисление
	cout << "Последовательное вычисление: " << measureExecutionTime(seqSum, array) << "s" << endl;

	return 0;
}
