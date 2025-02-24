#ifndef BUFCONVERT_H
#define BUFCONVERT_H
#include "common/include/Common_def.h"
#include "common/include/util/UtilExport.h"
#include <string>
#include <string.h>

#define BASE64_ENCODE_OUT_SIZE(s)   (((s) + 2) / 3 * 4)//base64编码后大小

#define BASE64_DECODE_OUT_SIZE(s)   (((s)) / 4 * 3)

BEGINE_NAMESPACE(mmrUtil)

//alder32校验算法
COMMON_FUN_API int32_t adler32(const char * buf, uint32_t len);

//根据大小端翻转字节序
COMMON_FUN_API void flipData(void* buf, size_t bufSize);

//编码成base64,返回编码长度
COMMON_FUN_API uint32_t base64Encode(const void* inBuf, uint32_t inlen, char *outBuf);

//解码base64，返回解码长度
COMMON_FUN_API uint32_t base64Decode(const void *inBuf, uint32_t inlen, uint8_t* outBuf);

static inline std::string inBase64Encode(const void* data, uint32_t len)
{
	uint32_t encoded_size = BASE64_ENCODE_OUT_SIZE(len);
	std::string encoded_str(encoded_size + 1, 0);
	encoded_size = base64Encode(data, len, (char*)encoded_str.data());
	encoded_str.resize(encoded_size);
	return encoded_str;
}

static inline std::string inBase64Decode(const char* str, uint32_t len = 0) {
	if (len == 0)
		len = strlen(str);
	int decoded_size = BASE64_DECODE_OUT_SIZE(len);
	std::string decoded_buf(decoded_size + 1, 0);
	decoded_size = base64Decode(str, len, (uint8_t*)decoded_buf.data());
	decoded_buf.resize(decoded_size);
	return decoded_buf;
}




END_NAMESPACE(mmrUtil)
#endif