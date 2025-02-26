#ifndef MIOT_H
#define MIOT_H
#include <string>
#include <iot/protocol.h>
namespace iot
{
    struct Parameters
    {
        std::string did;
        int8_t siid;
        int8_t piid;
        int8_t value;
    };

    struct SIID_PIID
    {
        int8_t siid;
        int8_t piid;
    };

    class Miot
    {
    private:
        int port = 54321;
        int id = 2;
        std::string ip_;
        std::string token_;
        uint32_t deviceID;
        uint32_t deviceStamp;
        bool _discovered = false;
        /* data */
    public:
        Message Send(const std::string &command, const std::string &payload);
        Miot(const std::string &ip, const std::string &token);
        Message Handshake();
        // static std::string socketSend(const std::string &ip, const std::string &data);
        // static Message InitHandshake(const std::string &ip);
        // static Message getDeviceProperties(const std::string &ip, const std::string &token, uint32_t deviceID, uint32_t deviceStamp);
        std::string createRequest(const std::string &command, const std::string &parameters);
    };

} // namespace iot

#endif