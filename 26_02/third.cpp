#include <iostream>
#include <chrono>
#include <cmath>
#include <omp.h>
#include <vector>
#include <windows.h>

using namespace std;
using Matrix = vector<vector<int>>;

int N = 10000; // Размерность матрицы и ветора

// Параллельные вычисления
void parallel(const Matrix &matrix, vector<int> &arr) {
	size_t rows = matrix.size();
	size_t cols = matrix[0].size();
	vector<int> result(rows, 0);

	#pragma omp parallel for
	for (size_t i = 0; i < rows; i++) {
		for (size_t j = 0; j < cols; j++) {
			result[i] += matrix[i][j] * arr[j];
		}
	}
}

// Последовательные вычисления
void sequential(const Matrix &matrix, vector<int> &arr) {
	size_t rows = matrix.size();
	size_t cols = matrix[0].size();
	vector<int> result(rows, 0);

	for (size_t i = 0; i < rows; i++) {
		for (size_t j = 0; j < cols; j++) {
			result[i] += matrix[i][j] * arr[j];
		}
	}
}

// Измерение времени выполнения функции
double measureExecutionTime(void (*func)(const Matrix&, vector<int>&), Matrix &matrix, vector<int> &arr) {
	auto start = chrono::high_resolution_clock::now();
	func(matrix, arr);
	auto end = chrono::high_resolution_clock::now();
	return chrono::duration<double>(end - start).count();
}

int main() {
	SetConsoleOutputCP(65001);

	omp_set_num_threads(4);

	// Заполнение матрицы
	Matrix A(N, vector<int>(N));
	for (auto &row : A) {
		for (auto &element : row) {
			element = rand() % 100;
		}
	}

	// Заполнение массива
	vector<int> arr(N);
	for (auto &el : arr) {
		el = rand() % 100;
	}

	// Параллельное вычисление
	cout << "Параллельное вычисление: " << measureExecutionTime(parallel, A, arr) << " секунд" << endl;

	// Последовательное вычисление
	cout << "Последовательное вычисление: " << measureExecutionTime(sequential, A, arr) << " секунд" << endl;

	return 0;
}
