
#include <iostream>
#include <thread>
#include <Windows.h>
#include <mutex>
#include <chrono>
#include <vector>

std::mutex m;
// Функция подсчет
int Calc(const std::vector<int>& data, int start, int end ) {
	int result = 0;
	for (int i = start; i < end; i++) {
		std::this_thread::sleep_for(std::chrono::milliseconds(100)); 
		result += data[i];
	}
	return result;
}
 // Функция для работы с потоками
void Work(const std::vector<int>& data, int num, int count, int& Sum){
	int size = static_cast<int>(data.size());
	int range = size / count;
	int start = num * range;
	int end = (num == count - 1) ? size : range * (num + 1);
	const char pbar = '|';

	// Рассчет времени

	auto startTime = std::chrono::high_resolution_clock::now();
	int partSum = Calc(data, start, end);
	auto endTime = std::chrono::high_resolution_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::milliseconds> (endTime - startTime);


	// Сумма в текущем потоке
	std::unique_lock<std::mutex> lock(m);
	
	Sum += partSum;
	// Инфо
	
	std::cout << "Номер потока: " << num + 1 << std::endl;
	std::cout << "Идентификатор потока: " << std::this_thread::get_id() << std::endl;

	// вывод прогресс-бара
	std::cout << "Прогресс: [";
	for (int i = 0; i < 50; ++i) {
		if (i < static_cast<int>((static_cast<double>(end) / size) * 50)) {
			std::cout << pbar;
		}
		else {
			std::cout << " ";
		}
	}
	std::cout << " ] Завершено. " << std::endl;
	std::cout << "Время: " << static_cast <double>(duration.count() / 100000.0) << std::endl << std::endl;
	
}


int main()
{
	setlocale(LC_ALL, "Russian");
	std::vector<std::thread> threads;
	std::vector<int> data{};
	for (int i = 0; i < 50; i++) {
		data.push_back(i);
	}
	int count = 4;
	int Sum = 0;

	for (int i = 0; i < count; ++i) {
		threads.emplace_back(Work,std::ref(data), i, count, std::ref(Sum));
	}
	for (auto& t : threads) {
		t.join();
	}
}
