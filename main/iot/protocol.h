//
// Created by 奉有余 on 25-2-19.
//
#ifndef IOT_PROTOCOL_H
#define IOT_PROTOCOL_H
// #include <cstdint>
#include <string>
// #include <vector>
namespace iot
{
    class Utils
    {
    public:
        static std::string stringToHexManual(const std::string &input);

        // static std::string md5(const std::string &data);
        static std::string md5Hash(const std::string &data);

        static std::string aes_cbc_encrypt(const std::string &key, const std::string &iv, const std::string &plaintext);

        static std::string aes_cbc_decrypt(const std::string &key, const std::string &iv, const std::string &ciphertext);

        static std::string pkcs7Padding(const std::string &data, size_t block_size);

        static std::string pkcs7UnPadding(const std::string &data);

        static std::string Md5Checksum(const std::string &header,
                                       const std::string &token,
                                       const std::string &data);

        static std::string hexStringToByteArray(const std::string &hexString);
    };

    struct Header
    {
        uint16_t magicNumber = 0x2131;
        uint16_t packetLength = 0;
        uint32_t unknown = 0;
        uint32_t deviceID = 0;
        uint32_t stamp = 0;
        std::string headerSerialize();
    };

    struct Message
    {
        Header header;
        std::string checksumOrToken;
        std::string data;
        bool success = false;
        bool parse(const std::string &packetData);
        std::string build(const std::string &msg, const std::string &token);
        std::string decrypt(const std::string &token);
    };
}

#endif