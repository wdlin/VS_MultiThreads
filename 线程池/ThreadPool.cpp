#include "ThreadPool.h"
#include <iostream>
#include <thread>
#include <cassert>
WorkThread::WorkThread(ThreadPool* pool) :thread_pool(pool), task(nullptr), isEnd(false)
{
	/*
	thread t(&WorkThread::run, this);
	t.detach();
	本来是这样写的，但是为了在线程池结束正确回收资源，所以不调用detach。
	detach后运行的线程运行结束自行回收资源。但是本线程池，WorkThread是new方式产生，
	所以如果delete时，线程还没有结束，出错。
	*/
	myThread = thread(&WorkThread::run, this);
}

WorkThread::~WorkThread()
{
}

void WorkThread::run()
{
	while (!isEnd)
	{
		unique_lock<mutex> lock(mut);
		cv.wait(lock);//等待唤醒
		if (task != nullptr)//如果是有任务唤醒执行，如果无任务唤醒不执行（为了结束线程）
		{
			//cout << "Thread:" << this_thread::get_id() << endl;
			task->run();
			task = nullptr;
			thread_pool->addIdleThread(this);
		}
	}	
}

void WorkThread::notify()//唤醒自己
{
	cv.notify_one();
}

void WorkThread::finish()//结束本线程
{
	isEnd = true;
	cv.notify_one();
}
void WorkThread::assign(shared_ptr<Task> atask)//指派任务给线程
{
	task = atask;
}

ThreadPool::ThreadPool(int num) : thread_number(num), isEnd(false)//线程池构造函数
{
	assert(num > 0);
	all_threads.reserve(num);//向量预留一定大小
	for (int i = 0; i < num; i++)//新建一定数量线程
	{
		WorkThread* wt = new WorkThread(this);
		all_threads.push_back(wt);//加入所有线程队列中
		idle_threads.push_back(wt);//加入空闲线程队列中
	}
	pool_thread = thread(&ThreadPool::run,this);//运行线程池主线程，用于接收分配任务
}

ThreadPool::~ThreadPool()//析构函数
{
	for (auto t : all_threads)
	{
		t->myThread.join();//等待所有子线程结束
	}
	for (auto t : all_threads)
	{
		delete t;//删除指针
	}
}

void ThreadPool::run()
{
	while (!isEnd)
	{
		unique_lock<mutex> lock1(task_mutex);
		if (task_queue.empty())//如果任务队列为空，阻塞，等待唤醒
		{
			cv_task.wait(lock1);
		}
		if (isEnd)break;//判断结束标志
		shared_ptr<Task> task = task_queue.top();
		task_queue.pop();
		lock1.unlock();

		unique_lock<mutex> lock2(task_mutex);
		if (idle_threads.empty())//如果空闲线程队列为空，阻塞，等待唤醒
		{
			cv_idle.wait(lock2);
		}
		if (isEnd)break;//判断结束标志
		WorkThread* wt = idle_threads.front();
		idle_threads.pop_front();
		lock2.unlock();

		wt->assign(task);//指派任务
		wt->notify();//唤醒指派了任务的线程
	}
}

void ThreadPool::finish()//结束线程池
{
	isEnd = true;//设置结束标志
	cv_task.notify_one();//唤醒主线程
	cv_idle.notify_one();//唤醒主线程
	for (auto t : all_threads)//结束所有子线程
	{
		t->finish();
	}
	pool_thread.join();//等待主线程结束，如果不等待结束，那么有可能调用finnish后，ThreadPool对象被析构，导致线程没结束就被析构
}

void ThreadPool::addTask(shared_ptr<Task> task)//添加任务
{
	unique_lock<mutex> lock(task_mutex);
	task_queue.push(task);
	cout <<"thread:"<<this_thread::get_id() << "  addTask: priority:" << task->getPriority() << ",data:" << dynamic_cast<MyTask*>(task.get())->getData() << endl;
	lock.unlock();
	cv_task.notify_one();//添加后，任务队列不为空，所以唤醒主线程处理
}

void ThreadPool::addIdleThread(WorkThread* idleThread)//添加到空闲的线程队列
{
	unique_lock<mutex> lock(idle_mutex);
	idle_threads.push_back(idleThread);
	lock.unlock();
	cv_idle.notify_one();//添加后，空闲线程队列不为空，所以唤醒主线程处理
}
//shared_ptr<Task>比较函数
bool operator<(const shared_ptr<Task>& task1, const shared_ptr<Task>& task2)
{
	return task1->getPriority() < task2->getPriority();
}