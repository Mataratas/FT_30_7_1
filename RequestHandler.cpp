#include "RequestHandler.h"
//==================================================================================
RequestHandler::RequestHandler() {
	m_tpool.start();
}
//----------------------------------------------------------------------------------
RequestHandler::~RequestHandler() {
	m_tpool.stop();
}
//----------------------------------------------------------------------------------
void RequestHandler::pushRequest(FuncType f, int* array, long left, long right, bool do_async){
	m_tpool.push_task(f, array, left, right,do_async);
}
//----------------------------------------------------------------------------------
