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

    bool ThingManager::GetStatesJson(std::string &json, bool delta)
    {
        if (!delta)
        {
            last_states_.clear();
        }
        bool changed = false;
        json = "[";
        // 枚举thing，获取每个thing的state，如果发生变化，则更新，保存到last_states_
        // 如果delta为true，则只返回变化的部分
        for (auto &thing : things_)
        {
            std::string state = thing->GetStateJson();
            if (delta)
            {
                // 如果delta为true，则只返回变化的部分
                auto it = last_states_.find(thing->name());
                if (it != last_states_.end() && it->second == state)
                {
                    continue;
                }
                changed = true;
                last_states_[thing->name()] = state;
            }
            json += state + ",";
        }
        if (json.back() == ',')
        {
            json.pop_back();
        }
        json += "]";
        return changed;
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
        // std::string url = "http://192.168.1.6:8000/api/v1/micloud/" + mac + "/iot?wifi_ssid=" + ssid;
        std::string url = "https://xiaozhi.uyuo.me/api/v1/micloud/" + mac + "/iot?wifi_ssid=" + ssid;
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
        // 判断是否是json格式

        cJSON *root = cJSON_Parse(devicesJsonStr.data());
        if (root == nullptr)
        {
            return;
        }

        cJSON *code = cJSON_GetObjectItem(root, "code");
        if (code->valueint != 0)
        {
            return;
        }
        cJSON *data = cJSON_GetObjectItem(root, "data");
        cJSON *open_iot = cJSON_GetObjectItem(data, "open_iot");
        if (cJSON_IsFalse(open_iot))
        {
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

} // namespace iot
