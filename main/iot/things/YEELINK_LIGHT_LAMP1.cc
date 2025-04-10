#include "iot/thing.h"
#include "iot/miot.h"
#include "esp_log.h"
#include "iot/miot_device.h"

#define TAG "YEELINK_LIGHT_LAMP1"

namespace iot
{
    // 米家台灯
    // https://home.miot-spec.com/spec/yeelink.light.lamp1
    class YEELINK_LIGHT_LAMP1 : public Thing
    {
    private:
        std::string ip_;
        std::string token_;
        MiotDevice miotDevice;

        std::map<std::string, SpecProperty> miotSpec = {

            // {
            //     "light:on",
            //     {2, 1, "on", "", kValueTypeBoolean, Permission::READ | Permission::WRITE, "setOn", "Switch Status"},
            // },

            // {
            //     "light:brightness",
            //     {2, 2, "brightness", "", kValueTypeNumber, Permission::READ | Permission::WRITE, "setBrightness", "Brightness"},
            // },

            // {
            //     "light:color-temperature",
            //     {2, 3, "color-temperature", "", kValueTypeNumber, Permission::READ | Permission::WRITE, "setColorTemperature", "Color Temperature"},
            // },

            // {
            //     "light:mode",
            //     {2, 4, "mode", "", kValueTypeNumber, Permission::WRITE, "setMode", "Mode"},
            // },

            // {
            //     "light-extension:brightness-delta",
            //     {3, 1, "brightness-delta", "", {{kValue}}, Permission::WRITE, "setBrightnessDelta", "Adjust brightness"},
            // },

            // {
            //     "light-extension:ct-delta",
            //     {3, 2, "ct-delta", "", {{kValue}}, Permission::WRITE, "setCtDelta", "Adjust Color Temperature"},
            // },

            // {
            //     "light-extension:ct-adjust-alexa",
            //     {3, 3, "ct-adjust-alexa", "", kValueTypeNumber, Permission::WRITE, "setCtAdjustAlexa", "Alexa Adjust Color Temperature"},
            // },

        };
        std::map<std::string, SpecAction> miotSpecAction = {

            // {
            //     "light:toggle",
            //     {2, 1, "Toggle", "Toggle", {}},
            // },

        };

    public:
        YEELINK_LIGHT_LAMP1() : Thing("米家台灯", "")
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
            methods_.AddMethod("set_natural_level", "设置亮度", ParameterList({Parameter("value", "风速0-100", kValueTypeNumber, true)}), [this](const ParameterList &parameters)
                               {
                                   auto value = static_cast<int8_t>(parameters["value"].number());
                                   miotDevice.send("set_natural_level", "[" + std::to_string(value) + "]"); //
                               });

            methods_.AddMethod("set_bright", "设置灯的亮度", ParameterList({Parameter("value", "亮度0-100", kValueTypeNumber, true)}), [this](const ParameterList &parameters)
                               {
                                   auto value = static_cast<int8_t>(parameters["value"].number());
                                   miotDevice.send("set_bright", "[" + std::to_string(value) + "]"); //
                               });

            // methods_.AddMethod("set_color_temperature", "设置灯的色温", ParameterList({Parameter("value", "1700-6500", kValueTypeNumber, true)}), [this](const ParameterList &parameters)
            //                    {
            //                        auto value = static_cast<int8_t>(parameters["value"].number());
            //                        miotDevice.send("set_color_temperature", "[" + std::to_string(value) + "]"); //
            //                    });
        }
    };

} // namespace iot

DECLARE_THING(YEELINK_LIGHT_LAMP1);