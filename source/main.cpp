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
	
	char hexStr[] = "303130307eff460128e1f30af1f63233303635303232383135383030353230303030303030303030303030303030303130303030303030303030303130303030303030303030303130323037313432363031363130303030303036313935343030303433333030383131323630313032303733303032303230373032303735393939303531303031f0f6303132333435f3f7323330363530323238313538303035323d33303032323036303030303034303730303030303735363536353630383736393132333435363738313233343536373839313233343535204f20526166616f2065682062616f206d65736d6f212e2e2e2e2e2e2e2e2e2e2e2e2e2e2e2e2e2ef0f0f75238303032545638343038343039383651da3e15599c3dd4f2f1f435463241303230383430383230323538303038343037413030303030303030343130313039353035303030303030303030303941303332313035323439433031303039463032303630303030303030303333303039463130313230313130413030303030303434303030444143313030303030303030303030303030303039463141303230383430394632363038363037433744363433313342363246423946323730313830394633333033453045384538394633343033343130333032394633363032303034313946333730343131463644373939f0f3f73031333330313239353030314930495939314a55514c57445a434c515250594e4450414a44f0f2f1303030303030303030303530303834303930323130f0f0f94d434730313133395a";
	DL_UINT8 * hexStr1 = (DL_UINT8*) hexStr;
	DL_UINT16 strLen = strlen(hexStr)+1;
	DL_UINT16 strLen1 = sizeof(*hexStr1)/sizeof(hexStr1);
	DL_UINT8 outBuff[strLen/2];
	std::cout << strLen << " - " << strLen1 <<  std::endl;
	
	//unsigned int * lengthPtr = &length;
	hex2bin(hexStr,outBuff);

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