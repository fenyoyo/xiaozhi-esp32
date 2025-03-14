#include "thing_manager.h"
#include "system_info.h"
#include "cJSON.h"

#include <esp_log.h>
#include <board.h>
#include <settings.h>
#include <wifi_station.h>

#define TAG "ThingManager"

namespace iot
{
    void ThingManager::AddThing(Thing *thing)
    {
        things_.push_back(thing);
    }

    void ThingManager::InitMoit()
    {
        // return;
        // 请求网络，获取micloud的设备列表
        // 发送广播，查看在线设备
        // TODO:经常性获取失败，需要重试
        auto &board = Board::GetInstance();
        if (board.GetBoardType() != "wifi")
        {
            // 如果不是wifi模式，则不执行iot
            return;
        }
        auto &wifiStation = WifiStation::GetInstance();
        auto ssid = wifiStation.GetSsid();
        std::string mac = SystemInfo::GetMacAddress();
        std::string url = "http://192.168.2.106:8000/api/v1/devices/" + mac + "/miot?wifi_ssid=" + ssid;
        auto http = board.CreateHttp();

        std::string method = "GET";
        std::string devicesJsonStr;
        Settings settings("micloud", true);
        if (!http->Open(method, url, ""))
        {
            ESP_LOGE(TAG, "Failed to open HTTP connection");
            delete http;
            devicesJsonStr = settings.GetString("devices");
            if (devicesJsonStr.empty())
            {
                ESP_LOGE(TAG, "Failed to get devices from micloud");
                return;
            }
        }
        else
        {
            auto response = http->GetBody();
            if (response.empty())
            {
                ESP_LOGE(TAG, "Failed to get response from server 2");
                http->Close();
                delete http;
                return;
            }
            settings.SetString("devices", response);
            devicesJsonStr = response;
            http->Close();
            delete http;
        }

        ESP_LOGI(TAG, "response:%s", devicesJsonStr.c_str());
        // 判断是否是json格式

        cJSON *root = cJSON_Parse(devicesJsonStr.data());
        if (root == nullptr)
        {
            ESP_LOGE(TAG, "Failed to parse json");
            return;
        }

        cJSON *code = cJSON_GetObjectItem(root, "code");
        if (code->valueint != 0)
        {
            ESP_LOGE(TAG, "Failed to get devices from micloud");
            return;
        }
        cJSON *data = cJSON_GetObjectItem(root, "data");
        cJSON *open_iot = cJSON_GetObjectItem(data, "open_iot");
        if (cJSON_IsFalse(open_iot))
        {
            ESP_LOGE(TAG, "open_miot is false");
            return;
        }
        cJSON *list = cJSON_GetObjectItem(data, "list");
        int size = cJSON_GetArraySize(list);
        for (int i = 0; i < size; i++)
        {
            cJSON *item = cJSON_GetArrayItem(list, i);
            cJSON *name = cJSON_GetObjectItem(item, "name");
            cJSON *model = cJSON_GetObjectItem(item, "model");
            cJSON *ip = cJSON_GetObjectItem(item, "localip");
            cJSON *token = cJSON_GetObjectItem(item, "token");
            cJSON *did = cJSON_GetObjectItem(item, "did");
            std::string model_str = processString(model->valuestring);
            ESP_LOGI(TAG, "name:%s", name->valuestring);
            ESP_LOGI(TAG, "model:%s", model->valuestring);
            ESP_LOGI(TAG, "ip:%s", ip->valuestring);
            ESP_LOGI(TAG, "token:%s", token->valuestring);
            ESP_LOGI(TAG, "model_str:%s", model_str.c_str());
            // ESP_LOGI(TAG, "name:%s,model:%s,ip:%s,token:%s", model_str.data(), model->valuestring, ip->valuestring, token->valuestring);
            auto thing = CreateThing(model_str);
            if (thing == nullptr)
            {
                ESP_LOGE(TAG, "Failed to create thing");
                continue;
            }
            thing->initMiot(ip->valuestring, token->valuestring, name->valuestring, std::stoi(did->valuestring));
            // thing->set_ip(ip->valuestring);
            // thing->set_token(token->valuestring);
            AddThing(thing);
        }

        // AddThing(iot::CreateThing("Fan"));
        // for (auto &thing : things_)
        // {
        //     thing->initMiot();
        // }
    }

    std::string ThingManager::GetDescriptorsJson()
    {
        std::string json_str = "[";
        for (auto &thing : things_)
        {
            json_str += thing->GetDescriptorJson() + ",";
        }
        if (json_str.back() == ',')
        {
            json_str.pop_back();
        }
        json_str += "]";
        ESP_LOGI(TAG, "json_str:%s", json_str.c_str());
        return json_str;
    }

    std::string ThingManager::GetStatesJson()
    {
        std::string json_str = "[";
        for (auto &thing : things_)
        {
            thing->getProperties();
            json_str += thing->GetStateJson() + ",";
        }
        if (json_str.back() == ',')
        {
            json_str.pop_back();
        }
        json_str += "]";
        return json_str;
    }

    void ThingManager::Invoke(const cJSON *command)
    {
        auto name = cJSON_GetObjectItem(command, "name");
        for (auto &thing : things_)
        {
            if (thing->name() == name->valuestring)
            {
                thing->Invoke(command);
                return;
            }
        }
    }

    std::string ThingManager::processString(const std::string &input)
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

} // namespace iot
