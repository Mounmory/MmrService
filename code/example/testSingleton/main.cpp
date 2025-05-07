#include <common/include/general/Singleton.hpp>

#include <iostream>

class Example
{
	friend class mmrComm::Singleton<Example>;
	Example(int32_t data)
		:m_Data(data)
	{
		std::cout << "class without template construct." << std::endl;
	};
public:
	~Example()
	{
		std::cout << "class without template construct." << std::endl;
	};
	void print()
	{
		std::cout << "function print data value " << m_Data << std::endl;
	}
private:
	int32_t m_Data;
};

//模板类作为单实例
template<typename Ty>
class ExampleTemplate
{
	//声明友元
	friend class mmrComm::Singleton<ExampleTemplate<Ty>>;

	//构造函数为私有的
	ExampleTemplate(Ty data)
		:m_Data(data)
	{
		std::cout << "class with template construct." << std::endl;
	};
public:
	~ExampleTemplate() 
	{
		std::cout << "class with template construct." << std::endl;
	};
	void print()
	{
		std::cout << "function template print data value " << m_Data << std::endl;
	}
private:
	Ty m_Data;
};


int main()
{
	std::cout << "test normal class" << std::endl;
	{
		mmrComm::Singleton<Example>::initInstance(40);
		mmrComm::Singleton<Example>::getInstance()->print();
		mmrComm::Singleton<Example>::destroyInstance();
	}
	std::cout << std::endl;
	std::cout << "test template class" << std::endl;
	{
		mmrComm::Singleton<ExampleTemplate<int>>::initInstance(10);
		mmrComm::Singleton<ExampleTemplate<int>>::getInstance()->print();
		mmrComm::Singleton<ExampleTemplate<int>>::destroyInstance();
	}
	std::cout << std::endl;
	std::cout << "输入任意字符继续..." << std::endl;
	std::cin.get();

	return 0;
}
