#include "iot_mqtt_protocol.h"
#include "board.h"
#include "application.h"
#include "iot/thing_manager.h"
#include "settings.h"

#include <esp_log.h>
#include <ml307_mqtt.h>
#include <ml307_udp.h>
#include <cstring>
#include <arpa/inet.h>
#include "assets/lang_config.h"
#include <system_info.h>

#define TAG "IOT_MQTT"

IotMqttProtocol::IotMqttProtocol()
{
    event_group_handle_ = xEventGroupCreate();
}

IotMqttProtocol::~IotMqttProtocol()
{
    ESP_LOGI(TAG, "MqttProtocol deinit");
    if (mqtt_ != nullptr)
    {
        delete mqtt_;
    }
    vEventGroupDelete(event_group_handle_);
}

bool IotMqttProtocol::Start(const std::string &endpoint, const std::string &client_id, const std::string &username, const std::string &password, const std::string &publish_topic, const std::string &subscribe_topic)
{
    endpoint_ = endpoint;
    client_id_ = client_id;
    username_ = username;
    password_ = password;
    publish_topic_ = publish_topic;
    subscribe_topic_ = subscribe_topic;

    return StartMqttClient(false);
}

bool IotMqttProtocol::StartMqttClient(bool report_error)
{
    if (mqtt_ != nullptr)
    {
        ESP_LOGW(TAG, "Mqtt client already started");
        delete mqtt_;
    }

    if (endpoint_.empty())
    {
        ESP_LOGW(TAG, "MQTT endpoint is not specified");
        if (report_error)
        {
        }
        return false;
    }

    mqtt_ = Board::GetInstance().CreateMqtt();
    mqtt_->SetKeepAlive(90);

    mqtt_->OnDisconnected([this]()
                          { ESP_LOGI(TAG, "Disconnected from endpoint"); });
    mqtt_->OnConnected([this]()
                       {
                           ESP_LOGI(TAG, "iot mqtt Connected to endpoint");
                           //   addSubscribeTopic(subscribe_topic_); //
                           //    sendIotCommand("{\"type\":\"device_discovery\",\"mac\":\"" + SystemInfo::GetMacAddress() + "\"}");
                       });

    mqtt_->OnMessage([this](const std::string &topic, const std::string &payload)
                     {
                        //  ESP_LOGI(TAG, "Received message: %s", payload.c_str());
                         cJSON *root = cJSON_Parse(payload.c_str());
                         if (root == nullptr)
                         {
                             ESP_LOGE(TAG, "Failed to parse json message %s", payload.c_str());
                             return;
                         }
                         cJSON *type = cJSON_GetObjectItem(root, "type");
                         if (type == nullptr)
                         {
                             ESP_LOGE(TAG, "Message type is not specified");
                             cJSON_Delete(root);
                             return;
                         }
                         on_incoming_json_(root); });

    ESP_LOGI(TAG, "Connecting to endpoint %s", endpoint_.c_str());
    std::string broker_address;
    int broker_port = 8883;
    size_t pos = endpoint_.find(':');
    if (pos != std::string::npos)
    {
        broker_address = endpoint_.substr(0, pos);
        broker_port = std::stoi(endpoint_.substr(pos + 1));
    }
    else
    {
        broker_address = endpoint_;
    }
    if (!mqtt_->Connect(broker_address, broker_port, client_id_, username_, password_))
    {
        ESP_LOGE(TAG, "Failed to connect to endpoint");
        return false;
    }

    ESP_LOGI(TAG, "Connected to endpoint");
    return true;
}

bool IotMqttProtocol::SendText(const std::string &text)
{
    if (publish_topic_.empty())
    {
        return false;
    }
    if (!mqtt_->Publish(publish_topic_, text))
    {
        ESP_LOGE(TAG, "Failed to publish message: %s", text.c_str());
        return false;
    }
    return true;
}

bool IotMqttProtocol::addSubscribeTopic(const std::string &text)
{
    mqtt_->Subscribe(text, 0);
    return true;
}

bool IotMqttProtocol::sendIotCommand(const std::string &text)
{
    // ESP_LOGI(TAG, "publish message: %s", text.c_str());
    // if (publish_topic_.empty())
    // {
    //     return false;
    // }
    if (!mqtt_->Publish("mihome", text))
    {
        ESP_LOGE(TAG, "Failed to publish message: %s", text.c_str());
        return false;
    }
    return true;
}

static const char hex_chars[] = "0123456789ABCDEF";
// 辅助函数，将单个十六进制字符转换为对应的数值
static inline uint8_t CharToHex(char c)
{
    if (c >= '0' && c <= '9')
        return c - '0';
    if (c >= 'A' && c <= 'F')
        return c - 'A' + 10;
    if (c >= 'a' && c <= 'f')
        return c - 'a' + 10;
    return 0; // 对于无效输入，返回0
}

std::string IotMqttProtocol::DecodeHexString(const std::string &hex_string)
{
    std::string decoded;
    decoded.reserve(hex_string.size() / 2);
    for (size_t i = 0; i < hex_string.size(); i += 2)
    {
        char byte = (CharToHex(hex_string[i]) << 4) | CharToHex(hex_string[i + 1]);
        decoded.push_back(byte);
    }
    return decoded;
}

void IotMqttProtocol::OnIncomingJson(std::function<void(const cJSON *root)> callback)
{
    on_incoming_json_ = callback;
}