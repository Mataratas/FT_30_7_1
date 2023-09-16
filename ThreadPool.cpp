#include "ThreadPool.h"
//==================================================================
ThreadPool::ThreadPool() :
	m_thread_count(std::thread::hardware_concurrency() > 0 ? std::thread::hardware_concurrency() : 4),
	m_thread_queues(m_thread_count)
{
#ifdef _DEBUG
	std::cout<<"m_thread_queues.size = " << m_thread_queues.size() << std::endl;
#endif // _DEBUG
}
//------------------------------------------------------------------
void ThreadPool::start() {
	for (size_t i = 0; i < m_thread_count; i++){
		m_threads.emplace_back(std::thread(&ThreadPool::threadFunc, this,i));
	}
	_ParentTaskPrmPtr = std::make_shared<std::promise<bool>>();
}
//------------------------------------------------------------------
void ThreadPool::stop() {
	for (size_t i = 0; i < m_thread_count; i++) {
		task_type empty_task;
		m_thread_queues[i].push(empty_task);
	}
	m_event_holder.notify_all();
	for (auto& t : m_threads) {
		if(t.joinable())
			t.join();
	}		
}
//------------------------------------------------------------------
bool ThreadPool::wait() {
	if (!_chld_cnt.load()) {
		auto f = _ParentTaskPrmPtr->get_future();
		f.wait();
		return f.get();
	}
	return false;
}
//------------------------------------------------------------------
void ThreadPool::push_task(FuncType f, int* array, long left, long right, bool do_async) {
	int queue_to_push = m_index++ % m_thread_count; // ��������� ������ �������, ���� ������� ������
	task_type new_task([=] {f(array, left, right, do_async); }); // ��������� �������
	m_thread_queues[queue_to_push].push(new_task); // ������ � �������
	_chld_cnt++;
	m_event_holder.notify_one();			// ��������� ��������� ����� � ����� ������
}
//------------------------------------------------------------------
void ThreadPool::threadFunc(int qindex) {
	while (true) {
		task_type task_to_do;
		bool res{ false };
		int i{ 0 };

		for (; i < m_thread_count; i++) { // ������� ������ ������� ������ �� ����� �������, ������� �� �����				
			if (res = m_thread_queues[(qindex + i) % m_thread_count].fast_pop(task_to_do)) 
				break;
		}
		if (!res) //���� �� ����� �������� ����������� ��������� �������
			m_thread_queues[qindex].pop(task_to_do);
		else if(!task_to_do) //���� �������� ������ ������, �� ��� ������� ��������� ������� � ��� ��� �� �� ������� � ������� ������, �� ����� �� �� �����, ����� �������� � �������
			m_thread_queues[(qindex + i) % m_thread_count].push(task_to_do);

		if (!task_to_do)
			return;
		//����� ������ ���� ��������� ���������� 
		try{
			task_to_do();// ��������� ������
		}
		catch (const std::exception& ex) {
			std::cout << "Tread " << std::this_thread::get_id() << " failed to execute task with error:" << ex.what() << std::endl;
		}
		
		_chld_cnt--;
		if (!_chld_cnt.load())
			_ParentTaskPrmPtr->set_value(true);
		
	}
}
//------------------------------------------------------------------


