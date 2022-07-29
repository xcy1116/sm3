/*
* sm3.c
*/
#include <stdio.h>
#include <memory.h>
#include "sm3.h"
#include <string.h>
#include <iostream>
#include <time.h>
#include <immintrin.h>
#include <mmintrin.h>  //MMX

#include <xmmintrin.h> 
#include <emmintrin.h> 
#include <pmmintrin.h> 
#include <tmmintrin.h> 
#include <smmintrin.h> 
#include <nmmintrin.h> 
#include <wmmintrin.h> 
#include <immintrin.h>
#include <intrin.h>   
#include <emmintrin.h>
#include <immintrin.h>

using namespace std;
const unsigned char allChar[63] = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
void generateString(unsigned char* dest, unsigned int len);

void generateString(unsigned char* dest, const unsigned int len)
{
	unsigned int cnt, randNo;
	srand((unsigned int)time(NULL));

	for (cnt = 0; cnt < len; cnt++)
	{
		randNo = rand() % 62;
		*dest = allChar[randNo];
		dest++;
	}

	*dest = '\0';
}

/*
* 判断运行环境是否为小端
*/
static const int endianTest = 1;
#define IsLittleEndian() (*(char *)&endianTest == 1)


/*
* 向左循环移位
*/
#define LeftRotate(word, bits) ( (word) << (bits) | (word) >> (32 - (bits)) )



/*
* 反转四字节整型字节序
*/
unsigned int* ReverseWord(unsigned int* word)
{
	unsigned char* byte, temp;

	byte = (unsigned char*)word;
	temp = byte[0];
	byte[0] = byte[3];
	byte[3] = temp;

	temp = byte[1];
	byte[1] = byte[2];
	byte[2] = temp;
	return word;
}

/*
* T
*/
unsigned int T(int i)
{
	if (i >= 0 && i <= 15)
		return 0x79CC4519;
	else if (i >= 16 && i <= 63)
		return 0x7A879D8A;
	else
		return 0;
}

/*
* FF
*/
unsigned int FF(unsigned int X, unsigned int Y, unsigned int Z, int i)
{
	if (i >= 0 && i <= 15)
		return X ^ Y ^ Z;
	else if (i >= 16 && i <= 63)
		return (X & Y) | (X & Z) | (Y & Z);
	else
		return 0;
}

/*
* GG
*/
unsigned int GG(unsigned int X, unsigned int Y, unsigned int Z, int i)
{
	if (i >= 0 && i <= 15)
		return X ^ Y ^ Z;
	else if (i >= 16 && i <= 63)
		return (X & Y) | (~X & Z);
	else
		return 0;
}

/*
* P0
*/
unsigned int P0(unsigned int X)
{
	return X ^ LeftRotate(X, 9) ^ LeftRotate(X, 17);
}

/*
* P1
*/
unsigned int P1(unsigned int X)
{
	return X ^ LeftRotate(X, 15) ^ LeftRotate(X, 23);
}

/*
* 初始化函数
*/
void SM3Init(SM3Context* context)
{
	/*context->intermediateHash[0] = 0x7380166F;
	context->intermediateHash[1] = 0x4914B2B9;
	context->intermediateHash[2] = 0x172442D7;
	context->intermediateHash[3] = 0xDA8A0600;
	context->intermediateHash[4] = 0xA96F30BC;
	context->intermediateHash[5] = 0x163138AA;
	context->intermediateHash[6] = 0xE38DEE4D;
	context->intermediateHash[7] = 0xB0FB0E4E;*/

	unsigned int iv[8] = { 0x66c7f0f4,0x62eeedd9,0xd1f2d46b,0xdc10e4e2,0x4167c487,0x5cf2f7a2,0x297da02b,0x8f4ba8e0 };
	_mm256_storeu_epi32(&context->intermediateHash[0], _mm256_loadu_epi32(&iv[0]));
}

void SM3Init_1(SM3Context* context)
{
	context->intermediateHash[0] = 0x7380166F;
	context->intermediateHash[1] = 0x4914B2B9;
	context->intermediateHash[2] = 0x172442D7;
	context->intermediateHash[3] = 0xDA8A0600;
	context->intermediateHash[4] = 0xA96F30BC;
	context->intermediateHash[5] = 0x163138AA;
	context->intermediateHash[6] = 0xE38DEE4D;
	context->intermediateHash[7] = 0xB0FB0E4E;

	/*unsigned int iv[8] = { 0x66c7f0f4,0x62eeedd9,0xd1f2d46b,0xdc10e4e2,0x4167c487,0x5cf2f7a2,0x297da02b,0x8f4ba8e0 };
	_mm256_storeu_epi32(&context->intermediateHash[0], _mm256_loadu_epi32(&iv[0]));*/
}
void SM3Init_2(SM3Context* context)
{

	unsigned int iv[8] = { 0x49019597,0x27066d2e,0x50d4c2f1,0x1ef3b47b,0xc74a174e,0x7362ff4c,0xd79a36b9,0xb068c072 };
	_mm256_storeu_epi32(&context->intermediateHash[0], _mm256_loadu_epi32(&iv[0]));
}
/*
* 处理消息块
*/
void SM3ProcessMessageBlock(SM3Context* context)
{
	int i;
	unsigned int W[68];
	unsigned int W_[64];
	unsigned int A, B, C, D, E, F, G, H, SS1, SS2, TT1, TT2;

	/* 消息扩展 */
	for (i = 0; i < 16; i+=2)
	{
		W[i] = *(unsigned int*)(context->messageBlock + i * 4);
		W[i+1] = *(unsigned int*)(context->messageBlock + (i+1) * 4);//add
		if (IsLittleEndian())
		{
			ReverseWord(W + i);
			ReverseWord(W + i+1);//add
		}
		//printf("%d: %x\n", i, W[i]);    
	}
	
	//for (i = 0; i< 16;i+=4)
	//{
	//	unsigned int* tt = (unsigned int*)(context->messageBlock+i);
	//	
	//	__m128i temp = _mm_loadu_epi32(&tt);
	//	_mm_storeu_epi32(&W[i],temp);
	//	/*__m128i temp = _mm_loadu_si32(&(context->messageBlock + 16));
	//	_mm_storeu_si32(&W[0], temp);*/
	//}
	//

	for (i = 16; i < 68; i+=2)
	{
		W[i] = P1(W[i - 16] ^ W[i - 9] ^ LeftRotate(W[i - 3], 15))
			^ LeftRotate(W[i - 13], 7)
			^ W[i - 6];
		W[i+1] = P1(W[i - 15] ^ W[i - 8] ^ LeftRotate(W[i - 2], 15))
			^ LeftRotate(W[i - 12], 7)
			^ W[i - 5];
		//printf("%d: %x\n", i, W[i]);
		/*__m128i temp3 =_mm_xor_epi32(_mm_loadu_epi32(&W[i - 16]), _mm_loadu_epi32(&W[i - 9]));
		__m128i temp4 = _mm_loadu_epi32(&(LeftRotate(W[i - 3], 15)));*/

	}
	for (i = 0; i < 64; i+=8)
	{
		
		__m256i temp3 = _mm256_xor_epi32(_mm256_loadu_epi32(&W[i]), _mm256_loadu_epi32(&W[i + 4]));
		_mm256_storeu_epi32(&W_[i],temp3);

		/*W_[i] = W[i] ^ W[i + 4];*/
		//W_[i+1] = W[i+1] ^ W[i + 5];//add
		//printf("%d: %x\n", i, W_[i]);    
	}

	/* 消息压缩 */
	A = context->intermediateHash[0];
	B = context->intermediateHash[1];
	C = context->intermediateHash[2];
	D = context->intermediateHash[3];
	E = context->intermediateHash[4];
	F = context->intermediateHash[5];
	G = context->intermediateHash[6];
	H = context->intermediateHash[7];
	
	for (i = 0; i < 64; i++)
	{
		SS1 = LeftRotate((LeftRotate(A, 12) + E + LeftRotate(T(i), i)), 7);
		SS2 = SS1 ^ LeftRotate(A, 12);
		TT1 = FF(A, B, C, i) + D + SS2 + W_[i];
		TT2 = GG(E, F, G, i) + H + SS1 + W[i];
		D = C;
		C = LeftRotate(B, 9);
		B = A;
		A = TT1;
		H = G;
		G = LeftRotate(F, 19);
		F = E;
		E = P0(TT2);
	}
	context->intermediateHash[0] ^= A;
	context->intermediateHash[1] ^= B;
	context->intermediateHash[2] ^= C;
	context->intermediateHash[3] ^= D;
	context->intermediateHash[4] ^= E;
	context->intermediateHash[5] ^= F;
	context->intermediateHash[6] ^= G;
	context->intermediateHash[7] ^= H;
}

/*
* SM3算法主函数
*/
unsigned char* SM3Calc(const unsigned char* message,
	unsigned int messageLen, unsigned char digest[SM3_HASH_SIZE])
{
	SM3Context context;
	unsigned int i, remainder, bitLen;

	/* 初始化上下文 */
	SM3Init(&context);

	/* 对前面的消息分组进行处理 */
	for (i = 0; i < messageLen / 64; i+=2)
	{
		memcpy(context.messageBlock, message + i * 64, 64);
		SM3ProcessMessageBlock(&context);
		memcpy(context.messageBlock, message + (i+1) * 64, 64);
		SM3ProcessMessageBlock(&context);
	}

	/* 填充消息分组，并处理 */
	bitLen = messageLen * 8;
	if (IsLittleEndian())
		ReverseWord(&bitLen);
	remainder = messageLen % 64;
	memcpy(context.messageBlock, message + i * 64, remainder);
	context.messageBlock[remainder] = 0x80;
	if (remainder <= 55)
	{
		/* 长度按照大端法占8个字节，该程序只考虑长度在 2**32 - 1（单位：比特）以内的情况，
		* 故将高 4 个字节赋为 0 。*/
		memset(context.messageBlock + remainder + 1, 0, 64 - remainder - 1 - 8 + 4);
		memcpy(context.messageBlock + 64 - 4, &bitLen, 4);
		SM3ProcessMessageBlock(&context);
	}
	else
	{
		memset(context.messageBlock + remainder + 1, 0, 64 - remainder - 1);
		SM3ProcessMessageBlock(&context);
		/* 长度按照大端法占8个字节，该程序只考虑长度在 2**32 - 1（单位：比特）以内的情况，
		* 故将高 4 个字节赋为 0 。*/
		memset(context.messageBlock, 0, 64 - 4);
		memcpy(context.messageBlock + 64 - 4, &bitLen, 4);
		SM3ProcessMessageBlock(&context);
	}

	/* 返回结果 */
	if (IsLittleEndian())
		for (i = 0; i < 8; i++)
			ReverseWord(context.intermediateHash + i);
	memcpy(digest, context.intermediateHash, SM3_HASH_SIZE);

	return digest;
}

unsigned char* SM3Calc_1(const unsigned char* message,
	unsigned int messageLen, unsigned char digest[SM3_HASH_SIZE])
{
	SM3Context context;
	unsigned int i, remainder, bitLen;

	/* 初始化上下文 */
	SM3Init_1(&context);

	/* 对前面的消息分组进行处理 */
	for (i = 0; i < messageLen / 64; i += 2)
	{
		memcpy(context.messageBlock, message + i * 64, 64);
		SM3ProcessMessageBlock(&context);
		memcpy(context.messageBlock, message + (i + 1) * 64, 64);
		SM3ProcessMessageBlock(&context);
	}

	/* 填充消息分组，并处理 */
	bitLen = messageLen * 8;
	if (IsLittleEndian())
		ReverseWord(&bitLen);
	remainder = messageLen % 64;
	memcpy(context.messageBlock, message + i * 64, remainder);
	context.messageBlock[remainder] = 0x80;
	if (remainder <= 55)
	{
		/* 长度按照大端法占8个字节，该程序只考虑长度在 2**32 - 1（单位：比特）以内的情况，
		* 故将高 4 个字节赋为 0 。*/
		memset(context.messageBlock + remainder + 1, 0, 64 - remainder - 1 - 8 + 4);
		memcpy(context.messageBlock + 64 - 4, &bitLen, 4);
		SM3ProcessMessageBlock(&context);
	}
	else
	{
		memset(context.messageBlock + remainder + 1, 0, 64 - remainder - 1);
		SM3ProcessMessageBlock(&context);
		/* 长度按照大端法占8个字节，该程序只考虑长度在 2**32 - 1（单位：比特）以内的情况，
		* 故将高 4 个字节赋为 0 。*/
		memset(context.messageBlock, 0, 64 - 4);
		memcpy(context.messageBlock + 64 - 4, &bitLen, 4);
		SM3ProcessMessageBlock(&context);
	}

	/* 返回结果 */
	if (IsLittleEndian())
		for (i = 0; i < 8; i++)
			ReverseWord(context.intermediateHash + i);
	memcpy(digest, context.intermediateHash, SM3_HASH_SIZE);

	return digest;
}

unsigned char* SM3Calc_2(const unsigned char* message,
	unsigned int messageLen, unsigned char digest[SM3_HASH_SIZE])
{
	SM3Context context;
	unsigned int i, remainder, bitLen;

	/* 初始化上下文 */
	SM3Init_2(&context);

	/* 对前面的消息分组进行处理 */
	for (i = 0; i < messageLen / 64; i += 2)
	{
		memcpy(context.messageBlock, message + i * 64, 64);
		SM3ProcessMessageBlock(&context);
		memcpy(context.messageBlock, message + (i + 1) * 64, 64);
		SM3ProcessMessageBlock(&context);
	}

	/* 填充消息分组，并处理 */
	bitLen = messageLen * 8;
	if (IsLittleEndian())
		ReverseWord(&bitLen);
	remainder = messageLen % 64;
	memcpy(context.messageBlock, message + i * 64, remainder);
	context.messageBlock[remainder] = 0x80;
	if (remainder <= 55)
	{
		/* 长度按照大端法占8个字节，该程序只考虑长度在 2**32 - 1（单位：比特）以内的情况，
		* 故将高 4 个字节赋为 0 。*/
		memset(context.messageBlock + remainder + 1, 0, 64 - remainder - 1 - 8 + 4);
		memcpy(context.messageBlock + 64 - 4, &bitLen, 4);
		SM3ProcessMessageBlock(&context);
	}
	else
	{
		memset(context.messageBlock + remainder + 1, 0, 64 - remainder - 1);
		SM3ProcessMessageBlock(&context);
		/* 长度按照大端法占8个字节，该程序只考虑长度在 2**32 - 1（单位：比特）以内的情况，
		* 故将高 4 个字节赋为 0 。*/
		memset(context.messageBlock, 0, 64 - 4);
		memcpy(context.messageBlock + 64 - 4, &bitLen, 4);
		SM3ProcessMessageBlock(&context);
	}

	/* 返回结果 */
	if (IsLittleEndian())
		for (i = 0; i < 8; i++)
			ReverseWord(context.intermediateHash + i);
	memcpy(digest, context.intermediateHash, SM3_HASH_SIZE);

	return digest;
}


unsigned char secret[256] = "abc";
const unsigned int strlen1 = 3;

unsigned char output[32];







//#pragma comment(lib,"sm3dll2")
//extern "C" void SM3Call(const unsigned char *message,unsigned int messageLen, unsigned char digest[SM3_HASH_SIZE]);

int main(int argc, char* argv[])
{
	
	printf("secret:\n");
	printf("%s\n", secret);


	//生成m'（append）并输出
	unsigned char m_1[strlen1 + 1] = {};
	generateString(m_1, strlen1);
	cout << "append message:" << endl;
	for (int i = 0; i < strlen1; i++)
	{
		printf("%02x", m_1[i]);
	}
	cout << endl;

	//这里更改初始向量为secret的hash值 并计算hash2
	unsigned char result[32];
	SM3Calc(m_1, strlen1,result);
	cout << "hash(append):" << endl;
	for (int i = 0; i < 32; i++)
	{
		printf("%02x", result[i]);
		if (((i + 1) % 4) == 0) printf(" ");
	}
	cout << endl;

	//计算secret+padding
	const unsigned int strlen = strlen1;
	unsigned int messageLen = strlen;
	SM3Context context;
	unsigned int i, remainder, bitLen;

	/* 初始化上下文 */
	SM3Init_1(&context);

	/* 对前面的消息分组进行处理 */
	for (i = 0; i < messageLen / 64; i += 2)
	{
		memcpy(context.messageBlock,secret + i * 64, 64);
		SM3ProcessMessageBlock(&context);
		memcpy(context.messageBlock, secret+ (i + 1) * 64, 64);
		SM3ProcessMessageBlock(&context);
	}

	/* 填充消息分组，并处理 */
	bitLen = messageLen * 8;
	if (IsLittleEndian())
		ReverseWord(&bitLen);
	remainder = messageLen % 64;
	memcpy(context.messageBlock, secret + i * 64, remainder);
	context.messageBlock[remainder] = 0x80;

		/* 长度按照大端法占8个字节，该程序只考虑长度在 2**32 - 1（单位：比特）以内的情况，
		* 故将高 4 个字节赋为 0 。*/
		memset(context.messageBlock + remainder + 1, 0, 64 - remainder - 1 - 8 + 4);
		memcpy(context.messageBlock + 64 - 4, &bitLen, 4);
		


		
		//生成最终消息secret+padding+append
		unsigned char array[64+strlen];
		for (int i = 0; i < 64; i++)
		{
			array[i] = context.messageBlock[i];
		}
		for (int i = 64; i < 64 + strlen; i++)
		{
			array[i] = m_1[i-64];
		}
		//输出最终消息
		cout << "secret+padding+append:" << endl;
		for (int i = 0; i < 64+strlen; i++)
		{
			printf("%02x",array[i]);
			if (((i + 1) % 4) == 0) printf(" ");
		}
		cout << endl;
		//加密最终消息secret+padding+append
		unsigned char result_1[32];
		SM3Calc_1(context.messageBlock, 64, result_1);
		unsigned char result_2[32];
		SM3Calc_2(m_1, strlen, result_2);
		//输出加密后hash3
		cout << "hash(secret+padding):" << endl;
		for (int i = 0; i < 32; i++)
		{
			printf("%02x", result_1[i]);
			if (((i + 1) % 4) == 0) printf(" ");
		}
		cout << endl;
		cout << "hash(secret+padding+append):" << endl;
		for (int i = 0; i < 32; i++)
		{
			printf("%02x", result[i]);
			if (((i + 1) % 4) == 0) printf(" ");
		}
		cout << endl;
	system("pause");
}