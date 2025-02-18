#include <iostream>
#include <thread>
#include <vector>
#include <mutex>

std::mutex mtx;
long long result = 1;

void factorialPart(int start, int end) {
    long long partResult = 1;
    for (int i = start; i <= end; ++i) {
        partResult *= i;
    }
    
    // Используем мьютекс для безопасного обновления общего результата
    std::lock_guard<std::mutex> lock(mtx);
    result *= partResult;
}

int main() {
    int n, numThreads;
    std::cout << "Введите число для вычисления факториала: ";
    std::cin >> n;
    std::cout << "Введите количество потоков: ";
    std::cin >> numThreads;

    std::vector<std::thread> threads;
    int chunkSize = n / numThreads;
    int remainder = n % numThreads;

    int start = 1;
    for (int i = 0; i < numThreads; ++i) {
        int end = start + chunkSize - 1 + (i < remainder ? 1 : 0);
        threads.emplace_back(factorialPart, start, end);
        start = end + 1;
    }

    // Ждем завершения всех потоков
    for (auto& t : threads) {
        t.join();
    }

    std::cout << "Факториал " << n << " равен " << result << std::endl;

    return 0;
}
