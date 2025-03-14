#include "iot/thing.h"
#include "iot/miot.h"
#include <esp_log.h>
#include "iot/miot_device.h"

#define TAG "ZHIMI_AIRCONDITION_MA1"

namespace iot
{
    // 米家互联网空调（三级能效）
    // https://home.miot-spec.com/spec/zhimi.aircondition.ma1
    class ZHIMI_AIRCONDITION_MA1 : public Thing
    {
    private:
        std::string ip_;
        std::string token_;
        MiotDevice miotDevice;

        std::map<std::string, SpecProperty> miotSpec = {

            {
                "air-conditioner:on",
                {2, 1, "电源", "", kValueTypeBoolean, Permission::READ | Permission::WRITE, "setOn", "开启或关闭"},
            },

            {
                "air-conditioner:mode",
                {2, 2, "模式", "", kValueTypeNumber, Permission::READ | Permission::WRITE, "setMode", "设置模式", "0=自动 1=制冷 2=除湿 3=加热 4=送风"},
            },

            {
                "air-conditioner:target-temperature",
                {2, 3, "温度", "", kValueTypeNumber, Permission::READ | Permission::WRITE, "setTargetTemperature", "设置温度", "16~32度"},
            },

            {
                "air-conditioner:heater",
                {2, 4, "辅热模式", "", kValueTypeBoolean, Permission::READ | Permission::WRITE, "setHeater", "是否开启辅热模式"},
            },

            {
                "air-conditioner:sleep-mode",
                {2, 5, "睡眠模式", "", kValueTypeBoolean, Permission::READ | Permission::WRITE, "setSleepMode", "是否开启睡眠模式"},
            },

            {
                "fan-control:fan-level",
                {3, 1, "风速", "", kValueTypeNumber, Permission::READ | Permission::WRITE, "setFanLevel", "设置风速", "0=自动 1~5级风力"},
            },

            {
                "fan-control:vertical-swing",
                {3, 2, "上下扫风", "", kValueTypeBoolean, Permission::READ | Permission::WRITE, "setVerticalSwing", "是否开启上下扫风"},
            },

            {
                "fan-control:vertical-angle",
                {3, 3, "名称", "", kValueTypeNumber, Permission::READ | Permission::WRITE, "setVerticalAngle", "属性描述"},
            },

            {
                "environment:temperature",
                {4, 1, "空调温度", "", kValueTypeNumber, Permission::READ},
            },
        };

    public:
        ZHIMI_AIRCONDITION_MA1() : Thing("米家互联网空调（三级能效）", "")
        {
            Register();
        }

        void initMiot(const std::string &ip, const std::string &token, const std::string &name, const uint32_t &did) override
        {
            ip_ = ip;
            token_ = token;
            set_description(name);
            miotDevice = MiotDevice(ip_, token_, did);
            miotDevice.setCallback([this](const std::string &data)
                                   {
                                       Message message;
                                       message.parse(data);

                                       if (message.header.packetLength == 32)
                                       {
                                           miotDevice.setstamp(message.header.stamp);
                                           miotDevice.setdeviceId(message.header.deviceID);
                                           ESP_LOGI(TAG, "get timestamp_:%ld,deviceID_:%ld", message.header.stamp, message.header.deviceID);

                                           return;
                                       }
                                       auto json = message.decrypt(token_);
                                       cJSON *root = cJSON_Parse(json.data());
                                       if (root == NULL)
                                       {
                                           ESP_LOGE(TAG, "getProperties cJSON_Parse failed2");
                                           return;
                                       }

                                       cJSON *result_ = cJSON_GetObjectItem(root, "result");
                                       if (result_->type != cJSON_Array)
                                       {
                                           return;
                                       }
                                       ESP_LOGI(TAG, "getProperties result:%s", cJSON_PrintUnformatted(result_));
                                       for (int i = 0; i < cJSON_GetArraySize(result_); i++)
                                       {

                                           cJSON *item = cJSON_GetArrayItem(result_, i);
                                           cJSON *code = cJSON_GetObjectItem(item, "code");
                                           if (code->valueint != 0)
                                           {
                                               continue;
                                           }
                                           cJSON *did = cJSON_GetObjectItem(item, "did");
                                           cJSON *value = cJSON_GetObjectItem(item, "value");
                                           if (value == NULL)
                                           {
                                               continue;
                                           }
                                           auto m = miotSpec.find(did->valuestring);
                                           m->second.value = value->valueint;
                                           ESP_LOGI(TAG, "set value :%s,%d", did->valuestring, value->valueint);
                                       }
                                       //
                                   });
            miotDevice.init();
        }

        void getProperties() override
        {
            miotDevice.getProperties2(miotSpec);
        }
        void Register()
        {
            for (auto it = miotSpec.begin(); it != miotSpec.end(); ++it)
            {
                switch (it->second.type)
                {
                case kValueTypeBoolean:
                    properties_.AddBooleanProperty(it->first, it->second.description, [this, it]() -> bool
                                                   { return miotSpec.find(it->first)->second.value; });

                    break;
                case kValueTypeNumber:
                    properties_.AddNumberProperty(it->first, it->second.description, [this, it]() -> int
                                                  { return miotSpec.find(it->first)->second.value; });
                    break;
                case kValueTypeString:
                default:
                    properties_.AddStringProperty(it->first, it->second.description, [this, it]() -> std::string
                                                  { return std::to_string(miotSpec.find(it->first)->second.value); });
                    break;
                }

                if ((static_cast<uint8_t>(it->second.perm) & static_cast<uint8_t>(Permission::WRITE)) != 0)
                {
                    methods_.AddMethod(it->second.method_name, it->second.method_description, ParameterList({Parameter("value", it->second.parameter_description, it->second.type, true)}), [this, it](const ParameterList &parameters)
                                       {
                                           if (it->second.type == kValueTypeBoolean)
                                           {
                                               auto value = static_cast<int8_t>(parameters["value"].boolean());
                                               miotDevice.setProperty2(miotSpec, it->first, value, true); //
                                           }
                                           else if (it->second.type == kValueTypeNumber)
                                           {
                                               auto value = static_cast<int8_t>(parameters["value"].number());
                                               miotDevice.setProperty2(miotSpec, it->first, value, false);
                                           }
                                           else
                                           {
                                               auto value = static_cast<int8_t>(parameters["value"].number());
                                               miotDevice.setProperty2(miotSpec, it->first, value, false);

                                           } //
                                       } //
                    );
                }
            }
        }
    };

} // namespace iot

DECLARE_THING(ZHIMI_AIRCONDITION_MA1);