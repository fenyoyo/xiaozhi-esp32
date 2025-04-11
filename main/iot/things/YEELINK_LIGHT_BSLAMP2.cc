#include "iot/thing.h"
#include "iot/miot.h"
#include "esp_log.h"
#include "iot/miot_device.h"
#include "map"
#define TAG "YEELINK_LIGHT_BSLAMP2"

namespace iot
{
    // Yeelight 床头灯2代 miio
    // https://home.miot-spec.com/spec/yeelink.light.bslamp2
    class YEELINK_LIGHT_BSLAMP2 : public Thing
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
        YEELINK_LIGHT_BSLAMP2() : Thing("Yeelight 床头灯2代", "")
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
            methods_.AddMethod("set_color_temperature", "日光模式|设置灯的色温", ParameterList({Parameter("value", "1700到6500之间的整数", kValueTypeNumber, true)}), [this](const ParameterList &parameters)
                               {
                                   auto value = static_cast<int>(parameters["value"].number());
                                   miotDevice.send("set_ct_abx", "[" + std::to_string(value) + ",\"smooth\",500]"); //
                               });

            methods_.AddMethod("set_rbg", "彩光模式|设置灯的颜色", ParameterList({Parameter("r", "对应RGB中的R,0到255", kValueTypeNumber, true), Parameter("g", "对应RGB中的G,0到255", kValueTypeNumber, true), Parameter("b", "对应RGB中的B,0到255", kValueTypeNumber, true)}), [this](const ParameterList &parameters)
                               {
                                   auto r = parameters["r"].number();
                                   auto g = parameters["g"].number();
                                   auto b = parameters["b"].number();
                                   auto rgb = (r << 16) + (g << 8) + b;
                                   ESP_LOGI(TAG, "r:%d,g:%d,b:%d", r, g, b);
                                   miotDevice.send("set_rgb", "[" + std::to_string(rgb) + "]"); //
                               });
        }

        void initMiot(const std::string &ip, const std::string &token, const std::string &name, const uint32_t &did) override
        {
            ip_ = ip;
            token_ = token;
            set_description(name);
            miotDevice = MiotDevice(ip_, token_, did);
            miotDevice.init();
        }

        // void getProperties() override
        // {
        //     miotDevice.getProperties2(miotSpec);
        // }
    };

} // namespace iot

DECLARE_THING(YEELINK_LIGHT_BSLAMP2);