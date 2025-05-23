#include "mi.h"

#define TAG "MI"

Mi::Mi(/* args */)
{
}

Mi::~Mi()
{
}

bool Mi::GetMi()
{
    // std::string url = "http://192.168.1.6:8000/api/v1/iot/mqtt";
    // // std::string url = std::string(CONFIG_IOT_URL) + "api/v1/iot/mqtt";
    // auto &board = Board::GetInstance();
    // const int MAX_RETRY = 3;
    // int retry_count = 0;
    // int retry_delay = 10; // 初始重试延迟为10秒

    // while (true)
    // {
    //     retry_count++;
    //     if (retry_count >= MAX_RETRY)
    //     {
    //         ESP_LOGE(TAG, "Too many retries, exit version check");
    //         return false;
    //         break;
    //     }

    //     auto http = board.CreateHttp();
    //     auto boardJson = board.GetJson();
    //     http->SetHeader("Content-Type", "application/json");
    //     std::string method = "POST";
    //     if (!http->Open(method, url, boardJson))
    //     {
    //         ESP_LOGE(TAG, "Failed to open HTTP connection");
    //         delete http;
    //         return false;
    //     }
    //     auto response = http->GetBody();
    //     delete http;
    //     root_ = cJSON_Parse(response.c_str());
    //     if (root_ == NULL)
    //     {
    //         ESP_LOGE(TAG, "Failed to parse JSON response");
    //         return false;
    //     }

    //     cJSON *activation_code = cJSON_GetObjectItem(root_, "activation_code");
    //     if (activation_code != NULL)
    //     {
    //         ESP_LOGE(TAG, "did not activation");

    //         // Alert(Lang::Strings::ERROR, message.c_str(), "sad");
    //         cJSON *code = cJSON_GetObjectItem(activation_code, "code");
    //         cJSON *url = cJSON_GetObjectItem(activation_code, "url");
    //         std::string message = std::string("请前往 ") + cJSON_GetStringValue(url) + " 绑定设备验证码" + cJSON_GetStringValue(code);
    //         ESP_LOGI(TAG, "message is %s", message.c_str());
    //         // display->SetStatus(Lang::Strings::CHECKING_NEW_VERSION);
    //         // display->SetChatMessage("system", "请前往 xiaozhi.uyuo.me 绑定设备验证码5555");
    //         // display->SetEmotion("sad");
    //         vTaskDelay(pdMS_TO_TICKS(retry_delay * 1000));
    //         continue;
    //     }
    //     cJSON *mihome = cJSON_GetObjectItem(root_, "mihome");
    //     if (mihome == NULL or cJSON_IsFalse(mihome))
    //     {
    //         ESP_LOGE(TAG, "Get MiHome binding mi failed");
    //         std::string message = "xiaozhi.uyuo.me 绑定米家";
    //         ESP_LOGI(TAG, "message is %s", message.c_str());
    //         // Alert(Lang::Strings::ERROR, "xiaozhi.uyuo.me 绑定米家", "sad");
    //         // display->SetChatMessage("system", "请前往 xiaozhi.uyuo.me 绑定米家");
    //         // display->SetEmotion("sad");
    //         vTaskDelay(pdMS_TO_TICKS(retry_delay * 1000));
    //         continue;
    //     }
    //     break;
    // }

    return true;
}

bool Mi::RegisterIot()
{

    // if (!GetMi())
    // {
    //     ESP_LOGE(TAG, "Get MiHome failed");
    //     return false;
    // }
    // auto &board = Board::GetInstance();

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
