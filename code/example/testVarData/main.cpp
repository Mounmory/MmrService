#include <iostream>
#include "util/CVarDatas.hpp"

int main(int argc ,char** argv)
{
	try
	{
		uint32_t ulCount = 0;
		//while (ulCount++ < 100000)
		{

			mmrUtil::CVariant var(10.5);

			std::cout << "get double value: " << var.getDoubleData() << std::endl;

			std::cout << "cast to int: " << var.CastToNum<int>() << std::endl;

			//this will be compiled failed
			//std::cout << "cast to string " << var.CastToNum<std::string>() << std::endl;

			var.setStringData("string data");

			std::cout << "get string value: " << var.getStringData() << std::endl;

			//equal to itself
			var = var;

			//move to itself
			var = std::move(var);

			mmrUtil::CVariant var2(std::move(var));

			std::cout << "var 2 get string value: " << var2.getStringData() << std::endl;

			//this will cause an exception
			//std::cout << "cast to int: " << var2.CastToNum<int>() << std::endl;
		}
	}
	catch (std::exception& e)
	{
		std::cout << "exception info: " << e.what() << std::endl;
	}

	std::cout << "输入任意字符继续..." << std::endl;
	std::cin.get();
	return 0;
}



