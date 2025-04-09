#include "iot/thing.h"
#include "iot/miot.h"
#include "esp_log.h"
#include "iot/miot_device.h"
#include "map"
#define TAG "YEELINK_LIGHT_BSLAMP3"

namespace iot
{
    // Yeelight 床头灯2代 miio
    // https://home.miot-spec.com/spec/yeelink.light.bslamp3
    class YEELINK_LIGHT_BSLAMP3 : public Thing
    {
    private:
        std::string ip_;
        std::string token_;
        MiotDevice miotDevice;

        std::map<std::string, SpecProperty> miotSpec = {
            {
                "light:on",
                {2, 1, "on", "", kValueTypeBoolean, Permission::READ | Permission::WRITE, "setOn", "Switch Status"},
            },

        };

    public:
        YEELINK_LIGHT_BSLAMP3() : Thing("Yeelight 床头灯2代", "")
        {
            methods_.AddMethod("set_on", "打开灯", ParameterList(), [this](const ParameterList &parameters)
                               {
                                   miotDevice.send("set_power", "[\"on\"]"); //
                               });
            methods_.AddMethod("set_off", "打开灯", ParameterList(), [this](const ParameterList &parameters)
                               {
                                   miotDevice.send("set_power", "[\"off\"]"); //
                               });
            methods_.AddMethod("set_brightness", "设置灯的亮度", ParameterList({Parameter("value", "亮度0-100", kValueTypeNumber, true)}), [this](const ParameterList &parameters)
                               {
                                   auto value = static_cast<int8_t>(parameters["value"].number());
                                   miotDevice.send("set_bright", "[" + std::to_string(value) + "]"); //
                               });
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
                                           return;
                                       }
                                       auto json = message.decrypt(token_);
                                       cJSON *root = cJSON_Parse(json.data());
                                       if (root == NULL)
                                       {
                                           return;
                                       }

                                       //    cJSON *result_ = cJSON_GetObjectItem(root, "result");
                                       //    if (result_->type != cJSON_Array)
                                       //    {
                                       //        return;
                                       //    }
                                       //    ESP_LOGI(TAG, "getProperties result:%s", cJSON_PrintUnformatted(result_));
                                       //    for (int i = 0; i < cJSON_GetArraySize(result_); i++)
                                       //    {

                                       //        cJSON *item = cJSON_GetArrayItem(result_, i);
                                       //        cJSON *code = cJSON_GetObjectItem(item, "code");
                                       //        if (code->valueint != 0)
                                       //        {
                                       //            continue;
                                       //        }
                                       //        cJSON *did = cJSON_GetObjectItem(item, "did");
                                       //        cJSON *value = cJSON_GetObjectItem(item, "value");
                                       //        if (value == NULL)
                                       //        {
                                       //            continue;
                                       //        }
                                       //        auto m = miotSpec.find(did->valuestring);
                                       //        m->second.value = value->valueint;
                                       //    }

                                       //
                                   });
            miotDevice.init();
        }

        // void getProperties() override
        // {
        //     miotDevice.getProperties2(miotSpec);
        // }
    };

} // namespace iot

DECLARE_THING(YEELINK_LIGHT_BSLAMP3);