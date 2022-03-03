//
// DL ISO8583 library demo
//
#include <iostream>
#include <string>
#include <sstream>
#include <oscar/dl_iso8583.h>
#include <oscar/dl_iso8583_defs_1987.h>
#include <oscar/dl_output.h> // for 'DL_OUTPUT_Hex'
#include <boost/algorithm/hex.hpp>
#include <stdio.h>
#include <iostream>
#include <boost/asio.hpp>

using namespace boost::asio;
using ip::tcp;
using std::string;
using std::cout;
using std::endl;

DL_UINT16 EBCDIC = 1;
DL_UINT16 DEBUG = 0;

std::string hexStr(const DL_UINT8 *iPtr, DL_UINT32 iNumBytes );
int asc2bcd(unsigned char * AscBuf, unsigned char * BcdBuf, int Len);
unsigned char hex2bcd (unsigned char x);

inline unsigned char *uc_str(char *s){
  return reinterpret_cast<unsigned char*>(s);
}

int main ( void )
{
	DL_ISO8583_HANDLER isoHandler;
	DL_ISO8583_MSG     isoMsg;
	DL_UINT8           packBuf[2048];
	DL_UINT8           responseBuf[2048];
	DL_UINT8           responseBufLen[2];
	DL_UINT16          packedSize;
	DL_ERR 			   err;

	/* get ISO-8583 1987 handler */
	DL_ISO8583_DEFS_1987_GetHandler(&isoHandler);
	
	//char hexStr0[] = "30313030feff460128e1f30a00000000000000023136323330363530323238313538303035323030303030303030303030303030303030313030303030303030303030313030303030303030303030313032303731343236303136313030303030303631393534303030343333303038313132363031303230373330303230323037303230373539393930353130303130363031323334353337323330363530323238313538303035323d333030323230363030303030343037303030303037353635363536303837363931323334353637383132333435363738393132333435354f20526166616f2065682062616f206d65736d6f2120202e2e2e2e2e2e2e2e2e2e2e2e2e205553413030375238303032545638343038343039383651da3e15599c3dd43130375f2a020840820258008407a0000000041010950500000000009a032105249c01009f02060000000033009f10120110a00000044000dac100000000000000009f1a0208409f2608607c7d64313b62fb9f2701809f3303e0e8e89f34034103029f360200419f370411f6d7993033373031333330313239353030314930495939314a55514c57445a434c515250594e4450414a443032313030303030303030303035303038343039303231303030394d434730313133395a3035303031303033303030303230333732303232303230373134323630313433333030383030303031323334353132333435363738";
	char hexStr0[] = "f0f1f0f07eff460128e1f30af1f6f2f3f0f6f5f0f2f2f8f1f5f8f0f0f5f2f0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f1f0f0f0f0f0f0f0f0f0f0f0f1f0f0f0f0f0f0f0f0f0f0f0f1f0f2f0f7f1f4f2f6f0f1f6f1f0f0f0f0f0f0f6f1f9f5f4f0f0f0f4f3f3f0f0f8f1f1f2f6f0f1f0f2f0f7f3f0f0f2f0f2f0f7f0f2f0f7f5f9f9f9f0f5f1f0f0f1f0f6f0f1f2f3f4f5f3f7f2f3f0f6f5f0f2f2f8f1f5f8f0f0f5f27ef3f0f0f2f2f0f6f0f0f0f0f0f4f0f7f0f0f0f0f0f7f5f6f5f6f5f6f0f8f7f6f9f1f2f3f4f5f6f7f8f1f2f3f4f5f6f7f8f9f1f2f3f4f5f540d640d98186819640858840828196409485a294965a4b4b4b4b4b4b4b4b4b4b4b4b4b4b4b4b4b4bf0f0f7d9f8f0f0f2e3e5f8f4f0f8f4f0f9f8f651da3e15599c3dd4f1f0f75f2a020840820258008407a0000000041010950500000000009a032105249c01009f02060000000033009f10120110a00000044000dac100000000000000009f1a0208409f2608607c7d64313b62fb9f2701809f3303e0e8e89f34034103029f360200419f370411f6d799f0f3f7f0f1f3f3f0f1f2f9f5f0f0f1c9f0c9e8f9f1d1e4d8d3e6c4e9c3d3d8d9d7e8d5c4d7c1d1c4f0f2f1f0f0f0f0f0f0f0f0f0f0f5f0f0f8f4f0f9f0f2f1f0f0f0f9d4c3c7f0f1f1f3f9e9";
	//char hexStr0[] = "01d7f0f1f0f07eff460128e1f30af1f6f2f3f0f6f5f0f2f2f8f1f5f8f0f0f5f2f0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f1f0f0f0f0f0f0f0f0f0f0f0f1f0f0f0f0f0f0f0f0f0f0f0f1f0f2f0f7f1f4f2f6f0f1f6f1f0f0f0f0f0f0f6f1f9f5f4f0f0f0f4f3f3f0f0f8f1f1f2f6f0f1f0f2f0f7f3f0f0f2f0f2f0f7f0f2f0f7f5f9f9f9f0f5f1f0f0f1f0f6f0f1f2f3f4f5f3f7f2f3f0f6f5f0f2f2f8f1f5f8f0f0f5f27ef3f0f0f2f2f0f6f0f0f0f0f0f4f0f7f0f0f0f0f0f7f5f6f5f6f5f6f0f8f7f6f9f1f2f3f4f5f6f7f8f1f2f3f4f5f6f7f8f9f1f2f3f4f5f540d640d98186819640858840828196409485a294965a4b4b4b4b4b4b4b4b4b4b4b4b4b4b4b4b4b4bf0f0f7d9f8f0f0f2e3e5f8f4f0f8f4f0f9f8f651da3e15599c3dd4f1f0f75f2a020840820258008407a0000000041010950500000000009a032105249c01009f02060000000033009f10120110a00000044000dac100000000000000009f1a0208409f2608607c7d64313b62fb9f2701809f3303e0e8e89f34034103029f360200419f370411f6d799f0f3f7f0f1f3f3f0f1f2f9f5f0f0f1c9f0c9e8f9f1d1e4d8d3e6c4e9c3d3d8d9d7e8d5c4d7c1d1c4f0f2f1f0f0f0f0f0f0f0f0f0f0f5f0f0f8f4f0f9f0f2f1f0f0f0f9d4c3c7f0f1f1f3f9e9";
	DL_UINT16 strLen = strlen(hexStr0);
	DL_UINT16 buffLen = strLen/2;
	DL_UINT8 outBuff[buffLen];
	
	asc2bcd(uc_str(hexStr0),outBuff,strLen);

	DL_ISO8583_MSG_Init(NULL,0,&isoMsg);

	err = DL_ISO8583_MSG_Unpack(&isoHandler,outBuff,buffLen,&isoMsg,EBCDIC);
	if (err != 0){
		printf("Error %d ocurred on Unpacking.\n",err);
	}

	cout << "\nOutput message: " << endl;
	DL_ISO8583_MSG_Dump(stdout,0,&isoHandler,&isoMsg);
	cout << endl;

	err = DL_ISO8583_MSG_Pack(&isoHandler,&isoMsg,&packBuf[2],&packedSize,EBCDIC);
	if (err != 0){
		printf("Error %d ocurred on Packing.\n",err);
	}

	packBuf[0] = (size_t) (packedSize/256);
	packBuf[1] = (size_t) (packedSize%256);

	packedSize +=2;

	//std::cout << "\nPacked Raw Hex: " << hexStr(packBuf,packedSize) << std::endl;
	//std::cout << "\nPacked Raw Hex: " << hexStr(outBuff,buffLen) << std::endl;


	cout << "\nOutput message: " << endl;
	DL_ISO8583_MSG_Dump(stdout,0,&isoHandler,&isoMsg);
	cout << endl;

	DL_ISO8583_MSG_Free(&isoMsg);


	boost::asio::io_service io_service;

	//socket creation
	tcp::socket socket(io_service);

	//connection
	socket.connect( tcp::endpoint( boost::asio::ip::address::from_string("10.190.16.174"), 10510 ));

	boost::system::error_code error;
	boost::asio::write( socket, boost::asio::buffer(packBuf, packedSize), error );

	if( !error ) {
		//cout << "Client sent hello message!" << endl;
	}
	else {
		cout << "send failed: " << error.message() << endl;
	}

	// getting response from server
	boost::asio::streambuf receive_buffer;
	//size_t responseLen = boost::asio::read(socket, receive_buffer, boost::asio::transfer_all(), error);

	char responseBuf1[2048];
	size_t responseLen = boost::asio::read(socket, boost::asio::buffer(responseBufLen, 2), error);

	size_t isoSize = (size_t) ( (size_t) (responseBufLen[0]*256)  + (size_t) (responseBufLen[1]));

	cout << "Bytes: " << responseLen << " Hex: " << hexStr(responseBufLen, responseLen) << " - " << isoSize << endl;

	responseLen = boost::asio::read(socket, boost::asio::buffer(responseBuf, isoSize), error);
	//receive_buffer.sgetn(responseBuf1,responseLen);

	// cout << "Read message. \n";

	if( error && error != boost::asio::error::eof ) {
		cout << "receive failed: " << error.message() << endl;
	}
	else {
		//const char* data = boost::asio::buffer_cast<const char*>(receive_buffer.data());
		// cout << hexStr(responseBuf, responseLen) << endl;
	}

	DL_ISO8583_MSG_Init(NULL,0,&isoMsg);

	err = DL_ISO8583_MSG_Unpack(&isoHandler,responseBuf,isoSize,&isoMsg,EBCDIC);
	if (err != 0){
		printf("Error %d ocurred on Unpacking.\n",err);
	}

	cout << "\nInput message: " << endl;
	DL_ISO8583_MSG_Dump(stdout, 0,&isoHandler,&isoMsg);
	cout << endl;

	DL_ISO8583_MSG_Free(&isoMsg);

	return 0;
}

std::string hexStr(const DL_UINT8 *iPtr, DL_UINT32 iNumBytes ){
	std::stringstream ss;
     ss << std::hex;

     for( int i(0) ; i < iNumBytes; ++i )
         ss << std::setw(2) << std::setfill('0') << (int)iPtr[i];

     return ss.str();
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