#include "iot/thing.h"
#include "iot/miot.h"
#include "esp_log.h"
#include "iot/miot_device.h"

#define TAG "QJIANG_ACPARTNER_WB20"

namespace iot
{
    // ICX智能空调遥控器
    // https://home.miot-spec.com/spec/qjiang.acpartner.wb20
    class QJIANG_ACPARTNER_WB20 : public Thing
    {
    private:
        std::string ip_;
        std::string token_;
        MiotDevice miotDevice;

        std::map<std::string, SpecProperty> miotSpec = {

            {
                "air-conditioner:on",
                {3, 1, "电源|是否打开", "", kValueTypeBoolean, Permission::READ | Permission::WRITE, "setOn", "打开或者关闭空调伴侣"},
            },

            {
                "air-conditioner:mode",
                {3, 2, "空调伴侣模式:0=自动 1=送风 2=制冷 3=制热 4=除湿", "", kValueTypeNumber, Permission::READ | Permission::WRITE, "setMode", "设置模式:0=自动 1=送风 2=制冷 3=制热 4=除湿"},
            },

            // {
            //     "air-conditioner:fault",
            //     {3, 3, "fault", "", kValueTypeNumber, Permission::READ, "setFault", "Device Fault"},
            // },

            {
                "air-conditioner:target-temperature",
                {3, 4, "空调温度", "", kValueTypeNumber, Permission::READ | Permission::WRITE, "setTargetTemperature", "设置空调温度16-30度"},
            },

            {
                "fan-control:fan-level",
                {4, 2, "风速:0=自动风 1=低速风 2=中速风 3=高速风", "", kValueTypeNumber, Permission::READ | Permission::WRITE, "setFanLevel", "设置风速:0=自动风 1=低速风 2=中速风 3=高速风"},
            },

            {
                "fan-control:vertical-swing",
                {4, 4, "是否扫风", "", kValueTypeBoolean, Permission::READ | Permission::WRITE, "setVerticalSwing", "设置扫风"},
            },

            // {
            //     "indicator-light:on",
            //     {6, 1, "on", "", kValueTypeBoolean, Permission::READ | Permission::WRITE, "setOn", "Switch Status"},
            // },

            // {
            //     "indicator-light:on",
            //     {6, 2, "on", "", kValueTypeBoolean, Permission::READ | Permission::WRITE, "setOn", "Switch Status"},
            // },

            // {
            //     "indicator-light:brightness",
            //     {6, 3, "brightness", "", kValueTypeNumber, Permission::READ | Permission::WRITE, "setBrightness", "Brightness"},
            // },

            // {
            //     "air-condition-outlet-matching:match-state",
            //     {8, 1, "match-state", "", kValueTypeNumber, Permission::READ | Permission::WRITE, "setMatchState", "Match State"},
            // },

            // {
            //     "air-condition-outlet-matching:brand-id",
            //     {8, 4, "brand-id", "", kValueTypeNumber, Permission::READ, "setBrandId", "Brand Id"},
            // },

            // {
            //     "air-condition-outlet-matching:remote-id",
            //     {8, 5, "remote-id", "", kValueTypeNumber, Permission::READ, "setRemoteId", "Remote Id"},
            // },

            // {
            //     "air-condition-outlet-matching:ac-type",
            //     {8, 6, "ac-type", "", kValueTypeNumber, Permission::READ, "setAcType", "Ac Type"},
            // },

            // {
            //     "air-condition-outlet-matching:ac-ctrl-library",
            //     {8, 7, "ac-ctrl-library", "", kValueTypeString, Permission::NONE, "setAcCtrlLibrary", "Ac Ctrl Library"},
            // },

            // {
            //     "air-condition-outlet-matching:ac-ctrl-library",
            //     {8, 8, "ac-ctrl-library", "", kValueTypeString, Permission::NONE, "setAcCtrlLibrary", "Ac Ctrl Library"},
            // },

            // {
            //     "air-condition-outlet-matching:ac-ctrl-library",
            //     {8, 9, "ac-ctrl-library", "", kValueTypeString, Permission::NONE, "setAcCtrlLibrary", "Ac Ctrl Library"},
            // },

            // {
            //     "matching-action:lightonoff",
            //     {9, 1, "lightonoff", "", kValueTypeNumber, Permission::READ, "setLightonoff", "lightonoff"},
            // },

            // {
            //     "matching-action:specified-state",
            //     {9, 2, "specified-state", "", kValueTypeString, Permission::WRITE, "setSpecifiedState", ""},
            // },

        };
        std::map<std::string, SpecAction> miotSpecAction = {

            // {
            //     "air-conditioner:toggle",
            //     {3, 1, "Toggle", "Toggle", {}},
            // },

            // {
            //     "fan-control:horizontal-to-middle",
            //     {4, 1, "HorizontalToMiddle", "Horizontal To Middle", {}},
            // },

            // {
            //     "air-condition-outlet-matching:set-ac-model",
            //     {8, 1, "SetAcModel", "Set Ac Model", {SpecActionParam(4, "value4", "参数描述", kValueTypeNumber), SpecActionParam(5, "value5", "参数描述", kValueTypeNumber), SpecActionParam(6, "value6", "参数描述", kValueTypeNumber)}},
            // },

            // {
            //     "air-condition-outlet-matching:set-ac-ctrl-info",
            //     {8, 2, "SetAcCtrlInfo", "Set Ac Ctrl Info", {SpecActionParam(5, "value5", "参数描述", kValueTypeNumber), SpecActionParam(7, "value7", "参数描述", kValueTypeNumber), SpecActionParam(8, "value8", "参数描述", kValueTypeNumber), SpecActionParam(9, "value9", "参数描述", kValueTypeNumber)}},
            // },

            // {
            //     "matching-action:match-control",
            //     {9, 1, "MatchControl", "match-control", {}},
            // },

        };

    public:
        QJIANG_ACPARTNER_WB20() : Thing("ICX智能空调遥控器|空调伴侣", "")
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
                                               miotSpec.find(it->first)->second.value = value;
                                           }
                                           else if (it->second.type == kValueTypeNumber)
                                           {
                                               auto value = static_cast<int8_t>(parameters["value"].number());
                                               miotDevice.setProperty2(miotSpec, it->first, value, false);
                                               miotSpec.find(it->first)->second.value = value;
                                           }
                                           else
                                           {
                                               auto value = static_cast<int8_t>(parameters["value"].number());
                                               miotDevice.setProperty2(miotSpec, it->first, value, false);
                                               miotSpec.find(it->first)->second.value = value;

                                           } //
                                       } //
                    );
                }
            }

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
                                       miotDevice.callAction2(miotSpecAction, it->first, av); //
                                   });
            };
        }
    };

} // namespace iot

DECLARE_THING(QJIANG_ACPARTNER_WB20);