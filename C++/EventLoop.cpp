#include "EventLoop.h"
#include <thread>
#include <condition_variable>
#include <list>
#include <vector>
#include <tuple>

class tag_EventHandlerID
{
public:
	std::string event;
	EventHandlerCB func;
};

template<class fnType>
	class Scheduler
{
public:
	virtual ~Scheduler() {}

	// shedule a function call 
	virtual void schedule(fnType fn) = 0;

	virtual bool empty() = 0;
};


template<class fnType> 
	class EventLoop
		: public Scheduler<fnType>
{
public:
	virtual ~EventLoop() {}

	// loop forever
	virtual void loop() = 0;
};


class BatchCaller
{
public:
	virtual ~BatchCaller() {}

	// call some function available
	virtual void batch_call() = 0;
};


template <class fnType>	class FnRunner {};

// single function
template <>
	class FnRunner<std::function<void()>>
{
public:
	inline bool condition(const std::function<void()>& fn) { return true; }
	inline void run(const std::function<void()>& fn) { fn(); }
};

// function with condition
typedef std::tuple<std::function<bool()>, std::function<void()>> ConditionFuntion;

template <>
	class FnRunner<ConditionFuntion>
	{
	public:
		inline bool condition(const ConditionFuntion& fn) { return std::get<0>(fn)(); }

		inline void run(const ConditionFuntion& fn) { std::get<1>(fn)(); }
	};

template <class fnType, 
	class fnRunner = FnRunner<fnType>>

	class EventLoopImpl
		: public EventLoop<fnType>
		, public BatchCaller
	{
	public:
		typedef fnType						container_elem;
		typedef std::list<container_elem>	container;
		typedef std::mutex					container_mutex;
		typedef std::condition_variable_any	container_cv;
		typedef std::unique_lock<container_mutex> container_lock;

		virtual void schedule(fnType fn)override
		{
			container_lock lock(_container_mutex);
			_container.emplace_back(std::move(fn));
			_container_cv.notify_one();
		}

		virtual void loop()override
		{
			while (1)
			{
				container_elem fun_to_run;
				{
					container_lock lock(_container_mutex);
					while (_container.empty())
					{
						_container_cv.wait(lock);
					}

					fun_to_run = _container.front();
					_container.pop_front();
				}

				if (_fn_runner.condition(fun_to_run))
					_fn_runner.run(fun_to_run);
				else 
				{
					schedule(fun_to_run);
				}
				//safe_run(fun_to_run);
			}
		}

		virtual void batch_call()override
		{
			container container_copy;
			{
				container_lock lock(_container_mutex);
				container_copy.swap(_container);
			}

			for (auto & fn : container_copy)
			{
				//safe_run(fn);

				if (_fn_runner.condition(fn))
					_fn_runner.run(fn);
				else
				{
					schedule(fn);
				}
			}
		}

		virtual bool empty()override
		{
			container_lock lock(_container_mutex);
			return _container.empty();
		}

	private:
		container		_container;
		container_cv	_container_cv;
		container_mutex _container_mutex;
		fnRunner		_fn_runner;
	};


template <class fnType>
	class ThreadDispatchContext
		: public Scheduler<fnType>
{
public:
	virtual void schedule(fnType fn)override {
		if (_event_loop)
			_event_loop->schedule(std::move(fn));
	}

	virtual bool empty()override {
		return _event_loop ? _event_loop->empty() : false;
	}
public:
	ThreadDispatchContext(EventLoop<fnType>* event_loop)
	{
		_event_loop = event_loop;
		_work_thread = std::thread(std::bind(&ThreadDispatchContext::work_proc, this));
	}

protected:
	void work_proc()
	{
		if (_event_loop)
			_event_loop->loop();
	}

private:
	EventLoop<fnType>* _event_loop;
	std::thread _work_thread;

};

	

static bool always_true() { return true; }

void run_on_background(std::function<void()> fn)
{
	typedef std::function<void()> fnType;

	static EventLoopImpl<ConditionFuntion> loop;
	static Scheduler<ConditionFuntion>* scheduler = new ThreadDispatchContext<ConditionFuntion>(&loop);

	scheduler->schedule({ always_true, std::move(fn) });
}


namespace EventCentral {
	static std::multimap<std::string, EventHandlerID> event_map;
}

EventHandlerID register_event(const std::string& event, EventHandlerCB cb)
{
	EventHandlerID handler = std::make_shared<tag_EventHandlerID>();
	handler->func = std::move(cb);
	handler->event = event;

	EventCentral::event_map.insert(std::make_pair(event, handler));

	return handler;
}

void wait_event(const std::string& event, EventHandlerCB cb)
{
	std::mutex mtx;
	std::unique_lock<std::mutex> lck(mtx);
	std::condition_variable_any cnd;
	
	std::map<std::string, std::string> params;

	auto id = register_event(event, 
		[&params,&cnd](const std::string& event,const std::map<std::string, std::string>& inner_params)
	{
		params = inner_params;
		cnd.notify_one();
	});

	cnd.wait(lck);

	cb(event, params);

	discard_event(id);
}

void discard_event(const EventHandlerID& cb)
{
	auto range = EventCentral::event_map.equal_range(cb->event);
	for (auto iter = range.first; iter != range.second; ++iter)
	{
		if (iter->second == cb)
		{
			EventCentral::event_map.erase(iter);
			break;
		}
	}
}

void discard_event(const std::string& event)
{
	auto range = EventCentral::event_map.equal_range(event);
	EventCentral::event_map.erase(range.first, range.second);
}


void signal_event(const std::string& event,const EventParams& params)
{
	auto range = EventCentral::event_map.equal_range(event);
	for (auto iter = range.first; iter != range.second; ++iter)
	{
		auto fn = iter->second->func;
		run_on_background([fn, event, params]()
		{
			fn(event, std::move(params));
		});
	}
}