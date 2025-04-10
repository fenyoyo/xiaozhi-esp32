#include "iot/thing.h"
#include "iot/miot.h"
#include <esp_log.h>
#include "iot/miot_device.h"

#define TAG "XIAOMI_LIGHT_LAMP31"

namespace iot
{
    // 米家台灯2
    // https://home.miot-spec.com/spec/xiaomi.light.lamp31
    class XIAOMI_LIGHT_LAMP31 : public Thing
    {
    private:
        std::string ip_;
        std::string token_;
        MiotDevice miotDevice;

        std::map<std::string, SpecProperty> miotSpec = {

            {
                "light:on",
                {2, 1, "on", "", kValueTypeBoolean, Permission::READ | Permission::WRITE, "setOn", "设置电源状态：开或关"},
            },

            {
                "light:brightness",
                {2, 2, "亮度", "", kValueTypeNumber, Permission::READ | Permission::WRITE, "setBrightness", "设置亮度:0-100"},
            },

            {
                "light:color-temperature",
                {2, 3, "色温", "", kValueTypeNumber, Permission::READ | Permission::WRITE, "setColorTemperature", "色温:2700~5100"},
            },

            {
                "light:mode",
                {2, 15, "模式", "", kValueTypeNumber, Permission::READ | Permission::WRITE, "setMode", "设置模式:0=自动 1=阅读 2=电脑 3=温暖 4=休闲 5=办公 6=娱乐"},
            },

        };
        std::map<std::string, SpecAction> miotSpecAction = {

            {
                "light:brightness-up",
                {2, 2, "setBrightnessUp", "调高亮度", {SpecActionParam(16, "value16", "0-30", kValueTypeNumber)}},
            },
            {
                "light:brightness-down",
                {2, 3, "setBrightnessDown", "调低亮度", {SpecActionParam(16, "value16", "0-30", kValueTypeNumber)}},
            },
            {
                "light:color-temperature-up",
                {2, 4, "setColorTemperatureUp", "调高色温", {SpecActionParam(17, "value17", "200~2400,步长100", kValueTypeNumber)}},
            },
            {
                "light:color-temperature-down",
                {2, 5, "setColorTemperatureDown", "调低色温", {SpecActionParam(17, "value17", "200~2400,步长100", kValueTypeNumber)}},
            },

        };

    public:
        XIAOMI_LIGHT_LAMP31() : Thing("米家台灯2", "")
        {
            Register();
        }

        void initMiot(const std::string &ip, const std::string &token, const std::string &name, const uint32_t &did) override
        {
            ip_ = ip;
            token_ = token;
            set_description(name);
            miotDevice = MiotDevice(ip_, token_, did);

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
                                               miotSpec.find(it->first)->second.value = value;            //
                                           }
                                           else if (it->second.type == kValueTypeNumber)
                                           {
                                               auto value = static_cast<int8_t>(parameters["value"].number());
                                               miotDevice.setProperty2(miotSpec, it->first, value, false);
                                               miotSpec.find(it->first)->second.value = value;
                                           }
                                           else
                                           {
                                               auto value = static_cast<int8_t>(parameters["value"].number());
                                               miotDevice.setProperty2(miotSpec, it->first, value, false);
                                               miotSpec.find(it->first)->second.value = value;

                                           } //
                                       } //
                    );
                }
            }

            for (auto it = miotSpecAction.begin(); it != miotSpecAction.end(); ++it)
            {
                ParameterList parameterList;
                for (auto &&i : it->second.parameters)
                {
                    parameterList.AddParameter(Parameter(i.key, i.parameter_description, i.type, true));
                }
                methods_.AddMethod(it->second.method_name, it->second.method_description, parameterList, [this, it](const ParameterList &parameters)
                                   {
                                       std::map<uint8_t, int> av;
                                       // std::map<uint_8 piid,uint_8 value> value;
                                       for (auto &&i : it->second.parameters)
                                       {
                                           auto value = static_cast<int8_t>(parameters[i.key].number());
                                           av.insert({i.piid, value});
                                       }
                                       miotDevice.callAction2(miotSpecAction, it->first, av); //
                                   });
            };
        }
    };

} // namespace iot

DECLARE_THING(XIAOMI_LIGHT_LAMP31);
