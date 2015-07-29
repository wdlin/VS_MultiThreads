#include <iostream>
#include "Task.h"
#include "ThreadPool.h"
#include <thread>
#include <chrono>
#include <memory>
using namespace std;

int main()
{
	cout<<this_thread::get_id()<<endl;
	ThreadPool threadPool(4);

	for (int i = 0; i < 30; i++)
	{
		this_thread::sleep_for(chrono::milliseconds(20));
		threadPool.addTask(shared_ptr<Task>(new MyTask(i, rand() % 10)));
	}

	this_thread::sleep_for(chrono::milliseconds(3000));

	for (int i = 0; i < 30; i++)
	{
		this_thread::sleep_for(chrono::milliseconds(20));
		threadPool.addTask(shared_ptr<Task>(new MyTask(i, rand() % 10)));
	}

	this_thread::sleep_for(chrono::milliseconds(1000));
	
	threadPool.finish();

	system("pause");
}