
#ifndef MIOT_DEVICE_H
#define MIOT_DEVICE_H
#include <map>
#include <string>
#include <cstdint>
#define MOIT_ON 1
#define MOIT_OFF 0

#define MOIT_PROPERTY false
#define MOIT_ACTION true

#define MOIT_PROPERTY_BOOL 0
#define MOIT_PROPERTY_INT 1
#define MOIT_PROPERTY_STRING 2

namespace iot
{
    struct SIID_PIID
    {

        int8_t siid;
        int8_t piid;
        int8_t type = 0;
        int value = 0;
        std::string description = "";
    };

    struct SIID_AIID
    {

        int8_t siid;
        int8_t aiid;
        int8_t piid;
        int8_t type = 0;
        int value = 0;
        std::string description = "";
    };
    

    class MiotDevice
    {
    private:
        std::string ip_;
        std::string token_;

    public:
        MiotDevice() = default;
        MiotDevice(const std::string &ip, const std::string &token);

        std::string getProperty(const std::string &did, const uint8_t &siid, const uint8_t &piid);
        std::map<std::string, int> getProperties(const std::map<std::string, SIID_PIID> &properties);
        // int8_t getPropertyDoubleValue(const std::string &did, const uint8_t &siid, const uint8_t &piid, double *value);
        // int8_t getPropertyIntValue(const std::string &did, const uint8_t &siid, const uint8_t &piid, int *value);

        std::string setProperty(const std::string &did, const uint8_t &siid, const uint8_t &piid,
                                const uint8_t &value, const bool &isBool = false);
        /**
         * 调用action
         * {"id":2,"result":{"code":0,"out":[]},"exe_time":30
         */
        std::string callAction(const uint8_t &siid, const uint8_t &aiid);
        std::string callAction(const uint8_t &siid, const uint8_t &aiid, const uint8_t &piid, const uint8_t &value);

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