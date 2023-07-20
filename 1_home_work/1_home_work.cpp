#include <iostream>
#include <thread>
#include <Windows.h>
#include <chrono>
#include <mutex>
#include <condition_variable>

std::mutex m;
std::condition_variable cv;
bool StopFlag = false;
std::atomic<int> clientCount = 0;  // Счетчик клиентов
std::atomic<int> maxClients = 10;  // Максимальное количество клиентов

// Функция, имитирующая поток клиента
void clientThread()
{
    while (!StopFlag)
    {
        std::this_thread::sleep_for(std::chrono::seconds(1));

        if (clientCount < maxClients)
        {
            std::unique_lock<std::mutex> lock(m);
            ++clientCount;
            std::cout << "Пришел клиент. Всего клиентов: " << clientCount << std::endl;
            cv.notify_all();
        }
    }
}

// Функция, имитирующая поток операциониста
void operatorThread()
{
    while (!StopFlag)
    {
        std::this_thread::sleep_for(std::chrono::seconds(2));
        
        std::unique_lock<std::mutex> lock(m);
        cv.wait(lock, [] {return clientCount > 0 || StopFlag; });

        if (StopFlag)
            break;

         --clientCount;
         std::cout << "Обслужен клиент. Осталось клиентов: " << clientCount << std::endl;
            

        
    }
}

int main()
{
    setlocale(LC_ALL, "Russian");
    std::thread client(clientThread);
    std::thread oper(operatorThread);


  
   // std::this_thread::sleep_for(std::chrono::seconds(11));
   // StopFlag = true;
    client.join();
    oper.join();

    return 0;
}