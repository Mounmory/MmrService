#include "CProtoServer.h"
#include "util/Clogger.h"
#include "protoBase/ProtoCodec.hpp"

using namespace mmrService::mmrComp;

CProtoServer::CProtoServer()
	//: m_usPort(0)
	//, m_rpcDispatcher(nullptr)
{
	// init protorpc_unpack_setting
	//unpack_setting_t protorpc_unpack_setting;
	//memset(&protorpc_unpack_setting, 0, sizeof(unpack_setting_t));
	//protorpc_unpack_setting.mode = UNPACK_BY_LENGTH_FIELD;
	//protorpc_unpack_setting.package_max_length = DEFAULT_PACKAGE_MAX_LENGTH;
	//protorpc_unpack_setting.body_offset = PROTORPC_HEAD_LENGTH;
	//protorpc_unpack_setting.length_field_offset = PROTORPC_HEAD_LENGTH_FIELD_OFFSET;
	//protorpc_unpack_setting.length_field_bytes = PROTORPC_HEAD_LENGTH_FIELD_BYTES;
	//protorpc_unpack_setting.length_field_coding = ENCODE_BY_BIG_ENDIAN;
	//setUnpack(&protorpc_unpack_setting);

	//�����Ӻ���
	onConnection = std::bind(&CProtoServer::onDealConnection, this, std::placeholders::_1);

	//��Ϣ��������
	onMessage = std::bind(&CProtoServer::onDealMessage, this, std::placeholders::_1, std::placeholders::_2);
}

CProtoServer::~CProtoServer()
{

}

void CProtoServer::setConnectInfo(std::string strIP, uint16_t usPort)
{
	host = std::move(strIP);
	port = usPort;
}


int CProtoServer::createSocket()
{
	//listenfd = Listen(m_usPort, m_strIP.c_str());
	listenfd = Listen(port, host.c_str());
	if (listenfd < 0)
		STD_CERROR << "TCP service start failed, listen on IP[" << host.c_str() << "] port[" << port << "]." << std::endl;
	else 
		std::cout << "TCP service start success,IP[" << host.c_str() << "] port[" << port << "]." << std::endl;

	return listenfd;
}

void CProtoServer::onDealConnection(const hv::SocketChannelPtr& channel)
{
	std::string peeraddr = channel->peeraddr();
	if (channel->isConnected())
	{
		LOG_INFO("%s connected, fd[%ld] connection count is %ld", channel->peeraddr().c_str(), channel->fd(), this->connectionNum());
		//channel->close(true);
	}
	else
	{
		LOG_INFO("%s disconnected, fd[%ld] connection count is %ld", channel->peeraddr().c_str(), channel->fd(), this->connectionNum() - 1);
	}
}

void CProtoServer::onDealMessage(const hv::SocketChannelPtr& channel, hv::Buffer* buf)
{
	if (!channel->getClientState())//δ��¼�ͻ���������
	{
		return onDealLoginMessage(channel, buf);
	}

	ProtoRpcMessage protoRequest;
	ProtoRpcMessage protoResponse;

	EM_ErrCode errCode = decodingFromBuf(&protoRequest, buf);
	if (EM_ErrCode::ERR_NONE == errCode)
	{
		LOG_DEBUG("deal request, sequ num[%ld] message name[%s] message\n%s"
			, protoRequest.ulSequeNum
			, protoRequest.strName.c_str()
			, protoRequest.protoMsgPtr->DebugString().c_str());

		auto response = dealProtobufMessage(protoRequest.protoMsgPtr);
		if (response)
		{
			protoResponse.protoMsgPtr = std::move(response);
			protoResponse.ulSequeNum = protoRequest.ulSequeNum;

			hv::Buffer outBuf;
			codingToBuf(&protoResponse, &outBuf);

			LOG_DEBUG("send response, sequ num[%ld] message name[%s] message\n%s"
				, protoResponse.ulSequeNum
				, protoResponse.strName.c_str()
				, protoResponse.protoMsgPtr->DebugString().c_str());

			channel->write(&outBuf);
		}
		else 
		{
			channel->close(true);
			LOG_ERROR("generate response failed! close socket [%s]!", channel->peeraddr().c_str());
		}
	}
	else
	{
		channel->close(true);
		LOG_ERROR("buf from %s decoding error, error code [%d], close socket!", channel->peeraddr().c_str(), errCode);
	}
}

void CProtoServer::onDealLoginMessage(const hv::SocketChannelPtr& channel, hv::Buffer* buf)
{
	ProtoRpcMessage protoRequest;
	ProtoRpcMessage protoResponse;

	EM_ErrCode errCode = decodingFromBuf(&protoRequest, buf);
	if (EM_ErrCode::ERR_NONE == errCode && protoRequest.protoMsgPtr->GetTypeName() == "mmrService.LoginRequest")
	{
		LOG_DEBUG("deal request, sequ num[%ld] message name[%s] message\n%s"
			, protoRequest.ulSequeNum
			, protoRequest.strName.c_str()
			, protoRequest.protoMsgPtr->DebugString().c_str());

		auto response = dealProtobufMessage(protoRequest.protoMsgPtr);
		if (response)
		{
			channel->setClientState(true);//�ͻ��˱��Ϊ�Ϸ�

			protoResponse.protoMsgPtr = std::move(response);
			protoResponse.ulSequeNum = protoRequest.ulSequeNum;

			hv::Buffer outBuf;
			codingToBuf(&protoResponse, &outBuf);

			LOG_DEBUG("send response, sequ num[%ld] message name[%s] message\n%s"
				, protoResponse.ulSequeNum
				, protoResponse.strName.c_str()
				, protoResponse.protoMsgPtr->DebugString().c_str());

			channel->write(&outBuf);
		}
		else
		{
			channel->close(true);
			LOG_ERROR("login info error! close socket [%s]!", channel->peeraddr().c_str());
		}
	}
	else
	{
		channel->close(true);
		LOG_ERROR("illegal buf from %s , error code [%d], close socket!", channel->peeraddr().c_str(), errCode);
	}
}

MessagePtr CProtoServer::dealProtobufMessage(const MessagePtr& request)
{
	auto it = m_mapCallbacks_.find(request->GetDescriptor());
	if (it == m_mapCallbacks_.end())
	{
		LOG_ERROR("unregister requset [%s]", request->GetTypeName().c_str());
		return nullptr;
	}
	return it->second->onMessage(request);
}