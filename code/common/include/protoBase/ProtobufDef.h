/**
 * @file ProtobufDef.h
 * @brief 一些公共信息定义
 * @author Mounmory (237628106@qq.com) https://github.com/Mounmory
 * @date 
 *
 * 
 */

#ifndef PROTOBUF_DEF_H
#define PROTOBUF_DEF_H
#include <google/protobuf/message.h>

//错误枚举值
enum EM_ErrCode : int32_t
{
	ERR_UNDEFINE = -1,	//未定义错误
	ERR_NONE = 0,	//无错误
	ERR_NULL_PTR,	//空指针

	//解析protoBuf错误
	ERR_INVALID_LENGTH = 0x0200,//不可用的数据长度
	ERR_CHECK_SUM,//校验错误
	ERR_INVALIDE_NAME_LEN,//ProtBuf数据名字长度错误
	ERR_UNKNOWN_MSG_TYPE,//不能识别的消息类型
	ERR_PARSE//解析Message失败

};

using MessageProto = google::protobuf::Message;
using MessagePtr = std::shared_ptr<MessageProto>;


#endif
