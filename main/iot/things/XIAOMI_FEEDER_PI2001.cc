#include "iot/thing.h"
#include "iot/miot.h"
#include <esp_log.h>
#include "iot/miot_device.h"

#define TAG "XIAOMI_FEEDER_PI2001"

namespace iot
{
    // 米家智能宠物喂食器2
    // https://home.miot-spec.com/spec/xiaomi.feeder.pi2001
    class XIAOMI_FEEDER_PI2001 : public Thing
    {
    private:
        std::string ip_;
        std::string token_;
        MiotDevice miotDevice;

        std::map<std::string, SpecProperty> miotSpec = {
            {
                "pet-feeder:target-feeding-measure",
                {2, 7, "target-feeding-measure", "", kValueTypeNumber, Permission::READ | Permission::WRITE, "setTargetFeedingMeasure", "Target Feeding Measure"},
            },
        };
        std::map<std::string, SpecAction> miotSpecAction = {
            {
                "pet-feeder:pet-food-out",
                {2, 1, "PetFoodOut", "Pet Food Out", {SpecActionParam(8, "value8", "参数描述", kValueTypeNumber)}},
            },
        };

    public:
        XIAOMI_FEEDER_PI2001() : Thing("米家智能宠物喂食器2", "")
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

DECLARE_THING(XIAOMI_FEEDER_PI2001);