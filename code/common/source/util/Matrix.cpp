#include <common/include/util/Matrix.h>
#include <cstring>

using namespace mmrUtil;

template<typename _Ty, typename _Alloc>
Matrix<_Ty, _Alloc>::Matrix()
	: m_ulRowCount(0)
	, m_ulColCount(0)
	, m_ptrBegin(nullptr)
	, m_ptrData(nullptr)
	, m_ulCapacity(0)
{
}

template<typename _Ty, typename _Alloc>
Matrix<_Ty, _Alloc>::Matrix(const uint32_t nRow, const uint32_t nCol)
	: m_ulRowCount(nRow)
	, m_ulColCount(nCol)
	, m_ptrBegin(nullptr)
	, m_ptrData(nullptr)
	, m_ulCapacity(0)
{
	allocateMemory();
}

template<typename _Ty, typename _Alloc>
Matrix<_Ty, _Alloc>::Matrix(const Matrix<_Ty, _Alloc> &rhs)
	: m_ulRowCount(rhs.m_ulRowCount)
	, m_ulColCount(rhs.m_ulColCount)
	, m_ptrBegin(rhs.m_ptrBegin)
	, m_ptrData(rhs.m_ptrData)
	, m_ulCapacity(rhs.m_ulCapacity)
{
	
}

template<typename _Ty, typename _Alloc>
Matrix<_Ty, _Alloc>::Matrix(Matrix<_Ty, _Alloc>&& rhs) noexcept
	: m_ulRowCount(std::exchange(rhs.m_ulRowCount, 0))
	, m_ulColCount(std::exchange(rhs.m_ulColCount, 0))
	, m_ptrBegin(std::exchange(rhs.m_ptrBegin, nullptr))
	, m_ptrData(std::exchange(rhs.m_ptrData, nullptr))
	, m_ulCapacity(std::exchange(rhs.m_ulCapacity, 0))
{

}

template<typename _Ty, typename _Alloc>
const _Ty  Matrix<_Ty, _Alloc>::operator() (uint32_t ulRowIndex, uint32_t ulColIndex) const 
{
	assert(ulRowIndex <= m_ulRowCount && ulColIndex <= m_ulColCount);
	return m_ptrBegin[ulRowIndex * m_ulColCount + ulColIndex];
}

//template<typename _Ty, typename _Alloc>
//_Ty&  Matrix<_Ty, _Alloc>::operator() (uint32_t ulRowIndex, uint32_t ulColIndex)
//{
//	assert(ulRowIndex <= m_ulRowCount && ulColIndex <= m_ulColCount);
//	if (m_ptrData.use_count() > 1)//是共享的，重新分配内存
//	{
//		allocateNewMemory();
//	}
//	return (m_ptrBegin[ulRowIndex * m_ulColCount + ulColIndex]);
//}

template<typename _Ty, typename _Alloc>
bool Matrix<_Ty, _Alloc>::setValue(uint32_t ulRowIndex, uint32_t ulColIndex, _Ty value) 
{
	if (ulRowIndex > m_ulRowCount || ulColIndex > m_ulColCount)
		return false;
	if (m_ptrData.use_count() > 1)//是共享的，重新分配内存
	{
		allocateNewMemory();
	}
	m_ptrBegin[ulRowIndex * m_ulColCount + ulColIndex] = value;
	return true;
}

template<typename _Ty, typename _Alloc>
Matrix<_Ty, _Alloc>& Matrix<_Ty, _Alloc>::operator=(const Matrix<_Ty, _Alloc> &rhs) 
{
	if (this != &rhs) 
	{
		m_ulRowCount = rhs.m_ulRowCount;
		m_ulColCount = rhs.m_ulColCount;
		m_ptrBegin = rhs.m_ptrBegin;
		m_ptrData = rhs.m_ptrData;
		m_ulCapacity = rhs.m_ulCapacity;
	}
	return *this;
}

template<typename _Ty, typename _Alloc>
Matrix<_Ty, _Alloc>& Matrix<_Ty, _Alloc>::operator=(Matrix<_Ty, _Alloc>&& rhs) noexcept 
{
	if (this != &rhs)
	{
		m_ulRowCount = std::exchange(rhs.m_ulRowCount, 0);
		m_ulColCount = std::exchange(rhs.m_ulColCount, 0);
		m_ptrBegin = std::exchange(rhs.m_ptrBegin, nullptr);
		m_ptrData = std::exchange(rhs.m_ptrData, nullptr);
		m_ulCapacity = std::exchange(rhs.m_ulCapacity, 0);
	}
	return *this;
}

template<typename _Ty, typename _Alloc>
void Matrix<_Ty, _Alloc>::resize(const uint32_t nRow, const uint32_t nCol)
{
	m_ulRowCount = nRow;
	m_ulColCount = nCol;
	allocateMemory();
}

template<typename _Ty, typename _Alloc>
void Matrix<_Ty, _Alloc>::zero()
{
	memset(m_ptrBegin, 0, sizeof(_Ty)*m_ulRowCount * m_ulColCount);
}


template<typename _Ty, typename _Alloc>
void Matrix<_Ty, _Alloc>::allocateMemory()
{
	uint32_t ulSize = m_ulRowCount * m_ulColCount;
	if (ulSize > m_ulCapacity)
	{
		auto memoData = mmrComm::Singleton<_Alloc>::initInstance()->template allocate<_Ty>(ulSize);
		m_ulCapacity = memoData.first;
		m_ptrData = std::move(memoData.second);
		m_ptrBegin = m_ptrData.get();
	}
}

template<typename _Ty, typename _Alloc>
void Matrix<_Ty, _Alloc>::allocateNewMemory()
{
	auto memoData = mmrComm::Singleton<_Alloc>::initInstance()->template allocate<_Ty>(m_ulRowCount * m_ulColCount);
	memcpy(memoData.second.get(), m_ptrBegin, sizeof(_Ty)*m_ulRowCount * m_ulColCount);//将值复制过来
	m_ulCapacity = memoData.first;
	m_ptrData = std::move(memoData.second);
	m_ptrBegin = m_ptrData.get();
}

