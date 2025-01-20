#ifndef CCMDSERVICE_H
#define CCMDSERVICE_H
#include "service/interface/iservice/AppController/ICmdService.h"

BEGINE_NAMESPACE(mmrService)
BEGINE_NAMESPACE(mmrComp)

class CCmdService : public ICmdService
{
public:
	CCmdService();
	~CCmdService();

	virtual void cmdLoop() override;
};


END_NAMESPACE(mmrComp)
END_NAMESPACE(mmrService)
#endif // !CCMDSERVICE_H
