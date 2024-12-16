#include "CProtoRpcClient.h"
#include "util/Clogger.h"
#include "protoBase/ProtoCodec.hpp"

CProtoRpcClient::CProtoRpcClient()
{
	m_connState.store(kInitialized);

	setConnectTimeout(5000);

	reconn_setting_t reconn;
	reconn_setting_init(&reconn);
	reconn.min_delay = 1000;
	reconn.max_delay = 10000;
	reconn.delay_policy = 2;
	setReconnect(&reconn);

	//// init protorpc_unpack_setting
	//unpack_setting_t protorpc_unpack_setting;
	//memset(&protorpc_unpack_setting, 0, sizeof(unpack_setting_t));
	//protorpc_unpack_setting.mode = UNPACK_BY_LENGTH_FIELD;
	//protorpc_unpack_setting.package_max_length = DEFAULT_PACKAGE_MAX_LENGTH;
	////protorpc_unpack_setting.body_offset = PROTORPC_HEAD_LENGTH;
	////protorpc_unpack_setting.length_field_offset = PROTORPC_HEAD_LENGTH_FIELD_OFFSET;
	////protorpc_unpack_setting.length_field_bytes = PROTORPC_HEAD_LENGTH_FIELD_BYTES;
	//protorpc_unpack_setting.length_field_coding = ENCODE_BY_BIG_ENDIAN;
	//setUnpack(&protorpc_unpack_setting);

	onConnection = std::bind(&CProtoRpcClient::onDealConnection, this, std::placeholders::_1);

	onMessage = std::bind(&CProtoRpcClient::onDealMessage, this, std::placeholders::_1, std::placeholders::_2);
}

CProtoRpcClient::~CProtoRpcClient()
{

}

int CProtoRpcClient::connect(std::string strHost, uint16_t port)
{
	createsocket(port, strHost.c_str());
	m_connState.store(kConnecting);
	start();
	return 0;
}

MessagePtr CProtoRpcClient::call(MessagePtr req, int timeout_ms /*= 10000*/)
{
	if (m_connState != kConnected) 
	{
		return nullptr;
	}
	static std::atomic<uint32_t> s_id = ATOMIC_VAR_INIT(0);
	ProtoRpcMessage protoMsg;
	protoMsg.ulSequeNum = ++s_id;
	protoMsg.protoMsgPtr = req;

	hv::Buffer bufSend;
	auto ret = codingToBuf(&protoMsg, &bufSend);

	auto ctx = std::make_shared<ProtoRpcContext>();
	ctx->requestPtr = req;

	{//将上下文添加到map
		std::lock_guard<std::mutex> lock(m_callsMutex);
		m_calls[protoMsg.ulSequeNum] = ctx;
	}
	

	channel->write(bufSend.base, bufSend.len);

	// wait until response come or timeout
	ctx->wait(timeout_ms);
	auto res = ctx->responsePtr;

	{//将上下移出到map
		std::lock_guard<std::mutex> lock(m_callsMutex);
		m_calls.erase(protoMsg.ulSequeNum);
	}
	
	return res;
}

void CProtoRpcClient::onDealConnection(const hv::SocketChannelPtr& channel)
{
	std::string peeraddr = channel->peeraddr();
	if (channel->isConnected()) 
	{
		m_connState.store(kConnected);
		LOG_INFO("connected to %s! connfd=%d\n", peeraddr.c_str(), channel->fd());
		printf("connected to %s! connfd=%d\n", peeraddr.c_str(), channel->fd());
	}
	else {
		m_connState.store(kDisconnectd);
		LOG_INFO("disconnected to %s! connfd=%d\n", peeraddr.c_str(), channel->fd());
		printf("disconnected to %s! connfd=%d\n", peeraddr.c_str(), channel->fd());
	}
}

void CProtoRpcClient::onDealMessage(const hv::SocketChannelPtr& channel, hv::Buffer* buf)
{
	ProtoRpcMessage protoRcv;
	EM_ErrCode errCode = decodingFromBuf(&protoRcv, buf);
	if (EM_ErrCode::ERR_NONE == errCode)
	{
		LOG_DEBUG("deal recieve, sequ num[%ld] message name[%s] message %s"
			, protoRcv.ulSequeNum
			, protoRcv.strName.c_str()
			, protoRcv.protoMsgPtr->DebugString().c_str());

		std::lock_guard<std::mutex> lock(m_callsMutex);
		auto iterContex = m_calls.find(protoRcv.ulSequeNum);
		if (iterContex != m_calls.end())
		{
			iterContex->second->responsePtr = std::move(protoRcv.protoMsgPtr);
			iterContex->second->notify();
		}
		else//超时数据或服务推送的数据 
		{
			if (SEQUE_NUM_MSG_PUSH == protoRcv.ulSequeNum)
			{
			}
			else//超时的数据 
			{
				LOG_WARN("time out data seque num %ld", protoRcv.ulSequeNum);
			}
		}
	}
}