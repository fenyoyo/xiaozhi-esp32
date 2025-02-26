#include "miot_device.h"
#include "iot/miot.h"
#include <cJSON.h>
#include <esp_log.h>

#define TAG "MiotDevice"
namespace iot
{
    MiotDevice::MiotDevice(const std::string &ip, const std::string &token)
    {
        ip_ = ip;
        token_ = token;
    }

    std::string MiotDevice::getProperty(const std::string &did, const uint8_t &siid, const uint8_t &piid)
    {
        std::string jsonStr = "[{\"did\": \"" + did + "\",";
        jsonStr += "\"siid\": " + std::to_string(siid) + ",";
        jsonStr += "\"piid\": " + std::to_string(piid);
        jsonStr += "}]";
        auto response = send("get_properties", jsonStr);
        return response;
    }

    std::string MiotDevice::setProperty(const std::string &did, const uint8_t &siid, const uint8_t &piid,
                                        const uint8_t &value)
    {
        std::string jsonStr = "[{\"did\": \"" + did + "\",";
        jsonStr += "\"siid\": " + std::to_string(siid) + ",";
        jsonStr += "\"piid\": " + std::to_string(piid) + ",";
        jsonStr += "\"value\": " + std::to_string(value);
        jsonStr += "}]";
        return send("set_properties", jsonStr);
    };

    std::string MiotDevice::send(const std::string &command, const std::string &parameters)
    {
        Miot miot(ip_, token_);
        // ESP_LOGI(TAG, "send command:%s,parameters:%s", command.c_str(), parameters.c_str());
        Message msg = miot.Send(command, parameters);
        if (msg.success == false)
        {
            ESP_LOGE(TAG, "msg.success:%d", msg.success);
            return "";
        }
        auto res = msg.decrypt(token_);
        return res;
    }

    void MiotDevice::parseJsonGetValue(const std::string &jsonStr, uint8_t *value_)
    {
        cJSON *root = cJSON_Parse(jsonStr.data());
        cJSON *result = cJSON_GetObjectItem(root, "result");
        cJSON *item = cJSON_GetArrayItem(result, 0);
        cJSON *value = cJSON_GetObjectItem(item, "value");
        *value_ = value->valueint;
    }

    void MiotDevice::parseJsonGetCode(const std::string &jsonStr, const uint8_t &index, uint8_t *code_)
    {

        cJSON *root = cJSON_Parse(jsonStr.data());
        cJSON *result = cJSON_GetObjectItem(root, "result");
        cJSON *item = cJSON_GetArrayItem(result, index);
        cJSON *code = cJSON_GetObjectItem(item, "code");

        // cJSON *root = cJSON_Parse(jsonStr.data());
        // cJSON *result = cJSON_GetObjectItem(root, "result");
        // cJSON *item = cJSON_GetArrayItem(result, 0);
        // cJSON *value = cJSON_GetObjectItem(item, "code");
        *code_ = code->valueint;
    }
} // namespace iot
