#ifndef MIOT_H
#define MIOT_H
#include <string>
#include <iot/protocol.h>
namespace iot
{
    class Miot
    {
    private:
        std::string ip_;
        int port = 54321;
        int id = 2;
        std::string token_;
        uint32_t deviceID;
        uint32_t deviceStamp;
        bool _discovered = false;
        /* data */
    public:
        void Send(const std::string &command, const std::string &payload);
        Miot(const std::string &ip, const std::string &token);
        Message Handshake();
        static Message InitHandshake(const std::string &ip);
        std::string createRequest(const std::string &command, const std::string &parameters);
    };

} // namespace iot

#endif