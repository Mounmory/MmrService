#ifndef MMR_UTIL_MATIX_H
#define MMR_UTIL_MATIX_H
/*
	矩阵类使用连续和共享内存保存数据，在拷贝构造和拷贝赋值运算时，直接使用共享。
	修改数据时若数据正在共享，则重新分配内存，类内部是无锁的，使用时外部保证不能并发读写
*/
#include <common/include/util/ChunkAllocator.h>
#include <assert.h>

BEGINE_NAMESPACE(mmrUtil)

template<typename _Ty, typename _Alloc = ChunkAllocator>
class COMMON_CLASS_API Matrix
{
public:
	Matrix();

	Matrix(const uint32_t nRow, const uint32_t nCol);

	Matrix(const Matrix<_Ty, _Alloc> &rhs);

	Matrix(Matrix<_Ty, _Alloc>&& rhs) noexcept;

	~Matrix() = default;

	const _Ty operator() (uint32_t ulRowIndex, uint32_t ulColIndex) const;

	//_Ty& operator() (uint32_t ulRowIndex, uint32_t ulColIndex);

	bool setValue(uint32_t ulRowIndex, uint32_t ulColIndex, _Ty value);

	Matrix<_Ty, _Alloc>& operator=(const Matrix<_Ty, _Alloc> &rhs);

	Matrix<_Ty, _Alloc>& operator=(Matrix<_Ty, _Alloc>&& rhs) noexcept;

	//矩阵行数
	const uint32_t getRowCount() const { return m_ulRowCount; }

	//矩阵列数
	const uint32_t getColCount() const { return m_ulColCount; }

	//重置矩阵大小
	void resize(const uint32_t nRow, const uint32_t nCol);

	//设置所有元素为0
	void zero();

private:
	void allocateMemory();

	void allocateNewMemory();
private:
	uint32_t m_ulRowCount;//行数
	uint32_t m_ulColCount;//列数
	_Ty* m_ptrBegin;//指针起始位置

	//std::shared_ptr<std::mutex> m_ptrMutex;//互斥量，暂时不使用，外部保证并发
	std::shared_ptr<_Ty> m_ptrData;//没存数据
	uint32_t m_ulCapacity;//内存容量
};

//模板实例化
template class COMMON_CLASS_API Matrix<bool>;
template class COMMON_CLASS_API Matrix<int8_t>;
template class COMMON_CLASS_API Matrix<uint8_t>;
template class COMMON_CLASS_API Matrix<int16_t>;
template class COMMON_CLASS_API Matrix<uint16_t>;
template class COMMON_CLASS_API Matrix<int32_t>;
template class COMMON_CLASS_API Matrix<uint32_t>;
template class COMMON_CLASS_API Matrix<int64_t>;
template class COMMON_CLASS_API Matrix<uint64_t>;
template class COMMON_CLASS_API Matrix<float>;
template class COMMON_CLASS_API Matrix<double>;

END_NAMESPACE(mmrUtil)

#endif // !MMR_UTIL_MATIX_H


