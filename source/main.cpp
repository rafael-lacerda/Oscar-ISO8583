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

DL_UINT16 EBCDIC = 0;

std::string hexStr(const DL_UINT8 *iPtr, DL_UINT32 iNumBytes );
int HexStringToBytes(const char *hexStr, unsigned char *output, unsigned int *outputLen);
unsigned char * hex2bytes(const char *hexStr);
void hex2bin(const char* src, unsigned char* target);
int asc2bcd(unsigned char * AscBuf, unsigned char * BcdBuf, int Len);
unsigned char hex2bcd (unsigned char x);


inline unsigned char *uc_str(char *s){
  return reinterpret_cast<unsigned char*>(s);
}

int main ( void )
{
	DL_ISO8583_HANDLER isoHandler;
	DL_ISO8583_MSG     isoMsg;
	DL_UINT8           packBuf[1000];
	DL_UINT16          packedSize;
	DL_ERR 			   error;

	//EBCDIC = 0;

	/* get ISO-8583 1993 handler */
	DL_ISO8583_DEFS_1987_GetHandler(&isoHandler);
	
	char hexStr[] = "30313030feff460128e1f30a00000000000000023136323330363530323238313538303035323030303030303030303030303030303030313030303030303030303030313030303030303030303030313032303731343236303136313030303030303631393534303030343333303038313132363031303230373330303230323037303230373539393930353130303130363031323334353337323330363530323238313538303035323d333030323230363030303030343037303030303037353635363536303837363931323334353637383132333435363738393132333435354f20526166616f2065682062616f206d65736d6f2120202e2e2e2e2e2e2e2e2e2e2e2e2e205553413030375238303032545638343038343039383651da3e15599c3dd43130375f2a020840820258008407a0000000041010950500000000009a032105249c01009f02060000000033009f10120110a00000044000dac100000000000000009f1a0208409f2608607c7d64313b62fb9f2701809f3303e0e8e89f34034103029f360200419f370411f6d7993033373031333330313239353030314930495939314a55514c57445a434c515250594e4450414a443032313030303030303030303035303038343039303231303030394d434730313133395a3035303031303033303030303230333732303232303230373134323630313433333030383030303031323334353132333435363738";
	DL_UINT8 * hexStr1 = (DL_UINT8*) hexStr;
	DL_UINT16 strLen = strlen(hexStr)+1;
	DL_UINT16 strLen1 = sizeof(*hexStr1)/sizeof(hexStr1);
	DL_UINT8 outBuff[strLen/2];
	std::cout << strLen << " - " << strLen1 <<  std::endl;
	
	//unsigned int * lengthPtr = &length;
	//hex2bin(hexStr,outBuff);
	asc2bcd(uc_str(hexStr),outBuff,strLen);

	printf("\nISO8583 Hex Buf:%s\n", hexStr);
	printf("\nISO8583 Req Buf:%s\n", outBuff);

	// for (int i = 0; i < strLen; i+=2) {
	// 	char hex[3];
	// 	memcpy(hex,&hexStr1[i],2);
	// 	hex[2] = '\0';
	// 	outBuff[i/2] = (int)strtol(hex, NULL, 16);  
	// }

	DL_ISO8583_MSG_Init(NULL,0,&isoMsg);

	error = DL_ISO8583_MSG_Unpack(&isoHandler,outBuff,strLen/2,&isoMsg);
	if (error != 0){
		printf("Error %d ocurred.\n",error);
	}

	DL_ISO8583_MSG_Dump(stdout,NULL,&isoHandler,&isoMsg);

	DL_ISO8583_MSG_Free(&isoMsg);
	// std::cout << hexStr(outBuff,strLen) << std::endl;

	// char hex[] = "30";                          // here is the hex string
	// int num = (int)strtol(hex, NULL, 16);       // number base 16
	// printf("%c\n", num);                        // print it as a char
	// printf("%d\n", num);                        // print it as decimal
	// printf("%X\n", num);

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