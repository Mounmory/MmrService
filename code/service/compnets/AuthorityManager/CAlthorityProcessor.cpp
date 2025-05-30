﻿#include <common/include/util/MemoryPool.hpp>
#include "CAlthorityProcessor.h"

using namespace mmrService::mmrComp;

CAlthorityProcessor::CAlthorityProcessor()
	: m_usrDBPtr(std::make_unique<CUserDatabase>())
{
	m_usrDBPtr->init();
}

CAlthorityProcessor::~CAlthorityProcessor()
{

}

//登录失败返回空指针
std::shared_ptr<mmrService::LoginResponse> CAlthorityProcessor::onLogin(const std::shared_ptr<mmrService::LoginRequest>& request)
{
	//std::cout << "login request data \n" << request->DebugString();

	std::shared_ptr<mmrService::LoginResponse> response = mmrUtil::Make_Shared<mmrService::LoginResponse>();
	response->set_errcode(1000);
	response->set_message(u8"登录成功！");
	//std::cout << "login response data \n" << response->DebugString();
	return response;
}
