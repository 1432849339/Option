#ifndef __THREADPOLL_H__
#define __THREADPOLL_H__

#include <thread>
#include <mutex>
#include <chrono>
#include <atomic>
#include <condition_variable>
#include <future>
#include <queue>
#include <algorithm>
#include <stdexcept>
#include <functional>

using namespace std;

namespace ThreadPool {
//线程池最大容量,应尽量设小一点
#define  THREADPOOL_MAX_NUM 1

class threadpool
{
	using Task = function<void()>;
	using m_pool = vector<thread>;
	using m_tasks = queue<Task>;
public:
	inline threadpool(unsigned short size = 4)
	{
		addThread(size);
	}
	inline ~threadpool()
	{
		_run = false;
		_task_cv.notify_all();//唤醒所有线程
		for (thread& thread : _pool)
		{
			if (thread.joinable())
			{
				thread.join();
			}
		}
	}
public:
	// 提交一个任务
	// 调用.get()获取返回值会等待任务执行完,获取返回值
	// 有两种方法可以实现调用类成员，
	// 一种是使用   bind： .commit(std::bind(&Dog::sayHello, &dog));
	// 一种是用   mem_fn： .commit(std::mem_fn(&Dog::sayHello), this)
	template<typename F,typename... ARGS>
	auto commit(F&& f, ARGS&&... args)->future<decltype(f(args...))>
	{
		if (!_run)		//线程池停止了吗?
		{
			throw runtime_error("commit on ThreadPool is stopped.");
		}

		using RetType = decltype(f(args...));//函数 f 的返回值类型
		auto task = make_shared<packaged_task<RetType()>>(
			bind(forward<F>(f), forward<ARGS>(args)...)
			); // 把函数入口及参数,打包(绑定)
		future<RetType> future = task->get_future();
		{
			// 添加任务到队列
			lock_guard<mutex> lock{ _lock };//对当前块的语句加锁  lock_guard 是 mutex 的 stack 封装类，构造的时候 lock()，析构的时候 unlock()
			_tasks.emplace([task]() { // push(Task{...}) 放到队列后面
				(*task)();
			});
		}
#ifdef THREADPOOL_AUTO_GROW
		if (_idlThrNum < 1 && _pool.size() < THREADPOOL_MAX_NUM)
			addThread(1);
#endif // !THREADPOOL_AUTO_GROW
		_task_cv.notify_one(); // 唤醒一个线程执行

		return future;
	}
	//空闲线程数量
	int idlCount() { 
		return _idlThrNum;
	}
	//线程数量
	int thrCount() { 
		return static_cast<int>(_pool.size());
	}
protected:
	//添加指定数量的线程
	void addThread(unsigned short size)
	{
		for (; _pool.size() < THREADPOOL_MAX_NUM && size >= 0; --size)
		{
			_pool.emplace_back([this] {//工作线程函数
				while (_run) {
					Task task;// 获取一个待执行的 task
					{
						// unique_lock 相比 lock_guard 的好处是：可以随时 unlock() 和 lock()
						unique_lock<mutex> lock{ _lock };
						_task_cv.wait(lock, [this] {
							return !_run || !_tasks.empty();
						});//wait直到有task
						if (!_run && _tasks.empty())
							return;
						task = std::move(_tasks.front());// 按先进先出从队列取一个 task
						_tasks.pop();
					}
					_idlThrNum--;
					task();//执行任务
					_idlThrNum++;
				}
			});
		}
	}
private:
	m_pool						_pool;				//线程池
	m_tasks						_tasks;				//任务队列
	mutex						_lock;				//同步
	condition_variable			_task_cv;			//条件阻塞
	atomic<bool>				_run{ true };		//线程池是否执行
	atomic<int>					_idlThrNum{ 0 };	//空闲线程数量
};
}
#endif//!__THREADPOLL_H__
