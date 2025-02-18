#include <iostream>
#include <vector>
#include <thread>
#include <mutex>
#include <future>
#include <chrono>
#include <cstdlib>

std::mutex resultMutex;
long long result = 1;

// Function to calculate part of the factorial
void factorialSegment(int start, int end, std::promise<long long>&& pr) {
    long long segmentResult = 1;
    for (int i = start; i <= end; ++i) {
        segmentResult *= i;
    }
    
    std::lock_guard<std::mutex> lock(resultMutex);
    result *= segmentResult;
    pr.set_value(segmentResult);
}

int main() {
    system("chcp 65001"); // Set UTF-8 output for Windows console

    int number;
    int threadsCount;

    std::cout << "Введите число для вычисления факториала: ";
    std::cin >> number;

    std::cout << "Введите количество потоков: ";
    std::cin >> threadsCount;

    if (number < 1 || threadsCount < 1) {
        std::cout << "Неверный формат данных\n";
        return 1;
    }

    // Determine how to divide work among threads
    std::vector<std::thread> threads;
    std::vector<std::future<long long>> futures;
    int chunkSize = number / threadsCount;
    int start = 1;

    auto startTime = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < threadsCount; ++i) {
        int end = (i == threadsCount - 1) ? number : start + chunkSize - 1;
        std::promise<long long> p;
        futures.push_back(p.get_future());
        
        threads.emplace_back(factorialSegment, start, end, std::move(p));
        start = end + 1;
    }

    // Wait for all threads to complete
    for (auto& thread : threads) {
        thread.join();
    }

    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(endTime - startTime);

    std::cout << "Факториал числа " << number << " равен " << result << std::endl;
    std::cout << "Время выполнения: " << duration.count() << " нс." << std::endl;

    return 0;
}
