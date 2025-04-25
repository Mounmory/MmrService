#ifndef MMR_NET_SOCKET_H
#define MMR_NET_SOCKET_H
#include "common/include/mmrNet/MmrNetExport.h"

#include <iostream>

#ifdef OS_MMR_WIN//window平台
#define socket_type			SOCKET//socket 类型
#define GetSocketError()	WSAGetLastError()


#pragma comment(lib,"ws2_32.lib")
//#elif defined OS_MMR_LINUX
#else
#include <sys/types.h> 
#include <sys/socket.h>
//#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>


#define socket_type			int	//socket 类型
#define GetSocketError()	errno
#define closesocket(s) close(s)//关闭socker
#define INVALID_SOCKET (-1)	//无效的socket
#endif

BEGINE_NAMESPACE(mmrNet)
#define MAX_LISTEN 1024

class Socket {
private:
	socket_type m_sockfd;
public:
	Socket() 
		:m_sockfd(INVALID_SOCKET)
	{}
	Socket(int fd) 
		: m_sockfd(fd) 
	{}

	~Socket() 
	{ Close(); }

	socket_type fd() { return m_sockfd; }
	// 1.创建套接字
	bool Create() 
	{
		bool bRet = false;
		do 
		{
#ifdef OS_MMR_WIN//window平台
			WSADATA wsaData;
			if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
			{
				std::cerr << "WSA start up failed!" << std::endl;
				break;
			}
			if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2)
			{
				Close();
				std::cerr << "WSA version error!" << std::endl;
				break;
			}
			//int socket (int domain,int type,int protocol);
			m_sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
#else 
			m_sockfd = socket(AF_INET, SOCK_STREAM, 0);
#endif
			if (INVALID_SOCKET == m_sockfd)
			{
				std::cerr << "CREATE SOCKET FAILED !!" << std::endl;
				break;
			}
			bRet = true;
		} 
		while (false);
		return bRet;
	}
	// 2.绑定地址信息
	bool Bind(const std::string &ip, uint16_t port) 
	{
		struct sockaddr_in addr;
		addr.sin_family = AF_INET;
		addr.sin_port = htons(port);
		addr.sin_addr.s_addr = inet_addr(ip.c_str());
		// int bind(int sockfd,struct sockaddr * addr,socklen_t len);
		int ret = bind(m_sockfd, (struct sockaddr*)&addr, sizeof(addr));
		if (ret < 0) 
		{
			std::cerr << "BIND ADDRESS FAILED!!!!" << std::endl;
			return false;
		}
		return true;
	}
	// 3.开始监听
	bool Listen(int backlog = MAX_LISTEN) 
	{
		// int listen(int backlog)
		int ret = listen(m_sockfd, backlog);
		if (ret < 0) 
		{
			std::cerr << "SOCKET LISTEN FAILED!!" << std::endl;
			return false;
		}
		return true;
	}
	// 4. 向服务器发起连接
	bool Connect(const std::string& ip, uint16_t port) 
	{
		struct sockaddr_in addr;
		addr.sin_family = AF_INET;
		addr.sin_port = htons(port);
		addr.sin_addr.s_addr = inet_addr(ip.c_str());
		// int bind(int sockfd,struct sockaddr * addr,socklen_t len);
		int ret = connect(m_sockfd, (struct sockaddr*)&addr, sizeof(addr));
		if (ret < 0) 
		{
			std::cerr << "CONNECT ADDRESS FAILED!!!!" << std::endl;
			return false;
		}
		return true;
	}
	// 5. 获取新连接
	int Accept() 
	{

		// int accept(int sockfd,struct sockaddr* addr,socklen_t *len) /
		int newfd = accept(m_sockfd, NULL, NULL);
		if (newfd < 0) 
		{
			std::cerr << "SOCKET ACCEPT FAILED!!!!" << std::endl;
			return false;
		}
		return newfd;
	}

	int Recv(void *buf, size_t len, int flag = 0) {
		// 6.接收数据
		//有符号长整型 
		//ssize_t Recv(int sockfd,void* buf,size_t len,int flag);
		int ret = recv(m_sockfd, (char*)buf, len, flag);
		if (ret <= 0) 
		{
#ifdef OS_MMR_WIN//window平台
			if (GetSocketError() == WSAEWOULDBLOCK)
			{
				return 0; // 没收到数据
			}
#else
			// EAGAIN 当前socket的接收缓冲区没有数据来，在非阻塞二点情况下才会有这个错误
			// ENTER 当前socket的阻塞等待，被信号打断了
			if (GetSocketError() == EAGAIN || GetSocketError() == EINTR)
			{
				return 0; // 没收到数据
			}
#endif
			std::cerr << "SOCKET RECV FAILED!!" << std::endl;
			return -1; // 出错
		}
		return ret;
	}
	int nonBlockRecv(void* buf, size_t len) 
	{
#ifdef OS_MMR_WIN//window平台
		return Recv(buf, len); // MSG_DONTWAIT 表示当前接受为非阻塞
#else
		return Recv(buf, len, MSG_DONTWAIT); // MSG_DONTWAIT 表示当前接受为非阻塞
#endif
		
	}
	// 7.发送数据
	int Send(const void* buf, size_t len, int flag = 0) 
	{
		// ssize_t send(int sockfd,void *data,size_t len,int flag) 
		int ret = send(m_sockfd, (char*)buf, len, flag);
		if (ret < 0) 
		{
			std::cerr << "SOCKET SEND FAILED!!" << std::endl;
			return -1; // 出错
		}
		return ret; // 实际发送数据长度！！
	}

	int nonBlockSend(void* buf, size_t len) 
	{
#ifdef OS_MMR_WIN//window平台
		return Send(buf, len);
#else
		return Send(buf, len, MSG_DONTWAIT); // MSG_DONTWAIT 表示当前接受为非阻塞
#endif

	}
	// 8.关闭套接字
	void Close() 
	{
		if (m_sockfd != INVALID_SOCKET)
		{
			closesocket(m_sockfd);
#ifdef OS_MMR_WIN//window平台
			::WSACleanup();
#endif
			m_sockfd = INVALID_SOCKET;
		}

	}
	// 9.创建一个服务端链接
	bool createServer(uint16_t port, const std::string &ip = "0.0.0.0", bool block_flag = false) 
	{
		// 1.创建套接字 2. 绑定地址 3.开始监听 4.设置非阻塞 5.启动地址重用
		bool bRet = false;
		do 
		{
			if (!Create())
				break;

			if (!Bind(ip, port))
				break;

			if (!Listen())
				break;

			if (block_flag) 
				NonBlock();

			ReuseAddress();

			bRet = true;
		} while (false);
		return bRet;
	}
	// 10.创建一个客户端链接 
	bool createClient(uint16_t port, const std::string &ip) 
	{
		bool bRet = false;
		do
		{
			if (!Create())
				break;

			if (!Connect(ip, port))
				break;
			bRet = true;
		} while (false);
		return bRet;
	}

	// 11. 设置套接字选项——开启地址端口重用！
	void ReuseAddress() 
	{
		// int setsockopt(int fd,int leve,int optname,void *val,int vallen)
#ifdef OS_MMR_WIN//window平台
		int reuse = 1;
		if (setsockopt(m_sockfd, SOL_SOCKET, SO_REUSEADDR,(const char*)&reuse, sizeof(reuse)) == SOCKET_ERROR)
		{
			std::cerr << "reuse socket addr failed!" << std::endl;
			Close();
		}
#else
		int reuse = 1;
		setsockopt(m_sockfd, SOL_SOCKET, SO_REUSEADDR, (void*)&reuse, sizeof(int));
		reuse = 1;
		setsockopt(m_sockfd, SOL_SOCKET, SO_REUSEPORT, (void*)&reuse, sizeof(int));
#endif

	}
	// 12. 设置套接字阻塞属性——设置为非阻塞！ 
	bool NonBlock() 
	{
		bool bRet = false;
		do 
		{
#ifdef OS_MMR_WIN//window平台
			u_long non_blocking = 1;  // 1=非阻塞，0=阻塞
			if (ioctlsocket(m_sockfd, FIONBIO, &non_blocking) == SOCKET_ERROR)
			{
				Close();
				std::cerr << "ioctlsocket error: " << WSAGetLastError() << std::endl;
				break;
			}
#else
			int flag = fcntl(m_sockfd, F_GETFL, 0);
			fcntl(m_sockfd, F_SETFL, flag | O_NONBLOCK);
#endif
			bRet = true;
		} while (false);
		return bRet;
	}
};

END_NAMESPACE(mmrNet)
#endif

