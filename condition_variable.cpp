// condition_variable.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <mutex>
#include <condition_variable>
#include <iostream>
#include <thread>

class Restaurant {
	enum class Status { idle, newOrder, ready };
	Status orderStatus = Status::idle;
	std::mutex order;
	std::condition_variable orderBell;

public:

	void chef() {
		std::unique_lock<std::mutex> ul(order);
		orderBell.wait(ul, [=]() { return orderStatus == Status::newOrder; });
		//приготовление блюд из заказа
		orderStatus = Status::ready;
		orderBell.notify_one();
	}

	void waiter() {
		{
			std::lock_guard<std::mutex> lg(order);
			orderStatus = Status::newOrder;
			orderBell.notify_one();
		} // lg вне области видимости = order.unlock()

		std::unique_lock<std::mutex> ul(order);
		orderBell.wait(ul, [=]() { return orderStatus == Status::ready; });
		orderStatus = Status::idle;
		ul.unlock();
		//приносят заказ
	}

};

int main() {
	Restaurant restaurant;
	//std::thread chef(&Restaurant::chef, std::ref(restaurant));
	//std::thread waiter(&Restaurant::waiter, std::ref(restaurant));
	std::thread chef(&Restaurant::chef, &restaurant);
	std::thread waiter(&Restaurant::waiter, &restaurant);
	chef.join();
	waiter.join();

	system("pause");
	return 0;
}