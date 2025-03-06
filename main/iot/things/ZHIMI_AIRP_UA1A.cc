#include "iot/thing.h"
#include "iot/miot.h"
#include <esp_log.h>
#include "iot/protocol.h"
#include "application.h"
#include "iot/miot_device.h"
#include "board.h"

#define TAG "ZHIMI_AIRP_UA1A"

namespace iot
{
    // 米家全效空气净化器 Ultra
    // https://home.miot-spec.com/spec/zhimi.airp.ua1a
    class ZHIMI_AIRP_UA1A : public Thing
    {
    private:
        std::string ip_;
        std::string token_;
        MiotDevice miotDevice;
        // fault 滤芯
        // mode 模式 0自动 1睡眠 2喜爱
        std::map<std::string, SIID_PIID> miotSpec = {
            {"air-purifier:on", {2, 1, MOIT_PROPERTY_BOOL, 0, "开关"}},
            {"air-purifier:mode", {2, 4, MOIT_PROPERTY_INT, 0, "模式: 0=自动 1=睡眠 2=喜爱 3=手动"}},
            {"air-purifier:fan-level", {2, 5, MOIT_PROPERTY_INT, 0, "模式: 1-3档"}},
            {"air-purifier:plasma", {2, 6, MOIT_PROPERTY_BOOL, 0, "开启等离子气体"}},
            {"air-purifier:uv", {2, 7, MOIT_PROPERTY_BOOL, 0, "开启紫外线杀毒"}},
            {"environment:relative-humidity", {3, 1, MOIT_PROPERTY_INT, 0, "空气湿度"}},
            {"environment:temperature", {3, 2, MOIT_PROPERTY_INT, 0, "温度"}},
            {"environment:air-quality", {3, 3, MOIT_PROPERTY_INT, 0, "空气质量"}},
            {"environment:pm2.5-density", {3, 4, MOIT_PROPERTY_INT, 0, "pm2.5"}},
            {"environment:pm10-density", {3, 5, MOIT_PROPERTY_INT, 0, "pm10"}},
            {"environment:hcho-density", {3, 6, MOIT_PROPERTY_INT, 0, "甲醛"}},
            {"environment:pm2.5", {3, 7, MOIT_PROPERTY_INT, 0, "pm2.5"}},

            {"alarm:alarm", {6, 1, MOIT_PROPERTY_BOOL, 0, "提示音"}},
            {"screen:brightness", {7, 2, MOIT_PROPERTY_INT, 0, "屏幕亮度"}},
            {"physical-controls-locked:physical-controls-locked", {8, 1, MOIT_PROPERTY_BOOL, 0, "儿童锁"}},
            {"air-purifier-favorite:fan-level", {1, 1, MOIT_PROPERTY_INT, 0, "最爱模式风力"}},
        };

    public:
        void initMiot(const std::string &ip, const std::string &token, const std::string &name) override
        {
            ip_ = ip;
            token_ = token;
            set_name(name);
            miotDevice = MiotDevice(ip_, token_);
        }

        ZHIMI_AIRP_UA1A() : Thing("ZHIMI_AIRP_UA1A", "空气净化器")
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

            methods_.AddMethod("TurnOn", "打开空气净化器", ParameterList(), [this](const ParameterList &parameters)
                               { miotDevice.setProperty(miotSpec, "air-purifier:on", MOIT_ON, true); });
            methods_.AddMethod("TurnOff", "关闭空气净化器", ParameterList(), [this](const ParameterList &parameters)
                               { miotDevice.setProperty(miotSpec, "air-purifier:on", MOIT_OFF, true); });

            methods_.AddMethod("SetMode", "切换电风扇吹风模式", ParameterList({Parameter("mode", "模式: 0=自动 1=睡眠 2=喜爱 3=手动", kValueTypeNumber, true)}), [this](const ParameterList &parameters)
                               {
                                   std::string key = "air-purifier:mode";
                                   auto value = static_cast<int8_t>(parameters["mode"].number());
                                   miotDevice.setProperty(miotSpec, key, value); //
                               });
            methods_.AddMethod("SetPlasma", "等离子", ParameterList({Parameter("value", "关闭或开启", kValueTypeBoolean, true)}), [this](const ParameterList &parameters)
                               {
                                   std::string key = "air-purifier:plasma";
                                   auto value = static_cast<int8_t>(parameters["value"].boolean());
                                   miotDevice.setProperty(miotSpec, key, value, true);
                                   //
                               });
            methods_.AddMethod("SetUv", "紫外线杀菌", ParameterList({Parameter("value", "关闭或开启", kValueTypeBoolean, true)}), [this](const ParameterList &parameters)
                               {
                                   std::string key = "air-purifier:uv";
                                   auto value = static_cast<int8_t>(parameters["value"].boolean());
                                   miotDevice.setProperty(miotSpec, key, value, true);
                                   //
                               });
            methods_.AddMethod("SetAlarm", "设置提示音", ParameterList({Parameter("alarm", "关闭或开启提示音", kValueTypeBoolean, true)}), [this](const ParameterList &parameters)
                               {
                                   auto value = static_cast<int8_t>(parameters["alarm"].boolean());
                                   miotDevice.setProperty(miotSpec, "alarm:alarm", value,true); });

            methods_.AddMethod("SetBrightness", "设置屏幕亮度", ParameterList({Parameter("brightness", "0=关闭屏幕,1=正常亮度,2=最亮", kValueTypeNumber, true)}), [this](const ParameterList &parameters)
                               {
                                   auto value = static_cast<int8_t>(parameters["brightness"].number());
                                   miotDevice.setProperty(miotSpec, "screen:brightness", value); });

            methods_.AddMethod("SetLocked", "设置儿童锁", ParameterList({Parameter("locked", "关闭或开启儿童锁", kValueTypeBoolean, true)}), [this](const ParameterList &parameters)
                               {
                                   auto value = static_cast<int8_t>(parameters["locked"].boolean());
                                   miotDevice.setProperty(miotSpec, "physical-controls-locked:physical-controls-locked", value,true); });
            methods_.AddMethod("Setfavorite", "设置最爱模式风力", ParameterList({Parameter("level", "0-14级风力", kValueTypeNumber, true)}), [this](const ParameterList &parameters)
                               {
                                   auto value = static_cast<int8_t>(parameters["level"].number());
                                   miotDevice.setProperty(miotSpec, "air-purifier-favorite:fan-level", value); });
        }
    };

} // namespace iot

DECLARE_THING(ZHIMI_AIRP_UA1A);
