#ifndef CTYPEINFO_HPP
#define CTYPEINFO_HPP

#include "Common_def.h"
#include <typeinfo>

BEGINE_NAMESPACE(mmrComm)

class CTypeInfo
{
public:
	// Constructors
	CTypeInfo() // needed for containers
	{
		class Nil {};
		pInfo_ = &typeid(Nil);
	}
	CTypeInfo(const std::type_info& ti)// non-explicit
		: pInfo_(&ti)
	{
	}

	// Access for the wrapped std::type_info
	const std::type_info& Get() const { return *pInfo_; }

	// Compatibility functions
	bool before(const CTypeInfo& rhs) const { return pInfo_->before(*rhs.pInfo_) != 0; }

	const char* name() const { return pInfo_->name(); }

	bool operator==(const CTypeInfo& rhs) const { return (this->Get() == rhs.Get()) != 0; }

	//bool operator==(const std::type_info& ti) const { return this->Get() == ti; }

	bool operator!=(const CTypeInfo& rhs) const { return !(*this == rhs); }

	bool operator<(const CTypeInfo& rhs) const { return this->before(rhs); }

	bool operator>(const CTypeInfo& rhs) const { return (*this) > rhs; }

	bool operator<=(const CTypeInfo& rhs) const { return !((*this) > rhs); }

	bool operator>=(const CTypeInfo& rhs) const { return !((*this) < rhs); }

private:
	const std::type_info* pInfo_;
};

END_NAMESPACE(mmrComm)
#endif