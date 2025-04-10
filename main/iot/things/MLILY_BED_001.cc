#include "iot/thing.h"
#include "iot/miot.h"
#include "esp_log.h"
#include "iot/miot_device.h"

#define TAG "MLILY_BED_001"

namespace iot
{
    // 梦百合0压智能床
    // https://home.miot-spec.com/spec/mlily.bed.001
    class MLILY_BED_001 : public Thing
    {
    private:
        std::string ip_;
        std::string token_;
        MiotDevice miotDevice;

        std::map<std::string, SpecProperty> miotSpec = {

            // {
            //     "bed:fault",
            //     {2, 1, "fault", "", kValueTypeNumber, Permission::READ, "setFault", "Device Fault"},
            // },

            {
                "bed:mode",
                {2, 4, "模式:0=零重力模式 1=阅读模式 2=电视模式 3=瑜伽模式 4=平躺模式", "", kValueTypeNumber, Permission::READ | Permission::WRITE, "setMode", "设置模式:0=零重力模式 1=阅读模式 2=电视模式 3=瑜伽模式 4=平躺模式"},
            },

            {
                "bed:lumbar-angle",
                {2, 6, "lumbar-angle", "", kValueTypeNumber, Permission::READ | Permission::WRITE, "setLumbarAngle", "Lumbar Angle"},
            },

            {
                "backrest-control:backrest-angle",
                {3, 1, "床角度", "", kValueTypeNumber, Permission::READ | Permission::WRITE, "setBackrestAngle", "设置智能床角度:0-60度"},
            },

            {
                "leg-rest-control:leg-rest-angle",
                {4, 1, "腿部角度", "", kValueTypeNumber, Permission::READ | Permission::WRITE, "setLegRestAngle", "设置智能床腿部角度:0-40度"},
            },

            // {
            //     "physical-controls-locked:physical-controls-locked",
            //     {5, 1, "physical-controls-locked", "", kValueTypeBoolean, Permission::READ | Permission::WRITE, "setPhysicalControlsLocked", "Physical Control Locked"},
            // },

            // {
            //     "light:on",
            //     {6, 1, "on", "", kValueTypeBoolean, Permission::READ | Permission::WRITE, "setOn", "Switch Status"},
            // },

            // {
            //     "massage:back-gear",
            //     {7, 1, "back-gear", "", kValueTypeNumber, Permission::READ | Permission::WRITE, "setBackGear", "back-gear"},
            // },

            // {
            //     "massage:leg-gear",
            //     {7, 2, "leg-gear", "", kValueTypeNumber, Permission::READ | Permission::WRITE, "setLegGear", ""},
            // },

            // {
            //     "massage:massage-mode",
            //     {7, 3, "massage-mode", "", kValueTypeNumber, Permission::READ | Permission::WRITE, "setMassageMode", ""},
            // },

            // {
            //     "massage:massage-countdown",
            //     {7, 4, "massage-countdown", "", kValueTypeNumber, Permission::READ | Permission::WRITE, "setMassageCountdown", "massage-countdown"},
            // },

            // {
            //     "music:on",
            //     {8, 1, "on", "", kValueTypeBoolean, Permission::READ | Permission::WRITE, "setOn", ""},
            // },

            // {
            //     "music:volume",
            //     {8, 2, "volume", "", kValueTypeNumber, Permission::READ | Permission::WRITE, "setVolume", "volume"},
            // },

            // {
            //     "music:gear",
            //     {8, 3, "gear", "", kValueTypeNumber, Permission::READ | Permission::WRITE, "setGear", ""},
            // },

            // {
            //     "music:option",
            //     {8, 4, "option", "", kValueTypeNumber, Permission::READ, "setOption", ""},
            // },

            // {
            //     "custom:snore-recognition",
            //     {9, 3, "snore-recognition", "", kValueTypeBoolean, Permission::READ | Permission::WRITE, "setSnoreRecognition", ""},
            // },

            // {
            //     "custom:clock",
            //     {9, 4, "clock", "", kValueTypeNumber, Permission::READ | Permission::WRITE, "setClock", ""},
            // },

            // {
            //     "custom:memory-save",
            //     {9, 5, "memory-save", "", kValueTypeNumber, Permission::WRITE, "setMemorySave", ""},
            // },

        };
        std::map<std::string, SpecAction> miotSpecAction = {

        };

    public:
        MLILY_BED_001() : Thing("梦百合0压智能床", "")
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

DECLARE_THING(MLILY_BED_001);