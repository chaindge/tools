#ifndef SEMAPHORE_H
#define SEMAPHORE_H
#include <mutex>
#include <condition_variable>
#include <list>
#include <queue>

class Semaphore {
public:
	Semaphore(long count = 0)
		: count_(count) {
	}

	void Signal() {
		std::unique_lock<std::mutex> lock(mutex_);
		++count_;
		cv_.notify_one();
	}

	void Wait() {
		std::unique_lock<std::mutex> lock(mutex_);
		cv_.wait(lock, [=] { return count_ > 0; });
		--count_;
	}

private:
	std::mutex mutex_;
	std::condition_variable_any cv_;
	long count_;
};

template<class T>
class mtp_vector
{
public:
	void push(const T& data) {
		std::unique_lock<std::mutex> lock(mutex_);
		_queue.push(data);
	}

	bool fetch(T& data) {
		std::unique_lock<std::mutex> lock(mutex_);
		if (!_queue.empty()) {
			data = _queue.front();
			_queue.pop();
			return true;
		}
		return false;
	}

	bool empty() {
		std::unique_lock<std::mutex> lock(mutex_);
		return _queue.empty();
	}

private:
	std::queue<T> _queue;
	std::mutex mutex_;
};

#define mtp_lock(_mtp_container) std::unique_lock<std::mutex> lock((_mtp_container)._mutex)

template<class T>
class mtp_list
{
public:
	void push_back(const T& elem) {
		mtp_lock(*this);
		_list.push_back(elem);
	}

	void remove(const T& elem) {
		std::unique_lock<std::mutex> lock(_mutex);
		_list.remove(elem);
	}

	template <class Pred>
	bool erase_if(Pred _Pred) {
		mtp_lock(*this);
		auto iter = std::find_if(_list.begin(), _list.end(), _Pred);
		if (iter != _list.end()) {
			_list.erase(iter);
			return  true;
		}
		return false;
	}

	template <class Pred>
	bool find_if(Pred _Pred, T& value) {
		mtp_lock(*this);
		auto iter = std::find_if(_list.begin(), _list.end(), _Pred);
		if (iter != _list.end()) {
			value = *iter;
			return true;
		}
		return false;
	}

public:
	std::mutex _mutex;
	std::list<T> _list;
};



#endif // !SEMAPHORE_H