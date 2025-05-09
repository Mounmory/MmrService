#ifndef MMR_NET_CHANNEL_H
#define MMR_NET_CHANNEL_H

#include "common/include/mmrNet/MmrNetExport.h"

#include <functional>

BEGINE_NAMESPACE(mmrNet)

#if 0



class Channel {

private:
	int m_fd;
	uint32_t m_events;  // ��ǰ��Ҫ��ص��¼�
	uint32_t m_revents; // ��ǰ���Ӵ������¼�
	using eventCallback = std::function<void()> ;
	eventCallback _read_callback;  // �ɶ��¼��������Ļص�����
	eventCallback _error_callback; // ��д�¼��������Ļص�����
	eventCallback _close_callback; // ���ӹر��¼��������Ļص�����
	eventCallback _event_callback; // �����¼��������Ļص�����
	eventCallback _write_callback; // ��д�¼��������Ļص�����
public:
	Channel(int fd) 
		: m_fd(fd)
	{
	}
	int Fd() { return m_fd; }
	void SetRevents(uint32_t events) { m_revents = events; }
	void setReadCallback(const eventCallback &cb) { _read_callback = cb; }
	void setWriteCallback(const eventCallback &cb) { _write_callback = cb; }
	void setErrorCallback(const eventCallback &cb) { _error_callback = cb; }
	void setCloseCallback(const eventCallback &cb) { _close_callback = cb; }
	void setEventCallback(const eventCallback &cb) { _event_callback = cb; }
	bool readAble() 
	{  // ��ǰ�Ƿ�ɶ� 
		return (m_events & EPOLLIN);
	}
	bool writeAble() 
	{  // ��ǰ�Ƿ��д
		return (m_events & EPOLLOUT);
	}
	void enableRead() 
	{// �������¼����
		m_events |= EPOLLIN; // �������ӵ�EventLoop���¼���أ�
	}
	void enableWrite() 
	{ // ����д�¼���� 
		m_events |= EPOLLOUT; // �������ӵ�EventLoop���¼���أ�
	}
	void disableRead() 
	{    // �رն��¼����
		m_events &= ~EPOLLIN;   // ������޸ĵ�EventLoop���¼���أ�
	}
	void disableWrite() 
	{ // �ر�д�¼���� 
		m_events &= ~EPOLLOUT;
	}
	void disableAll() 
	{ // �ر������¼����
		m_events = 0;
	}
	void Remove();  // ��������EventLoop�ӿ��Ƴ����
	void HandleEvent() 
	{
		if ((m_revents & EPOLLIN) || (m_revents & EPOLLRDHUP) || (m_revents & EPOLLPRI)) {

			if (_read_callback) _read_callback();
		}
		/*�п��ܻ��ͷ����ӵĲ����¼���һ��ֻ����һ��*/
		if (m_revents & EPOLLOUT) {
			if (_write_callback) _write_callback();
		}
		else if (m_revents & EPOLLERR) {
			if (_error_callback) _error_callback();//һ�������ͻ��ͷ����ӣ����Ҫ�ŵ�ǰ�ߵ�������ص�
		}
		else if (m_revents & EPOLLHUP) {
			if (_close_callback) _close_callback();
		}
		/*�����κ��¼��������õĻص�����*/
		if (_event_callback) _event_callback();
	}

};

#endif

END_NAMESPACE(mmrNet)
#endif // !1
