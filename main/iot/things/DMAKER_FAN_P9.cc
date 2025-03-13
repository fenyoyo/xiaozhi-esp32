#include "iot/thing.h"
#include "iot/miot.h"
#include <esp_log.h>
#include "iot/miot_device.h"

#define TAG "DMAKER_FAN_P9"

namespace iot
{
    // 这里仅定义 Lamp 的属性和方法，不包含具体的实现
    class DMAKER_FAN_P9 : public Thing
    {
    private:
        std::string ip_;
        std::string token_;
        MiotDevice miotDevice;
        std::map<std::string, SpecProperty> miotSpec = {
            {
                "fan:on",
                {2, 1, "开关", "", kValueTypeBoolean, Permission::READ | Permission::WRITE, "setOn", "设备开关"},
            },
            {
                "fan:fan-leve",
                {2, 2, "风力:1-4档", "", kValueTypeNumber, Permission::READ | Permission::WRITE, "setFanLevel", "设置风力", "1-4档"},
            },
            {
                "fan:horizontal-swing",
                {2, 5, "是否开启扫风", "", kValueTypeBoolean, Permission::READ | Permission::WRITE, "setHorizontal", "设置是否开启扫风"},
            },
            {
                "fan:mode",
                {2, 4, "风扇模式:0=直吹模式,1=睡眠模式", "", kValueTypeNumber, Permission::READ | Permission::WRITE, "setMode", "设置风扇模式", "0=直吹模式,1=睡眠模式"},
            },
            {
                "fan:alarm",
                {2, 7, "提示音是否打开", "", kValueTypeBoolean, Permission::READ | Permission::WRITE, "setAlarm", "设置提示音是否打开"},
            },
            {
                "fan:off-delay-time",
                {2, 8, "定时关闭时间", "", kValueTypeNumber, Permission::READ | Permission::WRITE, "setOffDelayTime", "设置定时关闭时间", "0~480分钟"},
            },
            {
                "fan:brightness",
                {2, 9, "指示灯是否开启", "", kValueTypeBoolean, Permission::READ | Permission::WRITE, "setBrightness", "设置指示灯是否开启"},
            },
            {
                "physical-controls-locked:physical-controls-locked",
                {3, 1, "儿童锁", "", kValueTypeBoolean, Permission::READ | Permission::WRITE, "setPhysicalControlsLocked", "设置儿童锁"},
            },
        };

    public:
        DMAKER_FAN_P9() : Thing("米家直流变频塔扇", "")
        {
            Register();
        }
        int initMiot(const std::string &ip, const std::string &token, const std::string &name) override
        {
            ip_ = ip;
            token_ = token;
            set_description(name);
            miotDevice = MiotDevice(ip_, token_);
            return miotDevice.tryHandshake();
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

DECLARE_THING(DMAKER_FAN_P9);
