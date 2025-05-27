#include <common/include/util/MemoryPool.hpp>//大块内存分配器
#include <common/include/util/TimeCounter.hpp>
#include <iostream>
#include <condition_variable>
#include <mutex>
#include <vector>
#include <list>
#include <thread>
#include <future>
#include <string>

void TestSmallAllocator();

void TestSmallAlloEffition();//效率测试

void TestChunkAllocator();

int main()
{
	TestSmallAllocator();

	TestSmallAlloEffition();

	TestChunkAllocator();

	std::cout << "输入任意字符继续..." << std::endl;
	std::cin.get();

	return 0;
}

void TestSmallAllocator() 
{
	//{//测试在大块内存中分配与归还内存,测试前将ChunkLockFree声明为public
	//	mmrUtil::FixedAllocator::ChunkLockFree clf;
	//	clf.init(1, 3);
	//	auto ptr1 = clf.allocate();
	//	auto ptr2 = clf.allocate();
	//	auto ptr3 = clf.allocate();
	//	auto ptr4 = clf.allocate();

	//	clf.deallocate(ptr1);
	//	clf.deallocate(ptr2);
	//	clf.deallocate(ptr3);
	//}

	class AType
	{
	public:
		AType(int lValue)
			:lData(lValue)
		{
			std::cout << "A construct" << std::endl;
		}
		~AType()
		{
			std::cout << "A destruct" << std::endl;
		}
		int lData = 0;
		//char data1[4];//解除屏蔽类大小大于1024
		char data2[1020];
	};


	//测试小内存分配器
	std::cout << "SmallAllocator 分配内存测试 " << std::endl;
	auto allocatorPtr = mmrComm::Singleton<mmrUtil::SmallAllocator>::initInstance();
	auto memInfo = allocatorPtr->getAvailableMemoryInfo();
	std::cout << "memory info " << memInfo.first << "/" << memInfo.second << std::endl;
	{
		auto ptrShare = mmrUtil::Make_Shared<AType>(70);
		auto ptrUnique = mmrUtil::Make_Unique<AType>(700);
		std::cout << "shared ptr AType strValue " << ptrShare->lData << std::endl;
		std::cout << "unique ptr AType strValue " << ptrUnique->lData << std::endl;

		std::shared_ptr<AType> sptr = std::move(ptrUnique);

		memInfo = allocatorPtr->getAvailableMemoryInfo();
		std::cout << "memory info " << memInfo.first << "/" << memInfo.second << std::endl;
	}
	memInfo = allocatorPtr->getAvailableMemoryInfo();
	std::cout << "memory info " << memInfo.first << "/" << memInfo.second << std::endl;
	//allocatorPtr->releaseFreeMemory();
	//memInfo = allocatorPtr->getAvailableMemoryInfo();
	//std::cout << "memory info " << memInfo.first << "/" << memInfo.second << std::endl;
	mmrComm::Singleton<mmrUtil::SmallAllocator>::destroyInstance();
}

void TestSmallAlloEffition()//效率测试
{
	class BType 
	{
	public:
		BType(int data)
			: m_lData(data)
		{

		}
		int m_lData;
		char m_szData[100];
	};

	std::cout << "SmallAllocator 分配内存效率测试 " << std::endl;
	auto allocatorPtr = mmrComm::Singleton<mmrUtil::SmallAllocator>::initInstance();
	size_t count = 1000;
	{
		std::cout << "第一次测试，内存池为空" << std::endl;
		std::vector<std::shared_ptr<BType>> vecAllPtr;
		vecAllPtr.reserve(count);
		mmrUtil::TimeCounter timeCount;
		for (size_t i = 0; i < count; i++)
		{
			vecAllPtr.emplace_back(std::make_shared<BType>(100));
		}
		std::cout << "1 std make shared ptr time cost " << timeCount.elapsed_micro() << " vector size " << vecAllPtr.size() << std::endl;
		vecAllPtr.clear();
		timeCount.reset();
		for (size_t i = 0; i < count; i++)
		{
			vecAllPtr.emplace_back(mmrUtil::Make_Shared<BType>(100));
		}
		std::cout << "1 allocator make ptr time cost " << timeCount.elapsed_micro() << " vector size " << vecAllPtr.size() << std::endl;
		vecAllPtr.clear();
	}

	{
		std::cout << "第二次测试，内存池为非空" << std::endl;
		std::vector<std::shared_ptr<BType>> vecAllPtr;
		vecAllPtr.reserve(count);
		mmrUtil::TimeCounter timeCount;
		for (size_t i = 0; i < count; i++)
		{
			vecAllPtr.emplace_back(std::make_shared<BType>(100));
		}
		std::cout << "2 std make shared ptr time cost " << timeCount.elapsed_micro() << " vector size " << vecAllPtr.size() << std::endl;
		vecAllPtr.clear();
		timeCount.reset();
		for (size_t i = 0; i < count; i++)
		{
			vecAllPtr.emplace_back(mmrUtil::Make_Shared<BType>(100));
		}
		std::cout << "2 allocator make ptr time cost " << timeCount.elapsed_micro() << " vector size " << vecAllPtr.size() << std::endl;
		vecAllPtr.clear();
	}
	auto memInfo = allocatorPtr->getAvailableMemoryInfo();
	std::cout << "memory info before clear " << memInfo.first << "/" << memInfo.second << std::endl;
	//allocatorPtr->releaseFreeMemory();
	//memInfo = allocatorPtr->getAvailableMemoryInfo();
	//std::cout << "memory info after clear " << memInfo.first << "/" << memInfo.second << std::endl;
	mmrComm::Singleton<mmrUtil::SmallAllocator>::destroyInstance();
}

void TestChunkAllocator() 
{
	std::cout << "ChunckAllocator内存分配器测试" << std::endl;
	//初始化内存分配器，参数1：内存过期时间、参数2：最大缓存（MB）
	auto ptrAllocator = mmrComm::Singleton<mmrUtil::ChunkAllocator<10>>::initInstance(60, 64);
	auto memInfo = ptrAllocator->getAvailableMemoryInfo();
	std::cout << "内存池中缓存大小(空闲/总数)：" << memInfo.first << "/" << memInfo.second << std::endl;
	{
		auto pariRet = ptrAllocator->allocate<int>(20);//分配第一块内存
		int* oriPtr = pariRet.second.get();
		oriPtr[0] = 5;
		std::cout << "int " << oriPtr[0] << std::endl;
		auto ptrInt2 = ptrAllocator->allocate<int>(20);//分配第二块内存
		auto ptrInt3 = ptrAllocator->allocate<int>(20);//分配第三块内存
		//离开作用域自动归还内存
		memInfo = ptrAllocator->getAvailableMemoryInfo();
		std::cout << "内存池中缓存大小(空闲/总数)：" << memInfo.first << "/" << memInfo.second << std::endl;
	}

	memInfo = ptrAllocator->getAvailableMemoryInfo();
	std::cout << "内存池中缓存大小(空闲/总数)：" << memInfo.first << "/" << memInfo.second << std::endl;

	{
		auto ptrInt2 = ptrAllocator->allocate<int>(512).second;
		int* oriPtr = ptrInt2.get();
		oriPtr[0] = 5;
		std::cout << "int " << oriPtr[0] << std::endl;

		memInfo = ptrAllocator->getAvailableMemoryInfo();
		std::cout << "内存池中缓存大小(空闲/总数)：" << memInfo.first << "/" << memInfo.second << std::endl;
	}

	memInfo = ptrAllocator->getAvailableMemoryInfo();
	std::cout << "内存池中缓存大小：" << memInfo.first << "/" << memInfo.second << std::endl;
	mmrComm::Singleton<mmrUtil::ChunkAllocator<>>::destroyInstance();
}