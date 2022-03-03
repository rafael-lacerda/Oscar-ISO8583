#ifndef ISO_8583_HPP
#define ISO_8583_HPP

#include <oscar/dl_iso8583.h>
#include <oscar/dl_iso8583_defs_1987.h>
#include <oscar/dl_iso8583_defs_1993.h>
#include <oscar/dl_output.h>
#include <string>

class Iso8583 {

    public:

    Iso8583();
    Iso8583(std::string hexString);
    Iso8583(std::string hexString, size_t isEbcdic);
    ~Iso8583();

    size_t pack(unsigned char *outputBuffer);
    size_t unpack(std::string hexString);
    size_t unpack(unsigned char *messageBytes, size_t *messageLen);

    size_t setEncoding(size_t isEbcdic);

    size_t haveField(size_t fieldID);
    size_t setField(std::string data, size_t fieldID);
    std::string getField(size_t fieldID);

    void print();
    
    private:

    DL_ISO8583_HANDLER      isoHandler;
	DL_ISO8583_MSG          isoMsg;
    DL_ERR                  errorCode;

    size_t isEbcdic;
};

#endif