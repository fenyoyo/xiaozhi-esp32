#include "iot/thing.h"
#include "iot/miot.h"
#include "esp_log.h"
#include "iot/miot_device.h"

#define TAG "CHUANGMI_PLUG_M3"

namespace iot
{
    // 小米米家智能插座WiFi版 miio
    // https://home.miot-spec.com/spec/chuangmi.plug.m3
    class CHUANGMI_PLUG_M3 : public Thing
    {
    private:
        std::string ip_;
        std::string token_;
        MiotDevice miotDevice;

        std::map<std::string, SpecProperty> miotSpec = {

            {
                "switch:on",
                {2, 1, "on", "", kValueTypeBoolean, Permission::READ | Permission::WRITE, "setOn", "Switch Status"},
            },

            // {
            //     "switch:temperature",
            //     {2, 2, "temperature", "", {{kValue}}, Permission::READ, "setTemperature", "Temperature"},
            // },

            {
                "indicator-light:on",
                {3, 1, "on", "", kValueTypeBoolean, Permission::READ | Permission::WRITE, "setOn", "Switch Status"},
            },

        };
        std::map<std::string, SpecAction> miotSpecAction = {

        };

    public:
        CHUANGMI_PLUG_M3() : Thing("小米米家智能插座WiFi版", "")
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
            methods_.AddMethod("set_on", "打开插座", ParameterList(), [this](const ParameterList &parameters)
                               {
                                   miotDevice.send("set_power", "[\"on\"]"); //
                               });
            methods_.AddMethod("set_off", "关闭插座", ParameterList(), [this](const ParameterList &parameters)
                               {
                                   miotDevice.send("set_power", "[\"off\"]"); //
                               });
        }
    };

} // namespace iot

DECLARE_THING(CHUANGMI_PLUG_M3);