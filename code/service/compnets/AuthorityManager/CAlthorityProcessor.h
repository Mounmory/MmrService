#ifndef CALTHORITYPROCESS_H
#define CALTHORITYPROCESS_H
#include "Common_def.h"
#include "Login.pb.h"

#include "CUserDatabase.h"

#include <memory>

BEGINE_NAMESPACE(mmrService)
BEGINE_NAMESPACE(mmrComp)

class CAlthorityProcessor 
{
public:
	CAlthorityProcessor();

	~CAlthorityProcessor();

	//登录
	std::shared_ptr<mmrService::LoginResponse> onLogin(const std::shared_ptr<mmrService::LoginRequest>& request);

	//添加用户

	//删除用户

	//查询用户

private:
	std::unique_ptr<CUserDatabase> m_usrDBPtr;
};


END_NAMESPACE(mmrService)
END_NAMESPACE(mmrComp)

#endif
