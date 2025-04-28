#include "mi.h"
#include <esp_log.h>
#include <board.h>
#include "system_info.h"

#include "iot/thing_manager.h"
#include "iot/thing.h"

#define TAG "MI"

Mi::Mi(/* args */)
{
}

Mi::~Mi()
{
}

bool Mi::GetMi()
{
    auto &board = Board::GetInstance();
    std::string mac = SystemInfo::GetMacAddress();

    std::string url = std::string(CONFIG_IOT_URL) + "api/v1/micloud/" + mac + "/iot3";

    auto http = board.CreateHttp();
    std::string method = "GET";

    if (!http->Open(method, url, ""))
    {
        ESP_LOGE(TAG, "Failed to open HTTP connection");
        delete http;
        return false;
    }
    auto response = http->GetBody();
    delete http;
    cJSON *root = cJSON_Parse(response.c_str());
    if (root == NULL)
    {
        ESP_LOGE(TAG, "Failed to parse JSON response");
        return false;
    }

    cJSON *code = cJSON_GetObjectItem(root, "code");
    cJSON *message = cJSON_GetObjectItem(root, "msg");

    if (code->valueint == 100)
    {
        err_msg = message->valuestring;
        ESP_LOGE(TAG, "Get MiHome info failed, code: %d, message: %s", code->valueint, err_msg.c_str());
        return false;
    }
    has_binding_ = true;
    if (code->valueint == 101)
    {
        err_msg = message->valuestring;
        return false;
    }
    has_mi_binding_ = true;
    cJSON *data = cJSON_GetObjectItem(root, "data");
    cJSON *open_iot = cJSON_GetObjectItem(data, "open_iot");
    if (cJSON_IsTrue(open_iot))
    {
        open_iot_ = true;
    }
    device_list_ = cJSON_GetObjectItem(data, "list");
    return true;
}

bool Mi::RegisterIot()
{
    auto &thing_manager = iot::ThingManager::GetInstance();
    int size = cJSON_GetArraySize(device_list_);
    for (int i = 0; i < size; i++)
    {
        // 输出剩余内存
        // ESP_LOGI(TAG, "剩余内存：%ld", esp_get_free_heap_size());
        cJSON *item = cJSON_GetArrayItem(device_list_, i);
        cJSON *name = cJSON_GetObjectItem(item, "name");
        cJSON *title = cJSON_GetObjectItem(item, "title");
        cJSON *model = cJSON_GetObjectItem(item, "model");
        cJSON *ip = cJSON_GetObjectItem(item, "localip");
        cJSON *token = cJSON_GetObjectItem(item, "token");
        cJSON *did = cJSON_GetObjectItem(item, "did");

        cJSON *miot = cJSON_GetObjectItem(item, "iot");
        // ESP_LOGI(TAG, "item%s", cJSON_PrintUnformatted(item));
        if (cJSON_IsNull(miot))
        {
            // std::string model_str = processString(model->valuestring);
            // auto thing = CreateThing(model_str);
            // if (thing == nullptr)
            // {
            //     ESP_LOGE(TAG, "Failed to create thing");
            //     continue;
            // }
            // ESP_LOGI(TAG, "miot is null, model:%s", cJSON_PrintUnformatted(item));
            // thing->initMiot(cJSON_IsNull(ip) ? "" : ip->valuestring, token->valuestring, name->valuestring, std::stoi(did->valuestring));
            // // thing->set_ip(ip->valuestring);
            // // thing->set_token(token->valuestring);
            // AddThing(thing);
        }
        else
        {
            auto thing2 = iot::CreateThing("MIHOME");
            if (thing2 == nullptr)
            {
                ESP_LOGE(TAG, "Failed to create thing");
                continue;
            }
            thing2->initMiot(cJSON_IsNull(ip) ? "" : ip->valuestring, token->valuestring, name->valuestring, did->valuestring);
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
            // thing2->set_name(name->valuestring);
            // thing2->set_description(name->valuestring);
            thing_manager.AddThing(thing2);
        }
        // cJSON_Delete(item);
    }
    return true;
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
