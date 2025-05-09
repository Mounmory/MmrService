#include <common/include/util/ChunkAllocator.h>//大块内存分配器
#include <common/include/util/MakeShared.hpp>//小块内存分配器

#include <iostream>
#include <condition_variable>
#include <mutex>
#include <vector>
#include <thread>
#include <future>
#include <string>
#include <set>

int main()
{
	using HeadType = uint16_t;
	//测试无锁ChunkLockFree类
	{
		std::atomic_bool g_bStart(false);
		size_t ckSize = 2;
		uint16_t ckNum = 3;
		std::cout << "小块内存分配器测试" << std::endl;

		mmrUtil::ChunkLockFree ckTest;
		ckTest.Init(ckSize, ckNum);

		auto ptr1 = ckTest.Allocate();
		auto ptr2 = ckTest.Allocate();
		auto ptr3 = ckTest.Allocate();
		auto ptr4 = ckTest.Allocate();
		ckTest.Deallocate(ptr1);
		ckTest.Deallocate(ptr2);
		ckTest.Deallocate(ptr3);

	}

	//测试固定大小的内存分配器
	{
		class AType 
		{
		public:
			AType(int lValue, std::string strValue) 
				:lData(lValue)
				, strValue(std::move(strValue))
			{ 
				std::cout << "A construct" << std::endl; 
			}
			~AType() 
			{ 
				std::cout << "A destruct" << std::endl; 
			}
			int lData = 0;
			std::string strValue;
			//char data[1024];//l类对象超过1024大小会报错
		};
		mmrUtil::FixedAllocator fixAlloc(sizeof(AType));//
		{
			auto ptrA = fixAlloc.AllocateAndContructData<AType>(6, "Hello");
			std::cout << "AType strValue " << ptrA->strValue << std::endl;
		}

		//测试小内存分配器
		mmrComm::Singleton<mmrUtil::SmallAllocator>::initInstance();
		
		{
			auto ptrAA = mmrUtil::Make_Shared<AType>(7, "Hello World");
			auto ptr11 = mmrUtil::Make_Shared<uint16_t>(7);
			std::cout << "AType strValue " << ptrAA->strValue << std::endl;
		}
		mmrComm::Singleton<mmrUtil::SmallAllocator>::destroyInstance();
	}

	
	{
		std::cout << "大块内存分配器测试" << std::endl;
		//初始化内存分配器，参数1：内存过期时间、参数2：最大缓存（MB）
		auto ptrAllocator = mmrComm::Singleton<mmrUtil::ChunkAllocator<10>>::initInstance(60, 64);
		{
			auto pariRet = ptrAllocator->allocate<int>(20);//分配第一块内存
			int* oriPtr = pariRet.second.get();
			oriPtr[0] = 5;
			std::cout << "int " << oriPtr[0];
			auto ptrInt2 = ptrAllocator->allocate<int>(20);//分配第二块内存
			auto ptrInt3 = ptrAllocator->allocate<int>(20);//分配第三块内存
			//离开作用域自动归还内存
		}

		{
			auto ptrInt2 = ptrAllocator->allocate<int>(512).second;
			int* oriPtr = ptrInt2.get();
			oriPtr[0] = 5;
			std::cout << "int " << oriPtr[0];
		}

		std::cout << "内存池中缓存大小：" << ptrAllocator->getCacheSize() << std::endl;
		mmrComm::Singleton<mmrUtil::ChunkAllocator<>>::destroyInstance();
	}


	std::cout << "输入任意字符继续..." << std::endl;
	std::cin.get();

	return 0;
}
