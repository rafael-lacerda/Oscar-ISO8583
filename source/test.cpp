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

//extern DL_UINT16 EBCDIC;

DL_UINT16 EBCDIC = 0;

std::string hexStr(const DL_UINT8 *iPtr, DL_UINT32 iNumBytes );
int HexStringToBytes(const char *hexStr, unsigned char *output, unsigned int *outputLen);
unsigned char * hex2bytes(const char *hexStr);

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

    //
    // Populate/Pack message
    //

	/* initialise ISO message */
	DL_ISO8583_MSG_Init(NULL,0,&isoMsg);

	/* set ISO message fields */
	error = DL_ISO8583_MSG_SetField_Str(0,(const unsigned char*)"0100",&isoMsg);
	(void)DL_ISO8583_MSG_SetField_Str(2,(const unsigned char*)"2306502281580052",&isoMsg);
	(void)DL_ISO8583_MSG_SetField_Str(3,(const unsigned char*)"000000",&isoMsg);
	(void)DL_ISO8583_MSG_SetField_Str(4,(const unsigned char*)"000000000001",&isoMsg);
	(void)DL_ISO8583_MSG_SetField_Str(5,(const unsigned char*)"000000000001",&isoMsg);
	(void)DL_ISO8583_MSG_SetField_Str(6,(const unsigned char*)"000000000001",&isoMsg);
	(void)DL_ISO8583_MSG_SetField_Str(7,(const unsigned char*)"0207142601",&isoMsg);
	(void)DL_ISO8583_MSG_SetField_Str(9,(const unsigned char*)"61000000",&isoMsg);
	(void)DL_ISO8583_MSG_SetField_Str(10,(const unsigned char*)"61954000",&isoMsg);
	(void)DL_ISO8583_MSG_SetField_Str(11,(const unsigned char*)"433008",&isoMsg);
	(void)DL_ISO8583_MSG_SetField_Str(12,(const unsigned char*)"112601",&isoMsg);
	(void)DL_ISO8583_MSG_SetField_Str(13,(const unsigned char*)"0207",&isoMsg);
	(void)DL_ISO8583_MSG_SetField_Str(14,(const unsigned char*)"3002",&isoMsg);
	(void)DL_ISO8583_MSG_SetField_Str(15,(const unsigned char*)"0207",&isoMsg);
	(void)DL_ISO8583_MSG_SetField_Str(16,(const unsigned char*)"0207",&isoMsg);
	(void)DL_ISO8583_MSG_SetField_Str(18,(const unsigned char*)"5999",&isoMsg);
	(void)DL_ISO8583_MSG_SetField_Str(22,(const unsigned char*)"051",&isoMsg);
	(void)DL_ISO8583_MSG_SetField_Str(23,(const unsigned char*)"001",&isoMsg);
	(void)DL_ISO8583_MSG_SetField_Str(32,(const unsigned char*)"012345",&isoMsg);
	//(void)DL_ISO8583_MSG_SetField_Str(33,(const unsigned char*)"022020",&isoMsg);
	(void)DL_ISO8583_MSG_SetField_Str(35,(const unsigned char*)"2306502281580052=30022060000040700000",&isoMsg);
	(void)DL_ISO8583_MSG_SetField_Str(37,(const unsigned char*)"756565608769",&isoMsg);
	(void)DL_ISO8583_MSG_SetField_Str(41,(const unsigned char*)"12345678",&isoMsg);
	(void)DL_ISO8583_MSG_SetField_Str(42,(const unsigned char*)"123456789123455",&isoMsg);
	(void)DL_ISO8583_MSG_SetField_Str(43,(const unsigned char*)" O Rafao eh bao mesmo!..................",&isoMsg);
	(void)DL_ISO8583_MSG_SetField_Str(48,(const unsigned char*)"R8002TV",&isoMsg);
	(void)DL_ISO8583_MSG_SetField_Str(49,(const unsigned char*)"840",&isoMsg);
	(void)DL_ISO8583_MSG_SetField_Str(50,(const unsigned char*)"840",&isoMsg);
	(void)DL_ISO8583_MSG_SetField_Str(51,(const unsigned char*)"986",&isoMsg);
	(void)DL_ISO8583_MSG_SetField_Str(52,(const unsigned char*)"51DA3E15599C3DD4",&isoMsg);
	//printf("%x\n",hex2bytes("51DA3E15599C3DD4"));
	(void)DL_ISO8583_MSG_SetField_Str(55,(const unsigned char*)"5F2A020840820258008407A0000000041010950500000000009A032105249C01009F02060000000033009F10120110A00000044000DAC100000000000000009F1A0208409F2608607C7D64313B62FB9F2701809F3303E0E8E89F34034103029F360200419F370411F6D799",&isoMsg);
	(void)DL_ISO8583_MSG_SetField_Str(56,(const unsigned char*)"013301295001I0IY91JUQLWDZCLQRPYNDPAJD",&isoMsg);
	(void)DL_ISO8583_MSG_SetField_Str(61,(const unsigned char*)"000000000050084090210",&isoMsg);
	(void)DL_ISO8583_MSG_SetField_Str(63,(const unsigned char*)"MCG01139Z",&isoMsg);
	//(void)DL_ISO8583_MSG_SetField_Str(127,(const unsigned char*)"01003000020372022020714260143300800001234512345678",&isoMsg);

	if (error != 0){
		printf("Error %d ocurred.",error);
	}

	/* output ISO message content */
	// DL_ISO8583_MSG_Dump(stdout,NULL,&isoHandler,&isoMsg);
	
	/* pack ISO message */
	error = DL_ISO8583_MSG_Pack(&isoHandler,&isoMsg,packBuf,&packedSize);
	if (error != 0){
		printf("Error %d ocurred.\n",error);
	}

	//printf("\n%s\n",packBuf);

	/* free ISO message */
	DL_ISO8583_MSG_Free(&isoMsg);

	/* output packed message (in hex) */
	//DL_OUTPUT_Hex(stdout,NULL,packBuf,packedSize);

	std::cout << hexStr(packBuf,packedSize) << std::endl;

	int mInt = 16;
	DL_UINT8 len = (unsigned char) mInt;
	int AscLen = 2;
	//DL_UINT8 *lenPtr = &len;
	DL_UINT8 lenPtr[10];
	//memcpy((void*)lenPtr,(const void*) len,1);
	// printf("%x\n",len);
	// printf("%x\n",*lenPtr);

	char  format[10], str[20];

	memset(format, 0, sizeof(format));
    sprintf(format, "%c0%lulu", '%', (unsigned long) (AscLen));
    sprintf((char *)str, (char *)format, mInt);
	std::cout << str << std::endl;
	memcpy(lenPtr, (DL_UINT8 *)str, AscLen );

	std::cout << hexStr(lenPtr,2) << std::endl;
	unsigned int length = 0;
	unsigned char buff[108];
	const char * hexStr1 = "5F2A020840820258008407A0000000041010950500000000009A032105249C01009F02060000000033009F10120110A00000044000DAC100000000000000009F1A0208409F2608607C7D64313B62FB9F2701809F3303E0E8E89F34034103029F360200419F370411F6D799";
	unsigned int * lengthPtr = &length;
	HexStringToBytes(hexStr1,buff,lengthPtr);

	std::cout << hexStr(buff,*lengthPtr) << std::endl;

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