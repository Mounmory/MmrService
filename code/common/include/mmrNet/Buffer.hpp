#ifndef MMR_NET_BUFFER_HPP
#define MMR_NET_BUFFER_HPP
#include "common/include/mmrNet/MmrNetExport.h"

#include <ctime>
#include <cstring>
#include <iostream>
#include <vector>
#include <cassert>
#include <string>

BEGINE_NAMESPACE(mmrNet)

#define NET_BUFFER_SIZE 64 * 1024

class Buffer {
public:
	Buffer() 
		:m_readIndex(0), 
		m_writeIndex(0), 
		m_buffer(NET_BUFFER_SIZE) 
	{
	}
	~Buffer() 
	{

	}
	//数据起始位置
	char* begin() { return &m_buffer[0]; }

	// 获取可写入起始地址
	char *writePosition() { return &m_buffer[m_writeIndex] ; }

	// 获取读取起始地址
	char *readPosition() { return &m_buffer[m_readIndex]; }

	// 获取缓冲区末尾空间大小
	uint32_t tailIdleSize() { return m_buffer.size() - m_writeIndex; }

	// 获取缓冲区起始空间大小
	uint32_t handIdleSize() { return m_readIndex; }

	// 获取可读空间大小
	uint32_t readAbleSize() { return m_writeIndex - m_readIndex; }

	// 将读偏移向后移动
	void moveReadOffset(uint32_t len) 
	{
		// 向后移动大小必须小于可读数据大小
		assert(len <= readAbleSize());
		m_readIndex += len;
	}

	// 将写偏移向后移动
	void moveWriteOffset(uint32_t len) 
	{
		assert(len <= tailIdleSize());
		m_writeIndex += len;
	}

	void ensureWriteSpace(uint32_t len) 
	{
		// 确保可写空间足够 （整体空间够了就移动数据，否则就扩容！）  
		if (tailIdleSize() >= len) return;
		// 不够的话 ，判断加上起始位置够不够,够了将数据移动到起始位置
		if (len <= tailIdleSize() + handIdleSize()) {
			uint32_t rsz = readAbleSize(); //帮当前数据大小先保存起来
			std::copy(readPosition(), readPosition() + rsz, begin()); // 把可读数据拷贝到起始位置
			m_readIndex = 0; // 读归为0
			m_writeIndex = rsz; // 可读数据大小是写的偏移量！
		}
		else { // 总体空间不够！需要扩容，不移动数据，直接给写偏移之后扩容足够空间即可！
			m_buffer.resize(m_writeIndex + len);

		}
	}
	// 写入数据
	void Write(const void *data, uint32_t len) 
	{
		ensureWriteSpace(len);
		const char *d = (const char*)data;
		std::copy(d, d + len, writePosition());
	}

	void WriteAndPush(void* data, uint32_t len) 
	{
		Write(data, len);
		moveWriteOffset(len);
	}

	void WriteStringAndPush(const std::string &data) 
	{
		writeString(data);

		moveWriteOffset(data.size());
	}

	void writeString(const std::string &data) 
	{
		return Write(data.c_str(), data.size());
	}

	void writeBuffer(Buffer &data) 
	{
		return Write(data.readPosition(), data.readAbleSize());
	}

	void writeBufferAndPush(Buffer &data) 
	{
		writeBuffer(data);
		moveWriteOffset(data.readAbleSize());

	}
	std::string readAsString(uint32_t len) 
	{
		assert(len <= readAbleSize());
		std::string str;
		str.resize(len);
		Read(&str[0], len);
		return str;
	}
	void Read(void *buf, uint32_t len) 
	{
		// 读取数据 1. 保证足够的空间 2.拷贝数据进去
		// 要求获取的大小必须小于可读数据大小！
		assert(len <= readAbleSize());
		std::copy(readPosition(), readPosition() + len, (char*)buf);
	}
	void readAndPop(void *buf, uint32_t len) 
	{
		Read(buf, len);
		moveReadOffset(len);
	}
	// 逐步调试！！！！！
	std::string ReadAsStringAndPop(uint32_t len) 
	{
		assert(len <= readAbleSize());
		std::string str = readAsString(len);
		moveReadOffset(len);
		return str;
	}
	char* FindCRLF() 
	{
		char *res = (char*)memchr(readPosition(), '\n', readAbleSize());
		return res;
	}
	// 通常获取一行数据，这种情况针对是：
	std::string getLine() 
	{
		char* pos = FindCRLF();
		if (pos == nullptr) {
			return "";
		}
		// +1 为了把换行数据取出来！
		return readAsString(pos - readPosition() + 1);
	}
	std::string getLineAndPop() 
	{
		std::string str = getLine();
		moveReadOffset(str.size());
		return str;
	}
	void Clear() 
	{
		m_readIndex = 0;
		m_writeIndex = 0;
	}
private:
		std::vector<char> m_buffer; // 使用vector进行内存空间管理
		uint32_t m_readIndex; // 读偏移
		uint32_t m_writeIndex; // 写偏移
};


END_NAMESPACE(mmrNet)

#endif // !MMR_NET_BUFFER_H
