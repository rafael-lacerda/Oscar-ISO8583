//
// DL ISO8583 library demo
//
#include <iostream>
#include <string>
#include "dl_iso8583.h"
#include "dl_iso8583_defs_1987.h"
#include "dl_output.h" // for 'DL_OUTPUT_Hex'
#include <boost/algorithm/hex.hpp>
#include <stdio.h>

void inSituEbcdicToAscii (char *s);
std::string hextobin(const std::string &s);
std::string bintohex(const std::string &s);
std::string hex2str(std::string &hex);
char char2int(char input);

int main ( void )
{
	DL_ISO8583_HANDLER isoHandler;
	DL_ISO8583_MSG     isoMsg;
	DL_UINT8           packBuf[2048];
	DL_UINT8           packBuf2[1000];
	DL_UINT16          packedSize;
	DL_ERR 			   error;

	/* get ISO-8583 1993 handler */
	DL_ISO8583_DEFS_1987_GetHandler(&isoHandler);

    //
    // Populate/Pack message
    //

	/* initialise ISO message */
	DL_ISO8583_MSG_Init(NULL,0,&isoMsg);

	/* set ISO message fields */
	error = DL_ISO8583_MSG_SetField_Str(0,(const unsigned char*)"0210",&isoMsg);
	(void)DL_ISO8583_MSG_SetField_Str(2,(const unsigned char*)"1234567890123456",&isoMsg);
	(void)DL_ISO8583_MSG_SetField_Str(4,(const unsigned char*)"5699",&isoMsg);
	(void)DL_ISO8583_MSG_SetField_Str(4,(const unsigned char*)"4599",&isoMsg);
	(void)DL_ISO8583_MSG_SetField_Str(11,(const unsigned char*)"234123",&isoMsg);
	(void)DL_ISO8583_MSG_SetField_Str(39,(const unsigned char*)"55",&isoMsg);
	(void)DL_ISO8583_MSG_SetField_Str(41,(const unsigned char*)"12345",&isoMsg);
	(void)DL_ISO8583_MSG_SetField_Str(42,(const unsigned char*)"678901234",&isoMsg);
	(void)DL_ISO8583_MSG_SetField_Str(125,(const unsigned char*)"BLAH BLAH",&isoMsg);
	if (error != 0){
		printf("Error %d ocurred.",error);
	}

	/* output ISO message content */
	DL_ISO8583_MSG_Dump(stdout,NULL,&isoHandler,&isoMsg);
	
	/* pack ISO message */
	error = DL_ISO8583_MSG_Pack(&isoHandler,&isoMsg,packBuf,&packedSize);
	if (error != 0){
		printf("Error %d ocurred.",error);
	}

	/* free ISO message */
	DL_ISO8583_MSG_Free(&isoMsg);

	/* output packed message (in hex) */
	DL_OUTPUT_Hex(stdout,NULL,packBuf,packedSize);
	printf((const char*)packBuf,sizeof(packBuf));
	//
	// Unpack message
	//

	/* initialise ISO message */
	DL_ISO8583_MSG_Init(NULL,0,&isoMsg);

	/* unpack ISO message */
	(void)DL_ISO8583_MSG_Unpack(&isoHandler,packBuf,packedSize,&isoMsg);

	//printf((const char *)packBuf);
	//std::string s(packBuf, packBuf+packedSize);
	//std::cout << std::string(packBuf,sizeof(packBuf)/sizeof(packBuf[0])) << "\n";

	/* free ISO message */
	DL_ISO8583_MSG_Free(&isoMsg);

	//std::string msg ="\xf0\xf1\xf0\xf0~?F\x01\xa8\xe1\xf2\n\xf1\xf6\xf2\xf3\xf0\xf6\xf5\xf0\xf2\xf0\xf1\xf3\xf6\xf0\xf6\xf6\xf6\xf2\xf0\xf0\xf3\xf0\xf0\xf0\xf0\xf0\xf0\xf0\xf0\xf0\xf0\xf0\xf0\xf0\xf0\xf1\xf0\xf0\xf0\xf0\xf0\xf0\xf0\xf0\xf0\xf0\xf0\xf1\xf0\xf0\xf0\xf0\xf0\xf0\xf0\xf0\xf0\xf0\xf0\xf1\xf0\xf1\xf2\xf1\xf1\xf9\xf4\xf6\xf0\xf8\xf6\xf7\xf9\xf7\xf9\xf8\xf1\xf6\xf4\xf6\xf0\xf8\xf0\xf1\xf2\xf1\xf3\xf0\xf0\xf8\xf0\xf1\xf2\xf1\xf0\xf1\xf2\xf1\xf5\xf9\xf9\xf9\xf0\xf5\xf1\xf0\xf0\xf1\xf0\xf6\xf0\xf1\xf2\xf3\xf4\xf5\xf0\xf6\xf0\xf2\xf2\xf0\xf2\xf0\xf3\xf7\xf2\xf3\xf0\xf6\xf5\xf0\xf2\xf0\xf1\xf3\xf6\xf0\xf6\xf6\xf6\xf2~\xf3\xf0\xf0\xf8\xf2\xf0\xf6\xf0\xf0\xf0\xf0\xf0\xf5\xf1\xf7\xf0\xf0\xf0\xf0\xf0\xf1\xf1\xf1\xf1\xf1\xf1\xf1\xf1\xf1\xf1\xf1\xf1\xf1\xf2\xf3\xf4\xf5\xf6\xf7\xf8\xf1\xf2\xf3\xf4\xf5\xf6\xf7\xf8\xf9\xf1\xf2\xf3\xf4\xf5\xf5@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\xf0\xf0\xf6\xd9\xf1\xf4\xf0\xf1\xc3\xf9\xf8\xf6\xf8\xf4\xf0\xf9\xf8\xf6\xa0\xe1\xcb\x153\xd1bi\xf1\xf0\xf7_*\x02\t\x86\x82\x02X\x00\x84\x07\xa0\x00\x00\x00\x04\x10\x10\x95\x05\x00\x00\x00\x00\x00\x9a\x03!\x12\x02\x9c\x01\x00\x9f\x02\x06\x00\x00\x00\x00\x14\x00\x9f\x10\x12\x01\x10%\x00\x00\x02@\x00\xda\xc1\x00\x00\x00\x00\x00\x00\x00\x00\x9f\x1a\x02\x00v\x9f&\x08\x96\x92\xf5\x18b\xe7 |\x9f\'\x01\x80\x9f3\x03\xe0\xe8\xe8\x9f4\x03A\x03\x02\x9f6\x02\x01|\x9f7\x04%\x0f=\xa0\xf0\xf2\xf1\xf0\xf0\xf0\xf0\xf0\xf0\xf0\xf0\xf0\xf0\xf5\xf0\xf0\xf0\xf7\xf6\xf9\xf0\xf2\xf1\xf0\xf0\xf0\xf9\xd4\xc3\xc7\xd9\xc8\xe2\xc6\xf6\xe8";
	//DL_UINT8 msg2[] ="30313030feff440108e1e10a0000000001000002313632333036353032333137383431373334303033303030303030303030303030353030303030303030303030323536303030303030303030353030303132343137353535333735313137373037363130303030303030303030303231373535353330313234333030373031323430313234353939393831303036303132333435353438313531363834343535304835553655324b31323334353637383930313233343552657374617572616e746520582020202020202020202042656c6f20486f72697a6f6e7420425241303139543432303730313033323130393230333332313938363834303938363033373031333330313239353030315a4151555635594b5333393941394d444d344c4249464432453032313130323531303030303630303030373639303231303030394d4347303131305a59303238414a4a4c5451412b495753384152454145424a5341314d414141413d303530303130303330303030323033373230323230313234313735353533303030303032303030303132333435304835553655324b";
	DL_UINT8 msgPA[] = "f0f1f0f07e3c4601a8e1f208f1f6f2f3f0f6f5f0f2f5f2f6f9f0f4f1f6f6f0f0f3f0f0f0f0f0f0f0f0f0f0f0f9f8f9f9f0f0f0f0f0f0f0f0f9f8f9f9f0f0f0f0f0f0f0f0f9f8f9f9f0f1f2f5f1f5f0f3f0f6f0f0f0f0f0f1f1f5f0f3f0f6f0f1f2f5f2f2f0f8f5f9f9f9f0f5f1f0f0f1f0f6f0f1f2f3f4f5f0f6f0f2f2f0f2f0f3f7f2f3f0f6f5f0f2f5f2f6f9f0f4f1f6f67ef3f0f0f9f2f0f6f0f0f0f0f0f0f2f8f0f0f0f0f0f6f4f5f3f6f8f8f8f8f7f4f5f1f2f3f4f5f6f7f8f1f2f3f4f5f6f7f8f9f1f2f3f4f5f6d985a2a381a4998195a38540e740404040404040404040c285939640c8969989a99695a340c2d9c1f0f0f6d9f1f4f0f1c3f9f8f6f9f8f6f9f8f6004dc716a1e22061f1f0f75f2a020986820258008407a0000000041010950500000000009a032201259c01009f02060000000098999f10120110250000024000dac100000000000000009f1a0200769f2608636b73da547e03b39f2701809f3303e0e8e89f34034103029f3602005b9f370419cc3428f0f2f1f0f0f0f0f0f0f0f0f0f0f5f0f0f0f7f6f9f0f2f1f0";
	// header = 0192
	DL_UINT8 msgVP[] = "\x30\x31\x30\x30\xfe\x3c\x46\x01\xa8\xe1\xf2\x08\x00\x00\x00\x00\x00\x00\x00\x02\x31\x36\x32\x33\x30\x36\x35\x30\x32\x36\x33\x32\x34\x36\x30\x35\x31\x37\x30\x30\x33\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x31\x30\x31\x39\x39\x30\x30\x30\x30\x30\x30\x30\x31\x30\x31\x39\x39\x30\x30\x30\x30\x30\x30\x30\x31\x30\x31\x39\x39\x30\x31\x32\x35\x31\x35\x30\x35\x35\x37\x30\x30\x30\x30\x30\x31\x31\x35\x30\x35\x35\x37\x30\x31\x32\x35\x33\x30\x30\x39\x35\x39\x39\x39\x30\x35\x31\x30\x30\x31\x30\x36\x30\x31\x32\x33\x34\x35\x30\x36\x30\x32\x32\x30\x32\x30\x33\x37\x32\x33\x30\x36\x35\x30\x32\x36\x33\x32\x34\x36\x30\x35\x31\x37\x3d\x33\x30\x30\x39\x32\x30\x36\x30\x30\x30\x30\x30\x32\x35\x32\x30\x30\x30\x30\x30\x36\x34\x35\x33\x36\x38\x38\x38\x38\x37\x34\x35\x31\x32\x33\x34\x35\x36\x37\x38\x31\x32\x33\x34\x35\x36\x37\x38\x39\x31\x32\x33\x34\x35\x36\x52\x65\x73\x74\x61\x75\x72\x61\x6e\x74\x65\x20\x58\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x42\x65\x6c\x6f\x20\x48\x6f\x72\x69\x7a\x6f\x6e\x74\x20\x42\x52\x41\x30\x30\x36\x52\x31\x34\x30\x31\x43\x39\x38\x36\x39\x38\x36\x39\x38\x36\x71\x48\xd1\x56\xa2\xde\x21\x3c\x31\x30\x37\x5f\x2a\x02\x09\x86\x82\x02\x58\x00\x84\x07\xa0\x00\x00\x00\x04\x10\x10\x95\x05\x00\x00\x00\x00\x00\x9a\x03\x22\x01\x25\x9c\x01\x00\x9f\x02\x06\x00\x00\x00\x01\x01\x99\x9f\x10\x12\x01\x10\x25\x00\x00\x02\x40\x00\xda\xc1\x00\x00\x00\x00\x00\x00\x00\x00\x9f\x1a\x02\x00\x76\x9f\x26\x08\x29\xe1\x68\xac\x9b\xf8\x01\x31\x9f\x27\x01\x80\x9f\x33\x03\xe0\xe8\xe8\x9f\x34\x03\x41\x03\x02\x9f\x36\x02\x00\x5b\x9f\x37\x04\x32\x97\x42\xf9\x30\x32\x31\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x35\x30\x30\x30\x37\x36\x39\x30\x32\x31\x30\x30\x35\x30\x30\x31\x30\x30\x33\x30\x30\x30\x30\x32\x30\x33\x37\x32\x30\x32\x32\x30\x31\x32\x35\x31\x35\x30\x35\x35\x37\x30\x30\x30\x30\x30\x31\x30\x30\x30\x30\x31\x32\x33\x34\x35\x31\x32\x33\x34\x35\x36\x37\x38";
	// header = 01cf
	const char* msgtest = "\x30\x31\x30\x30\xfe\x3c\x46\x01\xa8\xe1\xf2\x08\x00\x00\x00\x00\x00\x00\x00\x02\x31\x36\x32\x33\x30\x36\x35\x30\x32\x36\x33\x32\x34\x36\x30\x35\x31\x37\x30\x30\x33\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x31\x30\x31\x39\x39\x30\x30\x30\x30\x30\x30\x30\x31\x30\x31\x39\x39\x30\x30\x30\x30\x30\x30\x30\x31\x30\x31\x39\x39\x30\x31\x32\x35\x31\x35\x30\x35\x35\x37\x30\x30\x30\x30\x30\x31\x31\x35\x30\x35\x35\x37\x30\x31\x32\x35\x33\x30\x30\x39\x35\x39\x39\x39\x30\x35\x31\x30\x30\x31\x30\x36\x30\x31\x32\x33\x34\x35\x30\x36\x30\x32\x32\x30\x32\x30\x33\x37\x32\x33\x30\x36\x35\x30\x32\x36\x33\x32\x34\x36\x30\x35\x31\x37\x3d\x33\x30\x30\x39\x32\x30\x36\x30\x30\x30\x30\x30\x32\x35\x32\x30\x30\x30\x30\x30\x36\x34\x35\x33\x36\x38\x38\x38\x38\x37\x34\x35\x31\x32\x33\x34\x35\x36\x37\x38\x31\x32\x33\x34\x35\x36\x37\x38\x39\x31\x32\x33\x34\x35\x36\x52\x65\x73\x74\x61\x75\x72\x61\x6e\x74\x65\x20\x58\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x42\x65\x6c\x6f\x20\x48\x6f\x72\x69\x7a\x6f\x6e\x74\x20\x42\x52\x41\x30\x30\x36\x52\x31\x34\x30\x31\x43\x39\x38\x36\x39\x38\x36\x39\x38\x36\x71\x48\xd1\x56\xa2\xde\x21\x3c\x31\x30\x37\x5f\x2a\x02\x09\x86\x82\x02\x58\x00\x84\x07\xa0\x00\x00\x00\x04\x10\x10\x95\x05\x00\x00\x00\x00\x00\x9a\x03\x22\x01\x25\x9c\x01\x00\x9f\x02\x06\x00\x00\x00\x01\x01\x99\x9f\x10\x12\x01\x10\x25\x00\x00\x02\x40\x00\xda\xc1\x00\x00\x00\x00\x00\x00\x00\x00\x9f\x1a\x02\x00\x76\x9f\x26\x08\x29\xe1\x68\xac\x9b\xf8\x01\x31\x9f\x27\x01\x80\x9f\x33\x03\xe0\xe8\xe8\x9f\x34\x03\x41\x03\x02\x9f\x36\x02\x00\x5b\x9f\x37\x04\x32\x97\x42\xf9\x30\x32\x31\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x35\x30\x30\x30\x37\x36\x39\x30\x32\x31\x30\x30\x35\x30\x30\x31\x30\x30\x33\x30\x30\x30\x30\x32\x30\x33\x37\x32\x30\x32\x32\x30\x31\x32\x35\x31\x35\x30\x35\x35\x37\x30\x30\x30\x30\x30\x31\x30\x30\x30\x30\x31\x32\x33\x34\x35\x31\x32\x33\x34\x35\x36\x37\x38";

	printf((const char*)msgVP,sizeof(msgVP));
	std::cout << std::endl << std::hex << msgtest << std::endl;
	printf("\n%s",msgtest);

	packedSize = sizeof(msgVP);
	// header = 019f
	//std::cout << msg << "\n";
	// std::string msg ="30313030feff440108e1e10a0000000001000002313632333036353032333137383431373334303033303030303030303030303030353030303030303030303030323536303030303030303030353030303132343137353535333735313137373037363130303030303030303030303231373535353330313234333030373031323430313234353939393831303036303132333435353438313531363834343535304835553655324b31323334353637383930313233343552657374617572616e746520582020202020202020202042656c6f20486f72697a6f6e7420425241303139543432303730313033323130393230333332313938363834303938363033373031333330313239353030315a4151555635594b5333393941394d444d344c4249464432453032313130323531303030303630303030373639303231303030394d4347303131305a59303238414a4a4c5451412b495753384152454145424a5341314d414141413d303530303130303330303030323033373230323230313234313735353533303030303032303030303132333435304835553655324b";
	// std::string msg3 = hex2str(msg);
	// auto charMsg = msg3.c_str();
	// strcpy((char*)packBuf2,charMsg);
	//auto msg2 = reinterpret_cast<DL_UINT8>(const_cast<char*>(charMsg));

	// printf("%s\nSize: %d\n", msg3, sizeof(msg3));

	// DL_UINT8 msg2[] = "0210323A40010A4180103800000000000000000420050805011392120805042004225132072000001000000115604000800411 163011511463331563GBAAASDD ERRR 1300101B54391001000017654350000000000090300000268410000000300000000000000898100009431000000000000000000 000000000000000036000299";
	// DL_UINT8 msg3[(sizeof(msg2)/2)];

	// for (int i = 0; i < sizeof(msg2); i=2+i){
	// 	msg3[i/2] = ("0x%s%s",msg2[i],msg2[i+1]);
	// }

	// packedSize = sizeof(msg3);
	// printf("%s\nSize: %d\n", msg3, packedSize);

	//char* tmpMsg = &msg[0];
	//DL_UINT8 msg2[] = (const char)uchrMsg[0];

	//inSituEbcdicToAscii ((char*) msg2);
    // printf ("%s\nSize: %d\n", msg2, sizeof(packBuf2));

	// packedSize = sizeof(msg2);
	// printf ("Size: %d",packedSize);
	
	//std::string hash = boost::algorithm::unhex(msg); 
	//std::copy(hash.begin(), hash.end(), packBuf);

    // declaring character array
    // char char_array[packedSize + 1];
 
    // // copying the contents of the
    // // string to char array
    // strcpy((char *) packBuf, msg.c_str());

	/* initialise ISO message */
	DL_ISO8583_MSG_Init(NULL,0,&isoMsg);

	/* unpack ISO message */
	(void)DL_ISO8583_MSG_Unpack(&isoHandler,msgVP,packedSize,&isoMsg);

	/* output ISO message content */
	DL_ISO8583_MSG_Dump(stdout,NULL,&isoHandler,&isoMsg);

	/* free ISO message */
	DL_ISO8583_MSG_Free(&isoMsg);

	return 0;
}

void inSituEbcdicToAscii (char *s) {
    static char etoa[] =
        "                                "
        "                                "
        "           .<(+|&         !$*); "  // first char here is real space
        "-/         ,%_>?         `:#@'=\""
        " abcdefghi       jklmnopqr      "
        "  stuvwxyz                      "
        " ABCDEFGHI       JKLMNOPQR      "
        "  STUVWXYZ      0123456789      ";

    while (*s != '\0') {
        *s = etoa[(unsigned char)*s];
        s++;
    }
}

// std::string hextobin(const std::string &s){
//     std::string out;
//     for(auto i: s){
//         uint8_t n;
//         if(i <= '9' and i >= '0')
//             n = i - '0';
//         else
//             n = 10 + i - 'A';
//         for(int8_t j = 3; j >= 0; --j)
//             out.push_back((n & (1<<j))? '1':'0');
//     }

//     return out;
// }

// std::string bintohex(const std::string &s){
//     std::string out;
//     for(uint i = 0; i < s.size(); i += 4){
//         int8_t n = 0;
//         for(uint j = i; j < i + 4; ++j){
//             n <<= 1;
//             if(s[j] == '1')
//                 n |= 1;
//         }

//         if(n<=9)
//             out.push_back('0' + n);
//         else
//             out.push_back('A' + n - 10);
//     }

//     return out;
// }

char char2int(char input)
{
    if (input >= '0' && input <= '9')
        return input - '0';
    if (input >= 'A' && input <= 'F')
        return input - 'A' + 10;
    if (input >= 'a' && input <= 'f')
        return input - 'a' + 10;

    throw std::runtime_error("Incorrect symbol in hex string");
};

std::string hex2str(std::string &hex)
{
    std::string out;
    out.resize(hex.size() / 2 + hex.size() % 2);

    std::string::iterator it = hex.begin();
    std::string::iterator out_it = out.begin();
    if (hex.size() % 2 != 0) {
        *out_it++ = char(char2int(*it++));
    }

    for (; it < hex.end() - 1; it++) {
        *out_it++ = char2int(*it++) << 4 | char2int(*it);
    };

    return out;
}