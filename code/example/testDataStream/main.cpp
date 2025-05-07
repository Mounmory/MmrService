#include "common/include/Common_def.h"
#include "common/include/util/CDataStream.hpp"
#include <iostream>


enum class EM_TEST :int16_t
{
	TYPE_1,
	TYPE_2,
	TYPE_3
};

struct Mydata// :public mmrUtil::IDealByStream<std::string>
{
	Mydata() = default;
	Mydata(std::string str1, int n, float f, std::string str2, EM_TEST emTest)
		: strValue1(std::move(str1))
		, nValue(n)
		, fValue(f)
		, strValue2(std::move(str2))
		, emTest(emTest)
	{
	}

	std::string strValue1;
	int nValue;
	float fValue;
	std::string strValue2;
	EM_TEST emTest;
	void marshal(mmrUtil::CDataStream<std::string>& dataStream) const
	{
		dataStream << strValue1;
		dataStream << nValue;
		dataStream << fValue;
		dataStream << strValue2;
		dataStream << emTest;
	}

	void unmarshal(mmrUtil::CDataStream<std::string>& dataStream)
	{
		dataStream >> strValue1;
		dataStream >> nValue;
		dataStream >> fValue;
		dataStream >> strValue2;
		dataStream >> emTest;
	}
};

int main()
{	//复制数据到byte
	char transData[1024] = {0};
	int byteLen = 0;
	//数据序列化二进制保存
	{
		mmrUtil::CDataStream<std::string> dataMarshal(mmrUtil::emEndian::BIG/*二进制数据为大端模式*/);
		Mydata dataTest = { "你好",5,2.5,"hello" ,EM_TEST::TYPE_2 };
		std::cout << "origin data is " << dataTest.strValue1 << " " << dataTest.nValue << " " << dataTest.fValue << " " << dataTest.strValue2 << std::endl;
		dataTest.marshal(dataMarshal);
		byteLen = dataMarshal.size();
		memcpy(transData, &dataMarshal[0], byteLen);
	}

	//反序列化
	{
		mmrUtil::CDataStream<std::string> dataUnmarshal(transData, byteLen, mmrUtil::emEndian::BIG);

		Mydata dataTrans;
		dataTrans.unmarshal(dataUnmarshal);
		std::cout << "unmarshal data is "
			<< dataTrans.strValue1 << " "
			<< dataTrans.nValue << " "
			<< dataTrans.fValue << " "
			<< dataTrans.strValue2 << " "
			<< static_cast<int16_t>(dataTrans.emTest)
			<< std::endl;
	}

	std::cout << "输入任意字符继续..." << std::endl;
	std::cin.get();
	return 0;
}
