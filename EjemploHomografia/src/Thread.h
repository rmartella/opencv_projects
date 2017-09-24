#ifndef _THREAD_H_
#define _THREAD_H_

#include <boost/thread.hpp>
#include <boost/thread/thread.hpp>

class Thread{

private:
	boost::thread m_Thread;

protected:
	bool running;

	
public:
	Thread(){running=false;}

	virtual ~Thread(){}

	virtual void processQueue(){}

	/*bool start(){
	running = true;
	return !pthread_create(&thread, NULL, Thread::thread_func, (void*)this);
	}*/
	void start()
	{
		this->running = true;
		m_Thread = boost::thread(&Thread::processQueue, this);
	}


	bool isRunning()
	{
		return this->running;
	}

	void join()
	{
		m_Thread.join();
		this->running = false;
	}

	void stop()
	{
		m_Thread.interrupt();
		this->running = false;
	}
};

#endif