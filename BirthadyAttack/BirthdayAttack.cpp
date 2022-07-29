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
void getString(unsigned char* dest, const unsigned int len)
{
	for (int i = 0; i < len; i++)
	{
		int number1 = rand() % 62;
		dest[i] = allChar[number1];
	}
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

	unsigned int iv[8] = { 0x7380166F,0x4914B2B9,0x172442D7,0xDA8A0600,0xA96F30BC,0x163138AA,0xE38DEE4D,0xB0FB0E4E };
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



//#pragma comment(lib,"sm3dll2")
//extern "C" void SM3Call(const unsigned char *message,unsigned int messageLen, unsigned char digest[SM3_HASH_SIZE]);

int main(int argc, char* argv[])
{


	const unsigned int strlen = 38;
	unsigned char str[strlen + 1] = {};
	generateString(str, strlen);
	for (int i = 0; i < strlen; i++)
	{
		printf("%02x", str[i]);
	}
	printf("\n");
	printf("\n");
	unsigned char output[32];
	
	SM3Calc(str, strlen, output);

	for (int i = 0; i < 32; i++)
	{
		printf("%02x", output[i]);
		if (((i + 1) % 4) == 0) printf(" ");
	}
	printf("\n");
	printf("\n");
	unsigned char dest[32];
	unsigned char get_str[strlen + 1] = {};
	generateString(get_str, strlen);
	while(1)
	{
		
		int j = 0;
		int k = 0;
		unsigned char get_str2[strlen + 1] = {};
		generateString(get_str2, strlen);
		while(j<=strlen)
		{
			if (get_str2[j] != str[j])
			{
				while (k<=strlen)
				{
					if (get_str2[j] != get_str[j])
					{
						//get_str=get_str2
						int i = 0;
						while (i < strlen)
						{
							get_str[i] = get_str2[i];
							i++;
						}
						break;

					}
					else if (k == strlen)
					{
						generateString(get_str2, strlen);
						k = 0;
					}
					else
						k++;
				}
				break;
			}
			else if (j == strlen)
			{
				generateString(get_str2, strlen);
				j = 0;
			}
			else
				j++;
		}

		SM3Calc(get_str, strlen, dest);
		if (dest[0] == output[0])
		{
			//get result
			for (int i = 0; i < strlen; i++)
			{
			printf("%02x", get_str[i]);
			}
			printf("\n");
			printf("\n");
			cout << "get result:" << endl;
			for (int i = 0; i < 32; i++)
			{
			printf("%02x", dest[i]);
			if (((i + 1) % 4) == 0) printf(" ");
			}
			printf("\n");
			printf("\n");
			//break;
		}
		
	
		
	}
	
	system("pause");
}