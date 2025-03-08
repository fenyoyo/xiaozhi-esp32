#include "iot/thing.h"
#include "iot/miot.h"
#include <esp_log.h>
#include "iot/miot_device.h"

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

        // std::map<std::string, SIID_PIID> miotSpec = {
        //     {"air-purifier:on", {2, 1, MOIT_PROPERTY_BOOL, 0, "开关"}},
        //     {"air-purifier:mode", {2, 4, MOIT_PROPERTY_INT, 0, "模式: 0=自动 1=睡眠 2=喜爱 3=手动"}},
        //     {"air-purifier:fan-level", {2, 5, MOIT_PROPERTY_INT, 0, "模式: 1-3档"}},
        //     {"air-purifier:plasma", {2, 6, MOIT_PROPERTY_BOOL, 0, "开启等离子气体"}},
        //     {"air-purifier:uv", {2, 7, MOIT_PROPERTY_BOOL, 0, "开启紫外线杀毒"}},
        //     {"environment:relative-humidity", {3, 1, MOIT_PROPERTY_INT, 0, "空气湿度"}},
        //     {"environment:temperature", {3, 2, MOIT_PROPERTY_INT, 0, "温度"}},
        //     {"environment:air-quality", {3, 3, MOIT_PROPERTY_INT, 0, "空气质量"}},
        //     {"environment:pm2.5-density", {3, 4, MOIT_PROPERTY_INT, 0, "pm2.5"}},
        //     {"environment:pm10-density", {3, 5, MOIT_PROPERTY_INT, 0, "pm10"}},
        //     {"environment:hcho-density", {3, 6, MOIT_PROPERTY_INT, 0, "甲醛"}},
        //     {"environment:pm2.5", {3, 7, MOIT_PROPERTY_INT, 0, "pm2.5"}},

        //     {"alarm:alarm", {6, 1, MOIT_PROPERTY_BOOL, 0, "提示音"}},
        //     {"screen:brightness", {7, 2, MOIT_PROPERTY_INT, 0, "屏幕亮度"}},
        //     {"physical-controls-locked:physical-controls-locked", {8, 1, MOIT_PROPERTY_BOOL, 0, "儿童锁"}},
        //     {"air-purifier-favorite:fan-level", {1, 1, MOIT_PROPERTY_INT, 0, "最爱模式风力"}},
        // };

        std::map<std::string, SpecProperty> miotSpec = {
            {
                "air-purifier:on",
                {2, 1, "开关", "", kValueTypeBoolean, Permission::READ | Permission::WRITE, "setOn", "设备开关"},
            },
            {
                "air-purifier:mode",
                {2, 4, "模式:0=自动 1=睡眠 2=喜爱 3=手动", "", kValueTypeNumber, Permission::READ | Permission::WRITE, "setMode", "设置模式", "0=自动 1=睡眠 2=喜爱 3=手动"},
            },
            {
                "air-purifier:fan-level",
                {2, 5, "风力:1-3档", "", kValueTypeNumber, Permission::READ | Permission::WRITE, "setFanLevel", "设置风力", "1-3档"},
            },
            {
                "air-purifier:plasma",
                {2, 6, "杀菌模式", "", kValueTypeBoolean, Permission::READ | Permission::WRITE, "setPlasma", "杀菌模式"},
            },
            {
                "environment:relative-humidity",
                {3, 1, "空气湿度", "", kValueTypeNumber, Permission::READ},
            },
            {
                "environment:temperature",
                {3, 2, "温度", "", kValueTypeNumber, Permission::READ},
            },
            {
                "environment:air-quality",
                {3, 3, "空气质量:0=优秀 1=良好 2=中等 3=差 4=严重污染 5=危险", "", kValueTypeNumber, Permission::READ},
            },
            {
                "environment:pm2.5-density",
                {3, 4, "pm2.5浓度", "", kValueTypeNumber, Permission::READ},
            },
            {
                "environment:pm10-density",
                {3, 5, "pm10浓度", "", kValueTypeNumber, Permission::READ},
            },
            {
                "environment:pm10-density",
                {3, 6, "甲醛", "", kValueTypeNumber, Permission::READ},
            },
            {
                "environment:pm2.5",
                {3, 7, "pm2.5", "", kValueTypeNumber, Permission::READ},
            },
            {
                "alarm:alarm",
                {6, 1, "提示音是否打开", "", kValueTypeBoolean, Permission::READ | Permission::WRITE, "setBrightness", "设置指示灯是否开启"},
            },
            {
                "screen:brightness",
                {13, 2, "屏幕亮度是否打开", "", kValueTypeBoolean, Permission::READ | Permission::WRITE, "setBrightness", "设置屏幕"},
            },
            {
                "physical-controls-locked:physical-controls-locked",
                {8, 1, "儿童锁", "", kValueTypeBoolean, Permission::READ | Permission::WRITE, "setPhysicalControlsLocked", "设置儿童锁"},
            },
            {
                "air-purifier-favorite:fan-level",
                {14, 1, "喜爱模式风力:1-14档", "", kValueTypeNumber, Permission::READ | Permission::WRITE, "setFavoriteFanLevel", "设置风力", "1-14档"},
            },
        };

    public:
        ZHIMI_AIRP_UA1A() : Thing("空气净化器", "")
        {
            Register();
        }

        void initMiot(const std::string &ip, const std::string &token, const std::string &name) override
        {
            ip_ = ip;
            token_ = token;
            set_description(name);
            miotDevice = MiotDevice(ip_, token_);
        }

        void getProperties() override
        {
            auto spec = miotDevice.getProperties2(miotSpec);
            if (spec.empty())
            {
                return;
            }
            for (auto it = spec.begin(); it != spec.end(); ++it)
            {
                auto property = miotSpec.find(it->first);
                property->second.value = it->second;
            }
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
                                           }
                                           else if (it->second.type == kValueTypeNumber)
                                           {
                                               auto value = static_cast<int8_t>(parameters["value"].number());
                                               miotDevice.setProperty2(miotSpec, it->first, value, false);
                                           }
                                           else
                                           {
                                               auto value = static_cast<int8_t>(parameters["value"].number());
                                               miotDevice.setProperty2(miotSpec, it->first, value, false);

                                           } //
                                       } //
                    );
                }
            }
        }
    };

} // namespace iot

DECLARE_THING(ZHIMI_AIRP_UA1A);
