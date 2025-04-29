#include "protocol.h"
#include <mbedtls/aes.h>
#include <mbedtls/md5.h>
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

    std::string Utils::md5Hash(const std::string &input)
    {
        mbedtls_md5_context ctx;
        unsigned char output[16]; // MD5 输出为 16 字节

        mbedtls_md5_init(&ctx);
        mbedtls_md5_starts(&ctx);
        mbedtls_md5_update(&ctx,
                           reinterpret_cast<const unsigned char *>(input.data()),
                           input.length());
        mbedtls_md5_finish(&ctx, output);
        mbedtls_md5_free(&ctx);

        return std::string(output, output + 16);
    }

    std::string Utils::aes_cbc_encrypt(
        const std::string &key,
        const std::string &iv,
        const std::string &plaintext)
    {
        mbedtls_aes_context ctx;
        std::string ciphertext(plaintext.size(), '\0');
        std::string iv_copy = iv; // 需要可修改的 IV 副本

        mbedtls_aes_init(&ctx);

        // 设置加密密钥
        if (mbedtls_aes_setkey_enc(&ctx,
                                   reinterpret_cast<const unsigned char *>(key.data()),
                                   key.size() * 8) != 0)
        {
            mbedtls_aes_free(&ctx);
        }

        // 执行加密
        if (mbedtls_aes_crypt_cbc(
                &ctx,
                MBEDTLS_AES_ENCRYPT,
                plaintext.size(),
                reinterpret_cast<unsigned char *>(iv_copy.data()),
                reinterpret_cast<const unsigned char *>(plaintext.data()),
                reinterpret_cast<unsigned char *>(ciphertext.data())) != 0)
        {
            mbedtls_aes_free(&ctx);
        }

        mbedtls_aes_free(&ctx);
        return ciphertext;
    }

    std::string Utils::aes_cbc_decrypt(
        const std::string &key,
        const std::string &iv,
        const std::string &ciphertext)
    {
        mbedtls_aes_context ctx;
        std::string plaintext(ciphertext.size(), '\0');
        std::string iv_copy = iv; // 需要可修改的 IV 副本

        mbedtls_aes_init(&ctx);

        // 设置解密密钥
        if (mbedtls_aes_setkey_dec(&ctx,
                                   reinterpret_cast<const unsigned char *>(key.data()),
                                   key.size() * 8) != 0)
        {
            mbedtls_aes_free(&ctx);
        }

        // 执行解密
        if (mbedtls_aes_crypt_cbc(
                &ctx,
                MBEDTLS_AES_DECRYPT,
                ciphertext.size(),
                reinterpret_cast<unsigned char *>(iv_copy.data()),
                reinterpret_cast<const unsigned char *>(ciphertext.data()),
                reinterpret_cast<unsigned char *>(plaintext.data())) != 0)
        {
            mbedtls_aes_free(&ctx);
        }

        mbedtls_aes_free(&ctx);
        return plaintext;
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

        return md5Hash(x);
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

    std::string Header::headerSerialize()
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

    std::string Message::build(const std::string &msg, const std::string &token)
    {
        std::string _token = Utils::hexStringToByteArray(token);
        std::string key = Utils::md5Hash(_token);
        std::string iv = Utils::md5Hash(key + _token);
        std::string _msg = msg;
        _msg.push_back(0x00);
        std::string padded_data = Utils::pkcs7Padding(_msg, 16);
        std::string ciphertext = Utils::aes_cbc_encrypt(key, iv, padded_data);
        header.packetLength = ciphertext.size() + 32;
        std::string headerData = header.headerSerialize();
        checksumOrToken = Utils::Md5Checksum(headerData, _token, ciphertext);
        std::string packetData;
        packetData += headerData;
        packetData += checksumOrToken;
        packetData += ciphertext;
        return packetData;
    }

    std::string Message::decrypt(const std::string &token)
    {
        std::string _token = Utils::hexStringToByteArray(token);
        std::string key = Utils::md5Hash(_token);
        std::string iv = Utils::md5Hash(key + _token);
        std::string decrypted = Utils::aes_cbc_decrypt(key, iv, data);
        std::string unpadded_plaintext = Utils::pkcs7UnPadding(decrypted);
        return unpadded_plaintext;
    }
}