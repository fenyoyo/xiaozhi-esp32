#include "iot/thing.h"
#include "iot/miot.h"
#include "esp_log.h"
#include "iot/miot_device.h"

#define TAG "CUCO_ACPARTNER_CP6"

namespace iot
{
    // gosund | 电小酷 CP6
    // https://home.miot-spec.com/spec/cuco.acpartner.cp6
    class CUCO_ACPARTNER_CP6 : public Thing
    {
    private:
        std::string ip_;
        std::string token_;
        MiotDevice miotDevice;

        std::map<std::string, SpecProperty> miotSpec = {

            // {
            //     "air-condition-outlet:on",
            //     {2, 1, "on", "", kValueTypeBoolean, Permission::READ | Permission::WRITE, "setOn", "Switch Status"},
            // },

            {
                "air-conditioner:on",
                {3, 1, "电源", "", kValueTypeBoolean, Permission::READ | Permission::WRITE, "setOn", "打开或关闭电源"},
            },

            {
                "air-conditioner:mode",
                {3, 2, "空调模式", "", kValueTypeNumber, Permission::READ | Permission::WRITE, "setMode", "设置空调模式:0=制冷 1=制热 2=自动 3=送风 4=除湿"},
            },

            // {
            //     "air-conditioner:fault",
            //     {3, 3, "fault", "", kValueTypeNumber, Permission::READ, "setFault", "Device Fault"},
            // },

            {
                "air-conditioner:target-temperature",
                {3, 4, "空调温度", "", kValueTypeNumber, Permission::READ | Permission::WRITE, "setTargetTemperature", "设置空调温度"},
            },

            {
                "fan-control:on",
                {4, 1, "风速开关", "", kValueTypeBoolean, Permission::READ | Permission::WRITE, "setOn", "打开风速"},
            },

            {
                "fan-control:fan-level",
                {4, 2, "空调伴侣风速", "", kValueTypeNumber, Permission::READ | Permission::WRITE, "setFanLevel", "设置空调伴侣风速:0=自动 1=低 2=中 3=高"},
            },

            // {
            //     "power-consumption:power-consumption",
            //     {7, 1, "power-consumption", "", {{kValue}}, Permission::READ, "setPowerConsumption", "Power Consumption"},
            // },

            // {
            //     "power-consumption:electric-current",
            //     {7, 2, "electric-current", "", {{kValue}}, Permission::READ, "setElectricCurrent", "Electric Current"},
            // },

            // {
            //     "power-consumption:voltage",
            //     {7, 3, "voltage", "", {{kValue}}, Permission::READ, "setVoltage", "Voltage"},
            // },

            // {
            //     "power-consumption:electric-power",
            //     {7, 6, "electric-power", "", {{kValue}}, Permission::READ, "setElectricPower", "Electric Power"},
            // },

            // {
            //     "start-fast-cool:status",
            //     {8, 1, "status", "", kValueTypeBoolean, Permission::READ | Permission::WRITE, "setStatus", "status"},
            // },

            // {
            //     "start-fast-cool:keep-time",
            //     {8, 2, "keep-time", "", kValueTypeNumber, Permission::READ | Permission::WRITE, "setKeepTime", "keep-time"},
            // },

            // {
            //     "sleep-mode:status",
            //     {9, 1, "status", "", kValueTypeBoolean, Permission::READ | Permission::WRITE, "setStatus", "status"},
            // },

            // {
            //     "sleep-mode:model-info",
            //     {9, 2, "model-info", "", kValueTypeString, Permission::READ | Permission::WRITE, "setModelInfo", "model-info"},
            // },

            // {
            //     "indicator-light:status",
            //     {10, 1, "status", "", kValueTypeBoolean, Permission::READ | Permission::WRITE, "setStatus", "status"},
            // },

            // {
            //     "indicator-light:model",
            //     {10, 2, "model", "", kValueTypeBoolean, Permission::READ | Permission::WRITE, "setModel", "model"},
            // },

            // {
            //     "indicator-light:start-time",
            //     {10, 3, "start-time", "", kValueTypeNumber, Permission::READ | Permission::WRITE, "setStartTime", "start-time"},
            // },

            // {
            //     "indicator-light:end-time",
            //     {10, 4, "end-time", "", kValueTypeNumber, Permission::READ | Permission::WRITE, "setEndTime", "end-time"},
            // },

            // {
            //     "alert:overload",
            //     {11, 1, "overload", "", kValueTypeBoolean, Permission::READ, "setOverload", "overload"},
            // },

            // {
            //     "alert:temperature-high",
            //     {11, 2, "temperature-high", "", kValueTypeBoolean, Permission::READ, "setTemperatureHigh", "temperature-high"},
            // },

            // {
            //     "air-expend-info:is-insert-air",
            //     {12, 1, "is-insert-air", "", kValueTypeBoolean, Permission::READ | Permission::WRITE, "setIsInsertAir", "is-insert-air"},
            // },

            // {
            //     "air-expend-info:control-id",
            //     {12, 2, "control-id", "", kValueTypeNumber, Permission::READ | Permission::WRITE, "setControlId", "control-id"},
            // },

            // {
            //     "air-expend-info:model-temp-speed",
            //     {12, 3, "model-temp-speed", "", kValueTypeString, Permission::READ | Permission::WRITE, "setModelTempSpeed", "model-temp-speed"},
            // },

            // {
            //     "air-expend-info:led-status",
            //     {12, 4, "led-status", "", kValueTypeBoolean, Permission::READ | Permission::WRITE, "setLedStatus", "led-status"},
            // },

            // {
            //     "air-expend-info:support-key",
            //     {12, 5, "support-key", "", kValueTypeString, Permission::READ, "setSupportKey", "support-key"},
            // },

            // {
            //     "air-expend-info:brand-id",
            //     {12, 6, "brand-id", "", kValueTypeNumber, Permission::READ | Permission::WRITE, "setBrandId", "brand-id"},
            // },

        };
        std::map<std::string, SpecAction> miotSpecAction = {

            // {
            //     "air-expend-info:cozy-mode",
            //     {12, 1, "CozyMode", "cozy-mode", {}},
            // },

            // {
            //     "air-expend-info:temp-add",
            //     {12, 2, "TempAdd", "temp-add", {}},
            // },

            // {
            //     "air-expend-info:temp-less",
            //     {12, 3, "TempLess", "temp-less", {}},
            // },

        };

    public:
        CUCO_ACPARTNER_CP6() : Thing("电小酷空调伴侣", "")
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

DECLARE_THING(CUCO_ACPARTNER_CP6);