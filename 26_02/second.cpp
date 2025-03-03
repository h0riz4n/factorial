#include <iostream>
#include <chrono>
#include <cmath>
#include <omp.h>
#include <windows.h>

using namespace std;

int N = 1000000; // Кол-во разбиений

// Последовательное вычисление
double sequential() {
	double a = 0.0, b = 3.1415 / 2;
	double h = (b - a) / N;
	double sum = 0.0;
	
	for (int i = 0; i < N; i++) {
		double x = a + (i + 0.5) * h;
		sum += tan(x);
	}
	return sum * h;
}

// Параллельное вычисление
double parallel() {
	double a = 0.0, b = 3.1415 / 2;
	double h = (b - a) / N;
	double sum = 0.0;

	#pragma omp parallel for reduction(+:sum)
	for (int i = 0; i < N; i++) {
		double x = a + (i + 0.5) * h;
		sum += tan(x);
	}
	return sum * h;
}

// Измерение времени выполнения функции
double measureExecutionTime(double (*func)()) {
	auto start = chrono::high_resolution_clock::now();
	func();
	auto end = chrono::high_resolution_clock::now();
	return chrono::duration<double>(end - start).count();
}

int main() {
	SetConsoleOutputCP(65001);

	omp_set_num_threads(4);

	// Параллельное вычисление
	cout << "Параллельное вычисление: " << measureExecutionTime(parallel) << " секунд" << endl;

	// Последовательное вычисление
	cout << "Последовательное вычисление: " << measureExecutionTime(sequential) << " секунд" << endl;

	return 0;
}
