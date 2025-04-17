#include "iot/thing.h"
#include "iot/miot.h"
#include <esp_log.h>
#include "iot/miot_device.h"

#define TAG "MIHOME"

namespace iot
{
    // 米家智能插座3
    // https://home.miot-spec.com/spec/cuco.plug.v3
    class MIHOME : public Thing
    {
    private:
        std::string ip_;
        std::string token_;
        MiotDevice miotDevice;

        std::map<std::string, SpecProperty> miotSpec = {};
        std::map<std::string, SpecAction> miotSpecAction = {};

    public:
        MIHOME() : Thing("设备", "")
        {
        }

        void initMiot(const std::string &ip, const std::string &token, const std::string &name, const uint32_t &did) override
        {
            ip_ = ip;
            token_ = token;
            set_description(name);
            // set_name(token);
            miotDevice = MiotDevice(ip_, token_, did);
            miotDevice.init();
            // ESP_LOGI(TAG, "设备：%s", name.c_str());
        }

        void registerProperty(const cJSON *iot) override
        {
            //[{"did":"fan:on","s":"2","p":"1","pn":"电源","d":"打开或关闭电源","t":"bool","v":0,"a":7,"m":"On"},{"did":"fan:fan-level","s":"2","p":"2","pn":"风速","d":"设置风速","t":"uint8","v":1,"a":7,"m":"FanLevel"},{"did":"fan:horizontal-swing","s":"2","p":"3","pn":"是否开启扫风","d":"设置是否开启扫风","t":"bool","v":0,"a":7,"m":"HorizontalSwing"},{"did":"fan:mode","s":"2","p":"7","pn":"风扇模式:0=直吹模式,1=睡眠模式","d":"设置风扇模式:0=直吹模式,1=睡眠模式","t":"uint8","v":1,"a":7,"m":"Mode"}]
            if (iot == nullptr)
            {
                ESP_LOGE(TAG, "iot is null");
                return;
            }
            int size = cJSON_GetArraySize(iot);
            for (int ii = 0; ii < size; ii++)
            {
                cJSON *item = cJSON_GetArrayItem(iot, ii);
                if (item == nullptr)
                {
                    ESP_LOGE(TAG, "item is null");
                    continue;
                }
                cJSON *did = cJSON_GetObjectItem(item, "did");
                if (did == nullptr)
                {
                    ESP_LOGE(TAG, "did is null");
                    continue;
                }

                cJSON *s = cJSON_GetObjectItem(item, "s");
                if (s == nullptr)
                {
                    ESP_LOGE(TAG, "s is null");
                    continue;
                }
                cJSON *p = cJSON_GetObjectItem(item, "p");
                if (p == nullptr)
                {
                    ESP_LOGE(TAG, "p is null");
                    continue;
                }
                cJSON *d = cJSON_GetObjectItem(item, "d");
                if (d == nullptr)
                {
                    ESP_LOGE(TAG, "d is null");
                    continue;
                }
                cJSON *v = cJSON_GetObjectItem(item, "v");
                if (v == nullptr)
                {
                    ESP_LOGE(TAG, "v is null");
                    continue;
                }
                cJSON *a = cJSON_GetObjectItem(item, "a");
                if (a == nullptr)
                {
                    ESP_LOGE(TAG, "a is null");
                    continue;
                }
                cJSON *mn = cJSON_GetObjectItem(item, "mn");
                if (mn == nullptr)
                {
                    ESP_LOGE(TAG, "mn is null");
                    continue;
                }
                cJSON *md = cJSON_GetObjectItem(item, "md");
                if (md == nullptr)
                {
                    ESP_LOGE(TAG, "md is null");
                    continue;
                }

                cJSON *pd = cJSON_GetObjectItem(item, "pd");
                if (pd == nullptr)
                {
                    ESP_LOGE(TAG, "pd is null");
                    continue;
                }
                if ((static_cast<uint8_t>(a->valueint) & static_cast<uint8_t>(Permission::WRITE)) != 0)
                {
                    methods_.AddMethod(mn->valuestring, d->valuestring, ParameterList({Parameter("value", pd->valuestring, static_cast<ValueType>(v->valueint), true)}), [this, did, s, p, v](const ParameterList &parameters)
                                       {
                        // ESP_LOGI(TAG, "id%s", ip_.c_str());
                        if (static_cast<ValueType>(v->valueint) == kValueTypeBoolean)
                        {
                            auto value = static_cast<int>(parameters["value"].boolean());
                            miotDevice.setCloudProperty(did->valuestring, s->valueint, p->valueint, value, true);
                                                                                                                 
                        }
                        else if (static_cast<ValueType>(v->valueint) == kValueTypeNumber)
                        {
                            auto value = static_cast<int>(parameters["value"].number());
                            miotDevice.setCloudProperty(did->valuestring, s->valueint, p->valueint, value, false);
                        }
                        else
                        {
                            auto value = static_cast<int>(parameters["value"].number());
                            miotDevice.setCloudProperty(did->valuestring, s->valueint, p->valueint, value, false);

                        } });
                }
            }
        }

        void registerAction(const cJSON *iot) override
        {
            if (iot == nullptr)
            {
                ESP_LOGE(TAG, "iot is null");
                return;
            }
            int size = cJSON_GetArraySize(iot);
            for (int ii = 0; ii < size; ii++)
            {
                cJSON *item = cJSON_GetArrayItem(iot, ii);
                if (item == nullptr)
                {
                    ESP_LOGE(TAG, "item is null");
                    continue;
                }
                cJSON *did = cJSON_GetObjectItem(item, "did");
                if (did == nullptr)
                {
                    ESP_LOGE(TAG, "did is null");
                    continue;
                }
                cJSON *s = cJSON_GetObjectItem(item, "s");
                if (s == nullptr)
                {
                    ESP_LOGE(TAG, "s is null");
                    continue;
                }
                cJSON *a = cJSON_GetObjectItem(item, "a");
                if (a == nullptr)
                {
                    ESP_LOGE(TAG, "a is null");
                    continue;
                }
                cJSON *mn = cJSON_GetObjectItem(item, "mn");
                if (mn == nullptr)
                {
                    ESP_LOGE(TAG, "mn is null");
                    continue;
                }
                cJSON *md = cJSON_GetObjectItem(item, "md");
                if (md == nullptr)
                {
                    ESP_LOGE(TAG, "md is null");
                    continue;
                }
                cJSON *p = cJSON_GetObjectItem(item, "p");
                if (p == nullptr)
                {
                    ESP_LOGE(TAG, "p is null");
                    continue;
                }
                int p_size = cJSON_GetArraySize(p);
                ParameterList parameterList;

                if (p_size > 0)
                {
                    for (int i = 0; i < p_size; i++)
                    {
                        cJSON *param = cJSON_GetArrayItem(p, i);
                        if (param == nullptr)
                        {
                            ESP_LOGE(TAG, "param is null");
                            continue;
                        }
                        cJSON *piid = cJSON_GetObjectItem(param, "piid");
                        if (piid == nullptr)
                        {
                            ESP_LOGE(TAG, "piid is null");
                            continue;
                        }
                        cJSON *name = cJSON_GetObjectItem(param, "name");
                        if (name == nullptr)
                        {
                            ESP_LOGE(TAG, "name is null");
                            continue;
                        }
                        cJSON *description = cJSON_GetObjectItem(param, "description");
                        if (description == nullptr)
                        {
                            ESP_LOGE(TAG, "description is null");
                            continue;
                        }
                        parameterList.AddParameter(Parameter(std::to_string(piid->valueint), description->valuestring, kValueTypeNumber, true));
                    }
                }
                methods_.AddMethod(mn->valuestring, md->valuestring, parameterList, [this, s, a, p](const ParameterList &parameters)
                                   {
                                       // callAction2 jsonStr:{"did": "call-2-5","siid": 2,"aiid": 5,"in": [{"piid": 5, "value": 1},]}
                                       std::map<int, int> av;

                                       auto p_size = cJSON_GetArraySize(p);
                                       for (size_t i = 0; i < p_size; i++)
                                       {
                                           cJSON *param = cJSON_GetArrayItem(p, i);
                                           cJSON *piid = cJSON_GetObjectItem(param, "piid");
                                           auto key = std::to_string(piid->valueint);
                                           auto value = static_cast<int8_t>(parameters[key].number());
                                           av.insert({piid->valueint, value});
                                       }
                                       miotDevice.callAction(s->valueint, a->valueint, av); });
            }
        }
    };

} // namespace iot

DECLARE_THING(MIHOME);