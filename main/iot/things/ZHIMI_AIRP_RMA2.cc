#include "iot/thing.h"
#include "iot/miot.h"
#include <esp_log.h>
#include "iot/protocol.h"
#include "application.h"
#include "iot/miot_device.h"
#include "board.h"

#define TAG "ZHIMI_AIRP_RMA2"

namespace iot
{
    // 米家空气净化器 4 Lite
    // https://home.miot-spec.com/spec/zhimi.airp.rma2
    class ZHIMI_AIRP_RMA2 : public Thing
    {
    private:
        bool id_ = 2;
        bool power_ = false;
        bool mode_ = 0;
        uint8_t fault = 100;
        uint8_t humidity_ = 0;
        uint16_t pm25_ = 0;
        double temperature_ = 0;
        uint8_t air_quality_ = 0;

        std::string ip_;
        std::string token_;
        uint32_t deviceID = 0;
        uint32_t deviceStamp = 0;
        MiotDevice miotDevice;
        // fault 滤芯
        // mode 模式 0自动 1睡眠 2喜爱
        std::map<std::string, SIID_PIID> miotSpec = {
            {"air-purifier:on", {2, 1, MOIT_PROPERTY_BOOL, 0, "开关"}},
            {"air-purifier:mode", {2, 4, MOIT_PROPERTY_INT, 0, "模式"}},
            {"environment:relative-humidity", {3, 1, MOIT_PROPERTY_INT, 0, "空气湿度"}},
            {"environment:pm2.5-density", {3, 4, MOIT_PROPERTY_INT, 0, "pm2.5"}},
            {"environment:temperature", {3, 7, MOIT_PROPERTY_INT, 0, "温度"}},
            {"environment:air-quality", {3, 8, MOIT_PROPERTY_INT, 0, "空气质量"}},
            {"screen:brightness", {7, 2, MOIT_PROPERTY_INT, 0, "屏幕亮度"}},
            {"air-purifier-favorite:fan-level", {11, 1, MOIT_PROPERTY_INT, 0, "最爱模式风力"}},
        };

    public:
        void initMiot(const std::string &ip, const std::string &token, const std::string &name) override
        {
            ip_ = ip;
            token_ = token;
            set_name(name);
            miotDevice = MiotDevice(ip_, token_);
        }

        ZHIMI_AIRP_RMA2() : Thing("ZHIMI_AIRP_RMA2", "空气净化器"), power_(false)
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
            methods_.AddMethod("SetMode", "切换电风扇吹风模式", ParameterList({Parameter("mode", "自动模式=0,睡眠模式=1,最爱模式=2", kValueTypeNumber, true)}), [this](const ParameterList &parameters)
                               {
                                   std::string key = "air-purifier:mode";
                                   auto value = static_cast<int8_t>(parameters["mode"].number());
                                   miotDevice.setProperty(miotSpec, key, value); //
                               });

            methods_.AddMethod("SetAlarm", "设置提示音", ParameterList({Parameter("alarm", "关闭或开启提示音", kValueTypeBoolean, true)}), [this](const ParameterList &parameters)
                               {
                                   std::string key = "alarm:alarm";
                                   auto value = static_cast<int8_t>(parameters["alarm"].boolean());
                                   miotDevice.setProperty(miotSpec, key, value, true); //
                               });

            methods_.AddMethod("SetBrightness", "设置屏幕亮度", ParameterList({Parameter("brightness", "0=关闭屏幕,1=正常亮度,2=最亮", kValueTypeNumber, true)}), [this](const ParameterList &parameters)
                               {
                                   auto value = static_cast<int8_t>(parameters["brightness"].number());
                                   miotDevice.setProperty(miotSpec, "screen:brightness", value); //
                               });

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

DECLARE_THING(ZHIMI_AIRP_RMA2);
