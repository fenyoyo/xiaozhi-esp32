#include "miot_device.h"
#include "iot/miot.h"
#include "iot/udp_task.h"

#include <esp_log.h>
#include <cJSON.h>

#define TAG "MiotDevice"
namespace iot
{
    MiotDevice::MiotDevice(const std::string &ip, const std::string &token, const uint32_t &did) : m_ip(ip), m_token(token), m_deviceId(did)
    {
        ESP_LOGI(TAG, "MiotDevice constructor %s %s %ld", ip.data(), token.data(), did);
    }
    // {
    //     ip_ = ip;
    //     token_ = token;
    //     deviceId_ = did;
    //     ESP_LOGI(TAG, "MiotDevice init %ld", deviceId_);
    // }

    void MiotDevice::init()
    {
    }

    std::string MiotDevice::getProperty(const std::string &did, const uint8_t &siid, const uint8_t &piid)
    {
        std::string jsonStr = "[{\"did\": \"" + did + "\",";
        jsonStr += "\"siid\": " + std::to_string(siid) + ",";
        jsonStr += "\"piid\": " + std::to_string(piid);
        jsonStr += "}]";
        auto response = send("get_properties", jsonStr);
        // std::map<std::string, SIID_PIID> properties;
        return response;
    }

    void MiotDevice::getProperties2(const std::map<std::string, SpecProperty> &properties)
    {
        std::string jsonStr = "[";
        for (auto it = properties.begin(); it != properties.end(); ++it)
        {
            jsonStr += "{\"did\": \"" + it->first + "\",";
            jsonStr += "\"siid\": " + std::to_string(it->second.siid) + ",";
            jsonStr += "\"piid\": " + std::to_string(it->second.piid);
            jsonStr += "},";
        }
        jsonStr.pop_back();
        jsonStr += "]";
        auto response = send("get_properties", jsonStr);
    }

    /**
     * @brief 设置设备属性
     *
     * 本函数通过构建一个JSON格式的请求字符串来设置设备的特定属性。它可以根据属性的类型（布尔值或整数）
     * 来适当格式化请求内容，然后发送到设备。
     *
     * @param did 设备ID，用于标识目标设备
     * @param siid 服务实例ID，用于标识设备上的特定服务实例
     * @param piid 属性实例ID，用于标识服务实例下的特定属性
     * @param value 属性值，根据isBool参数的不同，可以被解释为布尔值或整数值
     * @param isBool 标志位，指示属性值是否应被解释为布尔值
     * @return std::string 返回设备响应的结果字符串
     */
    std::string MiotDevice::setProperty(const std::string &did, const uint8_t &siid, const uint8_t &piid,
                                        const uint8_t &value, const bool &isBool)
    {
        // 构建JSON请求字符串的起始部分，包括设备ID、服务实例ID和属性实例ID
        std::string jsonStr = "[{\"did\": \"" + did + "\",";
        jsonStr += "\"siid\": " + std::to_string(siid) + ",";
        jsonStr += "\"piid\": " + std::to_string(piid) + ",";

        // 根据isBool标志决定如何格式化属性值
        if (isBool)
        {
            // 如果属性值为布尔类型，根据value的值添加对应的布尔值到JSON字符串中
            if (value == 0)
            {
                jsonStr += "\"value\": false";
            }
            else
            {
                jsonStr += "\"value\": true";
            }
        }
        else
        {
            // 如果属性值为非布尔类型，直接添加其整数值到JSON字符串中
            jsonStr += "\"value\": " + std::to_string(value);
        }

        // 完成JSON请求字符串的构建
        jsonStr += "}]";

        // 发送构建好的JSON请求字符串到设备，并返回设备的响应结果
        return send("set_properties", jsonStr);
    }
    void MiotDevice::setProperty2(std::map<std::string, SpecProperty> miotSpec, std::string key, const uint8_t &value, const bool &isBool)
    {
        auto spec = miotSpec.find(key);
        if (spec == miotSpec.end())
        {
            ESP_LOGE(TAG, "%s not found", key.data());
            return;
        }
        std::string did = spec->first;
        SpecProperty sp = spec->second;
        auto res = setProperty(did, sp.siid, sp.piid, value, isBool);
    }
    std::string MiotDevice::callAction(const uint8_t &siid, const uint8_t &aiid)
    {
        std::string jsonStr = "{\"did\": \"call-" + std::to_string(siid) + "-" + std::to_string(aiid) + "\",";
        jsonStr += "\"siid\": " + std::to_string(siid) + ",";
        jsonStr += "\"aiid\": " + std::to_string(aiid) + ",";
        jsonStr += "\"in\": []";
        jsonStr += "}";
        return send("action", jsonStr);
    }
    //{"id": 2, "method": "action", "params": {"did": "call-2-1", "siid": 2, "aiid": 1, "in": [{"piid": 5, "value": 1}]}}
    std::string MiotDevice::callAction(const uint8_t &siid, const uint8_t &aiid, const uint8_t &piid, const uint8_t &value)
    {
        std::string jsonStr = "{\"did\": \"call-" + std::to_string(siid) + "-" + std::to_string(piid) + "\",";
        jsonStr += "\"siid\": " + std::to_string(siid) + ",";
        jsonStr += "\"aiid\": " + std::to_string(aiid) + ",";
        jsonStr += "\"in\": [{\"piid\": " + std::to_string(piid) + ", \"value\": " + std::to_string(value) + "}]";
        jsonStr += "}";
        return send("action", jsonStr);
    }
    std::string MiotDevice::callAction2(std::map<std::string, SpecAction> miotSpec, std::string key, std::map<uint8_t, int> av)
    {
        auto spec = miotSpec.find(key);
        if (spec == miotSpec.end())
        {
            ESP_LOGE(TAG, "%s not found", key.data());
            return "";
        }

        std::string jsonStr = "{\"did\": \"call-" + std::to_string(spec->second.siid) + "-" + std::to_string(spec->second.aiid) + "\",";
        jsonStr += "\"siid\": " + std::to_string(spec->second.siid) + ",";
        jsonStr += "\"aiid\": " + std::to_string(spec->second.aiid) + ",";
        jsonStr += "\"in\": [";
        for (auto a = av.begin(); a != av.end(); a++)
        {
            // {\"piid\": " + std::to_string(piid) + ", \"value\": " + std::to_string(value) + "}
            jsonStr += "{\"piid\": " + std::to_string(a->first) + ", \"value\": " + std::to_string(a->second) + "}";
            if (av.end() != a)
            {
                jsonStr += ",";
            }
        };

        jsonStr += "]";
        jsonStr += "}";
        ESP_LOGI(TAG, "callAction2 jsonStr:%s", jsonStr.c_str());
        return send("action", jsonStr);
        return std::string();
    };

    std::string MiotDevice::send(const std::string &command, const std::string &parameters)
    {
        // ESP_LOGI(TAG, "send ip:%s", m_ip.data());
        UdpTask *udp = new UdpTask(m_ip, m_token, command, parameters);
        udp->setCallback([this](bool success, const std::string &response)
                         {
                               if (success)
                               {
                                    // ESP_LOGI(TAG, "Received response: %s", response.c_str());

                               }
                               else
                               {
                                //    ESP_LOGE(TAG, "Failed to receive response");
                               } });
        udp->start();
        return "";
    }
    void MiotDevice::parseJsonHasError(const std::string &jsonStr, int8_t *value_)
    {
        cJSON *root = cJSON_Parse(jsonStr.data());
        cJSON *error = cJSON_GetObjectItem(root, "error");
        if (error != NULL)
        {
            // cJSON *code = cJSON_GetObjectItem(error, "code");
            cJSON *message = cJSON_GetObjectItem(error, "message");
            ESP_LOGE(TAG, "error:%s", message->valuestring);
            *value_ = -1;
            return;
        }
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
        *code_ = code->valueint;
    }

    void MiotDevice::parseCallGetCode(const std::string &jsonStr, uint8_t *code_)
    {

        cJSON *root = cJSON_Parse(jsonStr.data());
        cJSON *result = cJSON_GetObjectItem(root, "result");
        cJSON *code = cJSON_GetObjectItem(result, "code");
        *code_ = code->valueint;
    }

} // namespace iot
