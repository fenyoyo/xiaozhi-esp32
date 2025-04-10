#include "iot/thing.h"
#include "iot/miot.h"
#include "esp_log.h"
#include "iot/miot_device.h"

#define TAG "DEERMA_HUMIDIFIER_JSQM"

namespace iot
{
    // 米家智能除菌加湿器
    // https://home.miot-spec.com/spec/deerma.humidifier.jsqm
    class DEERMA_HUMIDIFIER_JSQM : public Thing
    {
    private:
        std::string ip_;
        std::string token_;
        MiotDevice miotDevice;

        std::map<std::string, SpecProperty> miotSpec = {

            {
                "humidifier:on",
                {2, 1, "是否打开", "", kValueTypeBoolean, Permission::READ | Permission::WRITE, "setOn", "打开或者关闭加湿器"},
            },

            // {
            //     "humidifier:fault",
            //     {2, 2, "fault", "", kValueTypeNumber, Permission::READ, "setFault", "Device Fault"},
            // },

            {
                "humidifier:fan-level",
                {2, 5, "风速", "", kValueTypeNumber, Permission::READ | Permission::WRITE, "setFanLevel ", "设置风速:1到4档"},
            },

            {
                "humidifier:target-humidity",
                {2, 6, "加湿器湿度", "", kValueTypeNumber, Permission::READ | Permission::WRITE, "setTargetHumidity", "设置加湿器湿度:40-80的整数"},
            },

            // {
            //     "environment:relative-humidity",
            //     {3, 1, "relative-humidity", "", kValueTypeNumber, Permission::READ, "setRelativeHumidity", "Relative Humidity"},
            // },

            {
                "environment:temperature",
                {3, 7, "加湿器温度", "", kValueTypeNumber, Permission::READ},
            },

            // {
            //     "alarm:alarm",
            //     {5, 1, "alarm", "", kValueTypeBoolean, Permission::READ | Permission::WRITE, "setAlarm", "Alarm"},
            // },

            // {
            //     "indicator-light:on",
            //     {6, 1, "on", "", kValueTypeBoolean, Permission::READ | Permission::WRITE, "setOn", "Switch Status"},
            // },

            // {
            //     "custom:water-shortage-fault",
            //     {7, 1, "water-shortage-fault", "", kValueTypeBoolean, Permission::READ, "setWaterShortageFault", "water-shortage-fault"},
            // },

            // {
            //     "custom:the-tank-filed",
            //     {7, 2, "the-tank-filed", "", kValueTypeBoolean, Permission::READ, "setTheTankFiled", "the-tank-filed"},
            // },

        };
        std::map<std::string, SpecAction> miotSpecAction = {

        };

    public:
        DEERMA_HUMIDIFIER_JSQM() : Thing("米家智能除菌加湿器", "")
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
                                               miotSpec.find(it->first)->second.value = value;
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

DECLARE_THING(DEERMA_HUMIDIFIER_JSQM);