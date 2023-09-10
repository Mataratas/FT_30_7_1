#pragma once
#include "ThreadPool.h"
//==================================================================================
class RequestHandler{
public:
	RequestHandler();
	~RequestHandler();
	void pushRequest(FuncType f, int* , long , long, bool);
	bool wait() { return m_tpool.wait(); };
private:
	ThreadPool m_tpool;
};

