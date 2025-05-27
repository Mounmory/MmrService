#include <common/include/util/Matrix.h>

#include <iostream>



int main()
{
	//先实例化内存分配器
	auto ptrAlloc = mmrComm::Singleton<mmrUtil::ChunkAllocator<>>::initInstance(64, 40);

	{
		mmrUtil::Matrix<int32_t> mt1(10, 10);
		mt1.zero();
		mt1(0, 0) = 10;

		mmrUtil::Matrix<int32_t> mt2(mt1);
		std::cout << "mt2(0, 0):" << mt2(0, 0) << std::endl;
		mt2(0, 0) = 20;
		std::cout << "mt1(0, 0):" << mt1(0, 0) << ", mt2(0, 0):" << mt2(0, 0) << std::endl;
		
		//自赋值
		mt2 = mt2;
		
		//移动赋值
		mt2 = std::move(mt1);
		auto memInfo = ptrAlloc->getAvailableMemoryInfo();
		std::cout << "pool mem info free/total " << memInfo.first << "/" << memInfo.second << std::endl;

		mt1.resize(200, 2);
		memInfo = ptrAlloc->getAvailableMemoryInfo();
		std::cout << "pool mem info free/total " << memInfo.first << "/" << memInfo.second << std::endl;
	}

	auto memInfoout = ptrAlloc->getAvailableMemoryInfo();
	std::cout << "pool mem info free/total " << memInfoout.first << "/" << memInfoout.second << std::endl;
	std::cout << "输入任意字符继续..." << std::endl;
	std::cin.get();

	return 0;
}
