#include "iot/thing.h"
#include "iot/miot.h"
#include <esp_log.h>
#include "iot/protocol.h"
#include "application.h"
#include "iot/miot_device.h"
#include "board.h"

#define TAG "YEELINK_LIGHT_MONO6"

namespace iot
{
    // Mi Smart LED Bulb
    // https://home.miot-spec.com/spec/yeelink.light.mono6
    class YEELINK_LIGHT_MONO6 : public Thing
    {
    private:
        std::string ip_;
        std::string token_;
        MiotDevice miotDevice;
        // fault 滤芯
        // mode 模式 0自动 1睡眠 2喜爱
        std::map<std::string, SIID_PIID> miotSpec = {
            {"light:on", {2, 1, MOIT_PROPERTY_INT, 0, "是否开启:0=关闭,1=开启"}},
            {"light:brightness", {2, 3, MOIT_PROPERTY_INT, 0, "亮度百分比:1-100%"}},
            {"light:off-delay-time", {2, 7, MOIT_PROPERTY_INT, 0, "多少分钟后关闭"}},
        };

        std::map<std::string, SIID_AIID> miotAction = {
            {"light:toggle", {2, 1, 0, MOIT_PROPERTY_INT, 0, "切换开关"}},
            {"light:brightness-down", {2, 2, 0, MOIT_PROPERTY_INT, 0, "调低亮度"}},
            {"light:brightness-up", {2, 3, 0, MOIT_PROPERTY_INT, 0, "调高亮度"}},
        };

    public:
        void initMiot(const std::string &ip, const std::string &token, const std::string &name) override
        {
            ip_ = ip;
            token_ = token;
            set_name(name);
            miotDevice = MiotDevice(ip_, token_);
        }

        YEELINK_LIGHT_MONO6() : Thing("YEELINK_LIGHT_MONO6", "小米LED智能灯")
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

            methods_.AddMethod("TurnOn", "开灯", ParameterList(), [this](const ParameterList &parameters)
                               {
                                   std::string key = "light:on";
                                   auto value = static_cast<int8_t>(parameters["mode"].number());
                                   miotDevice.setProperty(miotSpec, key, MOIT_ON, true); //
                               });
            methods_.AddMethod("TurnOff", "关灯", ParameterList(), [this](const ParameterList &parameters)
                               {
                                   //    power_ = true;
                                   std::string key = "light:on";
                                   auto value = static_cast<int8_t>(parameters["mode"].number());
                                   miotDevice.setProperty(miotSpec, key, MOIT_OFF, true); //
                               });

            methods_.AddMethod("setBrightness", "设置亮度", ParameterList({Parameter("value", "1-100百分比", kValueTypeNumber, true)}), [this](const ParameterList &parameters)
                               {
                                   std::string key = "light:brightness";
                                   auto value = static_cast<int8_t>(parameters["value"].number());
                                   miotDevice.setProperty(miotSpec, "screen:brightness", value); //
                               });

            methods_.AddMethod("setOffDelayTime", "设置定制关闭", ParameterList({Parameter("value", "0-120分钟", kValueTypeNumber, true)}), [this](const ParameterList &parameters)
                               {
                                   std::string key = "light:off-delay-time";
                                   auto value = static_cast<int8_t>(parameters["value"].number());
                                   miotDevice.setProperty(miotSpec, key, value); //
                               });

            methods_.AddMethod("Toggle", "切换开关", ParameterList(), [this](const ParameterList &parameters)
                               {
                                   //    power_ = true;
                                   auto action = miotAction.find("light:toggle");
                                   std::string did = action->first;
                                   SIID_AIID sp = action->second;
                                   auto res = miotDevice.callAction(sp.siid, sp.piid);
                                   if (res.empty())
                                   {
                                       return;
                                   } });
        }
    };

} // namespace iot

DECLARE_THING(YEELINK_LIGHT_MONO6);
