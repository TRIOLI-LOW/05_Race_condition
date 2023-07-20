#include <iostream>
#include <mutex>
#include <thread>


template <typename T>
class Data {
private: 
	T scalarData;
public: 

	Data(T scalarData_) : scalarData(scalarData_) {};

	T getData() const {
		return scalarData;
	}
	void setData(T newData) {
		scalarData = newData;
	}

	std::mutex m;
};


template <typename T>
void swap_lock(Data<T>& data1, Data<T>& data2) {
	std::lock(data1.m, data2.m);
	std::lock_guard<std::mutex> lock1(data1.m, std::adopt_lock);
	std::lock_guard<std::mutex> lock2(data2.m, std::adopt_lock);

	T temp = data1.getData();
	data1.setData(data2.getData());
	data2.setData(temp);
 }
template <typename T>
void swap_scoped(Data<T>& data1, Data<T>& data2) {
	std::scoped_lock lock(data1.m, data2.m);

	T temp = data1.getData();
	data1.setData(data2.getData());
	data2.setData(temp);
}

template <typename T>
void swap_unique(Data<T>& data1, Data<T>& data2) {
	std::unique_lock<std::mutex> lock1(data1.m, std::defer_lock);
	std::unique_lock<std::mutex> lock2(data2.m, std::defer_lock);
	std::lock(lock1, lock2);


	T temp = data1.getData();
	data1.setData(data2.getData());
	data2.setData(temp);
}



	 int main() {
	 Data<int> data1(15);
	 Data<int> data2(54);

	 std::cout << data1.getData() << std::endl;
	 std::cout << data2.getData() << std::endl;
	 std::cout << std::endl;

	 swap_lock(data1, data2);

	 std::cout << data1.getData() << std::endl;
	 std::cout << data2.getData() << std::endl;
	 std::cout << std::endl;

	 swap_scoped(data1, data2);

	 std::cout << data1.getData() << std::endl;
	 std::cout << data2.getData() << std::endl;
	 std::cout << std::endl;

	 swap_unique(data1, data2);

	 std::cout << data1.getData() << std::endl;
	 std::cout << data2.getData() << std::endl;
 }