#include "iot/thing.h"
#include "iot/miot.h"
#include "esp_log.h"
#include "iot/miot_device.h"

#define TAG "YEELINK_LIGHT_FANCL1"

namespace iot
{
    // Yeelight 逸扬风扇吊灯（智能款）
    // https://home.miot-spec.com/spec/yeelink.light.fancl1
    class YEELINK_LIGHT_FANCL1 : public Thing
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
                "light:mode",
                {2, 2, "模式", "", kValueTypeNumber, Permission::READ | Permission::WRITE, "setMode", "设置模式:0=白天模式 1=夜晚模式"},
            },

            {
                "light:brightness",
                {2, 3, "亮度", "", kValueTypeNumber, Permission::READ | Permission::WRITE, "setBrightness", "设置亮度:0-100"},
            },

            // {
            //     "light:color-temperature",
            //     {2, 5, "色温", "", kValueTypeNumber, Permission::READ | Permission::WRITE, "setColorTemperature", "设置灯光色温:1700-6500"},
            // },

            // {
            //     "light:flow",
            //     {2, 6, "flow", "", kValueTypeNumber, Permission::READ | Permission::WRITE, "setFlow", "Flow"},
            // },

            // {
            //     "light:off-delay-time",
            //     {2, 7, "off-delay-time", "", kValueTypeNumber, Permission::READ | Permission::WRITE, "setOffDelayTime", "Power Off Delay Time"},
            // },

            {
                "fan:on",
                {3, 1, "是否打开风扇", "", kValueTypeBoolean, Permission::READ | Permission::WRITE, "setOn", "打开或者关闭风扇"},
            },

            {
                "fan:fan-level",
                {3, 2, "风扇风力", "", kValueTypeNumber, Permission::READ | Permission::WRITE, "setFanLevel", "设置风扇风力:0-2档"},
            },

            // {
            //     "fan:mode",
            //     {3, 7, "mode", "", kValueTypeNumber, Permission::READ | Permission::WRITE, "setMode", "Mode"},
            // },

            // {
            //     "fan:status",
            //     {3, 8, "status", "", kValueTypeNumber, Permission::READ, "setStatus", "Status"},
            // },

            // {
            //     "fan:fault",
            //     {3, 9, "fault", "", kValueTypeNumber, Permission::READ, "setFault", "Device Fault"},
            // },

            // {
            //     "fan:off-delay-time",
            //     {3, 10, "off-delay-time", "", kValueTypeNumber, Permission::READ | Permission::WRITE, "setOffDelayTime", "Power Off Delay Time"},
            // },

            // {
            //     "yl-light:init-power-opt",
            //     {4, 1, "init-power-opt", "", kValueTypeNumber, Permission::READ | Permission::WRITE, "setInitPowerOpt", "上电是否开灯"},
            // },

            // {
            //     "yl-light:scene-param",
            //     {4, 3, "scene-param", "", kValueTypeString, Permission::READ | Permission::WRITE, "setSceneParam", ""},
            // },

            // {
            //     "yl-fan:fan-init-power-opt",
            //     {5, 1, "fan-init-power-opt", "", kValueTypeNumber, Permission::READ | Permission::WRITE, "setFanInitPowerOpt", "上电是否开风扇"},
            // },

        };
        std::map<std::string, SpecAction> miotSpecAction = {

            // {
            //     "light:toggle",
            //     {2, 1, "Toggle", "Toggle", {}},
            // },

            // {
            //     "fan:toggle",
            //     {3, 1, "Toggle", "Toggle", {}},
            // },

            // {
            //     "yl-light:set-scene",
            //     {4, 1, "SetScene", "设置灯光情景", {SpecActionParam(3, "value3", "参数描述", kValueTypeNumber)}},
            // },

            // {
            //     "yl-light:brightness-cycle",
            //     {4, 2, "BrightnessCycle", "亮度切换（智能联动）", {}},
            // },

            // {
            //     "yl-light:ct-cycle",
            //     {4, 3, "CtCycle", "色温切换（智能联动）", {}},
            // },

            // {
            //     "yl-light:fan-gears-cycle",
            //     {4, 4, "FanGearsCycle", "风扇档位切换（智能联动）", {}},
            // },

            // {
            //     "yl-fan:fan-gears-cycle",
            //     {5, 1, "FanGearsCycle", "风扇档位切换（智能联动）", {}},
            // },

        };

    public:
        YEELINK_LIGHT_FANCL1() : Thing("Yeelight 逸扬风扇吊灯（智能款）", "")
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

DECLARE_THING(YEELINK_LIGHT_FANCL1);