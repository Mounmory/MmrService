#include <common/include/util/Matrix.h>
#include <cstring>

using namespace mmrUtil;

template<typename _Ty>
Matrix<_Ty>::Matrix()
	: m_ulRowCount(0)
	, m_ulColCount(0)
	, m_ptrBegin(nullptr)
	, m_ptrData(nullptr)
	, m_ulCapacity(0)
{
}

template<typename _Ty>
Matrix<_Ty>::Matrix(const uint32_t nRow, const uint32_t nCol)
	: m_ulRowCount(nRow)
	, m_ulColCount(nCol)
	, m_ptrBegin(nullptr)
	, m_ptrData(nullptr)
	, m_ulCapacity(0)
{
	allocateMemory();
}

template<typename _Ty>
Matrix<_Ty>::Matrix(const Matrix &rhs)
	: m_ulRowCount(rhs.m_ulRowCount)
	, m_ulColCount(rhs.m_ulColCount)
	, m_ptrBegin(nullptr)
	, m_ptrData(nullptr)
	, m_ulCapacity(0)
{
	allocateMemory();
	memcpy(m_ptrBegin, rhs.m_ptrBegin, sizeof(_Ty) * m_ulRowCount * m_ulColCount);
}

template<typename _Ty>
Matrix<_Ty>::Matrix(Matrix&& rhs) noexcept
	: m_ulRowCount(std::exchange(rhs.m_ulRowCount, 0))
	, m_ulColCount(std::exchange(rhs.m_ulColCount, 0))
	, m_ptrBegin(std::exchange(rhs.m_ptrBegin, nullptr))
	, m_ptrData(std::exchange(rhs.m_ptrData, nullptr))
	, m_ulCapacity(std::exchange(rhs.m_ulCapacity, 0))
{

}

template<typename _Ty>
const _Ty  Matrix<_Ty>::operator() (uint32_t ulRowIndex, uint32_t ulColIndex) const 
{
	assert(ulRowIndex < m_ulRowCount && ulColIndex < m_ulColCount);
	return m_ptrBegin[ulRowIndex * m_ulColCount + ulColIndex];
}

template<typename _Ty>
_Ty&  Matrix<_Ty>::operator() (uint32_t ulRowIndex, uint32_t ulColIndex)
{
	assert(ulRowIndex < m_ulRowCount && ulColIndex < m_ulColCount);
	return (m_ptrBegin[ulRowIndex * m_ulColCount + ulColIndex]);
}

template<typename _Ty>
bool Matrix<_Ty>::setValue(uint32_t ulRowIndex, uint32_t ulColIndex, _Ty value) 
{
	if (ulRowIndex > m_ulRowCount || ulColIndex > m_ulColCount)
		return false;

	m_ptrBegin[ulRowIndex * m_ulColCount + ulColIndex] = value;
	return true;
}

template<typename _Ty>
Matrix<_Ty>& Matrix<_Ty>::operator=(const Matrix &rhs)
{
	if (this != &rhs) 
	{
		m_ulRowCount = rhs.m_ulRowCount;
		m_ulColCount = rhs.m_ulColCount;
		allocateMemory();
		memcpy(m_ptrBegin, rhs.m_ptrBegin, sizeof(_Ty) * m_ulRowCount * m_ulColCount);
	}
	return *this;
}

template<typename _Ty>
Matrix<_Ty>& Matrix<_Ty>::operator=(Matrix&& rhs) noexcept
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

template<typename _Ty>
void Matrix<_Ty>::resize(const uint32_t nRow, const uint32_t nCol)
{
	m_ulRowCount = nRow;
	m_ulColCount = nCol;
	allocateMemory();
}

template<typename _Ty>
void Matrix<_Ty>::zero()
{
	memset(m_ptrBegin, 0, sizeof(_Ty)*m_ulRowCount * m_ulColCount);
}

template<typename _Ty>
void Matrix<_Ty>::allocateMemory()
{
	uint32_t ulSize = m_ulRowCount * m_ulColCount * sizeof(_Ty);//申请字节数量
	if (ulSize > m_ulCapacity)
	{
		auto memoData = mmrComm::Singleton<mmrUtil::ChunkAllocator<>>::initInstance()->template allocate<uint8_t>(ulSize);
		m_ulCapacity = memoData.first;
		m_ptrData = std::move(memoData.second);
		m_ptrBegin = reinterpret_cast<_Ty*>(m_ptrData.get());
	}
}

