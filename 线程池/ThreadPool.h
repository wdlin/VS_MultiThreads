#ifndef _THREAD_POOL_H_
#define _THREAD_POOL_H_
#include <iostream>
#include <mutex>
#include <vector>
#include <list>
#include <queue>
#include <memory>
#include <condition_variable>
#include <thread>
#include "Task.h"
using namespace std;
class ThreadPool;

//工作线程，在线程池中有若干工作线程
class WorkThread
{
public:
	WorkThread(ThreadPool* pool);//传入线程池指针的构造函数
	~WorkThread();//析构函数
	void run();//本线程运行的函数
	void notify();//唤醒本线程的函数
	void finish();//结束本线程的函数
	void assign(shared_ptr<Task> atask);//指派线程的任务
	thread myThread;//本线程的对象
private:
	ThreadPool* thread_pool;//线程池
	shared_ptr<Task> task;//任务
	bool isEnd;//结束线程标志
	mutex mut;//互斥
	condition_variable cv;//条件变量
};

class cmp//用于比较两个Task智能指针优先级 方法1
{
public:
	bool operator()(const shared_ptr<Task>& task1, const shared_ptr<Task>& task2)
	{
		return task1->getPriority() > task2->getPriority();
	}
};

//用于比较两个Task智能指针优先级 方法2
bool operator<(const shared_ptr<Task>& task1, const shared_ptr<Task>& task2);

//线程池
class ThreadPool
{
public:
	ThreadPool(int num = 5);//构造函数,输入线程数量
	~ThreadPool();//析构函数，清理资源
	void run();//线程池主线程运行的函数
	void finish();//结束线程池的函数
	void addTask(shared_ptr<Task> task);//添加任务
	void addIdleThread(WorkThread* idleThread);//将空闲线程添加到空闲队列
private:
	int thread_number;//线程的数目
	bool isEnd;//线程池结束标记
	thread pool_thread;//线程池主线程
	mutex task_mutex;//任务队列的互斥量
	mutex idle_mutex;//空闲线程队列的互斥量
	condition_variable cv_task;//任务队列的条件变量
	condition_variable cv_idle;//空闲线程队列的条件变量
	vector<WorkThread*> all_threads;//池中所有线程
	list<WorkThread*> idle_threads;//空闲的线程
	priority_queue<shared_ptr<Task>, vector<shared_ptr<Task>>> task_queue;//任务队列，任务具有优先级
	//priority_queue<shared_ptr<Task>, vector<shared_ptr<Task>>,cmp> task_queue;//任务优先队列另一种方式
};

#endif