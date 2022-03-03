#include <iso8583.hpp>
#include <boost/algorithm/hex.hpp>

Iso8583::Iso8583()
{
    DL_ISO8583_DEFS_1987_GetHandler(&this->isoHandler);
    DL_ISO8583_MSG_Init(NULL,0,&this->isoMsg);
}

Iso8583::Iso8583(std::string hexString)
{
    DL_ISO8583_DEFS_1987_GetHandler(&this->isoHandler);
    DL_ISO8583_MSG_Init(NULL,0,&this->isoMsg);
    this->unpack(hexString);
}

Iso8583::Iso8583(std::string hexString, size_t isEbcdic)
{
    DL_ISO8583_DEFS_1987_GetHandler(&this->isoHandler);
    DL_ISO8583_MSG_Init(NULL,0,&this->isoMsg);
    this->setEncoding(isEbcdic);
    this->unpack(hexString);
}

Iso8583::~Iso8583()
{
    DL_ISO8583_MSG_Free(&this->isoMsg);
}

size_t Iso8583::pack(unsigned char *outputBuffer)
{
    unsigned int packedSize = 0;
    DL_ISO8583_MSG_Pack(&this->isoHandler,&this->isoMsg,&outputBuffer[2],&packedSize,this->isEbcdic);
    return packedSize;
}

size_t Iso8583::unpack(std::string hexString)
{  
    size_t strSize = hexString.size();
    if (strSize == 0) {
        this->errorCode = -1;
        return this->errorCode;
    }
    size_t buffLen = strSize/2;

    unsigned char messageBytes[buffLen];

    boost::algorithm::unhex(hexString.begin(),hexString.end(),messageBytes);
    this->errorCode = DL_ISO8583_MSG_Unpack(&this->isoHandler,messageBytes,(unsigned int) buffLen,&this->isoMsg,this->isEbcdic);

    return this->errorCode;
}

size_t Iso8583::unpack(unsigned char *messageBytes, size_t *messageLen)
{
    this->errorCode = DL_ISO8583_MSG_Unpack(&this->isoHandler,messageBytes,*messageLen,&this->isoMsg,this->isEbcdic);
    this->isEbcdic;
    return this->errorCode;
}

size_t Iso8583::setEncoding(size_t isEbcdic)
{
    if (isEbcdic < 0){
        return -1;
    }
    this->isEbcdic = isEbcdic; //strange error
    return this->isEbcdic;
}

size_t Iso8583::haveField(size_t fieldID)
{   
    return DL_ISO8583_MSG_HaveField(fieldID,&this->isoMsg);
}

size_t Iso8583::setField(std::string data, size_t fieldID)
{   
    size_t dataSize = data.size();
    if (dataSize == 0) {
        this->errorCode = -1;
        return this->errorCode;
    }

    this->errorCode = DL_ISO8583_MSG_SetField_Str(fieldID,(unsigned char*)data.c_str(),&this->isoMsg);
    return this->errorCode;
}

std::string Iso8583::getField(size_t fieldID)
{   
    unsigned char* dataBufPtr;
    this->errorCode = DL_ISO8583_MSG_GetField_Str(fieldID,&this->isoMsg,&dataBufPtr);

    if (this->errorCode  != 0) {
        return std::string("ERRO");
    }
    return std::string(reinterpret_cast<char*>(dataBufPtr));
}

void Iso8583::print(){
    DL_ISO8583_MSG_Dump(stdout,0,&this->isoHandler,&this->isoMsg);
}

