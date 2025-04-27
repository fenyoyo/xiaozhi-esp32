#include "iot/thing.h"
#include "iot/miot.h"
#include <esp_log.h>
#include "iot/miot_device.h"

#define TAG "MIHOME"

namespace iot
{
    class MIHOME : public Thing
    {
    private:
        std::string ip_;
        std::string token_;
        MiotDevice miotDevice;

        // std::map<std::string, SpecProperty> miotSpec = {};
        // std::map<std::string, SpecAction> miotSpecAction = {};
        std::map<std::string, cJSON *> miotSpec = {};

    public:
        MIHOME() : Thing("设备", "")
        {
        }

        void initMiot(const std::string &ip, const std::string &token, const std::string &name, const std::string &deviceId) override
        {
            ip_ = ip;
            token_ = token;
            set_description(name);
            // set_name(token);
            miotDevice = MiotDevice(ip_, token_, deviceId);
            miotDevice.init();
            miotDevice.setCallback([this](const std::string &data)
                                   {
                                    // {"code":0,"msg":"Success","data":[{"did":"682588579","iid":"0.2.1","siid":2,"piid":1,"value":true,"code":0,"updateTime":1745186665,"exe_time":0},{"did":"682588579","iid":"0.2.2","siid":2,"piid":2,"value":0,"code":0,"updateTime":1745178312,"exe_time":0},{"did":"682588579","iid":"0.2.4","siid":2,"piid":4,"value":0,"code":0,"updateTime":1745178313,"exe_time":0},{"did":"682588579","iid":"0.3.1","siid":3,"piid":1,"value":64,"code":0,"updateTime":1745186634,"exe_time":0},{"did":"682588579","iid":"0.3.4","siid":3,"piid":4,"value":13,"code":0,"updateTime":1745186488,"exe_time":0},{"did":"682588579","iid":"0.3.7","siid":3,"piid":7,"value":29,"code":0,"updateTime":1745186126,"exe_time":0},{"did":"682588579","iid":"0.3.8","siid":3,"piid":8,"value":0,"code":0,"updateTime":1745178316,"exe_time":0}]}
                                    //    ESP_LOGI(TAG, "云端返回：%s", data.c_str());
                                       // TODO 解析数据
                                    //    for (auto &item : miotSpec)
                                    //    {
                                    //         ESP_LOGI(TAG, "key:%s", item.first.c_str());
                                    //    }
                                       cJSON *json = cJSON_Parse(data.c_str());
                                       if (json == nullptr)
                                       {
                                           ESP_LOGE(TAG, "json is null");
                                           return;
                                       }
                                        cJSON *dataJson = cJSON_GetObjectItem(json, "data");
                                        cJSON *code = cJSON_GetObjectItem(json, "code");
                                        if (code == nullptr || code->valueint != 0)
                                        {
                                            ESP_LOGE(TAG, "code is null or not 0");
                                            cJSON_Delete(json);
                                            return;
                                        }
                                        if (dataJson == nullptr)
                                        {
                                            ESP_LOGE(TAG, "dataJson is null");
                                            cJSON_Delete(json);
                                            return;
                                        }
                                        int size = cJSON_GetArraySize(dataJson);
                                        for (int ii = 0; ii < size; ii++)
                                        {
                                            cJSON *item = cJSON_GetArrayItem(dataJson, ii);
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
                                            cJSON *siid = cJSON_GetObjectItem(item, "siid");
                                            if (siid == nullptr)
                                            {
                                                ESP_LOGE(TAG, "siid is null");
                                                continue;
                                            }
                                            cJSON *piid = cJSON_GetObjectItem(item, "piid");
                                            if (piid == nullptr)
                                            {
                                                ESP_LOGE(TAG, "piid is null");
                                                continue;
                                            }
                                            cJSON *value = cJSON_GetObjectItem(item, "value");
                                            if (value == nullptr)
                                            {
                                                ESP_LOGE(TAG, "value is null");
                                                continue;
                                            }
                                            cJSON *iid = cJSON_GetObjectItem(item, "iid");
                                            if (value == nullptr)
                                            {
                                                ESP_LOGE(TAG, "iid is null");
                                                continue;
                                            }
    
                                            auto spec = miotSpec.find(iid->valuestring);
                                            if (spec != miotSpec.end())
                                            {
                                                // ESP_LOGI(TAG, "iid found: %s,%d", iid->valuestring,value->valuedouble);
                                                if(value->type == cJSON_Number)
                                                {
                                                    spec->second->valuedouble = value->valuedouble;
                                                }
                                                else if(value->type == cJSON_String)
                                                {
                                                    spec->second->valuestring = value->valuestring;
                                                }
                                                else if(value->type == cJSON_True)
                                                {
                                                    spec->second->valuedouble = 1;
                                                    // ESP_LOGI(TAG, "iid:%s value true",iid->valuestring);
                                                }
                                                else if(value->type == cJSON_False)
                                                {
                                                    spec->second->valuedouble = 0;
                                                    // ESP_LOGI(TAG, "iid:%s value false",iid->valuestring);
                                                }

                                                // ESP_LOGI(TAG, "iid:%s type:%d  value:%f", iid->valuestring, spec->second->type, spec->second->valuedouble);
                                            }
                                            else
                                            {
                                                ESP_LOGE(TAG, "spec not found: %s", iid->valuestring);
                                            }
                                        } });
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
                if ((static_cast<uint8_t>(a->valueint) & static_cast<uint8_t>(Permission::READ)) != 0)
                {
                    std::string iid = "0." + std::to_string(s->valueint) + "." + std::to_string(p->valueint);
                    if (static_cast<ValueType>(v->valueint) == kValueTypeBoolean)
                    {

                        cJSON *json = cJSON_CreateBool(false);
                        miotSpec.insert({iid, json});
                        properties_.AddBooleanProperty(did->valuestring, d->valuestring, [this, iid]() -> bool
                                                       {
                                                           //    ESP_LOGI(TAG, "get iid value:%s", iid.c_str());
                                                           auto prop = miotSpec.find(iid);
                                                           if (prop == miotSpec.end())
                                                           {
                                                               ESP_LOGE(TAG, "property not found: %s", iid.c_str());
                                                               return false;
                                                           }
                                                           auto b = static_cast<bool>(miotSpec.find(iid)->second->valuedouble);
                                                           //    ESP_LOGI(TAG, "get iid value:%s %d", iid.c_str(), b);
                                                           return b; //
                                                       });
                    }
                    else if (static_cast<ValueType>(v->valueint) == kValueTypeNumber)
                    {
                        cJSON *json = cJSON_CreateNumber(0);
                        miotSpec.insert({iid, json});
                        properties_.AddNumberProperty(did->valuestring, d->valuestring, [this, iid]() -> int
                                                      {
                                                          //   ESP_LOGI(TAG, "get iid value:%s", iid.c_str());
                                                          auto prop = miotSpec.find(iid);
                                                          if (prop == miotSpec.end())
                                                          {
                                                              ESP_LOGE(TAG, "property not found: %s", iid.c_str());
                                                              return 0;
                                                          }
                                                          //   ESP_LOGI(TAG, "get iid value:%s %f", iid.c_str(), miotSpec.find(iid)->second->valuedouble);
                                                          return miotSpec.find(iid)->second->valuedouble; //
                                                      });
                    }
                    else
                    {
                        cJSON *json = cJSON_CreateString("");
                        miotSpec.insert({iid, json});
                        properties_.AddStringProperty(did->valuestring, d->valuestring, [this, iid]() -> std::string
                                                      {
                                                          //   ESP_LOGI(TAG, "get iid value:%s type %d  %d %d", iid.c_str(), miotSpec.find(iid)->second->type, cJSON_Number, cJSON_String);
                                                          auto prop = miotSpec.find(iid);
                                                          if (prop == miotSpec.end())
                                                          {
                                                              ESP_LOGE(TAG, "property not found: %s", iid.c_str());
                                                              return "";
                                                          }

                                                          return std::to_string(miotSpec.find(iid)->second->valuedouble);
                                                          //   if (miotSpec.find(iid)->second->type == cJSON_String && size_t(miotSpec.find(iid)->second->valuestring) == 0)
                                                          //   {
                                                          //       ESP_LOGI(TAG, "get string iid value:%s , %f", iid.c_str(), miotSpec.find(iid)->second->valuedouble);
                                                          //       return std::to_string(miotSpec.find(iid)->second->valuedouble);
                                                          //   }
                                                          //   return miotSpec.find(iid)->second->valuestring; //
                                                      });
                    }
                }
            }
            // 初始化设备状态
            miotDevice.getProperties();
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
                                       miotDevice.callCloudAction(s->valueint, a->valueint, av); });
            }
        }
    };

} // namespace iot

DECLARE_THING(MIHOME);