#include "miot_device.h"
#include "iot/protocol.h"
#include "iot_mqtt_protocol.h"
#include <board.h>
#include <esp_log.h>
#include <cJSON.h>
// #include "system_info.h"
#define TAG "MiotDevice"
namespace iot
{
    MiotDevice::MiotDevice(const std::string &ip, const std::string &token, const std::string &did, const std::string &mac) : m_ip(ip), m_token(token), m_deviceId(did), mac_(mac)
    {
        // ESP_LOGI(TAG, "MiotDevice constructor %s %s %ld", ip.data(), token.data(), did);
    }

    void MiotDevice::setCloudProperty(const std::string &did, const uint8_t &siid, const uint8_t &piid, const int &value, const bool &isBool)
    {
        // ESP_LOGI(TAG, "setCloudProperty");
        auto jsonStr = "[{\"did\": \"" + m_deviceId + "\",";
        jsonStr += "\"siid\": " + std::to_string(siid) + ",";
        jsonStr += "\"piid\": " + std::to_string(piid) + ",";
        if (isBool)
        {
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
            jsonStr += "\"value\": " + std::to_string(value);
        }
        jsonStr += "}]";
        auto request = createRequest("set_properties", jsonStr);
        sendCloud(request);
    }

    std::string MiotDevice::callCloudAction(const uint8_t &siid, const uint8_t &aiid, std::map<int, int> av)
    {
        std::string jsonStr = "{\"did\": \"" + m_deviceId + "\",";
        jsonStr += "\"siid\": " + std::to_string(siid) + ",";
        jsonStr += "\"aiid\": " + std::to_string(aiid) + ",";
        jsonStr += "\"in\": [";
        for (auto a = av.begin(); a != av.end(); a++)
        {
            // {\"piid\": " + std::to_string(piid) + ", \"value\": " + std::to_string(value) + "}
            jsonStr += "{\"piid\": " + std::to_string(a->first) + ", \"value\": " + std::to_string(a->second) + "}";
            if (std::next(a) != av.end()) // 检查下一个元素是否是end
            {
                jsonStr += ",";
            }
        };

        jsonStr += "]";
        jsonStr += "}";
        // {"did": "call-2-5","siid": 2,"aiid": 5,"in": [{"piid": 5, "value": 1},]}
        // ESP_LOGI(TAG, "callAction2 jsonStr:%s", jsonStr.c_str());
        auto request = createRequest("action", jsonStr);
        sendCloud(request);
        return "";
    }

    void MiotDevice::sendCloud(const std::string &request)
    {
        // ESP_LOGI(TAG, "request is %s", request.c_str());
        std::string url = std::string(CONFIG_IOT_URL) + "api/v1/micloud/io3";
        auto post_data = "{\"command\": " + request + ",\"mac\": \"" + mac_ + "\", \"did\": \"" + m_deviceId + "\", \"type\": \"device_control\"}";
        std::string response = sendRequest(url, post_data);
        // userCallback(response);
    }

    std::string MiotDevice::createRequest(const std::string &command, const std::string &parameters)
    {
        std::string request = "{\"id\": " + std::to_string(1) + ", \"method\": \"" + command + "\"";
        if (!parameters.empty())
        {
            request += ", \"params\": " + parameters;
        }
        else
        {
            request += ", \"params\": []";
        }
        request += "}";
        return request;
    }
    std::string MiotDevice::sendRequest(const std::string &url, const std::string &post_data)
    {
        auto &iot_mqtt_protocol = IotMqttProtocol::GetInstance();
        iot_mqtt_protocol.sendIotCommand(post_data);
        return "";
    }

} // namespace iot
