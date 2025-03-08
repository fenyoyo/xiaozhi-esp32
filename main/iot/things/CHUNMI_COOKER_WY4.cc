#include "iot/thing.h"
#include "iot/miot.h"
#include <esp_log.h>
#include "iot/miot_device.h"

#define TAG "CHUNMI_COOKER_WY4"

namespace iot
{
    // 米家智能电饭煲 4L
    // https://home.miot-spec.com/spec/chunmi.cooker.wy4
    class CHUNMI_COOKER_WY4 : public Thing
    {
    private:
        std::string ip_;
        std::string token_;
        MiotDevice miotDevice;

        std::map<std::string, SpecProperty> miotSpec = {
            {
                "cooker:status",
                {2, 1, "工作状态:1=待机 2=工作中 3=任务工作中 4=保暖 5=错误 6=更新中 7=烹煮完成", "", kValueTypeNumber, Permission::READ},
            },
            {
                "cooker:cook-mode",
                {2, 5, "模式:1=精细煮 2=快速煮 3=煮粥 4=保暖 5=定制", "", kValueTypeNumber, Permission::READ},
            }

        };
        // , "1=精细煮,2=快速煮,3=煮粥,4=保暖"
        std::map<std::string, SpecAction> miotSpecAction = {
            {
                "cooker:start-cook",
                {2, 1, "setStartCook", "模式", {SpecActionParam(5, "mode", "1=精细煮,2=快速煮,3=煮粥,4=保暖", kValueTypeNumber)}},
            },
            {
                "cooker:cancel-cooking",
                {2, 2, "setCancelCook", "取消"},
            }

        };

        // std::map<std::string, SIID_AIID> miotAction = {
        //     {"cooker:start-cook", {2, 1, 5, MOIT_PROPERTY_INT, 0, "开始煮"}},
        //     {"cooker:cancel-cooking", {2, 2, 0, MOIT_PROPERTY_INT, 0, "取消"}},
        // };

    public:
    CHUNMI_COOKER_WY4() : Thing("米家智能小饭煲", "")
        {
            Register();

            for (auto it = miotSpecAction.begin(); it != miotSpecAction.end(); ++it)
            {
                ParameterList parameterList;
                for (auto &&i : it->second.parameters)
                {
                    parameterList.AddParameter(Parameter(i.key, i.parameter_description, i.type, true));
                }
                methods_.AddMethod(it->second.method_name, it->second.method_description, parameterList, [this, it](const ParameterList &parameters)
                                   {
                                       std::map<uint8_t, int> av;
                                       // std::map<uint_8 piid,uint_8 value> value;
                                       for (auto &&i : it->second.parameters)
                                       {
                                           auto value = static_cast<int8_t>(parameters[i.key].number());
                                           av.insert({i.piid, value});
                                       }
                                       miotDevice.callAction2(miotSpecAction, it->first, av);
                                   });
            };
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

DECLARE_THING(CHUNMI_COOKER_WY4);
