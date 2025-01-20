#ifndef CPROTOSERVER_H
#define CPROTOSERVER_H

#include "TcpServer.h"
#include "service/interface/iservice/ProtoRpcServer/IProtoRpcServer.h"

BEGINE_NAMESPACE(mmrService)
BEGINE_NAMESPACE(mmrComp)

class CProtoServer: public hv::TcpServer, public IProtoRpcServer
{
public:
	CProtoServer();
	~CProtoServer();


public:
	void setConnectInfo(std::string strIP, uint16_t usPort);

	int createSocket();

private:
	void onDealConnection(const hv::SocketChannelPtr& channel);

	void onDealMessage(const hv::SocketChannelPtr& channel, hv::Buffer* buf);

	/*
		����ͻ��˵�һ����Ϣ��ӦΪ��¼��Ϣ
	*/
	void onDealLoginMessage(const hv::SocketChannelPtr& channel, hv::Buffer* buf);

	MessagePtr dealProtobufMessage(const MessagePtr& request);
private:
	//���������ж���
	//std::string m_strIP;
	//uint16_t m_usPort;

};

END_NAMESPACE(mmrService)
END_NAMESPACE(mmrComp)
#endif
