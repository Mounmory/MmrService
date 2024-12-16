#ifndef PROTO_MSG_CALLBACK_H
#define PROTO_MSG_CALLBACK_H

/*
服务端处理protobuf格式request和response的回调函数
*/
#include "Noncopyable.h"
#include "protoBase/ProtobufDef.h"

#include <functional>
#include <memory>



class ProtoRpcCallback : mmrComm::NonCopyable
{
public:
	ProtoRpcCallback() = default;
	virtual ~ProtoRpcCallback() = default;
	virtual MessagePtr onMessage(const MessagePtr& request) const = 0;
};


template <typename Request, typename Response>
class ProtoRpcCallbackT : public ProtoRpcCallback
{
	static_assert(std::is_base_of<google::protobuf::Message, Response>::value,
		"Response type must be derived from google::protobuf::Message.");
	static_assert(std::is_base_of<google::protobuf::Message, Request>::value,
		"Request type must be derived from google::protobuf::Message.");
public:
	using ProtobufMessageTCallbackFunc = std::function<std::shared_ptr<Response>(const std::shared_ptr<Request>&)>;

	ProtoRpcCallbackT(ProtobufMessageTCallbackFunc callbackFunc)
		: m_callbackFunc(std::move(callbackFunc))
	{
	}

	~ProtoRpcCallbackT() = default;

	virtual MessagePtr onMessage(const MessagePtr& request) const override
	{
#if 0//debug测试用
		std::shared_ptr<Request> concrete = std::dynamic_pointer_cast<Request>(request);
		assert(concrete != nullptr);
#else
		std::shared_ptr<Request> concrete = std::static_pointer_cast<Request>(request);
#endif
		
		return (m_callbackFunc(concrete));
	}

private:
	ProtobufMessageTCallbackFunc m_callbackFunc;
};

#endif