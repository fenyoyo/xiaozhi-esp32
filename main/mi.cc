#include "mi.h"
#include <esp_log.h>
#include <board.h>
#include <display.h>
#include "system_info.h"

#include "iot/thing_manager.h"
#include "iot/thing.h"
#include "iot_mqtt_protocol.h"

#include <cstring>

#define TAG "MI"

Mi::Mi(/* args */)
{
}

Mi::~Mi()
{
}

bool Mi::GetMi()
{
    return false;
}

void Mi::RegisterIot()
{

    auto &board = Board::GetInstance();
    auto display = Board::GetInstance().GetDisplay();
    std::string mac = SystemInfo::GetMacAddress();
    // std::string url = "http://192.168.1.6:8000/api/v1/iot/mqtt";
    std::string url = std::string(CONFIG_IOT_URL) + "api/v1/iot/mqtt";

    const int MAX_RETRY = 5;
    int retry_count = 0;
    int retry_delay = 10; // 初始重试延迟为10秒
    cJSON *root_ = nullptr;
    while (true)
    {
        retry_count++;
        if (retry_count >= MAX_RETRY)
        {
            ESP_LOGE(TAG, "Too many retries, exit version check");
            display->SetChatMessage("system", "加载失败，请检查");
            vTaskDelay(pdMS_TO_TICKS(retry_delay * 1000));
            return;
        }

        auto http = board.CreateHttp();
        auto boardJson = board.GetJson();
        http->SetHeader("Content-Type", "application/json");
        std::string method = "POST";
        http->SetContent(std::move(boardJson));
        if (!http->Open(method, url))
        {
            ESP_LOGE(TAG, "Failed to open HTTP connection");
            delete http;
            continue;
        }
        auto response = http->ReadAll();
        delete http;
        root_ = cJSON_Parse(response.c_str());
        if (root_ == NULL)
        {
            ESP_LOGE(TAG, "Failed to parse JSON response");
            continue;
        }

        cJSON *activation_code = cJSON_GetObjectItem(root_, "activation_code");
        if (activation_code != NULL)
        {
            ESP_LOGE(TAG, "did not activation");
            // Alert(Lang::Strings::ERROR, message.c_str(), "sad");
            cJSON *code = cJSON_GetObjectItem(activation_code, "code");
            cJSON *url = cJSON_GetObjectItem(activation_code, "url");
            std::string message = std::string("请前往 ") + cJSON_GetStringValue(url) + " 绑定设备验证码" + cJSON_GetStringValue(code);
            ESP_LOGI(TAG, "message is %s", message.c_str());
            display->SetStatus("加载米家");
            display->SetChatMessage("system", message.c_str());
            // display->SetEmotion("sad");
            vTaskDelay(pdMS_TO_TICKS(retry_delay * 1000));
            continue;
        }
        cJSON *mihome = cJSON_GetObjectItem(root_, "mihome");
        if (mihome == NULL or cJSON_IsFalse(mihome))
        {
            ESP_LOGE(TAG, "Get MiHome binding mi failed");
            std::string message = "xiaozhi.uyuo.me 绑定米家";
            ESP_LOGI(TAG, "message is %s", message.c_str());
            display->SetChatMessage("system", message.c_str());
            // Alert(Lang::Strings::ERROR, "xiaozhi.uyuo.me 绑定米家", "sad");
            // display->SetChatMessage("system", "请前往 xiaozhi.uyuo.me 绑定米家");
            // display->SetEmotion("sad");
            vTaskDelay(pdMS_TO_TICKS(retry_delay * 1000));
            continue;
        }
        break;
    }

    cJSON *mqtt = cJSON_GetObjectItem(root_, "mqtt");
    if (mqtt == NULL)
    {
        ESP_LOGE(TAG, "MQTT not found in JSON response");
        cJSON_Delete(mqtt);
        return;
    }

    cJSON *broker = cJSON_GetObjectItem(mqtt, "broker");
    if (broker == NULL)
    {
        ESP_LOGE(TAG, "Broker not found in JSON response");
        cJSON_Delete(broker);
        return;
    }
    cJSON *username = cJSON_GetObjectItem(mqtt, "username");
    if (username == NULL)
    {
        ESP_LOGE(TAG, "Username not found in JSON response");
        cJSON_Delete(username);
        return;
    }
    cJSON *password = cJSON_GetObjectItem(mqtt, "password");
    if (password == NULL)
    {
        ESP_LOGE(TAG, "Password not found in JSON response");
        cJSON_Delete(password);
        return;
    }
    cJSON *client_id = cJSON_GetObjectItem(mqtt, "client_id");
    if (client_id == NULL)
    {
        ESP_LOGE(TAG, "Client ID not found in JSON response");
        cJSON_Delete(client_id);
        return;
    }
    cJSON *publish_topic = cJSON_GetObjectItem(mqtt, "publish_topic");
    if (publish_topic == NULL)
    {
        ESP_LOGE(TAG, "Publish topic not found in JSON response");
        cJSON_Delete(publish_topic);
        return;
    }
    cJSON *subscribe_topic = cJSON_GetObjectItem(mqtt, "subscribe_topic");
    if (subscribe_topic == NULL)
    {
        ESP_LOGE(TAG, "Subscribe topic not found in JSON response");
        cJSON_Delete(subscribe_topic);
        return;
    }

    // /api/v1/iot/mqtt
    auto &iot_mqtt_protocol = IotMqttProtocol::GetInstance();
    iot_mqtt_protocol.Start(broker->valuestring, client_id->valuestring, username->valuestring, password->valuestring, publish_topic->valuestring, subscribe_topic->valuestring);
    iot_mqtt_protocol.OnIncomingJson([](const cJSON *root)
                                     {
                                         //  ESP_LOGI(TAG, "Received message: %s", cJSON_Print(root));

                                         //  cJSON *device_list_ = cJSON_GetObjectItem(root, "data");
                                         cJSON *type = cJSON_GetObjectItem(root, "type");
                                         auto &thing_manager = iot::ThingManager::GetInstance();
                                         std::string mac = SystemInfo::GetMacAddress();
                                         if (strcmp(type->valuestring, "device_discovery") == 0)
                                         {

                                             cJSON *item = cJSON_GetObjectItem(root, "data");
                                             cJSON *name = cJSON_GetObjectItem(item, "name");
                                             cJSON *title = cJSON_GetObjectItem(item, "title");
                                             cJSON *token = cJSON_GetObjectItem(item, "token");
                                             cJSON *did = cJSON_GetObjectItem(item, "did");
                                             cJSON *miot = cJSON_GetObjectItem(item, "iot");

                                             if (cJSON_IsNull(miot) == false)
                                             {
                                                 auto thing2 = iot::CreateThing("MIHOME");
                                                 if (thing2 == nullptr)
                                                 {
                                                     ESP_LOGE(TAG, "Failed to create thing");
                                                 }
                                                 thing2->initMiot(token->valuestring, name->valuestring, did->valuestring, mac);
                                                 cJSON *p = cJSON_GetObjectItem(miot, "p");
                                                 cJSON *a = cJSON_GetObjectItem(miot, "a");
                                                 thing2->registerProperty(p);
                                                 thing2->registerAction(a);
                                                 if (cJSON_IsNull(title))
                                                 {
                                                     thing2->set_name(name->valuestring);
                                                     thing2->set_description(name->valuestring);
                                                 }
                                                 else
                                                 {
                                                     thing2->set_name(name->valuestring);
                                                     thing2->set_description(title->valuestring);
                                                 }
                                                 thing2->set_did(did->valuestring);
                                                 thing_manager.AddThing(thing2);
                                             }

                                             //  cJSON_Delete(item);
                                         }
                                         if (strcmp(type->valuestring, "device_status") == 0)
                                         {

                                             cJSON *item = cJSON_GetObjectItem(root, "data");
                                             cJSON *did = cJSON_GetObjectItem(root, "did");
                                             thing_manager.GetThing(did->valuestring)->setProperties(item);
                                         }
                                         //  } //
                                     });

    iot_mqtt_protocol.addSubscribeTopic("mihome/" + SystemInfo::GetMacAddress());
    vTaskDelay(pdMS_TO_TICKS(300));
    iot_mqtt_protocol.sendIotCommand("{\"type\":\"device_discovery\",\"mac\":\"" + SystemInfo::GetMacAddress() + "\"}");

    // auto &thing_manager = iot::ThingManager::GetInstance();
    // std::string mac = SystemInfo::GetMacAddress();
    // int size = cJSON_GetArraySize(device_list_);
    // for (int i = 0; i < size; i++)
    // {
    //     // 输出剩余内存
    //     // ESP_LOGI(TAG, "剩余内存：%ld", esp_get_free_heap_size());
    //     cJSON *item = cJSON_GetArrayItem(device_list_, i);
    //     cJSON *name = cJSON_GetObjectItem(item, "name");
    //     cJSON *title = cJSON_GetObjectItem(item, "title");
    //     // cJSON *model = cJSON_GetObjectItem(item, "model");
    //     cJSON *ip = cJSON_GetObjectItem(item, "localip");
    //     cJSON *token = cJSON_GetObjectItem(item, "token");
    //     cJSON *did = cJSON_GetObjectItem(item, "did");

    //     cJSON *miot = cJSON_GetObjectItem(item, "iot");
    //     ESP_LOGI(TAG, "item%s", cJSON_PrintUnformatted(item));
    //     if (cJSON_IsNull(miot))
    //     {
    //         // std::string model_str = processString(model->valuestring);
    //         // auto thing = CreateThing(model_str);
    //         // if (thing == nullptr)
    //         // {
    //         //     ESP_LOGE(TAG, "Failed to create thing");
    //         //     continue;
    //         // }
    //         // ESP_LOGI(TAG, "miot is null, model:%s", cJSON_PrintUnformatted(item));
    //         // thing->initMiot(cJSON_IsNull(ip) ? "" : ip->valuestring, token->valuestring, name->valuestring, std::stoi(did->valuestring));
    //         // // thing->set_ip(ip->valuestring);
    //         // // thing->set_token(token->valuestring);
    //         // AddThing(thing);
    //     }
    //     else
    //     {
    //         auto thing2 = iot::CreateThing("MIHOME");
    //         if (thing2 == nullptr)
    //         {
    //             ESP_LOGE(TAG, "Failed to create thing");
    //             continue;
    //         }
    //         thing2->initMiot(cJSON_IsNull(ip) ? "" : ip->valuestring, token->valuestring, name->valuestring, did->valuestring, mac);
    //         cJSON *p = cJSON_GetObjectItem(miot, "p");
    //         cJSON *a = cJSON_GetObjectItem(miot, "a");
    //         thing2->registerProperty(p);
    //         thing2->registerAction(a);
    //         if (cJSON_IsNull(title))
    //         {
    //             thing2->set_name(name->valuestring);
    //             thing2->set_description(name->valuestring);
    //         }
    //         else
    //         {
    //             thing2->set_name(name->valuestring);
    //             thing2->set_description(title->valuestring);
    //         }
    //         // thing2->set_name(name->valuestring);
    //         // thing2->set_description(name->valuestring);
    //         thing_manager.AddThing(thing2);
    //     }
    //     // cJSON_Delete(item);
    // }
    return;
}
std::string Mi::processString(const std::string &input)
{
    std::string result;
    for (char c : input)
    {
        if (c == '.')
        {
            result += '_';
        }
        else if (std::isalpha(c))
        {
            result += std::toupper(c);
        }
        else
        {
            result += c;
        }
    }
    return result;
    // return std::string();
}
