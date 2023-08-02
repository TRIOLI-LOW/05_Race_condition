#include <iostream>
#include <thread>
#include <mutex>
#include <vector>
#include <chrono>
#include <cstdlib>
#include <condition_variable>

std::mutex m;
std::condition_variable cv;
bool ready = false;

// Функция подсчет
int Calc(const std::vector<int>& data, int start, int end) {
    int result = 0;
    for (int i = start; i < end; i++) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100 + (std::rand() % 700)));
        result += data[i];
    }
    return result;
}

// Функция для работы с потоками
void Work(const std::vector<int>& data, int num, int count, int& Sum) {
    int size = static_cast<int>(data.size());
    int range = size / count;
    int start = num * range;
    int end = (num == count - 1) ? size : range * (num + 1);

    int progress = 0;
    const int maxProgress = 10;

    // Выводим заголовок прогресс-бара
    std::unique_lock<std::mutex> lock(m);
    std::cout << num << "\t";
    std::cout << std::this_thread::get_id() << "\t";
    std::cout << "|";
    std::cout << std::string(maxProgress, ' ');
    std::cout << "\n";
    lock.unlock();

    auto startTime = std::chrono::high_resolution_clock::now();

    // Ожидаем, пока все потоки будут готовы
    std::unique_lock<std::mutex> readyLock(m);
    while (!ready) {
        cv.wait(readyLock);
    }
    readyLock.unlock();

    for (int i = start; i < end; i++) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100 + (std::rand() % 700)));
        progress = (i - start + 1) * maxProgress / (end - start);

        // Обновляем прогресс-бар
        lock.lock();
        std::cout << "\033[" << num + 2 << ";0H";
        std::cout << num << "\t";
        std::cout << std::this_thread::get_id() << "\t";
        std::cout << "|";
        std::cout << std::string(progress, '|');
        std::cout << std::string(maxProgress - progress, ' ');
        std::cout << "\n";
        lock.unlock();
    }

    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);

    // Обновляем прогресс для данного потока
    progress = maxProgress;
    Sum += Calc(data, start, end);

    //Выводим завершение работы потока
    lock.lock();
    std::cout << "\033[" << num + 2 << ";0H";
    std::cout << num << "\t";
    std::cout << std::this_thread::get_id() << "\t";
    std::cout << "|";
    std::cout << std::string(progress, '|');
    std::cout << " - Завершено. ";
    std::cout << "Время: " << static_cast<double>(duration.count() / 1000.0) << "\n";
    lock.unlock();
}

int main() {
    setlocale(LC_ALL, "Russian");
    std::vector<std::thread> threads;
    std::vector<int> data{};

    for (int i = 0; i < 50; i++) {
        data.push_back(i);
    }

    int count = 4;
    int Sum = 0;
    std::cout << "#";
    std::cout << "\tid\t";
    std::cout << "Прогресс" << std::endl;

    for (int i = 0; i < count; ++i) {
        threads.emplace_back(Work, std::ref(data), i, count, std::ref(Sum));
    }

    // Уведомляем о готовности всех потоков
    {
        std::unique_lock<std::mutex> lock(m);
        ready = true;
        lock.unlock();
        cv.notify_all();
    }

    
    for (auto& t : threads) {
        t.join();
    }

}