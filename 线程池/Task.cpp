#include "Task.h"
#include <iostream>
#include <thread>
#include <chrono>
#include <memory>
using namespace std;

Task::Task() :_priority(5)//构造函数
{}

Task::Task(int priority) : _priority(priority)//带优先级参数的构造函数
{}

Task::~Task()//析构函数
{}

void Task::setPriority(int priority)//设置任务优先级
{
	_priority = priority;
}

int Task::getPriority()const//获取任务优先级
{
	return _priority;
}

MyTask::MyTask() :_data(0)//构造函数
{}

MyTask::MyTask(int data) : _data(data)//带数据参数的构造函数
{}

MyTask::MyTask(int data, int priority) : Task(priority), _data(data)//带数据和优先级参数的构造函数
{}

MyTask::~MyTask()//析构函数
{}

void MyTask::setData(int data)//设置数据
{
	_data = data;
}

int MyTask::getData()//获取数据
{
	return _data;
}

void MyTask::run()//任务运行函数
{
	this_thread::sleep_for(chrono::milliseconds(200));//休眠0.2s表示运行时间
	cout << "thread :" << this_thread::get_id() << " deal with data :" << _data << " priority: " << getPriority()<<endl;//输出处理信息
}
