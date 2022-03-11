#include <iostream>
#include <string>
#include <sstream>
#include <stdio.h>
#include <iostream>
#include <boost/asio.hpp>
#include "../include/iso8583.hpp"
#include <boost/algorithm/hex.hpp>

#define printHex(buff,size)  for (int i=0; i < size; i++ ) { std::cout << std::setw(2) << std::setfill('0') << std::hex << (int)(buff[i]); }; std::cout << std::endl;

int main(int argc, char* argv[])
{
	unsigned char   packBuf[2048];
	unsigned char   responseBuf[2048];
	unsigned char   responseBufLen[2];
	size_t          packedSize;

	std::string hexString = "f0f1f0f07eff460128e1f30af1f6f2f3f0f6f5f0f2f2f8f1f5f8f0f0f5f2f0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f1f0f0f0f0f0f0f0f0f0f0f0f1f0f0f0f0f0f0f0f0f0f0f0f1f0f2f0f7f1f4f2f6f0f1f6f1f0f0f0f0f0f0f6f1f9f5f4f0f0f0f4f3f3f0f0f8f1f1f2f6f0f1f0f2f0f7f3f0f0f2f0f2f0f7f0f2f0f7f5f9f9f9f0f5f1f0f0f1f0f6f0f1f2f3f4f5f3f7f2f3f0f6f5f0f2f2f8f1f5f8f0f0f5f27ef3f0f0f2f2f0f6f0f0f0f0f0f4f0f7f0f0f0f0f0f7f5f6f5f6f5f6f0f8f7f6f9f1f2f3f4f5f6f7f8f1f2f3f4f5f6f7f8f9f1f2f3f4f5f540d640d98186819640858840828196409485a294965a4b4b4b4b4b4b4b4b4b4b4b4b4b4b4b4b4b4bf0f0f7d9f8f0f0f2e3e5f8f4f0f8f4f0f9f8f651da3e15599c3dd4f1f0f75f2a020840820258008407a0000000041010950500000000009a032105249c01009f02060000000033009f10120110a00000044000dac100000000000000009f1a0208409f2608607c7d64313b62fb9f2701809f3303e0e8e89f34034103029f360200419f370411f6d799f0f3f7f0f1f3f3f0f1f2f9f5f0f0f1c9f0c9e8f9f1d1e4d8d3e6c4e9c3d3d8d9d7e8d5c4d7c1d1c4f0f2f1f0f0f0f0f0f0f0f0f0f0f5f0f0f8f4f0f9f0f2f1f0f0f0f9d4c3c7f0f1f1f3f9e9";
	std::string hex2tring = "f0f1f0f07eff460128e1f30af1f6f2f3f0f6f5f0f2f2f8f1f5f8f0f0f5f2f0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f1f0f0f0f0f0f0f0f0f0f0f0f1f0f0f0f0f0f0f0f0f0f0f0f1f0f2f0f7f1f4f2f6f0f1f6f1f0f0f0f0f0f0f6f1f9f5f4f0f0f0f4f3f3f0f0f8f1f1f2f6f0f1f0f2f0f7f3f0f0f2f0f2f0f7f0f2f0f7f5f9f9f9f0f5f1f0f0f1f0f6f0f1f2f3f4f5f3f7f2f3f0f6f5f0f2f2f8f1f5f8f0f0f5f27ef3f0f0f2f2f0f6f0f0f0f0f0f4f0f7f0f0f0f0f0f7f5f6f5f6f5f6f0f8f7f6f9f1f2f3f4f5f6f7f8f1f2f3f4f5f6f7f8f9f1f2f3f4f5f540d640d98186819640858840828196409485a294965a4b4b4b4b4b4b4b4b4b4b4b4b4b4b4b4b4b4bf0f0f7d9f8f0f0f2e3e5f8f4f0f8f4f0f9f8f651da3e15599c3dd4f1f0f75f2a020840820258008407a0000000041010950500000000009a032105249c01009f02060000000033009f10120110a00000044000dac100000000000000009f1a0208409f2608607c7d64313b62fb9f2701809f3303e0e8e89f34034103029f360200419f370411f6d799f0f3f7f0f1f3f3f0f1f2f9f5f0f0f1c9f0c9e8f9f1d1e4d8d3e6c4e9c3d3d8d9d7e8d5c4d7c1d1c4f0f2f1f0f0f0f0f0f0f0f0f0f0f5f0f0f8f4f0f9f0f2f1f0f0f0f9d4c3c7f0f1f1f3f9e9";
	// size_t packLen = hexString.size()/2;
	// boost::algorithm::unhex(hexString.begin(),hexString.end(),responseBuf);

	//std::cout << std::string(&responseBuf,packLen) << std::endl;
	//printf("%s\n",responseBuf);
	//std::cout << hexStr(responseBuf,packLen) << std::endl << std::endl;

	std::unique_ptr<Iso8583> ioMsg(new Iso8583(hexString,1));

	//ioMsg.unpack(hexString);


	ioMsg->print();

	// inputMsg.

	// if (ioMsg->haveField(50)){
	// 	std::cout << "Before DE50: " << ioMsg->getField(50) << std::endl;
	// 	ioMsg->setField("999",50);
	// 	std::cout << "Later DE50: " << ioMsg->getField(50) << std::endl;
	// }

	// if (ioMsg->haveField(0)){
	// 	std::cout << "Before MSGType: " << ioMsg->getField(0) << std::endl;
	// 	ioMsg->setField("0110",0);
	// 	std::cout << "Later MSGType: " << ioMsg->getField(0) << std::endl;
	// }

	// ioMsg->print();

	// ioMsg->unsetField(50);

	// ioMsg->print();
	
	packedSize = ioMsg->pack(packBuf);

	printHex(packBuf,packedSize);

 	return 0;
}