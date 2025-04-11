#include "iot/thing.h"
#include "iot/miot.h"
#include "esp_log.h"
#include "iot/miot_device.h"

#define TAG "ROCKROBO_VACUUM_V1"

namespace iot
{
    // 米家扫地机器人
    // https://home.miot-spec.com/spec/rockrobo.vacuum.v1
    class ROCKROBO_VACUUM_V1 : public Thing
    {
    private:
        std::string ip_;
        std::string token_;
        MiotDevice miotDevice;

        std::map<std::string, SpecProperty> miotSpec = {
            {
                "vacuum:status",
                {2, 1, "status", "", kValueTypeNumber, Permission::READ, "setStatus", "Status"},
            },
            {
                "vacuum:speed-level",
                {2, 2, "speed-level", "", kValueTypeNumber, Permission::READ | Permission::WRITE, "setSpeedLevel", "Speed Level"},
            },
            {
                "battery:battery-level",
                {3, 1, "battery-level", "", kValueTypeNumber, Permission::READ, "setBatteryLevel", "Battery Level"},
            },

            {
                "battery:charging-state",
                {3, 2, "charging-state", "", kValueTypeNumber, Permission::READ, "setChargingState", "Charging State"},
            },

        };
        std::map<std::string, SpecAction> miotSpecAction = {

            {
                "vacuum:start-sweep",
                {2, 1, "StartSweep", "Start Sweep", {}},
            },

            {
                "vacuum:stop-sweeping",
                {2, 2, "StopSweeping", "Stop Sweeping", {}},
            },

            {
                "battery:start-charge",
                {3, 1, "StartCharge", "Start Charge", {}},
            },

        };

    public:
        ROCKROBO_VACUUM_V1() : Thing("米家扫地机器人", "")
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
            methods_.AddMethod("set_start", "开始打扫", ParameterList(), [this](const ParameterList &parameters)
                               {
                                   miotDevice.send("app_start", "[]"); //
                               });
            // methods_.AddMethod("set_spot", "开始局部打扫", ParameterList(), [this](const ParameterList &parameters)
            //                    {
            //                        miotDevice.send("app_spot", "[]"); //
            //                    });
            // methods_.AddMethod("set_stop", "暂停打扫", ParameterList(), [this](const ParameterList &parameters)
            //                    {
            //                        miotDevice.send("app_pause", "[]"); //
            //                    });
            methods_.AddMethod("set_pause", "暂停打扫", ParameterList(), [this](const ParameterList &parameters)
                               {
                                   miotDevice.send("app_pause", "[]"); //
                               });
            methods_.AddMethod("set_charge", "去充电|取消扫地|取消任务", ParameterList(), [this](const ParameterList &parameters)
                               {
                                   miotDevice.send("app_pause", "[]"); //
                                   vTaskDelay(500 / portTICK_PERIOD_MS);
                                   miotDevice.send("app_charge", "[]"); //
                               });
        }
    };

} // namespace iot

DECLARE_THING(ROCKROBO_VACUUM_V1);