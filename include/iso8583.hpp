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
    Iso8583(const std::string &hexString);
    Iso8583(const std::string &hexString, const size_t isEbcdic);
    ~Iso8583();

    size_t pack(unsigned char *outputBuffer);
    size_t unpack(const std::string &hexString);
    size_t unpack(const unsigned char *messageBytes, size_t &messageLen);

    size_t setEncoding(const size_t isEbcdic);

    size_t haveField(const size_t fieldID);
    size_t setField(const std::string &data, const size_t fieldID);
    size_t unsetField(const size_t fieldID);
    std::string getField(const size_t fieldID);

    void print();
    
    private:

    DL_ISO8583_HANDLER      isoHandler;
	DL_ISO8583_MSG          isoMsg;
    DL_ERR                  errorCode;

    size_t isEbcdic;
};

#endif