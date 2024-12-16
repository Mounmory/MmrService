#ifndef ICMDSERVICE_H
#define ICMDSERVICE_H

#include "Common_def.h"
#include "iservice/InterfaceCommon.h"

BEGINE_NAMESPACE(mmrService)
BEGINE_NAMESPACE(mmrComp)

class ICmdService
{
	//INTERFACE_GUID_DEFINE();
public:
	ICmdService() = default;
	virtual ~ICmdService() = default;

	virtual void cmdLoop() = 0;
};

END_NAMESPACE(mmrComp)
END_NAMESPACE(mmrService)
#endif // !ICMDSERVICE
