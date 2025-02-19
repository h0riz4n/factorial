#include <iostream>
#include <thread>
#include <vector>
#include <mutex>
#include <cmath>

std::mutex mtx;

// Функция для вычисления минора матрицы
double minor(const std::vector<std::vector<double>>& matrix, int i, int j) {
    std::vector<std::vector<double>> minor_matrix(matrix.size() - 1, std::vector<double>(matrix.size() - 1));
    int row = 0;
    for (int x = 0; x < matrix.size(); ++x) {
        if (x != i) {
            int col = 0;
            for (int y = 0; y < matrix.size(); ++y) {
                if (y != j) {
                    minor_matrix[row][col] = matrix[x][y];
                    ++col;
                }
            }
            ++row;
        }
    }
    return determinant(minor_matrix);
}

// Рекурсивная функция для вычисления определителя
double determinant(const std::vector<std::vector<double>>& matrix) {
    if (matrix.size() == 1) return matrix[0][0];
    if (matrix.size() == 2) return matrix[0][0] * matrix[1][1] - matrix[0][1] * matrix[1][0];

    double det = 0;
    for (int j = 0; j < matrix.size(); ++j) {
        det += (j % 2 == 0 ? 1 : -1) * matrix[0][j] * minor(matrix, 0, j);
    }
    return det;
}

// Функция для многопоточного вычисления детерминанта
void threadDeterminant(const std::vector<std::vector<double>>& matrix, int start, int end, double& result) {
    double local_result = 0;
    for (int i = start; i < end; ++i) {
        local_result += (i % 2 == 0 ? 1 : -1) * matrix[0][i] * minor(matrix, 0, i);
    }
    std::lock_guard<std::mutex> lock(mtx);
    result += local_result;
}

int main() {
    const int size = 4;
    std::vector<std::vector<double>> matrix = {
        {1, 2, 3, 4},
        {5, 6, 7, 8},
        {9, 10, 11, 12},
        {13, 14, 15, 16}
    };

    // Для простоты используем 2 потока
    std::vector<std::thread> threads;
    double result = 0;

    int chunk_size = size / 2;
    threads.push_back(std::thread(threadDeterminant, std::ref(matrix), 0, chunk_size, std::ref(result)));
    threads.push_back(std::thread(threadDeterminant, std::ref(matrix), chunk_size, size, std::ref(result)));

    for (auto& t : threads) {
        t.join();
    }

    std::cout << "Determinant of matrix is: " << result << std::endl;
    return 0;
}
