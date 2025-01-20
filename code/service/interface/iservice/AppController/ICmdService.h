#ifndef ICMDSERVICE_H
#define ICMDSERVICE_H

#include "common/include/Common_def.h"
#include "service/core/include/ServiceCtrlPolicies.hpp"
//#include "iservice/InterfaceCommon.h"

BEGINE_NAMESPACE(mmrService)
BEGINE_NAMESPACE(mmrComp)

class ICmdService
{
	IMPLEMENT_INDEXABLE_CLASS
	IMPLEMENT_GUID_CLASS("F2C2688E-C687-4541-B120-F5800C136774")
public:
	ICmdService() = default;
	virtual ~ICmdService() = default;

	

	virtual void cmdLoop() = 0;
};

END_NAMESPACE(mmrComp)
END_NAMESPACE(mmrService)
#endif // !ICMDSERVICE
