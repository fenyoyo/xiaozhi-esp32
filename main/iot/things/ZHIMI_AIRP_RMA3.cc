#include "iot/thing.h"
#include "iot/miot.h"
#include <esp_log.h>
#include "iot/miot_device.h"

#define TAG "ZHIMI_AIRP_RMA3"

namespace iot
{
    // 米家空气净化器 4 Lite
    // https://home.miot-spec.com/spec/zhimi.airp.rma3
    class ZHIMI_AIRP_RMA3 : public Thing
    {
    private:
        std::string ip_;
        std::string token_;
        MiotDevice miotDevice;

        std::map<std::string, SpecProperty> miotSpec = {
            {
                "air-purifier:on",
                {2, 1, "开关", "", kValueTypeBoolean, Permission::READ | Permission::WRITE, "setOn", "设备开关"},
            },
            {
                "air-purifier:mode",
                {2, 4, "模式:0=自动模式 1=睡眠模式 2=最爱模式", "", kValueTypeNumber, Permission::READ | Permission::WRITE, "setMode", "设置模式", "自动模式 1=睡眠模式 2=最爱模式"},
            },
            {
                "environment:relative-humidity",
                {3, 1, "空气湿度", "", kValueTypeNumber, Permission::READ},
            },
            {
                "environment:pm2.5-density",
                {3, 4, "pm2.5", "", kValueTypeNumber, Permission::READ},
            },
            {
                "environment:temperature",
                {3, 7, "温度", "", kValueTypeNumber, Permission::READ},
            },
            {
                "environment:air-quality",
                {3, 8, "空气质量:0=优秀 1=良好 2=中等 3=差 4=严重污染 5=危险", "", kValueTypeNumber, Permission::READ},
            },
            {
                "alarm:alarm",
                {6, 1, "提示音是否打开", "", kValueTypeBoolean, Permission::READ | Permission::WRITE, "setBrightness", "设置指示灯是否开启"},
            },
            {
                "screen:brightness",
                {7, 2, "屏幕亮度:0=息屏 1=微亮 2=正常", "", kValueTypeNumber, Permission::READ | Permission::WRITE, "setBrightness", "设置屏幕亮度", "0=息屏 1=微亮 2=正常"},
            },
            {
                "physical-controls-locked:physical-controls-locked",
                {8, 1, "儿童锁", "", kValueTypeBoolean, Permission::READ | Permission::WRITE, "setPhysicalControlsLocked", "设置儿童锁"},
            },
            {
                "air-purifier-favorite:fan-level",
                {11, 1, "风力:1-14档", "", kValueTypeNumber, Permission::READ | Permission::WRITE, "setFanLevel", "设置风力", "1-14档"},
            },
        };

    public:
        ZHIMI_AIRP_RMA3() : Thing("空气净化器", "")
        {
            Register();
        }

        int initMiot(const std::string &ip, const std::string &token, const std::string &name) override
        {
            ip_ = ip;
            token_ = token;
            set_description(name);
            miotDevice = MiotDevice(ip_, token_);
            // return miotDevice.tryHandshake();
            return 0;
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

DECLARE_THING(ZHIMI_AIRP_RMA3);
