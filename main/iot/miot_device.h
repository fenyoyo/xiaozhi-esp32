
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
        std::string getProperty(const std::string &did, const uint8_t &siid, const uint8_t &piid);
        void getProperties2(const std::map<std::string, SpecProperty> &properties);

        std::string setProperty(const std::string &did, const uint8_t &siid, const uint8_t &piid,
                                const uint8_t &value, const bool &isBool = false);
        void setProperty2(std::map<std::string, SpecProperty> miotSpec, std::string key, const int &value, const bool &isBool = false);
        void setBleProperty(std::map<std::string, SpecProperty> miotSpec, std::string key, const int &value, const bool &isBool = false);
        std::string createRequest(const std::string &command, const std::string &parameters);
        /**
         * 调用action
         * {"id":2,"result":{"code":0,"out":[]},"exe_time":30
         */
        std::string callAction(const uint8_t &siid, const uint8_t &aiid);
        std::string callAction(const uint8_t &siid, const uint8_t &aiid, const uint8_t &piid, const uint8_t &value);
        std::string callAction2(std::map<std::string, SpecAction> miotSpec, std::string key, std::map<uint8_t, int> av);

        std::string send(const std::string &command, const std::string &parameters);
        /**
         * 解析获取属性的函数 {"id":2,"result":[{"did":"fan:on","siid":2,"piid":1,"code":0,"value":true}],"exe_time":60}
         */
        static void parseJsonGetValue(const std::string &jsonStr, uint8_t *value_);
        /**
         * 解析获取属性的函数 {"id":2,"result":[{"did":"fan:on","siid":2,"piid":1,"code":0,"value":true}],"exe_time":60}
         * 获取code 0为成功
         */
        static void parseJsonGetCode(const std::string &jsonStr, const uint8_t &index, uint8_t *code_);

        /**
         * 解析调用GetCode指令的函数 {"id":2,"result":{"code":0,"out":[]},"exe_time":60}
         *
         * 本函数负责解析给定的JSON字符串，从中提取出指定索引位置的代码，并将其存储在提供的代码指针中
         * 主要用于处理和解析来自某个API或指令集的响应数据，以方便进一步的处理或展示
         *
         * @param jsonStr 包含代码信息的JSON字符串
         * @param code_ 指向用于存储提取出的代码的指针
         */
        void parseCallGetCode(const std::string &jsonStr, uint8_t *code_);
        /**
         * 检查是否存在错误
         */
        static void parseJsonHasError(const std::string &jsonStr, int8_t *value_);
    };
} // namespace iot

#endif // THING_MANAGER_H