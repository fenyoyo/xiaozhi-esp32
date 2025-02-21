
#ifndef MIOT_DEVICE_H
#define MIOT_DEVICE_H

#include <string>
#include <cstdint>

namespace iot
{
    class MiotDevice
    {
    private:
        std::string ip_;
        std::string token_;

    public:
        MiotDevice() = default;
        MiotDevice(const std::string &ip, const std::string &token);

        std::string getProperty(const std::string &did, const uint8_t &siid, const uint8_t &piid);

        std::string setProperty(const std::string &did, const uint8_t &siid, const uint8_t &piid,
                                const uint8_t &value);

        std::string send(const std::string &command, const std::string &parameters);
    };
} // namespace iot

#endif // THING_MANAGER_H