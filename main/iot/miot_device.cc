#include "miot_device.h"
#include "iot/miot.h"
#include <esp_log.h>
#include <cJSON.h>

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
        // std::map<std::string, SIID_PIID> properties;
        return response;
    }

    std::map<std::string, int> MiotDevice::getProperties(const std::map<std::string, SIID_PIID> &properties)
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
        std::map<std::string, int> result;

        cJSON *root = cJSON_Parse(response.data());
        if (root == NULL)
        {
            ESP_LOGE(TAG, "getProperties cJSON_Parse failed");
            return result;
        }

        cJSON *result_ = cJSON_GetObjectItem(root, "result");
        for (int i = 0; i < cJSON_GetArraySize(result_); i++)
        {

            cJSON *item = cJSON_GetArrayItem(result_, i);
            cJSON *code = cJSON_GetObjectItem(item, "code");
            if (code->valueint != 0)
            {
                continue;
            }
            cJSON *did = cJSON_GetObjectItem(item, "did");
            cJSON *value = cJSON_GetObjectItem(item, "value");
            result[did->valuestring] = value->valueint;
        }
        return result;
    }

    // int8_t MiotDevice::getPropertyDoubleValue(const std::string &did, const uint8_t &siid, const uint8_t &piid, double *value)
    // {
    //     std::string jsonStr = "[{\"did\": \"" + did + "\",";
    //     jsonStr += "\"siid\": " + std::to_string(siid) + ",";
    //     jsonStr += "\"piid\": " + std::to_string(piid);
    //     jsonStr += "}]";
    //     auto response = send("get_properties", jsonStr);
    //     if (response.empty())
    //     {
    //         return -1;
    //     }
    //     cJSON *root = cJSON_Parse(response.data());
    //     cJSON *result = cJSON_GetObjectItem(root, "result");
    //     cJSON *item = cJSON_GetArrayItem(result, 0);
    //     cJSON *code = cJSON_GetObjectItem(item, "code");
    //     if (code->valueint != 0)
    //     {
    //         return -2;
    //     }
    //     cJSON *value_ = cJSON_GetObjectItem(item, "value");
    //     *value = value_->valuedouble;
    //     return 0;
    // }

    // int8_t MiotDevice::getPropertyIntValue(const std::string &did, const uint8_t &siid, const uint8_t &piid, int *value)
    // {
    //     std::string jsonStr = "[{\"did\": \"" + did + "\",";
    //     jsonStr += "\"siid\": " + std::to_string(siid) + ",";
    //     jsonStr += "\"piid\": " + std::to_string(piid);
    //     jsonStr += "}]";
    //     auto response = send("get_properties", jsonStr);

    //     if (response.empty())
    //     {
    //         return -1;
    //     }
    //     ESP_LOGI(TAG, "response:%s", response.c_str());
    //     cJSON *root = cJSON_Parse(response.data());
    //     cJSON *result = cJSON_GetObjectItem(root, "result");
    //     cJSON *item = cJSON_GetArrayItem(result, 0);
    //     ESP_LOGI(TAG, "response item:%s", cJSON_PrintUnformatted(item));
    //     cJSON *code = cJSON_GetObjectItem(item, "code");

    //     if (code->valueint != 0)
    //     {
    //         return -2;
    //     }
    //     cJSON *value_ = cJSON_GetObjectItem(item, "value");
    //     *value = value_->valueint;
    //     return 0;
    // }

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
    void MiotDevice::setProperty(std::map<std::string, SIID_PIID> miotSpec, std::string key, const uint8_t &value, const bool &isBool)
    {
        auto spec = miotSpec.find(key);
        if (spec == miotSpec.end())
        {
            ESP_LOGE(TAG, "%s not found", key.data());
            return;
        }
        std::string did = spec->first;
        SIID_PIID sp = spec->second;
        auto res = setProperty(did, sp.siid, sp.piid, value, isBool);
        return;
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
    };

    std::string MiotDevice::send(const std::string &command, const std::string &parameters)
    {
        Miot miot(ip_, token_);
        ESP_LOGI(TAG, "send command:%s,parameters:%s", command.c_str(), parameters.c_str());
        Message msg = miot.Send(command, parameters);
        if (msg.success == false)
        {
            ESP_LOGE(TAG, "msg.error:%d", msg.success);
            return "";
        }
        auto res = msg.decrypt(token_);
        return res;
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
