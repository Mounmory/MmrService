#ifndef PROTOCODEC_H
#define PROTOCODEC_H
/*
	protobuf网络传输编解码相关
*/
#include "google/protobuf/message.h"
#include "common/include/protoBase/ProtobufDef.h"
#include "common/include/util/UtilFunc.h"
#include "common/include/util/BufConvert.h"
#include "Buffer.h"

#define PROTO_MSG_MIN_LEN 16 //proto数据最小长度

#define SEQUE_NUM_MSG_PUSH 0 //数据推送顺序号
class ProtoRpcMessage
{
public:
	ProtoRpcMessage() 
		: ulSequeNum(0)
		, usNameLen(0)
		, lMsgLen(0)
		, protoMsgPtr(nullptr)
		, lCheckSum(0)
	{
	}
	~ProtoRpcMessage() = default;

public:
	uint32_t	ulSequeNum;	//顺序号
	uint16_t	usNameLen;	//名称长度
	std::string	strName;	//请求名称
	int32_t		lMsgLen;	//信息长度
	MessagePtr protoMsgPtr;
	int32_t		lCheckSum;	//校验码
};

namespace 
{
	/// the order of bytes that are on the left
	enum class emEndian //计算机大小端
	{
		UNKNOW = -1,
		BIG = 0,
		LITTLE = 1,
		
	};

	emEndian g_endian = emEndian::UNKNOW;

	inline void initEndian() 
	{
		if (emEndian::UNKNOW == g_endian)
		{
			long one(1);
			char e = (reinterpret_cast<char*>(&one))[0];

			if (e == (char)1)
			{
				g_endian = emEndian::LITTLE;
			}
			else
			{
				g_endian = emEndian::BIG;
			}
		}
	}

	//报文以大端模式发送，对于小端计算机，翻转字节
	void protoFlipBuf(void* buf, size_t bufSize)
	{
		initEndian();

		if (emEndian::BIG != g_endian &&  bufSize > 2)
		{
			return mmrUtil::flipData(buf, bufSize);
		}
	}

	MessagePtr createMessage(const std::string& typeName)
	{
		MessagePtr message = nullptr;
		const google::protobuf::Descriptor* descriptor =
			google::protobuf::DescriptorPool::generated_pool()->FindMessageTypeByName(typeName);
		if (descriptor)
		{
			const google::protobuf::Message* prototype =
				google::protobuf::MessageFactory::generated_factory()->GetPrototype(descriptor);
			if (prototype)
			{
				message.reset(prototype->New());
			}
		}
		return message;
	}

	//解码buf到proto数据
	EM_ErrCode decodingFromBuf(ProtoRpcMessage* protoMsg, hv::Buffer* buf) 
	{
		EM_ErrCode ret = EM_ErrCode::ERR_UNDEFINE;
		do 
		{
			if (!protoMsg || !buf) 
			{
				ret = EM_ErrCode::ERR_NULL_PTR;
				break;
			}

			//检查数据长度
			if (buf->len < PROTO_MSG_MIN_LEN)
			{
				ret = EM_ErrCode::ERR_INVALID_LENGTH;
				break;
			}

			//校验码
			int32_t lcheckSum = mmrUtil::adler32((const char*)buf->data(), buf->len - 4);

			unsigned char* pData = (unsigned char*)buf->data() + buf->len - 4;

			int32_t lBufSum = 0;
			protoFlipBuf(pData, sizeof(lBufSum));
			memcpy(&lBufSum, pData, sizeof(lBufSum));
			pData += sizeof(lBufSum);

			if (lcheckSum != lBufSum)
			{
				ret = EM_ErrCode::ERR_CHECK_SUM;
				break;
			}

			//顺序号
			pData = (unsigned char*)buf->data();
			protoFlipBuf(pData, sizeof(protoMsg->ulSequeNum));
			memcpy(&protoMsg->ulSequeNum, pData, sizeof(protoMsg->ulSequeNum));
			pData += sizeof(protoMsg->ulSequeNum);
			
			//名字长度
			protoFlipBuf(pData, sizeof(protoMsg->usNameLen));
			memcpy(&protoMsg->usNameLen, pData, sizeof(protoMsg->usNameLen));
			pData += sizeof(protoMsg->usNameLen);

			protoMsg->strName.append((char*)pData, (char*)(pData + protoMsg->usNameLen - 1));

			protoMsg->protoMsgPtr = createMessage(protoMsg->strName) ;

			if (!protoMsg->protoMsgPtr)
			{
				ret = EM_ErrCode::ERR_UNKNOWN_MSG_TYPE;
				break;
			}

			pData += protoMsg->usNameLen;

			//protobuf数据长度
			protoFlipBuf(pData, sizeof(protoMsg->lMsgLen));
			memcpy(&protoMsg->lMsgLen, pData, sizeof(protoMsg->lMsgLen));
			pData += sizeof(protoMsg->lMsgLen);
			
			if (!protoMsg->protoMsgPtr->ParseFromArray(pData, protoMsg->lMsgLen))
			{
				ret = EM_ErrCode::ERR_PARSE;
				break;
			}

			ret = EM_ErrCode::ERR_NONE;
		} while (false);

		return ret;
	}
	
	// proto数据转成buf
	EM_ErrCode codingToBuf(ProtoRpcMessage* protoMsg, hv::Buffer* buf)
	{
		EM_ErrCode ret = EM_ErrCode::ERR_UNDEFINE;
		do
		{
			if (!protoMsg || !protoMsg->protoMsgPtr || !buf)
			{
				ret = EM_ErrCode::ERR_NULL_PTR;
				break;
			}

			protoMsg->strName = protoMsg->protoMsgPtr->GetTypeName();
			protoMsg->usNameLen = protoMsg->strName.size() + 1;

			int32_t lBufSize = protoMsg->protoMsgPtr->ByteSize();
			buf->resize(14 + protoMsg->usNameLen + lBufSize);
			unsigned char* p = (unsigned char*)(buf->data());

			//顺序号
			memcpy(p, &protoMsg->ulSequeNum, sizeof(protoMsg->ulSequeNum));
			protoFlipBuf(p, sizeof(protoMsg->ulSequeNum));
			p += sizeof(protoMsg->ulSequeNum);

			//名字长度
			memcpy(p, &protoMsg->usNameLen, sizeof(protoMsg->usNameLen));
			protoFlipBuf(p, sizeof(protoMsg->usNameLen));
			p += sizeof(protoMsg->usNameLen);

			//名字
			memcpy(p, protoMsg->strName.c_str(), protoMsg->usNameLen);
			p += protoMsg->usNameLen;

			//buf长度
			memcpy(p, &lBufSize, sizeof(lBufSize));
			protoFlipBuf(p, sizeof(lBufSize));
			p += sizeof(lBufSize);

			//序列化
			if (!protoMsg->protoMsgPtr->SerializeToArray(p, lBufSize))
			{
				ret = EM_ErrCode::ERR_PARSE;
				break;
			}

			int32_t lcheckSum = mmrUtil::adler32(buf->base, buf->len - 4);

			p += lBufSize;

			//校验码
			memcpy(p, &lcheckSum, sizeof(lcheckSum));
			protoFlipBuf(p, sizeof(lcheckSum));
			p += sizeof(lcheckSum);

			ret = EM_ErrCode::ERR_NONE;
		} while (false);

		return ret;
	}

}

#endif