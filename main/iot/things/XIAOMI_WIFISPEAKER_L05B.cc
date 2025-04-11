#include "iot/thing.h"
#include "iot/miot.h"
#include "esp_log.h"
#include "iot/miot_device.h"

#define TAG "XIAOMI_WIFISPEAKER_L05B"

namespace iot
{
    // 小爱音箱Play
    // https://home.miot-spec.com/spec/xiaomi.wifispeaker.l05b
    class XIAOMI_WIFISPEAKER_L05B : public Thing
    {
    private:
        std::string ip_;
        std::string token_;
        MiotDevice miotDevice;

        std::map<std::string, SpecProperty> miotSpec = {

            {
                "speaker:volume",
                {2, 1, "音量", "", kValueTypeNumber, Permission::READ | Permission::WRITE, "setVolume", "设置声音:0-100"},
            },

            {
                "speaker:mute",
                {2, 2, "静音", "", kValueTypeBoolean, Permission::READ | Permission::WRITE, "setMute", "设置静音"},
            },

            // {
            //     "play-control:playing-state",
            //     {3, 1, "playing-state", "", kValueTypeNumber, Permission::READ, "setPlayingState", "Playing State"},
            // },

            // {
            //     "play-control:seek-time",
            //     {3, 2, "seek-time", "", {{kValue}}, Permission::NONE, "setSeekTime", "Seek Time"},
            // },

            // {
            //     "play-control:play-loop-mode",
            //     {3, 3, "play-loop-mode", "", kValueTypeNumber, Permission::READ | Permission::WRITE, "setPlayLoopMode", "Play Loop Mode"},
            // },

            {
                "microphone:mute",
                {4, 1, "麦克风静音", "", kValueTypeBoolean, Permission::READ | Permission::WRITE, "setMute", "设置麦克风静音"},
            },

            // {
            //     "intelligent-speaker:text-content",
            //     {5, 1, "text-content", "", kValueTypeString, Permission::NONE, "setTextContent", "Text Content"},
            // },

            // {
            //     "intelligent-speaker:silent-execution",
            //     {5, 2, "silent-execution", "", kValueTypeBoolean, Permission::NONE, "setSilentExecution", "Silent Execution"},
            // },

            // {
            //     "intelligent-speaker:sleep-mode",
            //     {5, 3, "sleep-mode", "", kValueTypeBoolean, Permission::READ | Permission::WRITE, "setSleepMode", "Sleep Mode"},
            // },

            // {
            //     "intelligent-speaker:audio-id",
            //     {5, 4, "audio-id", "", kValueTypeString, Permission::READ, "setAudioId", "Audio Id"},
            // },

            {
                "clock:on",
                {6, 1, "on", "", kValueTypeBoolean, Permission::READ | Permission::WRITE, "setOn", "Switch Status"},
            },

            {
                "clock:status",
                {6, 2, "status", "", kValueTypeNumber, Permission::READ, "setStatus", "Status"},
            },

            {
                "clock:fault",
                {6, 3, "fault", "", kValueTypeNumber, Permission::READ, "setFault", "Device Fault"},
            },

            {
                "clock:ringtone",
                {6, 4, "ringtone", "", kValueTypeNumber, Permission::READ | Permission::WRITE, "setRingtone", "Ringtone"},
            },

            {
                "no-disturb:no-disturb",
                {7, 1, "no-disturb", "", kValueTypeBoolean, Permission::READ | Permission::WRITE, "setNoDisturb", "No Disturb"},
            },

            {
                "no-disturb:enable-time-period",
                {7, 2, "enable-time-period", "", kValueTypeString, Permission::READ | Permission::WRITE, "setEnableTimePeriod", "Enable Time Period"},
            },

        };
        std::map<std::string, SpecAction> miotSpecAction = {

            // {
            //     "play-control:seek",
            //     {3, 1, "Seek", "Seek", {SpecActionParam(2, "value2", "参数描述", kValueTypeNumber)}},
            // },

            {
                "play-control:play",
                {3, 2, "Play", "播放", {}},
            },

            {
                "play-control:pause",
                {3, 3, "Pause", "暂停", {}},
            },

            {
                "play-control:stop",
                {3, 4, "Stop", "停止", {}},
            },

            {
                "play-control:previous",
                {3, 5, "Previous", "上一首", {}},
            },

            {
                "play-control:next",
                {3, 6, "Next", "下一首", {}},
            },

            {
                "intelligent-speaker:wake-up",
                {5, 1, "WakeUp", "唤醒", {}},
            },

            {
                "intelligent-speaker:play-radio",
                {5, 2, "PlayRadio", "播放收音机", {}},
            },

            {
                "intelligent-speaker:play-text",
                {5, 3, "PlayText", "播放文字", {SpecActionParam(1, "value1", "被播放的文字", kValueTypeNumber)}},
            },

            {
                "intelligent-speaker:execute-text-directive",
                {5, 4, "ExecuteTextDirective", "执行文本命令", {SpecActionParam(1, "value1", "文本命令", kValueTypeNumber), SpecActionParam(2, "value2", "参数描述", kValueTypeNumber)}},
            },

            {
                "intelligent-speaker:play-music",
                {5, 5, "PlayMusic", "播放音乐", {}},
            },

            // {
            //     "clock:stop-alarm",
            //     {6, 1, "StopAlarm", "Stop Alarm", {}},
            // },

        };

    public:
        XIAOMI_WIFISPEAKER_L05B() : Thing("小爱音箱Play", "")
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

DECLARE_THING(XIAOMI_WIFISPEAKER_L05B);