//
// DL ISO8583 library demo
//
#include <iostream>
#include <string>
#include <sstream>
#include "dl_iso8583.h"
#include "dl_iso8583_defs_1987.h"
#include "dl_output.h" // for 'DL_OUTPUT_Hex'
#include <boost/algorithm/hex.hpp>
#include <stdio.h>

DL_UINT16 EBCDIC = 1;
DL_UINT16 DEBUG = 1;

std::string hexStr(const DL_UINT8 *iPtr, DL_UINT32 iNumBytes );
int HexStringToBytes(const char *hexStr, unsigned char *output, unsigned int *outputLen);
unsigned char * hex2bytes(const char *hexStr);
void hex2bin(const char* src, unsigned char* target);
int asc2bcd(unsigned char * AscBuf, unsigned char * BcdBuf, int Len);
unsigned char hex2bcd (unsigned char x);
int bcd2asc(unsigned char * BcdBuf, unsigned char * AscBuf, int Len);
DL_ERR _bytes_to_hexstr(const unsigned char *bytes,
                     	unsigned char *output,
                    	unsigned int *outputLen);


inline unsigned char *uc_str(char *s){
  return reinterpret_cast<unsigned char*>(s);
}

int main ( void )
{
	DL_ISO8583_HANDLER isoHandler;
	DL_ISO8583_MSG     isoMsg;
	DL_UINT8           packBuf[2048];
	DL_UINT16          packedSize;
	DL_ERR 			   error;

	//EBCDIC = 0;

	/* get ISO-8583 1993 handler */
	DL_ISO8583_DEFS_1987_GetHandler(&isoHandler);
	
	//char hexStr0[] = "30313030feff460128e1f30a00000000000000023136323330363530323238313538303035323030303030303030303030303030303030313030303030303030303030313030303030303030303030313032303731343236303136313030303030303631393534303030343333303038313132363031303230373330303230323037303230373539393930353130303130363031323334353337323330363530323238313538303035323d333030323230363030303030343037303030303037353635363536303837363931323334353637383132333435363738393132333435354f20526166616f2065682062616f206d65736d6f2120202e2e2e2e2e2e2e2e2e2e2e2e2e205553413030375238303032545638343038343039383651da3e15599c3dd43130375f2a020840820258008407a0000000041010950500000000009a032105249c01009f02060000000033009f10120110a00000044000dac100000000000000009f1a0208409f2608607c7d64313b62fb9f2701809f3303e0e8e89f34034103029f360200419f370411f6d7993033373031333330313239353030314930495939314a55514c57445a434c515250594e4450414a443032313030303030303030303035303038343039303231303030394d434730313133395a3035303031303033303030303230333732303232303230373134323630313433333030383030303031323334353132333435363738";
	char hexStr0[] = "f0f1f0f07eff460128e1f30af1f6f2f3f0f6f5f0f2f2f8f1f5f8f0f0f5f2f0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f1f0f0f0f0f0f0f0f0f0f0f0f1f0f0f0f0f0f0f0f0f0f0f0f1f0f2f0f7f1f4f2f6f0f1f6f1f0f0f0f0f0f0f6f1f9f5f4f0f0f0f4f3f3f0f0f8f1f1f2f6f0f1f0f2f0f7f3f0f0f2f0f2f0f7f0f2f0f7f5f9f9f9f0f5f1f0f0f1f0f6f0f1f2f3f4f5f3f7f2f3f0f6f5f0f2f2f8f1f5f8f0f0f5f27ef3f0f0f2f2f0f6f0f0f0f0f0f4f0f7f0f0f0f0f0f7f5f6f5f6f5f6f0f8f7f6f9f1f2f3f4f5f6f7f8f1f2f3f4f5f6f7f8f9f1f2f3f4f5f540d640d98186819640858840828196409485a294965a4b4b4b4b4b4b4b4b4b4b4b4b4b4b4b4b4b4bf0f0f7d9f8f0f0f2e3e5f8f4f0f8f4f0f9f8f651da3e15599c3dd4f1f0f75f2a020840820258008407a0000000041010950500000000009a032105249c01009f02060000000033009f10120110a00000044000dac100000000000000009f1a0208409f2608607c7d64313b62fb9f2701809f3303e0e8e89f34034103029f360200419f370411f6d799f0f3f7f0f1f3f3f0f1f2f9f5f0f0f1c9f0c9e8f9f1d1e4d8d3e6c4e9c3d3d8d9d7e8d5c4d7c1d1c4f0f2f1f0f0f0f0f0f0f0f0f0f0f5f0f0f8f4f0f9f0f2f1f0f0f0f9d4c3c7f0f1f1f3f9e9";
	DL_UINT8 * hexStr1 = (DL_UINT8*) hexStr0;
	DL_UINT16 strLen = strlen(hexStr0);
	DL_UINT16 strLen1 = sizeof(*hexStr1)/sizeof(hexStr1);
	DL_UINT8 outBuff[strLen/2];
	//std::cout << strLen << " - " << strLen1 <<  std::endl;
	
	//unsigned int * lengthPtr = &length;
	//hex2bin(hexStr,outBuff);
	
	asc2bcd(uc_str(hexStr0),outBuff,strLen);

	// printf("\nISO8583 Hex Buf:%s\n", hexStr0);
	// printf("\nISO8583 Req Buf:%s\n", outBuff);

	// for (int i = 0; i < strLen; i+=2) {
	// 	char hex[3];
	// 	memcpy(hex,&hexStr1[i],2);
	// 	hex[2] = '\0';
	// 	outBuff[i/2] = (int)strtol(hex, NULL, 16);  
	// }

	DL_ISO8583_MSG_Init(NULL,0,&isoMsg);

	error = DL_ISO8583_MSG_Unpack(&isoHandler,outBuff,strLen/2,&isoMsg);
	if (error != 0){
		printf("Error %d ocurred on Unpacking.\n",error);
	}

	// printf("\nField data: %lu\n\n",*isoMsg.field[52].ptr);

	error = DL_ISO8583_MSG_Pack(&isoHandler,&isoMsg,packBuf,&packedSize);
	if (error != 0){
		printf("Error %d ocurred on Packing.\n",error);
	}

	// DL_ISO8583_MSG_Dump(stdout,NULL,&isoHandler,&isoMsg);

	// std::cout << "\nISO8583 Hex Buf: " << hexStr(packBuf,packedSize) << std::endl;
	// printf("\nISO8583 Req Buf: %s\n", packBuf);
	// DL_OUTPUT_Hex(stdout,NULL,packBuf,packedSize);
	// DL_OUTPUT_Hex(stdout,NULL,outBuff,packedSize);
	//fwrite(hexStr0,strLen-1,1,stdout);
	printf("\nOriginal:   %s\n", hexStr0);
	std::cout << "\nPacked:     " << hexStr(packBuf,packedSize) << std::endl;


	DL_ISO8583_MSG_Free(&isoMsg);
	// std::cout << hexStr(outBuff,strLen) << std::endl;

	// char hex[] = "30";                          // here is the hex string
	// int num = (int)strtol(hex, NULL, 16);       // number base 16
	// printf("%c\n", num);                        // print it as a char
	// printf("%d\n", num);                        // print it as decimal
	// printf("%X\n", num);


	// char de52[] = "5F2A020840820258008407A0000000041010950500000000009A032105249C01009F02060000000033009F10120110A00000044000DAC100000000000000009F1A0208409F2608607C7D64313B62FB9F2701809F3303E0E8E89F34034103029F360200419F370411F6D799";
	// DL_UINT8 bcdBuff[214];
	// DL_UINT8 ascBuff[428];
	// unsigned int size =214;

	// printf("\n%s\n",de52);
	// asc2bcd(uc_str(de52),bcdBuff,size);
	// //printf("%c\n",*de52);
	// //printf("%s\n",bcdBuff);
	// printf("\nBefore: ");
	// fwrite(bcdBuff, size, 1, stdout);
	// printf("\n\n");


	// bcd2asc(bcdBuff,ascBuff,size);
	// //printf("%s\n",ascBuff);

	// printf("Later: ");
	// fwrite(ascBuff, size, 1, stdout);
	// printf("\n\n");



	return 0;
}

std::string hexStr(const DL_UINT8 *iPtr, DL_UINT32 iNumBytes ){
	std::stringstream ss;
     ss << std::hex;

     for( int i(0) ; i < iNumBytes; ++i )
         ss << std::setw(2) << std::setfill('0') << (int)iPtr[i];

     return ss.str();
}

int HexStringToBytes(const char *hexStr,
                     unsigned char *output,
                     unsigned int *outputLen) {
	size_t len = strlen(hexStr);
	if (len % 2 != 0) {
		return NULL;
	}
	size_t finalLen = len / 2;
	*outputLen = finalLen;
	for (size_t inIdx = 0, outIdx = 0; outIdx < finalLen; inIdx += 2, outIdx++) {
		if ((hexStr[inIdx] - 48) <= 9 && (hexStr[inIdx + 1] - 48) <= 9) {
		goto convert;
		} else {
		if ((hexStr[inIdx] - 65) <= 5 && (hexStr[inIdx + 1] - 65) <= 5) {
			goto convert;
		} else {
			*outputLen = 0;
			return NULL;
		}
		}
	convert:
		output[outIdx] =
			(hexStr[inIdx] % 32 + 9) % 25 * 16 + (hexStr[inIdx + 1] % 32 + 9) % 25;
	}
	output[finalLen] = '\0';
	return 0;
}

unsigned char * hex2bytes(const char *hexStr) {
	size_t len = strlen(hexStr);
	if (len % 2 != 0) {
		return NULL;
	}
	size_t finalLen = len / 2;
	unsigned char output[finalLen];
	for (size_t inIdx = 0, outIdx = 0; outIdx < finalLen; inIdx += 2, outIdx++) {
		if ((hexStr[inIdx] - 48) <= 9 && (hexStr[inIdx + 1] - 48) <= 9) {
		goto convert;
		} else {
		if ((hexStr[inIdx] - 65) <= 5 && (hexStr[inIdx + 1] - 65) <= 5) {
			goto convert;
		} else {
			return NULL;
		}
		}
	convert:
		output[outIdx] =
			(hexStr[inIdx] % 32 + 9) % 25 * 16 + (hexStr[inIdx + 1] % 32 + 9) % 25;
	}
	output[finalLen] = '\0';
	return output;
}

int char2int(char input)
{
  if(input >= '0' && input <= '9')
    return input - '0';
  if(input >= 'A' && input <= 'F')
    return input - 'A' + 10;
  if(input >= 'a' && input <= 'f')
    return input - 'a' + 10;
  throw std::invalid_argument("Invalid input string");
}

void hex2bin(const char* src, unsigned char* target)
{
  while(*src && src[1])
  {
    *(target++) = char2int(*src)*16 + char2int(src[1]);
    src += 2;
  }
}

int asc2bcd(unsigned char * AscBuf, unsigned char * BcdBuf, int Len)
{
    int  i;
    unsigned char str[2] = {0};

    for (i = 0; i < Len; i += 2)
    {
        if ((AscBuf[i] >= 'a') && (AscBuf[i] <= 'f'))
        {
            str[0] = AscBuf[i] - 'a' + 0x0A;
        }
        else if ((AscBuf[i] >= 'A') && (AscBuf[i] <= 'F'))
        {
            str[0] = AscBuf[i] - 'A' + 0x0A;
        }
        else if (AscBuf[i] >= '0')
        {
            str[0] = AscBuf[i] - '0';
        }
        else
        {
            str[0] = 0;
        }

        if ((AscBuf[i + 1] >= 'a') && (AscBuf[i + 1] <= 'f'))
        {
            str[1] = AscBuf[i + 1] - 'a' + 0x0A;
        }
        else if ((AscBuf[i + 1] >= 'A') && (AscBuf[i + 1] <= 'F'))
        {
            str[1] = AscBuf[i + 1] - 'A' + 0x0A;
        }
        else if (AscBuf[1] >= '0')
        {
            str[1] = AscBuf[i + 1] - '0';
        }
        else
        {
            str[1] = 0;
        }

        BcdBuf[i / 2] = (str[0] << 4) | (str[1] & 0x0F);
    }

    return 0;
}


unsigned char hex2bcd (unsigned char x)
{
    unsigned char y;
    y = (x / 10) << 4;
    y = y | (x % 10);
    return (y);
}

int bcd2asc(unsigned char * BcdBuf, unsigned char * AscBuf, int Len)
{
    int i;

    for (i = 0; i < Len; i++)
    {
        *AscBuf = (i % 2) ? (*BcdBuf & 0x0f) : ((*BcdBuf >> 4) & 0x0f);
		*AscBuf += ((*AscBuf > 9) ? ('A' - 10) : '0');
		AscBuf++;
		if (i % 2) {
			BcdBuf++;
		}
    }

	//printf("Converted bcd: %s\n",*AscBuf);

    return 0;
}
// {
//     int i;

//     for (i = 0; i < Len; i++)
//     {
//         AscBuf[i] = (i % 2) ? (BcdBuf[i / 2] & 0x0f) : ((BcdBuf[i / 2] >> 4) & 0x0f);
//         AscBuf[i] += ((AscBuf[i] > 9) ? ('A' - 10) : '0');
//     }

//     return 0;
// }

DL_ERR _bytes_to_hexstr(const unsigned char *bytes,
                     	unsigned char *output,
                    	unsigned int *outputLen) 
{	
	DL_UINT8 ch;
	size_t len = strlen((const char*)bytes);
	if (len % 2 != 0) {
		return -1;
	}
	size_t finalLen = len * 2;
	*outputLen = finalLen;

	while ( finalLen-- > 0 )
		{
			ch = (*bytes >> 4) & 0xf;
			*output++ = DL_NIBBLE_2_ASCHEX(ch);
			ch = *bytes & 0xf;
			*output++ = DL_NIBBLE_2_ASCHEX(ch);
		}
	return 0;
}