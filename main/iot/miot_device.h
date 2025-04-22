
#ifndef MIOT_DEVICE_H
#define MIOT_DEVICE_H
#include <map>
#include <string>
#include <cstdint>
#include "iot/thing.h"
#include <esp_udp.h>
#define MOIT_ON 1
#define MOIT_OFF 0

#define MOIT_PROPERTY false
#define MOIT_ACTION true

#define MOIT_PROPERTY_BOOL 0
#define MOIT_PROPERTY_INT 1
#define MOIT_PROPERTY_STRING 2

namespace iot
{
    // struct SIID_PIID
    // {

    //     int8_t siid;
    //     int8_t piid;
    //     int8_t type = 0;
    //     int value = 0;
    //     std::string description = "";
    // };

    // struct SIID_AIID
    // {

    //     int8_t siid;
    //     int8_t aiid;
    //     int8_t piid;
    //     int8_t type = 0;
    //     int value = 0;
    //     std::string description = "";
    // };

    enum class Permission : uint8_t
    {
        READ = 1 << 2,
        WRITE = 1 << 1,
        EXECUTE = 1 << 0,
        NONE = 0 << 0
    };

    inline Permission operator|(Permission lhs, Permission rhs)
    {
        return static_cast<Permission>(static_cast<uint8_t>(lhs) | static_cast<uint8_t>(rhs));
    }

    struct SpecProperty
    {
        uint8_t siid;
        uint8_t piid;
        // std::string key;
        std::string description;
        std::string unit;
        // PropertyType type;
        ValueType type;
        Permission perm;
        std::string method_name = "";
        std::string method_description = "";
        std::string parameter_description = "";
        int value = 0;
    };

    struct SpecActionParam
    {
        uint8_t piid;
        std::string key = "";
        std::string parameter_description = "";
        ValueType type;
        int value = 0;
    };

    struct SpecAction
    {
        uint8_t siid;
        uint8_t aiid;
        std::string method_name = "";
        std::string method_description = "";
        std::vector<SpecActionParam> parameters;
    };

    class MiotDevice
    {
    private:
        std::string m_ip;
        std::string m_token;
        uint32_t stamp_;
        uint32_t m_deviceId;
        std::function<void(const std::string &)> userCallback;

    public:
        MiotDevice() = default;
        MiotDevice(const std::string &ip, const std::string &token, const u_int32_t &did);
        void setCallback(std::function<void(const std::string &)> callback)
        {
            userCallback = callback;
        }
        void setstamp(uint32_t stamp)
        {
            stamp_ = stamp;
        }
        void setdeviceId(uint32_t deviceId)
        {
            m_deviceId = deviceId;
        }
        void init();

        void setCloudProperty(const std::string &did, const uint8_t &siid, const uint8_t &piid, const int &value, const bool &isBool = false);
        std::string createRequest(const std::string &command, const std::string &parameters);
        std::string callCloudAction(const uint8_t &siid, const uint8_t &aiid, std::map<int, int> av);
        void sendCloud(const std::string &jsonStr);
        void getProperties();
        std::string sendRequest(const std::string &url, const std::string &post_data);
    };
} // namespace iot

#endif // THING_MANAGER_H