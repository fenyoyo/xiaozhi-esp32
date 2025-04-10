#include "iot/thing.h"
#include "iot/miot.h"
#include "esp_log.h"
#include "iot/miot_device.h"

#define TAG "ZHIMI_FAN_SA1"

namespace iot
{
    // 米家直流变频落地扇
    // https://home.miot-spec.com/spec/zhimi.fan.sa1
    class ZHIMI_FAN_SA1 : public Thing
    {
    private:
        std::string ip_;
        std::string token_;
        MiotDevice miotDevice;

        std::map<std::string, SpecProperty> miotSpec = {

            {
                "fan:on",
                {2, 1, "on", "", kValueTypeBoolean, Permission::READ | Permission::WRITE, "setOn", "Switch Status"},
            },

            {
                "fan:fan-level",
                {2, 2, "fan-level", "", kValueTypeNumber, Permission::READ | Permission::WRITE, "setFanLevel", "Gear Fan Level"},
            },

            {
                "fan:horizontal-swing",
                {2, 3, "horizontal-swing", "", kValueTypeBoolean, Permission::READ | Permission::WRITE, "setHorizontalSwing", "Horizontal Swing"},
            },

            {
                "fan:horizontal-swing-included-angle",
                {2, 4, "horizontal-swing-included-angle", "", kValueTypeNumber, Permission::READ | Permission::WRITE, "setHorizontalSwingIncludedAngle", "Horizontal Swing Included Angle"},
            },

            {
                "fan:mode",
                {2, 5, "mode", "", kValueTypeNumber, Permission::READ | Permission::WRITE, "setMode", "Mode"},
            },

            {
                "fan:fan-level",
                {2, 6, "fan-level", "", kValueTypeNumber, Permission::READ | Permission::WRITE, "setFanLevel", "Stepless Fan Level"},
            },

            {
                "physical-controls-locked:physical-controls-locked",
                {3, 1, "physical-controls-locked", "", kValueTypeBoolean, Permission::READ | Permission::WRITE, "setPhysicalControlsLocked", "Physical Control Locked"},
            },

            {
                "alarm:alarm",
                {4, 1, "alarm", "", kValueTypeBoolean, Permission::READ | Permission::WRITE, "setAlarm", "Alarm"},
            },

            {
                "indicator-light:brightness",
                {5, 1, "brightness", "", kValueTypeNumber, Permission::READ | Permission::WRITE, "setBrightness", "Brightness"},
            },

            {
                "countdown:countdown-time",
                {6, 1, "countdown-time", "", kValueTypeNumber, Permission::READ | Permission::WRITE, "setCountdownTime", "Countdown Time"},
            },

        };
        std::map<std::string, SpecAction> miotSpecAction = {

            {
                "fan:turn-left",
                {2, 1, "TurnLeft", "Turn Left", {}},
            },

            {
                "fan:turn-right",
                {2, 2, "TurnRight", "Turn Right", {}},
            },

        };

    public:
        ZHIMI_FAN_SA1() : Thing("米家直流变频落地扇", "")
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
            // miotDevice.getProperties2(miotSpec);
        }
        void Register()
        {
            methods_.AddMethod("set_power", "打开", ParameterList(), [this](const ParameterList &parameters)
                               {
                                   miotDevice.send("set_power", "[\"on\"]"); //
                               });
            methods_.AddMethod("set_power", "关闭", ParameterList(), [this](const ParameterList &parameters)
                               {
                                   miotDevice.send("set_power", "[\"off\"]"); //
                               });
            methods_.AddMethod("set_natural_level", "设置自然风风速", ParameterList({Parameter("value", "风速0-100", kValueTypeNumber, true)}), [this](const ParameterList &parameters)
                               {
                                   auto value = static_cast<int8_t>(parameters["value"].number());
                                   miotDevice.send("set_natural_level", "[" + std::to_string(value) + "]"); //
                               });

            methods_.AddMethod("set_speed_level", "设置直吹风风速", ParameterList({Parameter("value", "风速0-100,1档=25,2档=50,3档=75,4档=100", kValueTypeNumber, true)}), [this](const ParameterList &parameters)
                               {
                                   auto value = static_cast<int8_t>(parameters["value"].number());
                                   if (value < 0)
                                   {
                                       value = 0;
                                   }
                                   if (value > 100)
                                   {
                                       value = 100;
                                   }
                                   miotDevice.send("set_speed_level", "[" + std::to_string(value) + "]"); //
                               });

            methods_.AddMethod("set_direction", "设置自然风风速", ParameterList({Parameter("value", "风速0-100", kValueTypeNumber, true)}), [this](const ParameterList &parameters)
                               {
                                   auto value = static_cast<int8_t>(parameters["value"].number());
                                   miotDevice.send("set_speed_level", "[" + std::to_string(value) + "]"); //
                               });
            // TODO 扫风模式，提示音，亮度等等
        }
    };

} // namespace iot

DECLARE_THING(ZHIMI_FAN_SA1);