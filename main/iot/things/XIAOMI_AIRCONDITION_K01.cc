#include "iot/thing.h"
#include "iot/miot.h"
#include "esp_log.h"
#include "iot/miot_device.h"

#define TAG "XIAOMI_AIRCONDITION_K01"

namespace iot
{
    // 米家空调 清凉版（大1匹）
    // https://home.miot-spec.com/spec/xiaomi.aircondition.k01
    class XIAOMI_AIRCONDITION_K01 : public Thing
    {
    private:
        std::string ip_;
        std::string token_;
        MiotDevice miotDevice;

        std::map<std::string, SpecProperty> miotSpec = {

            {
                "air-conditioner:on",
                {2, 1, "开关", "", kValueTypeBoolean, Permission::READ | Permission::WRITE, "setOn", "开启或关闭空调"},
            },

            {
                "air-conditioner:mode",
                {2, 2, "模式", "", kValueTypeNumber, Permission::READ | Permission::WRITE, "setMode", "设置模式:2=制冷 3=送风"},
            },

            {
                "air-conditioner:target-temperature",
                {2, 4, "温度", "", kValueTypeString, Permission::READ | Permission::WRITE, "setTargetTemperature", "设置温度:16-31度"},
            },

            {
                "air-conditioner:sleep-mode",
                {2, 11, "睡眠模式", "", kValueTypeBoolean, Permission::READ | Permission::WRITE, "setSleepMode", "设置或者关闭睡眠模式"},
            },
            {
                "fan-control:fan-level",
                {3, 2, "风速", "", kValueTypeNumber, Permission::READ | Permission::WRITE, "setFanLevel", "设置风速:1-7级"},
            },

            {
                "fan-control:vertical-swing",
                {3, 4, "水平扫风", "", kValueTypeBoolean, Permission::READ | Permission::WRITE, "setVerticalSwing", "是否开启水平扫风"},
            },

        };
        std::map<std::string, SpecAction> miotSpecAction = {

        };

    public:
        XIAOMI_AIRCONDITION_K01() : Thing("米家空调 清凉版（大1匹）", "")
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

DECLARE_THING(XIAOMI_AIRCONDITION_K01);