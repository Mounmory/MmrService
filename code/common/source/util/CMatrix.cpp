#include "common/include/util/CMatrix.h"
#include <string.h>

using namespace mmrUtil;

template<class _Ty>
CMatrix<_Ty>::CMatrix()
	: m_rows(0)
	, m_cols(0)
	, m_array(nullptr)
{

}

template<class _Ty>
CMatrix<_Ty>::CMatrix(const size_t nRow, const size_t nCol)
	: m_rows(nRow)
	, m_cols(nCol)
	, m_array(nullptr)
{
	m_array = new _Ty*[m_rows];
	for (size_t i = 0; i < m_rows; ++i)
	{
		m_array[i] = new _Ty[m_cols];
	}
}

template<class _Ty>
CMatrix<_Ty>::CMatrix(const CMatrix<_Ty> &rhs)
	: m_rows(0)
	, m_cols(0)
	, m_array(nullptr)
{
	*this = rhs;
}

template<class _Ty>
CMatrix<_Ty>::CMatrix(CMatrix<_Ty>&& rhs) noexcept
	: m_rows(std::exchange(rhs.m_rows, 0))
	, m_cols(std::exchange(rhs.m_cols, 0))
	, m_array(std::exchange(rhs.m_array, nullptr))
{

}

template<class _Ty>
CMatrix<_Ty>::~CMatrix()
{
	destroyArray();
}

template<class _Ty>
void CMatrix<_Ty>::reSizeArray(const size_t nRow, const size_t nCol)
{
	if (this->m_rows != nRow || this->m_cols != nCol)
	{
		//new分配的时候，一般会分配除了它本身大小以外的内存头和内存尾，一般形式为：[内存头][可用数据][内存尾]。
		//而你的ptr指针返回的是[可用数据]部分的起始地址。大部分编译器在用户分配的size为0的情况下，会自动分配1字节的可用内存，可以用delete删除
		destroyArray();
		m_rows = nRow;
		m_cols = nCol;
		m_array = new _Ty*[m_rows];
		for (size_t i = 0; i < m_rows; ++i)
		{
			m_array[i] = new _Ty[m_cols];
		}
	}
}

template<class _Ty>
bool CMatrix<_Ty>::zeroArray()
{
	if (isBuildInDataType())
	{
		for (size_t i = 0; i < m_rows; ++i)
		{
			memset(m_array[i], 0, m_cols * sizeof(_Ty));
		}
		return true;
	}
	else
	{
		return false;
	}

}

template<class _Ty>
_Ty* CMatrix<_Ty>::operator[](const size_t index)
{
	return m_array[index];
}

template<class _Ty>
const _Ty* CMatrix<_Ty>::operator[](const size_t index) const
{
	return m_array[index];
}

template<class _Ty>
CMatrix<_Ty>& CMatrix<_Ty>::operator=(const _Ty& value)
{
	if (true == isBuildInDataType())
	{
		for (size_t i = 0; i < m_rows; ++i)
		{
			if (0 == i)
			{
				for (size_t j = 0; j < m_cols; ++j)
				{
					m_array[i][j] = value;
				}
			}
			else
			{
				memcpy(m_array[i], m_array[0], sizeof(_Ty) * m_cols);
			}
		}
	}
	else
	{
		for (size_t i = 0; i < m_rows; ++i)
		{
			for (size_t j = 0; j < m_cols; ++j)
			{
				m_array[i][j] = value;
			}
		}
	}
	return *this;
}

template<class _Ty>
CMatrix<_Ty>& CMatrix<_Ty>::operator=(const CMatrix<_Ty> &rhs)
{
	if (m_array != rhs.m_array || m_rows != rhs.m_rows || m_cols != rhs.m_cols)//非自身赋值
	{
		if (m_rows != rhs.m_rows || m_cols != rhs.m_cols)
		{
			reSizeArray(rhs.m_rows, rhs.m_cols);
		}
		if (true == isBuildInDataType())
		{
			for (size_t i = 0; i < m_rows; ++i)
			{
				memcpy(m_array[i], rhs.m_array[i], m_cols * sizeof(_Ty));
			}
		}
		else
		{
			for (size_t i = 0; i < m_rows; ++i)
			{
				for (size_t j = 0; j < m_cols; ++j)
				{
					m_array[i][j] = rhs.m_array[i][j];
				}
			}
		}
	}
	return *this;
}

template<class _Ty>
CMatrix<_Ty>& CMatrix<_Ty>::operator=(CMatrix<_Ty>&& rhs) noexcept
{
	if (this->m_array != rhs.m_array)
	{
		destroyArray();
		m_rows = std::exchange(rhs.m_rows, 0);
		m_cols = std::exchange(rhs.m_cols, 0);
		m_array = std::exchange(rhs.m_array, nullptr);
	}
	return *this;
}

template<class _Ty>
bool CMatrix<_Ty>::operator==(const CMatrix<_Ty> &rhs)
{
	if (this->m_cols != rhs.m_cols || this->m_rows != rhs.m_rows)
	{
		return false;
	}
	for (int i = 0; i < this->m_rows; ++i)
	{
		for (int j = 0; j < this->m_cols; ++j)
		{
			if (this->m_array[i][j] != rhs.m_array[i][j])
			{
				return false;
			}
		}
	}
	return true;
}

template<class _Ty>
bool CMatrix<_Ty>::operator!=(const CMatrix<_Ty> &rhs)
{
	return !(*this == rhs);
}

template<class _Ty>
bool CMatrix<_Ty>::isBuildInDataType()
{
	return typeid(_Ty) == typeid(bool) ||
		typeid(_Ty) == typeid(uint8_t) ||
		typeid(_Ty) == typeid(int8_t) ||
		typeid(_Ty) == typeid(uint16_t) ||
		typeid(_Ty) == typeid(int16_t) ||
		typeid(_Ty) == typeid(uint32_t) ||
		typeid(_Ty) == typeid(int32_t) ||
		typeid(_Ty) == typeid(uint64_t) ||
		typeid(_Ty) == typeid(int64_t) ||
		typeid(_Ty) == typeid(float) ||
		typeid(_Ty) == typeid(double);
}

template<class _Ty>
void CMatrix<_Ty>::destroyArray()
{
	if (0 == m_rows && 0 == m_cols && nullptr == m_array)
	{
		return;
	}

	for (size_t i = 0; i < m_rows; ++i)
	{
		delete[] m_array[i];
	}

	delete m_array;
	m_array = nullptr;
}

//template<typename _Ty>
//std::ostream& operator<<(std::ostream &os, const mmrUtil::CMatrix<_Ty>& rhs)
//{
//	os << "CMatrix Type:" << typeid(_Ty).name() <<
//		"\t" << "nRow:" << rhs.m_rows << "\t" << "nCol:" << rhs.m_cols << std::endl;
//	for (size_t i = 0; i < rhs.m_rows; ++i)
//	{
//		for (size_t j = 0; j < rhs.m_cols; ++j)
//		{
//			os << rhs.m_array[i][j] << "\t";
//		}
//		os << std::endl;
//	}
//	return os;
//}