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
	//������ʼλ��
	char* begin() { return &m_buffer[0]; }

	// ��ȡ��д����ʼ��ַ
	char *writePosition() { return &m_buffer[m_writeIndex] ; }

	// ��ȡ��ȡ��ʼ��ַ
	char *readPosition() { return &m_buffer[m_readIndex]; }

	// ��ȡ������ĩβ�ռ��С
	uint32_t tailIdleSize() { return m_buffer.size() - m_writeIndex; }

	// ��ȡ��������ʼ�ռ��С
	uint32_t handIdleSize() { return m_readIndex; }

	// ��ȡ�ɶ��ռ��С
	uint32_t readAbleSize() { return m_writeIndex - m_readIndex; }

	// ����ƫ������ƶ�
	void moveReadOffset(uint32_t len) 
	{
		// ����ƶ���С����С�ڿɶ����ݴ�С
		assert(len <= readAbleSize());
		m_readIndex += len;
	}

	// ��дƫ������ƶ�
	void moveWriteOffset(uint32_t len) 
	{
		assert(len <= tailIdleSize());
		m_writeIndex += len;
	}

	void ensureWriteSpace(uint32_t len) 
	{
		// ȷ����д�ռ��㹻 ������ռ乻�˾��ƶ����ݣ���������ݣ���  
		if (tailIdleSize() >= len) return;
		// �����Ļ� ���жϼ�����ʼλ�ù�����,���˽������ƶ�����ʼλ��
		if (len <= tailIdleSize() + handIdleSize()) {
			uint32_t rsz = readAbleSize(); //�ﵱǰ���ݴ�С�ȱ�������
			std::copy(readPosition(), readPosition() + rsz, begin()); // �ѿɶ����ݿ�������ʼλ��
			m_readIndex = 0; // ����Ϊ0
			m_writeIndex = rsz; // �ɶ����ݴ�С��д��ƫ������
		}
		else { // ����ռ䲻������Ҫ���ݣ����ƶ����ݣ�ֱ�Ӹ�дƫ��֮�������㹻�ռ伴�ɣ�
			m_buffer.resize(m_writeIndex + len);

		}
	}
	// д������
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
		// ��ȡ���� 1. ��֤�㹻�Ŀռ� 2.�������ݽ�ȥ
		// Ҫ���ȡ�Ĵ�С����С�ڿɶ����ݴ�С��
		assert(len <= readAbleSize());
		std::copy(readPosition(), readPosition() + len, (char*)buf);
	}
	void readAndPop(void *buf, uint32_t len) 
	{
		Read(buf, len);
		moveReadOffset(len);
	}
	// �𲽵��ԣ���������
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
	// ͨ����ȡһ�����ݣ������������ǣ�
	std::string getLine() 
	{
		char* pos = FindCRLF();
		if (pos == nullptr) {
			return "";
		}
		// +1 Ϊ�˰ѻ�������ȡ������
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
		std::vector<char> m_buffer; // ʹ��vector�����ڴ�ռ����
		uint32_t m_readIndex; // ��ƫ��
		uint32_t m_writeIndex; // дƫ��
};


END_NAMESPACE(mmrNet)

#endif // !MMR_NET_BUFFER_H
