#ifndef IOT_MQTT_PROTOCOL_H
#define IOT_MQTT_PROTOCOL_H

#include "protocol.h"
#include <mqtt.h>
#include <udp.h>
#include <cJSON.h>
#include <mbedtls/aes.h>
#include <freertos/FreeRTOS.h>
#include <freertos/event_groups.h>

#include <functional>
#include <string>
#include <map>
#include <mutex>

#define MQTT_PING_INTERVAL_SECONDS 90
#define MQTT_RECONNECT_INTERVAL_MS 10000

#define MQTT_PROTOCOL_SERVER_HELLO_EVENT (1 << 0)

class IotMqttProtocol
{
public:
    IotMqttProtocol();
    ~IotMqttProtocol();
    bool Start(const std::string &endpoint, const std::string &client_id, const std::string &username, const std::string &password, const std::string &publish_topic, const std::string &subscribe_topic);

    bool addSubscribeTopic(const std::string &text);
    bool sendIotCommand(const std::string &text);
    void OnIncomingJson(std::function<void(const cJSON *root)> callback);
    static IotMqttProtocol &GetInstance()
    {
        static IotMqttProtocol instance; // C++11保证线程安全
        return instance;
    }

private:
    EventGroupHandle_t event_group_handle_;

    std::string endpoint_;
    std::string client_id_;
    std::string username_;
    std::string password_;
    std::string publish_topic_;
    std::string subscribe_topic_;

    Mqtt *mqtt_ = nullptr;

    std::function<void(const cJSON *root)> on_incoming_json_;
    bool StartMqttClient(bool report_error = false);
    void ParseServerHello(const cJSON *root);
    std::string DecodeHexString(const std::string &hex_string);

    bool SendText(const std::string &text);
};

#endif // MQTT_PROTOCOL_H
