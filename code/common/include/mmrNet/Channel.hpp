#ifndef MMR_NET_CHANNEL_H
#define MMR_NET_CHANNEL_H

#include "common/include/mmrNet/MmrNetExport.h"

#include <functional>

BEGINE_NAMESPACE(mmrNet)

#if 0



class Channel {

private:
	int m_fd;
	uint32_t m_events;  // 当前需要监控的事件
	uint32_t m_revents; // 当前连接触发的事件
	using eventCallback = std::function<void()> ;
	eventCallback _read_callback;  // 可读事件被触发的回调函数
	eventCallback _error_callback; // 可写事件被触发的回调函数
	eventCallback _close_callback; // 连接关闭事件被触发的回调函数
	eventCallback _event_callback; // 任意事件被触发的回调函数
	eventCallback _write_callback; // 可写事件被触发的回调函数
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
	{  // 当前是否可读 
		return (m_events & EPOLLIN);
	}
	bool writeAble() 
	{  // 当前是否可写
		return (m_events & EPOLLOUT);
	}
	void enableRead() 
	{// 启动读事件监控
		m_events |= EPOLLIN; // 后面会添加到EventLoop的事件监控！
	}
	void enableWrite() 
	{ // 启动写事件监控 
		m_events |= EPOLLOUT; // 后面会添加到EventLoop的事件监控！
	}
	void disableRead() 
	{    // 关闭读事件监控
		m_events &= ~EPOLLIN;   // 后面会修改到EventLoop的事件监控！
	}
	void disableWrite() 
	{ // 关闭写事件监控 
		m_events &= ~EPOLLOUT;
	}
	void disableAll() 
	{ // 关闭所有事件监控
		m_events = 0;
	}
	void Remove();  // 后面会调用EventLoop接口移除监控
	void HandleEvent() 
	{
		if ((m_revents & EPOLLIN) || (m_revents & EPOLLRDHUP) || (m_revents & EPOLLPRI)) {

			if (_read_callback) _read_callback();
		}
		/*有可能会释放连接的操作事件，一次只处理一个*/
		if (m_revents & EPOLLOUT) {
			if (_write_callback) _write_callback();
		}
		else if (m_revents & EPOLLERR) {
			if (_error_callback) _error_callback();//一旦出错，就会释放连接，因此要放到前边调用任意回调
		}
		else if (m_revents & EPOLLHUP) {
			if (_close_callback) _close_callback();
		}
		/*不管任何事件，都调用的回调函数*/
		if (_event_callback) _event_callback();
	}

};

#endif

END_NAMESPACE(mmrNet)
#endif // !1
