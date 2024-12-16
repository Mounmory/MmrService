#include <stdio.h>
#include <vector>
#include "util/BufConvert.h"

/* ==========================alder32校验相关全局定义 begin==================================== */
#define BASE 65521U     /* largest prime smaller than 65536 */
#define NMAX 5552
/* NMAX is the largest n such that 255n(n+1)/2 + (n+1)(BASE-1) <= 2^32-1 */

#define DO1(buf,i)  {adler += (buf)[i]; sum2 += adler;}
#define DO2(buf,i)  DO1(buf,i); DO1(buf,i+1);
#define DO4(buf,i)  DO2(buf,i); DO2(buf,i+2);
#define DO8(buf,i)  DO4(buf,i); DO4(buf,i+4);
#define DO16(buf)   DO8(buf,0); DO8(buf,8);

/* use NO_DIVIDE if your processor does not do division in hardware --
try it both ways to see which is faster */
#ifdef NO_DIVIDE
/* note that this assumes BASE is 65521, where 65536 % 65521 == 15
(thank you to John Reiser for pointing this out) */
#  define CHOP(a) \
    do { \
        unsigned long tmp = a >> 16; \
        a &= 0xffffUL; \
        a += (tmp << 4) - tmp; \
    } while (0)
#  define MOD28(a) \
    do { \
        CHOP(a); \
        if (a >= BASE) a -= BASE; \
    } while (0)
#  define MOD(a) \
    do { \
        CHOP(a); \
        MOD28(a); \
    } while (0)
#  define MOD63(a) \
    do { /* this assumes a is not negative */ \
        z_off64_t tmp = a >> 32; \
        a &= 0xffffffffL; \
        a += (tmp << 8) - (tmp << 5) + tmp; \
        tmp = a >> 16; \
        a &= 0xffffL; \
        a += (tmp << 4) - tmp; \
        tmp = a >> 16; \
        a &= 0xffffL; \
        a += (tmp << 4) - tmp; \
        if (a >= BASE) a -= BASE; \
    } while (0)
#else
#  define MOD(a) a %= BASE
#  define MOD28(a) a %= BASE
#  define MOD63(a) a %= BASE
#endif
/* ==========================alder32校验相关全局定义 end==================================== */

/* ==========================base64编码相关全局定义 begin==================================== */
/* BASE 64 encode table */
static const char base64en[] = {
	'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',
	'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
	'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',
	'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',
	'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
	'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
	'w', 'x', 'y', 'z', '0', '1', '2', '3',
	'4', '5', '6', '7', '8', '9', '+', '/',
};

#define BASE64_PAD      '='
#define BASE64DE_FIRST  '+'
#define BASE64DE_LAST   'z'
/* ASCII order for BASE 64 decode, -1 in unused character */
static const signed char base64de[] = {
	/* '+', ',', '-', '.', '/', '0', '1', '2', */
	62,  -1,  -1,  -1,  63,  52,  53,  54,
	/* '3', '4', '5', '6', '7', '8', '9', ':', */
	55,  56,  57,  58,  59,  60,  61,  -1,
	/* ';', '<', '=', '>', '?', '@', 'A', 'B', */
	-1,  -1,  -1,  -1,  -1,  -1,   0,   1,
	/* 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', */
	2,   3,   4,   5,   6,   7,   8,   9,
	/* 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', */
	10,  11,  12,  13,  14,  15,  16,  17,
	/* 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', */
	18,  19,  20,  21,  22,  23,  24,  25,
	/* '[', '\', ']', '^', '_', '`', 'a', 'b', */
	-1,  -1,  -1,  -1,  -1,  -1,  26,  27,
	/* 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', */
	28,  29,  30,  31,  32,  33,  34,  35,
	/* 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', */
	36,  37,  38,  39,  40,  41,  42,  43,
	/* 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', */
	44,  45,  46,  47,  48,  49,  50,  51,
};
/* ==========================base64编码相关全局定义 end==================================== */

/* ==========================加密解密相关定义 begin==================================== */
uint8_t GetTransFlag() //生成随机8位二进制字节
{
	return (rand() % 256 & 0xFF);//生成一个随机数，取最后一个字节
}


/* ==========================加密解密相关定义 end==================================== */

int32_t mmrUtil::adler32(const char * buf, uint32_t len)
{
	int32_t adler = 1;
	//uint32_t sum2;
	//uint32_t n;
	unsigned long sum2;
	unsigned n;

	/* split Adler-32 into component sums */
	sum2 = (adler >> 16) & 0xffff;
	adler &= 0xffff;

	/* in case user likes doing a byte at a time, keep it fast */
	if (len == 1) {
		adler += buf[0];
		if (adler >= BASE)
			adler -= BASE;
		sum2 += adler;
		if (sum2 >= BASE)
			sum2 -= BASE;
		return adler | (sum2 << 16);
	}

	/* initial Adler-32 value (deferred check for len == 1 speed) */
	if (buf == nullptr)
		return 1L;

	/* in case short lengths are provided, keep it somewhat fast */
	if (len < 16) {
		while (len--) {
			adler += *buf++;
			sum2 += adler;
		}
		if (adler >= BASE)
			adler -= BASE;
		MOD28(sum2);            /* only added so many BASE's */
		return adler | (sum2 << 16);
	}

	/* do length NMAX blocks -- requires just one modulo operation */
	while (len >= NMAX) {
		len -= NMAX;
		n = NMAX / 16;          /* NMAX is divisible by 16 */
		do {
			DO16(buf);          /* 16 sums unrolled */
			buf += 16;
		} while (--n);
		MOD(adler);
		MOD(sum2);
	}

	/* do remaining bytes (less than NMAX, still just one modulo) */
	if (len) {                  /* avoid modulos if none remaining */
		while (len >= 16) {
			len -= 16;
			DO16(buf);
			buf += 16;
		}
		while (len--) {
			adler += *buf++;
			sum2 += adler;
		}
		MOD(adler);
		MOD(sum2);
	}

	/* return recombined sums */
	return adler | (sum2 << 16);
}

void  mmrUtil::flipData(void* buf, size_t bufSize)
{
	if (bufSize < 2)
		return;

	char* start = &((char*)buf)[0];
	char* end = &((char*)buf)[bufSize - 1];
	while (start < end)
	{
		char temp = *start;

		*start = *end;
		*end = temp;

		++start;
		--end;
	}
}

uint32_t  mmrUtil::base64Encode(const void* inBuf, uint32_t inlen, char *outBuf)
{
	const char* in = (const char*)inBuf;
	unsigned int i = 0, j = 0;

	for (; i < inlen; i++) {
		int s = i % 3;

		switch (s) {
		case 0:
			outBuf[j++] = base64en[(in[i] >> 2) & 0x3F];
			continue;
		case 1:
			outBuf[j++] = base64en[((in[i - 1] & 0x3) << 4) + ((in[i] >> 4) & 0xF)];
			continue;
		case 2:
			outBuf[j++] = base64en[((in[i - 1] & 0xF) << 2) + ((in[i] >> 6) & 0x3)];
			outBuf[j++] = base64en[in[i] & 0x3F];
		}
	}

	/* move back */
	i -= 1;

	/* check the last and add padding */
	if ((i % 3) == 0) {
		outBuf[j++] = base64en[(in[i] & 0x3) << 4];
		outBuf[j++] = BASE64_PAD;
		outBuf[j++] = BASE64_PAD;
	}
	else if ((i % 3) == 1) {
		outBuf[j++] = base64en[(in[i] & 0xF) << 2];
		outBuf[j++] = BASE64_PAD;
	}

	return j;
}

uint32_t mmrUtil::base64Decode(const void *inBuf, uint32_t inlen, uint8_t* outBuf)
{
	const char* in = (const char*)inBuf;

	unsigned int i = 0, j = 0;

	for (; i < inlen; i++) {
		int c;
		int s = i % 4;

		if (in[i] == '=')
			return j;

		if (in[i] < BASE64DE_FIRST || in[i] > BASE64DE_LAST ||
			(c = base64de[in[i] - BASE64DE_FIRST]) == -1)
			return -1;

		switch (s) {
		case 0:
			outBuf[j] = ((unsigned int)c << 2) & 0xFF;
			continue;
		case 1:
			outBuf[j++] += ((unsigned int)c >> 4) & 0x3;

			/* if not last char with padding */
			if (i < (inlen - 3) || in[inlen - 2] != '=')
				outBuf[j] = ((unsigned int)c & 0xF) << 4;
			continue;
		case 2:
			outBuf[j++] += ((unsigned int)c >> 2) & 0xF;

			/* if not last char with padding */
			if (i < (inlen - 2) || in[inlen - 1] != '=')
				outBuf[j] = ((unsigned int)c & 0x3) << 6;
			continue;
		case 3:
			outBuf[j++] += (unsigned char)c;
		}
	}

	return j;
}
