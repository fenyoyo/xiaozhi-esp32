#include "protocol.h"

#include <iostream>
#include <vector>
#include <wolfssl/wolfcrypt/aes.h>
#include <wolfssl/wolfcrypt/logging.h>
#include <wolfssl/wolfcrypt/md5.h>
#include <lwip/def.h>
namespace iot
{
    std::string Utils::stringToHexManual(const std::string &input)
    {
        static const char hexDigits[] = "0123456789abcdef";
        std::string hexStr;
        hexStr.reserve(input.length() * 2);
        for (unsigned char c : input)
        {
            hexStr += hexDigits[(c >> 4) & 0x0F];
            hexStr += hexDigits[c & 0x0F];
        }
        return hexStr;
    }

    std::string Utils::md5(const std::string &data)
    {
        Md5 md5;
        byte checksum[MD5_DIGEST_SIZE];
        int ret = wc_InitMd5(&md5);
        if (ret != 0)
        {
            wc_Md5Free(&md5);
            WOLFSSL_MSG("wc_Initmd5 failed");
            return {};
        }
        ret = wc_Md5Update(&md5, reinterpret_cast<const byte *>(data.c_str()), data.size());
        if (ret != 0)
        {
            wc_Md5Free(&md5);
            WOLFSSL_MSG("wc_Md5Update failed");
            return {};
        }
        ret = wc_Md5Final(&md5, checksum);
        if (ret != 0)
        {
            wc_Md5Free(&md5);
            WOLFSSL_MSG("wc_Md5Final failed");
            return {};
        }
        wc_Md5Free(&md5);
        std::string _c(checksum, checksum + MD5_DIGEST_SIZE);
        return _c;
    }

    std::string Utils::pkcs7Padding(const std::string &data, size_t blockSize)
    {
        size_t paddingLength = blockSize - (data.size() % blockSize);
        std::string paddedData = data;
        for (size_t i = 0; i < paddingLength; ++i)
        {
            paddedData.push_back(paddingLength);
        }
        return paddedData;
    }

    std::string Utils::pkcs7UnPadding(const std::string &data)
    {
        if (data.empty())
            return data;
        unsigned char paddingByte = data.back();
        if (paddingByte > data.size())
            return data;
        size_t paddingLength = static_cast<size_t>(paddingByte);
        for (size_t i = 0; i < paddingLength; ++i)
        {
            if (data[data.size() - 1 - i] != paddingByte)
                return data;
        }
        return std::string(data.begin(), data.end() - paddingLength);
    }

    std::string Utils::Md5Checksum(const std::string &header,
                                   const std::string &token,
                                   const std::string &data)
    {
        const std::string x = header + token + data;

        return md5(x);
    }

    std::string Utils::hexStringToByteArray(const std::string &hexString)
    {
        std::string bytes;
        for (size_t i = 0; i < hexString.length(); i += 2)
        {
            std::string byteString = hexString.substr(i, 2);
            char byte = static_cast<char>(std::stoul(byteString, nullptr, 16));
            bytes.push_back(byte);
        }
        return bytes;
    }

    std::string Header::headerSerialize() const
    {
        std::string headerData;
        headerData.reserve(16);
        headerData.push_back(static_cast<char>(magicNumber >> 8));
        headerData.push_back(static_cast<char>(magicNumber & 0xFF));
        headerData.push_back(static_cast<char>(packetLength >> 8));
        headerData.push_back(static_cast<char>(packetLength & 0xFF));
        for (int i = 3; i >= 0; --i)
        {
            headerData.push_back(static_cast<char>((unknown >> (i * 8)) & 0xFF));
        }
        for (int i = 3; i >= 0; --i)
        {
            headerData.push_back(static_cast<char>((deviceID >> (i * 8)) & 0xFF));
        }
        for (int i = 3; i >= 0; --i)
        {
            headerData.push_back(static_cast<char>((stamp >> (i * 8)) & 0xFF));
        }
        return headerData;
    }

    bool Message::parse(const std::string &packetData)
    {
        Message msg;
        uint16_t magic;
        memcpy(&magic, packetData.data(), sizeof(magic));
        header.magicNumber = ntohs(magic);
        if (header.magicNumber != 0x2131)
        {
            return false;
        }
        //
        // 解析Packet Length
        uint16_t pkt_len;
        memcpy(&pkt_len, packetData.data() + 2, sizeof(pkt_len));
        header.packetLength = ntohs(pkt_len);
        if (packetData.size() != header.packetLength)
        {
        }
        //
        // 解析Unknown1
        uint32_t unknown1;
        memcpy(&unknown1, packetData.data() + 4, sizeof(unknown1));
        header.unknown = ntohl(unknown1);

        // // 解析Device ID
        uint32_t did;
        memcpy(&did, packetData.data() + 8, sizeof(did));
        header.deviceID = ntohl(did);
        //
        // 解析Stamp
        uint32_t stamp1;
        memcpy(&stamp1, packetData.data() + 12, sizeof(stamp1));
        header.stamp = ntohl(stamp1);

        const char *token_start = packetData.data() + 16;
        checksumOrToken.append(token_start, token_start + 16);
        // 提取数据部分
        size_t data_len = header.packetLength - 32;
        if (data_len > 0)
        {
            const char *data_start = packetData.data() + 32;
            data.append(data_start, data_start + data_len);
        }
        else
        {
            data.clear();
        }
        return true;
    }

    std::string Message::headerSerialize()
    {
        std::vector<uint8_t> headerData;
        headerData.reserve(16);
        headerData.push_back(header.magicNumber >> 8);
        headerData.push_back(header.magicNumber & 0xFF);
        headerData.push_back(header.packetLength >> 8);
        headerData.push_back(header.packetLength & 0xFF);
        for (int i = 3; i >= 0; --i)
        {
            headerData.push_back((header.unknown >> (i * 8)) & 0xFF);
        }
        for (int i = 3; i >= 0; --i)
        {
            headerData.push_back((header.deviceID >> (i * 8)) & 0xFF);
        }
        for (int i = 3; i >= 0; --i)
        {
            headerData.push_back((header.stamp >> (i * 8)) & 0xFF);
        }
        std::string headerDataStr(headerData.begin(), headerData.end());

        return headerDataStr;
    }

    std::string Message::build(const std::string &msg, const std::string &token)
    {
        std::string _token = Utils::hexStringToByteArray(token);

        std::string key = Utils::md5(_token);
        // std::cout << "key:" << Utils::stringToHexManual(key) << std::endl;
        std::string iv = Utils::md5(key + _token);
        // std::cout << "iv:" << Utils::stringToHexManual(iv) << std::endl;

        std::string _msg = msg;
        // std::cout << "msg:" << _msg << std::endl;
        // std::cout << "msg size:" << std::to_string(_msg.size()) << std::endl;
        _msg.push_back(0x00);
        // std::cout << "msg size:" << std::to_string(_msg.size()) << std::endl;
        std::string padded_data = Utils::pkcs7Padding(_msg, AES_BLOCK_SIZE);
        // std::cout << "padded_data:" << Utils::stringToHexManual(padded_data) << std::endl;
        Aes aes;
        if (wc_AesSetKey(&aes, reinterpret_cast<const byte *>(key.data()), key.size(),
                         reinterpret_cast<const byte *>(iv.data()), AES_ENCRYPTION) != 0)
        {
            std::cerr << "AES key set failed." << std::endl;
        }

        byte encrypted[padded_data.size()];
        // 进行加密操作
        if (wc_AesCbcEncrypt(&aes, encrypted, reinterpret_cast<const byte *>(padded_data.data()),
                             padded_data.size()) != 0)
        {
            std::cerr << "Encryption failed." << std::endl;
            wc_AesFree(&aes);
        }
        wc_AesFree(&aes);

        // data.reserve(padded_data.size());
        // // 进行加密操作'
        // 这里的data赋值不成功
        // data.append(encrypted, encrypted + padded_data.size());
        std::string _encrypted(encrypted, encrypted + padded_data.size());
        header.packetLength = _encrypted.size() + 32;
        checksumOrToken = Utils::Md5Checksum(headerSerialize(), _token, _encrypted);

        std::string packetData;
        std::string headerData = header.headerSerialize();
        packetData += headerData;
        packetData += checksumOrToken;
        packetData += _encrypted;
        return packetData;
    }

    std::string Message::decrypt(const std::string &token)
    {
        std::string _token = Utils::hexStringToByteArray(token);
        std::string key = Utils::md5(_token);

        std::string iv = Utils::md5(key + _token);

        Aes aes;
        if (wc_AesSetKey(&aes, reinterpret_cast<const byte *>(key.data()), key.size(),
                         reinterpret_cast<const byte *>(iv.data()), AES_DECRYPTION) != 0)
        {
            std::cerr << "AES key set failed." << std::endl;
        }
        // std::vector<unsigned char> decrypted(data.size());
        byte decrypted[data.size()];
        // 进行加密操作
        if (wc_AesCbcDecrypt(&aes, decrypted, reinterpret_cast<const byte *>(data.data()), data.size()) != 0)
        {
            std::cerr << "Encryption failed." << std::endl;
            wc_AesFree(&aes);
        }
        std::string _decrypted(decrypted, decrypted + data.size());
        std::string unpadded_plaintext = Utils::pkcs7UnPadding(_decrypted);
        return unpadded_plaintext;
    }
}