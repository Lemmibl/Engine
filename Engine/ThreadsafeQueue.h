#pragma once
#include <queue>
#include <tinythread.h>
#include <fast_mutex.h>

using namespace tthread;

//This class is largely based on this excellent article: http://www.justsoftwaresolutions.co.uk/threading/implementing-a-thread-safe-queue-using-condition-variables.html
//So props to them!

template<typename T>
class ThreadsafeQueue
{
public:
	void Shutdown()
	{
		lock_guard<fast_mutex> guard(mutexVariable);
		conditionVariable.notify_all();
	}

	void Push(T const& data)
	{
		lock_guard<fast_mutex> guard(mutexVariable);
		queue.push(data);
		conditionVariable.notify_one();
	}

	bool Empty() const
	{
		lock_guard<fast_mutex> guard(mutexVariable);
		return queue.empty();
	}

	bool Try_pop(T& popped_value)
	{
		lock_guard<fast_mutex> guard(mutexVariable);
		if(queue.empty())
		{
			return false;
		}

		popped_value = queue.front();
		queue.pop();

		return true;
	}

	void Wait_and_pop(T& popped_value)
	{
		mutexVariable.lock();

		while(queue.empty())
		{
			conditionVariable.wait(mutexVariable);
		}

		popped_value = queue.front();
		queue.pop();

		//Mutex should be unlocked before value is accessed, hence I can't use a lock_guard because it has no unlock feature.
		mutexVariable.unlock();
	}

	//Unfortunately, std::queue doesn't have a built-in clear function, so this will have to do.
	void Clear()
	{
			lock_guard<fast_mutex> guard(mutexVariable);
			std::queue<T> empty;
			std::swap( queue, empty );
	}

private:
	condition_variable conditionVariable;
	mutable fast_mutex mutexVariable;
	std::queue<T> queue;
};