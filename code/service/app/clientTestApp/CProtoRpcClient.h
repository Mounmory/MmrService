#ifndef CPROTORPCCLIENT_H
#define CPROTORPCCLIENT_H

#include "common/include/protoBase/ProtobufDef.h"
#include "TcpClient.h"
#include <condition_variable>
#include <mutex>

class ProtoRpcContext {

public:
	void wait(int timeout_ms) {
		std::unique_lock<std::mutex> locker(_mutex);
		_cond.wait_for(locker, std::chrono::milliseconds(timeout_ms));
	}

	void notify() {
		_cond.notify_one();
	}

	MessagePtr requestPtr;
	MessagePtr responsePtr;
private:
	std::mutex              _mutex;
	std::condition_variable _cond;


};
//
//template <typename RequestPtr, typename ResponsePtr>
//class  ProtoRpcContextT : public ProtoRpcCallback
//{
//	static_assert(std::is_base_of<google::protobuf::Message, ResponsePtr>::value,
//		"Response type must be derived from google::protobuf::Message.");
//	static_assert(std::is_base_of<google::protobuf::Message, RequestPtr>::value,
//		"Request type must be derived from google::protobuf::Message.");
//public:
//	RequestPtr	reqPtr;
//	ResponsePtr	resPtr;
//};

typedef std::shared_ptr<ProtoRpcContext>    ContextPtr;

class CProtoRpcClient : public hv::TcpClient
{
public:
	CProtoRpcClient();

	~CProtoRpcClient();

	int connect(std::string strHost, uint16_t port);

	MessagePtr call(MessagePtr req, int timeout_ms = 10000);

	const int32_t getConnectState() const { return m_connState.load(std::memory_order_relaxed); }

	enum emConnState : int32_t
	{
		kInitialized,
		kConnecting,
		kConnected,
		kDisconnectd
	};

protected:
	void onDealConnection(const hv::SocketChannelPtr& channel);

	void onDealMessage(const hv::SocketChannelPtr& channel, hv::Buffer* buf);
private:
	std::atomic<emConnState> m_connState;

	std::map<uint32_t, ContextPtr> m_calls;//所有调用
	std::mutex m_callsMutex;//调用互斥量
};



#endif