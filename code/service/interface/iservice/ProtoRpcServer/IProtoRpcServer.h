#ifndef IRPCDISPATCHER_H
#define IRPCDISPATCHER_H

#include "Common_def.h"
#include "ServiceCtrlPolicies.hpp"
//#include "iservice/InterfaceCommon.h"
#include "util/UtilFunc.h"

#include "protoBase/ProtobufDef.h"
#include "protoBase/ProtoMsgCallback.h"

BEGINE_NAMESPACE(mmrService)
BEGINE_NAMESPACE(mmrComp)

class IProtoRpcServer
{
	IMPLEMENT_INDEXABLE_CLASS
	IMPLEMENT_GUID_CLASS("27F32F5F-BBE8-4515-8A5D-EEACFAFACFA5")
protected:
	IProtoRpcServer() = default;

	virtual ~IProtoRpcServer(){
		m_mapCallbacks_.clear();
	}

public:
	//virtual MessagePtr dealProtobufMessage(const MessagePtr& request) = 0;

	template <typename Request, typename Response>
	void registerMessageCallback(typename ProtoRpcCallbackT<Request, Response>::ProtobufMessageTCallbackFunc callback)
	{
		std::shared_ptr<ProtoRpcCallbackT<Request, Response> > pd(new ProtoRpcCallbackT<Request, Response>(std::move(callback)));
		auto descrip = Request::descriptor();
		if (m_mapCallbacks_.find(descrip) != m_mapCallbacks_.end())
		{
			STD_CERROR << "call backe request hase been added to map!" << std::endl;
		}
		else
		{
			m_mapCallbacks_[Request::descriptor()] = pd;
		}
	}

protected:
	typedef std::map<const google::protobuf::Descriptor*, std::shared_ptr<ProtoRpcCallback> > CallbackMap;

	CallbackMap m_mapCallbacks_;
};

END_NAMESPACE(mmrService)
END_NAMESPACE(mmrComp)
#endif