#include "iot/thing.h"
#include "iot/miot.h"
#include <esp_log.h>
#include "iot/protocol.h"
#include "application.h"
#include "iot/miot_device.h"
#include "board.h"

#define TAG "CUCO_PLUG_V3"

namespace iot
{
    // 米家智能插座3
    // https://home.miot-spec.com/spec/cuco.plug.v3
    // 只对接的开启电源状态
    class CUCO_PLUG_V3 : public Thing
    {
    private:
        std::string ip_;
        std::string token_;
        MiotDevice miotDevice;
        // fault 滤芯
        // mode 模式 0自动 1睡眠 2喜爱
        std::map<std::string, SIID_PIID> miotSpec = {
            {"switch:on", {2, 1, MOIT_PROPERTY_BOOL, 0, "是否开启:0=关闭,1=开启"}},
            {"switch:default-power-on-state", {2, 2, MOIT_PROPERTY_INT, 0, "默认电源状态"}},
            {"switch:fault", {2, 3, MOIT_PROPERTY_INT, 0, "设备状态:0=正常,1=温度过高,2=过载"}},
        };

        // std::map<std::string, SIID_AIID> miotAction = {
        //     {"light:toggle", {2, 1, 0, MOIT_PROPERTY_INT, 0, "切换开关"}},
        //     {"light:brightness-down", {2, 2, 0, MOIT_PROPERTY_INT, 0, "调低亮度"}},
        //     {"light:brightness-up", {2, 3, 0, MOIT_PROPERTY_INT, 0, "调高亮度"}},
        // };
        // TODO 把属性和方法分离出来

    public:
        void initMiot(const std::string &ip, const std::string &token, const std::string &name) override
        {
            ip_ = ip;
            token_ = token;
            set_name(name);
            miotDevice = MiotDevice(ip_, token_);
        }

        CUCO_PLUG_V3() : Thing("CUCO_PLUG_V3", "小米智能插座3")
        {
            properties_.AddBooleanProperty("properties", "获取设备所有状态", [this]() -> bool
                                           {
                                               auto spec = miotDevice.getProperties(miotSpec);
                                               if (spec.empty())
                                               {
                                                   return false;
                                               }
                                               for (auto it = spec.begin(); it != spec.end(); ++it)
                                               {
                                                   auto property = miotSpec.find(it->first);
                                                   property->second.value = it->second;
                                               }
                                               return true; //
                                           });

            for (auto it = miotSpec.begin(); it != miotSpec.end(); ++it)
            {
                switch (it->second.type)
                {
                case MOIT_PROPERTY_BOOL:
                    properties_.AddBooleanProperty(it->first, it->second.description, [this, it]() -> bool
                                                   { return miotSpec.find(it->first)->second.value; });
                    break;
                case MOIT_PROPERTY_INT:
                    properties_.AddNumberProperty(it->first, it->second.description, [this, it]() -> int
                                                  { return miotSpec.find(it->first)->second.value; });
                    break;
                case MOIT_PROPERTY_STRING:
                default:
                    properties_.AddStringProperty(it->first, it->second.description, [this, it]() -> std::string
                                                  { return std::to_string(miotSpec.find(it->first)->second.value); });
                    break;
                }
            }

            methods_.AddMethod("TurnOn", "开启", ParameterList(), [this](const ParameterList &parameters)
                               {
                                   std::string key = "switch:on";
                                   auto value = static_cast<int8_t>(parameters["mode"].number());
                                   miotDevice.setProperty(miotSpec, key, MOIT_ON, true); //
                               });
            methods_.AddMethod("TurnOff", "关闭", ParameterList(), [this](const ParameterList &parameters)
                               {
                                   std::string key = "switch:on";
                                   auto value = static_cast<int8_t>(parameters["mode"].number());
                                   miotDevice.setProperty(miotSpec, key, MOIT_OFF, true); //
                               });
        }
    };

} // namespace iot

DECLARE_THING(CUCO_PLUG_V3);
