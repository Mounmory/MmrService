#include <common/include/util/Matrix.h>

#include <iostream>



int main()
{
	
//#if __cplusplus >= 201402L//需要c++14标准
//	auto ptrAlloc = mmrComm::Singleton<mmrUtil::ChunkAllocator>::initInstance(
//		mmrUtil::ChunkAllocParas::Create().template Set<mmrUtil::TagExpiredTime>(60).template Set<mmrUtil::TagMaxCache>(40));
//#else
//	auto ptrAlloc = mmrComm::Singleton<mmrUtil::ChunkAllocator>::initInstance(64, 40);
//#endif

	{
		mmrUtil::Matrix<int16_t> mt1(10, 10);
		mt1.zero();

		mt1.setValue(0, 0, 10);
		mmrUtil::Matrix<int16_t> mt2(mt1);
		std::cout << "mt2(0, 0):" << mt2(0, 0) << std::endl;
		mt2.setValue(0, 0, 20);
		std::cout << "mt1(0, 0):" << mt1(0, 0) << ", mt2(0, 0)" << mt2(0, 0) << std::endl;
		mt2 = mt1;
		std::cout << "pool mem size " << mmrComm::Singleton<mmrUtil::ChunkAllocator>::getInstance()->getCacheSize() << std::endl;
		mt2 = std::move(mt1);
		mt1.resize(200, 2);
		std::cout << "pool mem size " << mmrComm::Singleton<mmrUtil::ChunkAllocator>::getInstance()->getCacheSize() << std::endl;
	}

	std::cout << "pool mem size " << mmrComm::Singleton<mmrUtil::ChunkAllocator>::getInstance()->getCacheSize() << std::endl;
	std::cout << "输入任意字符继续..." << std::endl;
	std::cin.get();

	return 0;
}
