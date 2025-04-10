#include "iot/thing.h"
#include "iot/miot.h"
#include "esp_log.h"
#include "iot/miot_device.h"

#define TAG "YEELINK_LIGHT_CEIL40"

namespace iot
{
    // Yeelight智能LED吸顶灯升级版
    // https://home.miot-spec.com/spec/yeelink.light.ceil40
    class YEELINK_LIGHT_CEIL40 : public Thing
    {
    private:
        std::string ip_;
        std::string token_;
        MiotDevice miotDevice;

        std::map<std::string, SpecProperty> miotSpec = {

            {
                "light:on",
                {2, 1, "是否打开灯", "", kValueTypeBoolean, Permission::READ | Permission::WRITE, "setOn", "打开或者关闭灯"},
            },

            {
                "light:brightness",
                {2, 2, "亮度", "", kValueTypeNumber, Permission::READ | Permission::WRITE, "setBrightness", "设置亮度:0-100"},
            },

            // {
            //     "light:color-temperature",
            //     {2, 3, "color-temperature", "", kValueTypeNumber, Permission::READ | Permission::WRITE, "setColorTemperature", "Color Temperature"},
            // },

            {
                "light:mode",
                {2, 7, "模式", "", kValueTypeNumber, Permission::READ | Permission::WRITE, "setMode", "设置模式:0=无模式 1=阅读模式 1=夜晚模式 3=电视模式 4=温馨模式 12=手动模式 15=白天模式 6=蜡烛模式"},
            },

            // {
            //     "light:flex-switch",
            //     {2, 12, "flex-switch", "", kValueTypeBoolean, Permission::READ | Permission::WRITE, "setFlexSwitch", "Flex Switch"},
            // },

            // {
            //     "light:sleep-aid-mode",
            //     {2, 13, "sleep-aid-mode", "", kValueTypeBoolean, Permission::READ | Permission::WRITE, "setSleepAidMode", "Sleep Aid Mode"},
            // },

            // {
            //     "light:wake-up-mode",
            //     {2, 14, "wake-up-mode", "", kValueTypeBoolean, Permission::READ | Permission::WRITE, "setWakeUpMode", "Wake Up Mode"},
            // },

            // {
            //     "light:default-power-on-state",
            //     {2, 15, "default-power-on-state", "", kValueTypeNumber, Permission::READ | Permission::WRITE, "setDefaultPowerOnState", "Default Power On State"},
            // },

            // {
            //     "light:enable-time-period",
            //     {2, 16, "enable-time-period", "", kValueTypeNumber, Permission::READ | Permission::WRITE, "setEnableTimePeriod", "Enable Time Period"},
            // },

            // {
            //     "light:night-light-switch",
            //     {2, 17, "night-light-switch", "", kValueTypeBoolean, Permission::READ | Permission::WRITE, "setNightLightSwitch", "Night Light Switch"},
            // },

            // {
            //     "ambient-light:on",
            //     {4, 1, "on", "", kValueTypeBoolean, Permission::READ | Permission::WRITE, "setOn", "Switch Status"},
            // },

            // {
            //     "ambient-light:mode",
            //     {4, 2, "mode", "", kValueTypeNumber, Permission::READ | Permission::WRITE, "setMode", "Mode"},
            // },

            // {
            //     "ambient-light:flow-speed-level",
            //     {4, 3, "flow-speed-level", "", kValueTypeNumber, Permission::READ | Permission::WRITE, "setFlowSpeedLevel", "Flow Speed Level"},
            // },

            // {
            //     "scene:toggle",
            //     {5, 1, "toggle", "", kValueTypeBoolean, Permission::WRITE, "setToggle", ""},
            // },

            // {
            //     "scene:brightness-add",
            //     {5, 2, "brightness-add", "", kValueTypeBoolean, Permission::WRITE, "setBrightnessAdd", ""},
            // },

            // {
            //     "scene:brightness-dec",
            //     {5, 3, "brightness-dec", "", kValueTypeBoolean, Permission::WRITE, "setBrightnessDec", ""},
            // },

            // {
            //     "scene:brightness-sw",
            //     {5, 4, "brightness-sw", "", kValueTypeBoolean, Permission::WRITE, "setBrightnessSw", ""},
            // },

            // {
            //     "scene:on-and-brightness",
            //     {5, 5, "on-and-brightness", "", kValueTypeBoolean, Permission::WRITE, "setOnAndBrightness", ""},
            // },

            // {
            //     "scene:colortemperature-add",
            //     {5, 6, "colortemperature-add", "", kValueTypeBoolean, Permission::WRITE, "setColortemperatureAdd", ""},
            // },

            // {
            //     "scene:colortemperature-dec",
            //     {5, 7, "colortemperature-dec", "", kValueTypeBoolean, Permission::WRITE, "setColortemperatureDec", ""},
            // },

            // {
            //     "scene:colortemperature-sw",
            //     {5, 8, "colortemperature-sw", "", kValueTypeBoolean, Permission::WRITE, "setColortemperatureSw", ""},
            // },

            // {
            //     "scene:on-and-colortemp",
            //     {5, 9, "on-and-colortemp", "", kValueTypeBoolean, Permission::WRITE, "setOnAndColortemp", ""},
            // },

            // {
            //     "mode-setting:wake-up-time",
            //     {6, 1, "wake-up-time", "", kValueTypeNumber, Permission::READ | Permission::WRITE, "setWakeUpTime", ""},
            // },

            // {
            //     "mode-setting:wake-up-brightness",
            //     {6, 2, "wake-up-brightness", "", kValueTypeNumber, Permission::READ | Permission::WRITE, "setWakeUpBrightness", ""},
            // },

            // {
            //     "mode-setting:sleep-aid-time",
            //     {6, 3, "sleep-aid-time", "", kValueTypeNumber, Permission::READ | Permission::WRITE, "setSleepAidTime", ""},
            // },

            // {
            //     "mode-setting:light-pattern",
            //     {6, 4, "light-pattern", "", kValueTypeNumber, Permission::READ | Permission::WRITE, "setLightPattern", ""},
            // },

            // {
            //     "mode-setting:hard-ver",
            //     {6, 5, "hard-ver", "", kValueTypeNumber, Permission::READ, "setHardVer", ""},
            // },

            // {
            //     "mode-setting:custom-light",
            //     {6, 6, "custom-light", "", kValueTypeBoolean, Permission::READ | Permission::WRITE, "setCustomLight", ""},
            // },

            // {
            //     "mode-setting:night-light-offtime",
            //     {6, 7, "night-light-offtime", "", kValueTypeNumber, Permission::READ | Permission::WRITE, "setNightLightOfftime", ""},
            // },

            // {
            //     "mode-setting:night-light-off",
            //     {6, 8, "night-light-off", "", kValueTypeBoolean, Permission::READ | Permission::WRITE, "setNightLightOff", ""},
            // },

            // {
            //     "mode-setting:mode-brightness",
            //     {6, 9, "mode-brightness", "", kValueTypeNumber, Permission::READ, "setModeBrightness", "mode-brightness"},
            // },

            // {
            //     "mode-setting:subsection-one",
            //     {6, 10, "subsection-one", "", kValueTypeNumber, Permission::READ | Permission::WRITE, "setSubsectionOne", "subsection-one"},
            // },

            // {
            //     "mode-setting:subsection-two",
            //     {6, 11, "subsection-two", "", kValueTypeNumber, Permission::READ | Permission::WRITE, "setSubsectionTwo", "subsection-two"},
            // },

            // {
            //     "mode-setting:subsection-three",
            //     {6, 12, "subsection-three", "", kValueTypeNumber, Permission::READ | Permission::WRITE, "setSubsectionThree", ""},
            // },

            // {
            //     "mode-setting:subsection-four",
            //     {6, 13, "subsection-four", "", kValueTypeNumber, Permission::READ | Permission::WRITE, "setSubsectionFour", ""},
            // },

            // {
            //     "remote-control-management:remote-control-addable",
            //     {7, 1, "remote-control-addable", "", kValueTypeBoolean, Permission::READ | Permission::WRITE, "setRemoteControlAddable", "Remote Control Addable"},
            // },

            // {
            //     "remote-control-management:remote-control-list",
            //     {7, 3, "remote-control-list", "", kValueTypeString, Permission::READ, "setRemoteControlList", "Remote Control List"},
            // },

        };
        std::map<std::string, SpecAction> miotSpecAction = {

            // {
            //     "light:toggle",
            //     {2, 1, "Toggle", "Toggle", {}},
            // },

            // {
            //     "remote-control-management:delete-remote-control",
            //     {7, 1, "DeleteRemoteControl", "Delete Remote Control", {SpecActionParam(3, "value3", "参数描述", kValueTypeNumber)}},
            // },

        };

    public:
        YEELINK_LIGHT_CEIL40() : Thing("Yeelight智能LED吸顶灯升级版", "")
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

DECLARE_THING(YEELINK_LIGHT_CEIL40);