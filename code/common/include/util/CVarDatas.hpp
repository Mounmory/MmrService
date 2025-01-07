#ifndef COMMON_CVARIANT_HPP
#define COMMON_CVARIANT_HPP
#include "Common_def.h"
#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <unordered_map>

BEGINE_NAMESPACE(mmrUtil)


class CVariant//注意：string长度最大微255，大于部分将截断
{
public:
	enum class EM_DataType : uint16_t
	{
		VAR_TYPE_INVALID = 0,
		VAR_TYPE_BOOL,
		VAR_TYPE_CHAR,
		VAR_TYPE_INT32,
		VAR_TYPE_UINT32,
		VAR_TYPE_INT64,
		VAR_TYPE_UINT64,
		VAR_TYPE_FLOAT,
		VAR_TYPE_DOUBLE,
		VAR_TYPE_STRING,
		VAR_TYPE_BYTE_ARRAY
	};

	CVariant()
		: m_type(EM_DataType::VAR_TYPE_INVALID)	{
		m_data.i64Value = 0;
	}

	CVariant(bool bValue)
		:m_type(EM_DataType::VAR_TYPE_BOOL) {
		m_data.bValue = bValue;
	}

	CVariant(char cValue) 
	:m_type(EM_DataType::VAR_TYPE_CHAR){
		m_data.cValue = cValue;
	}

	CVariant(int32_t i32Value)
		:m_type(EM_DataType::VAR_TYPE_INT32) {
		m_data.i32Value = i32Value;
	}

	CVariant(uint32_t u32Value)
		:m_type(EM_DataType::VAR_TYPE_UINT32) {
		m_data.u32Value = u32Value;
	}

	CVariant(int64_t i64Value)
		:m_type(EM_DataType::VAR_TYPE_INT64) {
		m_data.i64Value = i64Value;
	}

	CVariant(uint64_t u64Value)
		:m_type(EM_DataType::VAR_TYPE_UINT64) {
		m_data.u64Value = u64Value;
	}

	CVariant(float fValue)
		:m_type(EM_DataType::VAR_TYPE_FLOAT) {
		m_data.fValue = fValue;
	}

	CVariant(double dValue)
		:m_type(EM_DataType::VAR_TYPE_DOUBLE) {
		m_data.dValue = dValue;
	}

	CVariant(const char* szValue)
		:m_type(EM_DataType::VAR_TYPE_STRING) {
		m_data.strValue = new std::string(szValue);
	}

	CVariant(const std::string& strValue)
		:m_type(EM_DataType::VAR_TYPE_STRING) {
		m_data.strValue = new std::string(strValue);
	}

	CVariant(std::string&& strValue)
		:m_type(EM_DataType::VAR_TYPE_STRING) {
		m_data.strValue = new std::string(std::forward<std::string>(strValue));
	}

	CVariant(const std::vector<char>& byteArrValue)
		:m_type(EM_DataType::VAR_TYPE_BYTE_ARRAY){
		m_data.byteArrValue = new std::vector<char>(byteArrValue);
	}

	CVariant(std::vector<char>&& byteArrValue)
		:m_type(EM_DataType::VAR_TYPE_BYTE_ARRAY) {
		m_data.byteArrValue = new std::vector<char>(std::forward<std::vector<char>>(byteArrValue));
	}

	CVariant(const CVariant& var)
	{
		*this = var;
	}

	CVariant(CVariant&& var) 
		:m_type(std::exchange(var.m_type,EM_DataType::VAR_TYPE_INVALID)){
		switch (this->m_type)
		{
		case EM_DataType::VAR_TYPE_INVALID:
		case EM_DataType::VAR_TYPE_BOOL:
		case EM_DataType::VAR_TYPE_CHAR:
		case EM_DataType::VAR_TYPE_INT32:
		case EM_DataType::VAR_TYPE_UINT32:
		case EM_DataType::VAR_TYPE_INT64:
		case EM_DataType::VAR_TYPE_UINT64:
		case EM_DataType::VAR_TYPE_FLOAT:
		case EM_DataType::VAR_TYPE_DOUBLE:
			this->m_data = var.m_data;
			break;
		case EM_DataType::VAR_TYPE_STRING:
		{
			m_data.strValue = std::exchange(var.m_data.strValue, nullptr);
		}
		break;
		case EM_DataType::VAR_TYPE_BYTE_ARRAY:
		{
			m_data.byteArrValue = std::exchange(var.m_data.byteArrValue, nullptr);
		}
		break;
		default:
			this->m_type = EM_DataType::VAR_TYPE_INVALID;
			break;
		}
	}

	~CVariant()
	{
		resetCheck();
	};

	CVariant& operator = (const CVariant& var)
	{
		if (this != &var)//避免自赋值
		{
			resetCheck();
			this->m_type = var.m_type;
			switch (m_type)
			{
			case EM_DataType::VAR_TYPE_INVALID:
			case EM_DataType::VAR_TYPE_BOOL:
			case EM_DataType::VAR_TYPE_CHAR:
			case EM_DataType::VAR_TYPE_INT32:
			case EM_DataType::VAR_TYPE_UINT32:
			case EM_DataType::VAR_TYPE_INT64:
			case EM_DataType::VAR_TYPE_UINT64:
			case EM_DataType::VAR_TYPE_FLOAT:
			case EM_DataType::VAR_TYPE_DOUBLE:
				this->m_data = var.m_data;
				break;
			case EM_DataType::VAR_TYPE_STRING:
				{
					m_data.strValue = new std::string(*(var.m_data.strValue));
				}
			break;
			case EM_DataType::VAR_TYPE_BYTE_ARRAY:
				{
					m_data.byteArrValue = new std::vector<char>(*(var.m_data.byteArrValue));
				}
			break;
			default:
				this->m_type = EM_DataType::VAR_TYPE_INVALID;
				break;
			}
		}
		return *this;
	}

	CVariant operator = (CVariant&& var) 
	{
		if (this != &var)//避免自赋值
		{
			resetCheck();
			this->m_type = std::exchange(var.m_type, EM_DataType::VAR_TYPE_INVALID);
			switch (this->m_type)
			{
			case EM_DataType::VAR_TYPE_INVALID:
			case EM_DataType::VAR_TYPE_BOOL:
			case EM_DataType::VAR_TYPE_CHAR:
			case EM_DataType::VAR_TYPE_INT32:
			case EM_DataType::VAR_TYPE_UINT32:
			case EM_DataType::VAR_TYPE_INT64:
			case EM_DataType::VAR_TYPE_UINT64:
			case EM_DataType::VAR_TYPE_FLOAT:
			case EM_DataType::VAR_TYPE_DOUBLE:
				this->m_data = var.m_data;
				break;
			case EM_DataType::VAR_TYPE_STRING:
			{
				m_data.strValue = std::exchange(var.m_data.strValue, nullptr);
			}
			break;
			case EM_DataType::VAR_TYPE_BYTE_ARRAY:
			{
				m_data.byteArrValue = std::exchange(var.m_data.byteArrValue, nullptr);
			}
			break;
			default:
				this->m_type = EM_DataType::VAR_TYPE_INVALID;
				break;
			}
		}
		return *this;
	}

	//显示的设置数据
	void setBoolData(bool bValue)
	{
		resetCheck();
		m_type = EM_DataType::VAR_TYPE_BOOL;
		m_data.bValue = bValue;
	}

	void setCharData(char cValue)
	{
		resetCheck();
		m_type = EM_DataType::VAR_TYPE_CHAR;
		m_data.cValue = cValue;
	}

	void setInt32Data(int32_t i32Value)
	{
		resetCheck();
		m_type = EM_DataType::VAR_TYPE_INT32;
		m_data.i32Value = i32Value;
	}

	void setUint32Data(uint32_t u32Value)
	{
		resetCheck();
		m_type = EM_DataType::VAR_TYPE_UINT32;
		m_data.u32Value = u32Value;
	}

	void setInt64Data(int64_t i64Value)
	{
		resetCheck();
		m_type = EM_DataType::VAR_TYPE_INT64;
		m_data.i64Value = i64Value;
	}

	void setUint64Data(uint64_t u64Value)
	{
		resetCheck();
		m_type = EM_DataType::VAR_TYPE_UINT64;
		m_data.u64Value = u64Value;
	}

	void setFloatData(float fValue)
	{
		resetCheck();
		m_type = EM_DataType::VAR_TYPE_FLOAT;
		m_data.fValue = fValue;
	}

	void setDoubleData(double dValue)
	{
		resetCheck();
		m_type = EM_DataType::VAR_TYPE_DOUBLE;
		m_data.dValue = dValue;
	}

	void setStringData(std::string strValue)
	{
		resetCheck();
		m_type = EM_DataType::VAR_TYPE_STRING;
		m_data.strValue = new std::string(std::move(strValue));
	}

	void setByteArrayData(std::vector<char> arrValue)
	{
		resetCheck();
		m_type = EM_DataType::VAR_TYPE_BYTE_ARRAY;
		m_data.byteArrValue = new std::vector<char>(std::move(arrValue));
	}

	//获取数据
	const bool getBoolData() const
	{
		if (EM_DataType::VAR_TYPE_BOOL != m_type) 
		{
			throw std::logic_error("variant m_type is not bool!");//对获取数据类型严格，避免隐式转换出bug
		}
		return m_data.bValue;
	}

	const char getCharData() const
	{
		if (EM_DataType::VAR_TYPE_CHAR != m_type)
		{
			throw std::logic_error("variant m_type is not char!");
		}
		return m_data.cValue;
	}

	const int32_t getInt32Data() const
	{
		if (EM_DataType::VAR_TYPE_INT32 != m_type) 
		{
			throw std::logic_error("variant m_type is not int32!");
		}
		return m_data.i32Value;
	}

	const uint32_t getUint32Data() const
	{
		if (EM_DataType::VAR_TYPE_UINT32 != m_type)
		{
			throw std::logic_error("variant m_type is not uint32!");
		}
		return m_data.u32Value;
	}

	const int64_t getInt64Data() const
	{
		if (EM_DataType::VAR_TYPE_INT64 != m_type)
		{
			throw std::logic_error("variant m_type is not int64!");
		}
		return m_data.i64Value;
	}

	const uint64_t getUint64Data() const
	{
		if (EM_DataType::VAR_TYPE_UINT64 != m_type)
		{
			throw std::logic_error("variant m_type is not uint64!");
		}
		return m_data.u64Value;
	}

	const float getFloatData() const
	{
		if (EM_DataType::VAR_TYPE_FLOAT != m_type) 
		{
			throw std::logic_error("variant m_type is not float!");
		}
		return m_data.fValue;
	}

	const double getDoubleData() const
	{
		if (EM_DataType::VAR_TYPE_DOUBLE != m_type)
		{
			throw std::logic_error("variant m_type is not double!");
		}
		return m_data.dValue;
	}

	const std::string& getStringData() const
	{
		if (EM_DataType::VAR_TYPE_STRING != m_type) 
		{
			throw std::logic_error("variant m_type is not string!");
		}
		return *(m_data.strValue);
	}

	const std::vector<char>& getByteArrayData() const
	{
		if (EM_DataType::VAR_TYPE_BYTE_ARRAY != m_type)
		{
			throw std::logic_error("variant m_type is not array!");
		}
		return *(m_data.byteArrValue);
	}

	EM_DataType getType() { return m_type; }

	const EM_DataType getType() const { return m_type; }

	template<typename _T>
	const _T CastToNum() const
	{
		static_assert(std::is_arithmetic<_T>::value, "Type _T is not convertible to a numeric type");
		switch (m_type)
		{
		case mmrUtil::CVariant::EM_DataType::VAR_TYPE_BOOL:
			return static_cast<_T>(m_data.bValue);
		case mmrUtil::CVariant::EM_DataType::VAR_TYPE_CHAR:
			return static_cast<_T>(m_data.cValue);
		case mmrUtil::CVariant::EM_DataType::VAR_TYPE_INT32:
			return static_cast<_T>(m_data.i32Value);
		case mmrUtil::CVariant::EM_DataType::VAR_TYPE_UINT32:
			return static_cast<_T>(m_data.u32Value);
		case mmrUtil::CVariant::EM_DataType::VAR_TYPE_INT64:
			return static_cast<_T>(m_data.i64Value);
		case mmrUtil::CVariant::EM_DataType::VAR_TYPE_UINT64:
			return static_cast<_T>(m_data.u64Value);
		case mmrUtil::CVariant::EM_DataType::VAR_TYPE_FLOAT:
			return static_cast<_T>(m_data.fValue);
		case mmrUtil::CVariant::EM_DataType::VAR_TYPE_DOUBLE:
			return static_cast<_T>(m_data.dValue);
		default:
			throw std::logic_error("cast to number failed! variant m_type is not number!");
			break;
		}
		return static_cast<_T>(0);
	}
private:
	void resetCheck()
	{
		switch (m_type)
		{
		case EM_DataType::VAR_TYPE_STRING:
		{
			delete m_data.strValue;
			m_data.strValue = nullptr;
		}
		break;
		case EM_DataType::VAR_TYPE_BYTE_ARRAY:
		{
			delete m_data.byteArrValue;
			m_data.byteArrValue = nullptr;
		}
		break;
		default:
			break;
		}
		//m_type = EM_DataType::VAR_TYPE_INVALID;
	}

private:
	union unData
	{
		//void* vPtr;
		bool bValue;
		char cValue;
		int32_t i32Value;
		uint32_t u32Value;
		int64_t i64Value;
		uint64_t u64Value;
		float fValue;
		double dValue;
		std::string* strValue;
		std::vector<char>* byteArrValue;
	};

	EM_DataType m_type = EM_DataType::VAR_TYPE_INVALID;
	unData m_data;
};

//数据解析类
class CVarDatas
{
	using MapVars = std::unordered_map<std::string, CVariant>;
public:
	CVarDatas()	
		:m_pMapVars(std::make_unique<MapVars>())
		, m_ulType(0)
	{
	}

	CVarDatas(const CVarDatas& rhs)
		:m_pMapVars(std::make_unique<MapVars>(*rhs.m_pMapVars))
		, m_strName(rhs.m_strName)
		, m_ulType(rhs.m_ulType)
	{
	}

	CVarDatas(CVarDatas&& rhs)
		:m_pMapVars(std::move(rhs.m_pMapVars))
		, m_strName(std::move(rhs.m_strName))
		, m_ulType(std::exchange(rhs.m_ulType,0))
	{
	}

	~CVarDatas() = default;

	CVarDatas& operator = (const CVarDatas& rhs) 
	{
		*m_pMapVars = *rhs.m_pMapVars;
		m_strName = rhs.m_strName;
		m_ulType = rhs.m_ulType;
	}

	CVarDatas& operator = (CVarDatas&& rhs)
	{
		m_pMapVars = std::move(rhs.m_pMapVars);
		m_strName = std::move(rhs.m_strName);
		m_ulType = std::exchange(rhs.m_ulType, 0);
	}

	bool const isContain(const std::string& strKey) const
	{
		return m_pMapVars->count(strKey) != 0;
	}

	std::vector<std::string> getAllKey() 
	{
		std::vector<std::string> vecStr;
		if (vecStr.capacity() < m_pMapVars->size())
		{
			vecStr.reserve(m_pMapVars->size());
		}

		for (const auto& iter : (*m_pMapVars))
		{
			vecStr.emplace_back(iter.first);
		}
		return vecStr;
	}

	void addVar(std::string strKey, CVariant var) 
	{
		m_pMapVars->insert(std::make_pair(std::move(strKey), var));
	}

	const CVariant& getVar(const std::string& strKey) const
	{
		auto iterVar = m_pMapVars->find(strKey);
		if (iterVar == m_pMapVars->end())
		{
			throw std::invalid_argument(std::string("invalid key " + strKey).c_str());
		}
		return iterVar->second;
	}

	void setName(std::string strName) { m_strName = std::move(strName); }

	const std::string& getName() const { return m_strName; }

	void setType(uint32_t ulType) { m_ulType = ulType; }

	const uint32_t getType() const { return m_ulType; }

private:
	std::unique_ptr<MapVars> m_pMapVars;
	std::string m_strName;
	uint32_t m_ulType;//事件类型，与strName二选一使用，用于枚举
};



END_NAMESPACE(mmrUtil)

#endif


