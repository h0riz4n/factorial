#include <iostream>
#include <future>

// Функция для вычисления произведения чисел от start до end
long long partialFactorial(int start, int end) {
    long long result = 1;
    for (int i = start; i <= end; i++) {
        result *= i;
    }
    return result;
}

// Основная многопоточная функция факториала
long long factorial(int n) {
    if (n <= 1) return 1; // Базовый случай

    int mid = n / 2; // Делим диапазон на две части

    // Запускаем асинхронный поток для первой половины
    std::future<long long> firstHalf = std::async(std::launch::async, partialFactorial, 1, mid);
    
    // Вычисляем вторую половину в основном потоке
    long long secondHalf = partialFactorial(mid + 1, n);

    // Собираем результат
    return firstHalf.get() * secondHalf;
}

int main() {
    int n;
    std::cout << "Введите число: ";
    std::cin >> n;

    std::cout << "Факториал числа: " << factorial(n) << std::endl;
}
