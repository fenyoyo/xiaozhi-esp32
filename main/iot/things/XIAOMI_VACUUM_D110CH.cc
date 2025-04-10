#include "iot/thing.h"
#include "iot/miot.h"
#include "esp_log.h"
#include "iot/miot_device.h"

#define TAG "XIAOMI_VACUUM_D110CH"

namespace iot
{
    // 米家扫拖机器人 M40
    // https://home.miot-spec.com/spec/xiaomi.vacuum.d110ch
    class XIAOMI_VACUUM_D110CH : public Thing
    {
    private:
        std::string ip_;
        std::string token_;
        MiotDevice miotDevice;

        std::map<std::string, SpecProperty> miotSpec = {

            // {
            //     "vacuum:status",
            //     {2, 1, "status", "", kValueTypeNumber, Permission::READ, "setStatus", "Status"},
            // },

            // {
            //     "vacuum:fault",
            //     {2, 2, "fault", "", kValueTypeNumber, Permission::READ, "setFault", "Device Fault"},
            // },

            {
                "vacuum:mode",
                {2, 3, "扫地机器人模式", "", kValueTypeNumber, Permission::READ | Permission::WRITE, "setMode", "扫地机器人模式:0=安静模式,1=标准模式,2=强力模式,3=全速模式"},
            },

            // {
            //     "vacuum:room-ids",
            //     {2, 4, "room-ids", "", kValueTypeString, Permission::NONE, "setRoomIds", "Room IDs"},
            // },

            // {
            //     "vacuum:dry-left-time",
            //     {2, 5, "dry-left-time", "", kValueTypeNumber, Permission::READ, "setDryLeftTime", "Dry Left Time"},
            // },

            // {
            //     "vacuum:sweep-mop-type",
            //     {2, 6, "sweep-mop-type", "", kValueTypeNumber, Permission::READ | Permission::WRITE, "setSweepMopType", "Sweep Mop Type"},
            // },

            // {
            //     "vacuum:vacuum-room-ids",
            //     {2, 7, "vacuum-room-ids", "", kValueTypeString, Permission::NONE, "setVacuumRoomIds", "Vacuum Room IDs"},
            // },

            // {
            //     "vacuum:mop-water-output-level",
            //     {2, 8, "mop-water-output-level", "", kValueTypeNumber, Permission::READ | Permission::WRITE, "setMopWaterOutputLevel", "Mop Water Output Level"},
            // },

            // {
            //     "vacuum:common-params",
            //     {2, 9, "common-params", "", kValueTypeString, Permission::NONE, "setCommonParams", "Common Params"},
            // },

            {
                "battery:battery-level",
                {3, 1, "扫地机器人剩余电量", "", kValueTypeNumber, Permission::READ, "setBatteryLevel", "Battery Level"},
            },

            // {
            //     "battery:charging-state",
            //     {3, 2, "charging-state", "", kValueTypeNumber, Permission::READ, "setChargingState", "Charging State"},
            // },

            // {
            //     "vacuum-extend:work-mode",
            //     {4, 1, "work-mode", "", kValueTypeNumber, Permission::READ, "setWorkMode", ""},
            // },

            // {
            //     "vacuum-extend:cleaning-time",
            //     {4, 2, "cleaning-time", "", kValueTypeNumber, Permission::READ, "setCleaningTime", ""},
            // },

            // {
            //     "vacuum-extend:cleaning-area",
            //     {4, 3, "cleaning-area", "", kValueTypeNumber, Permission::READ, "setCleaningArea", ""},
            // },

            // {
            //     "vacuum-extend:cleaning-mode",
            //     {4, 4, "cleaning-mode", "", {{kValue}}, Permission::READ, "setCleaningMode", ""},
            // },

            // {
            //     "vacuum-extend:mop-mode",
            //     {4, 5, "mop-mode", "", {{kValue}}, Permission::READ | Permission::WRITE, "setMopMode", ""},
            // },

            // {
            //     "vacuum-extend:waterbox-status",
            //     {4, 6, "waterbox-status", "", {{kValue}}, Permission::READ, "setWaterboxStatus", ""},
            // },

            // {
            //     "vacuum-extend:task-status",
            //     {4, 7, "task-status", "", kValueTypeNumber, Permission::READ, "setTaskStatus", ""},
            // },

            // {
            //     "vacuum-extend:clean-start-time",
            //     {4, 8, "clean-start-time", "", kValueTypeString, Permission::NONE, "setCleanStartTime", ""},
            // },

            // {
            //     "vacuum-extend:clean-log-filename",
            //     {4, 9, "clean-log-filename", "", kValueTypeString, Permission::NONE, "setCleanLogFilename", ""},
            // },

            // {
            //     "vacuum-extend:clean-extend-data",
            //     {4, 10, "clean-extend-data", "", kValueTypeString, Permission::WRITE, "setCleanExtendData", ""},
            // },

            // {
            //     "vacuum-extend:break-point-restart",
            //     {4, 11, "break-point-restart", "", kValueTypeNumber, Permission::READ | Permission::WRITE, "setBreakPointRestart", ""},
            // },

            // {
            //     "vacuum-extend:carpet-press",
            //     {4, 12, "carpet-press", "", kValueTypeNumber, Permission::READ | Permission::WRITE, "setCarpetPress", ""},
            // },

            // {
            //     "vacuum-extend:cleanlog-status",
            //     {4, 13, "cleanlog-status", "", kValueTypeNumber, Permission::NONE, "setCleanlogStatus", ""},
            // },

            // {
            //     "vacuum-extend:serial-number",
            //     {4, 14, "serial-number", "", kValueTypeString, Permission::READ, "setSerialNumber", ""},
            // },

            // {
            //     "vacuum-extend:remote-state",
            //     {4, 15, "remote-state", "", kValueTypeString, Permission::WRITE, "setRemoteState", ""},
            // },

            // {
            //     "vacuum-extend:clean-rags-tip",
            //     {4, 16, "clean-rags-tip", "", kValueTypeNumber, Permission::READ | Permission::WRITE, "setCleanRagsTip", ""},
            // },

            // {
            //     "vacuum-extend:keep-sweeper-time",
            //     {4, 17, "keep-sweeper-time", "", {{kValue}}, Permission::READ, "setKeepSweeperTime", ""},
            // },

            // {
            //     "vacuum-extend:faults",
            //     {4, 18, "faults", "", kValueTypeString, Permission::READ, "setFaults", ""},
            // },

            // {
            //     "vacuum-extend:nation-matched",
            //     {4, 19, "nation-matched", "", kValueTypeString, Permission::READ, "setNationMatched", ""},
            // },

            // {
            //     "vacuum-extend:relocation-status",
            //     {4, 20, "relocation-status", "", {{kValue}}, Permission::READ, "setRelocationStatus", ""},
            // },

            // {
            //     "vacuum-extend:laser-switch",
            //     {4, 21, "laser-switch", "", kValueTypeNumber, Permission::NONE, "setLaserSwitch", ""},
            // },

            // {
            //     "vacuum-extend:camera-switch",
            //     {4, 22, "camera-switch", "", kValueTypeNumber, Permission::NONE, "setCameraSwitch", ""},
            // },

            // {
            //     "vacuum-extend:clean-setting",
            //     {4, 23, "clean-setting", "", kValueTypeNumber, Permission::NONE, "setCleanSetting", ""},
            // },

            // {
            //     "vacuum-extend:stop-map",
            //     {4, 24, "stop-map", "", kValueTypeNumber, Permission::NONE, "setStopMap", ""},
            // },

            // {
            //     "vacuum-extend:wash-station",
            //     {4, 25, "wash-station", "", kValueTypeNumber, Permission::READ, "setWashStation", ""},
            // },

            // {
            //     "vacuum-extend:custom-enable",
            //     {4, 26, "custom-enable", "", kValueTypeNumber, Permission::NONE, "setCustomEnable", ""},
            // },

            // {
            //     "vacuum-extend:child-lock",
            //     {4, 27, "child-lock", "", kValueTypeNumber, Permission::READ | Permission::WRITE, "setChildLock", ""},
            // },

            // {
            //     "vacuum-extend:sensitivity",
            //     {4, 28, "sensitivity", "", kValueTypeNumber, Permission::NONE, "setSensitivity", ""},
            // },

            // {
            //     "vacuum-extend:mop-way",
            //     {4, 29, "mop-way", "", kValueTypeNumber, Permission::NONE, "setMopWay", ""},
            // },

            // {
            //     "vacuum-extend:clean-cancel",
            //     {4, 30, "clean-cancel", "", kValueTypeNumber, Permission::READ, "setCleanCancel", ""},
            // },

            // {
            //     "vacuum-extend:y-clean",
            //     {4, 31, "y-clean", "", kValueTypeNumber, Permission::NONE, "setYClean", ""},
            // },

            // {
            //     "vacuum-extend:electric-water",
            //     {4, 32, "electric-water", "", kValueTypeNumber, Permission::NONE, "setElectricWater", ""},
            // },

            // {
            //     "vacuum-extend:carpet-recognition",
            //     {4, 33, "carpet-recognition", "", kValueTypeNumber, Permission::NONE, "setCarpetRecognition", ""},
            // },

            // {
            //     "vacuum-extend:smart-wash-switch",
            //     {4, 34, "smart-wash-switch", "", kValueTypeNumber, Permission::NONE, "setSmartWashSwitch", ""},
            // },

            // {
            //     "vacuum-extend:warn-status",
            //     {4, 35, "warn-status", "", kValueTypeNumber, Permission::NONE, "setWarnStatus", ""},
            // },

            // {
            //     "vacuum-extend:carpet-excape",
            //     {4, 36, "carpet-excape", "", kValueTypeNumber, Permission::NONE, "setCarpetExcape", ""},
            // },

            // {
            //     "vacuum-extend:clean-fluid-switch",
            //     {4, 37, "clean-fluid-switch", "", kValueTypeNumber, Permission::NONE, "setCleanFluidSwitch", ""},
            // },

            // {
            //     "vacuum-extend:capability",
            //     {4, 38, "capability", "", kValueTypeNumber, Permission::NONE, "setCapability", ""},
            // },

            // {
            //     "vacuum-extend:save-water-tips",
            //     {4, 39, "save-water-tips", "", kValueTypeNumber, Permission::NONE, "setSaveWaterTips", ""},
            // },

            // {
            //     "vacuum-extend:drying-time",
            //     {4, 40, "drying-time", "", kValueTypeNumber, Permission::NONE, "setDryingTime", ""},
            // },

            // {
            //     "vacuum-extend:no-water-info",
            //     {4, 41, "no-water-info", "", kValueTypeNumber, Permission::NONE, "setNoWaterInfo", ""},
            // },

            // {
            //     "vacuum-extend:cruise-map-id",
            //     {4, 42, "cruise-map-id", "", kValueTypeNumber, Permission::NONE, "setCruiseMapId", ""},
            // },

            // {
            //     "vacuum-extend:cruise-map-name",
            //     {4, 43, "cruise-map-name", "", kValueTypeString, Permission::NONE, "setCruiseMapName", ""},
            // },

            // {
            //     "vacuum-extend:cruise-task-type",
            //     {4, 44, "cruise-task-type", "", kValueTypeNumber, Permission::NONE, "setCruiseTaskType", ""},
            // },

            // {
            //     "vacuum-extend:auto-install-mop",
            //     {4, 45, "auto-install-mop", "", kValueTypeNumber, Permission::NONE, "setAutoInstallMop", ""},
            // },

            // {
            //     "vacuum-extend:mop-wash-mode",
            //     {4, 46, "mop-wash-mode", "", kValueTypeNumber, Permission::NONE, "setMopWashMode", ""},
            // },

            // {
            //     "vacuum-extend:clean-disable-mode",
            //     {4, 47, "clean-disable-mode", "", kValueTypeNumber, Permission::NONE, "setCleanDisableMode", ""},
            // },

            // {
            //     "vacuum-extend:clean-quick",
            //     {4, 48, "clean-quick", "", kValueTypeNumber, Permission::NONE, "setCleanQuick", ""},
            // },

            // {
            //     "vacuum-extend:reloc-enable",
            //     {4, 49, "reloc-enable", "", kValueTypeNumber, Permission::NONE, "setRelocEnable", ""},
            // },

            // {
            //     "vacuum-extend:clean-switch-set",
            //     {4, 50, "clean-switch-set", "", kValueTypeString, Permission::NONE, "setCleanSwitchSet", ""},
            // },

            // {
            //     "vacuum-extend:auto-water",
            //     {4, 51, "auto-water", "", kValueTypeNumber, Permission::NONE, "setAutoWater", ""},
            // },

            // {
            //     "vacuum-extend:mop-in-station",
            //     {4, 52, "mop-in-station", "", kValueTypeNumber, Permission::NONE, "setMopInStation", ""},
            // },

            // {
            //     "vacuum-extend:mop-pad-installed",
            //     {4, 53, "mop-pad-installed", "", kValueTypeNumber, Permission::NONE, "setMopPadInstalled", ""},
            // },

            // {
            //     "vacuum-extend:water-self-check",
            //     {4, 54, "water-self-check", "", kValueTypeNumber, Permission::NONE, "setWaterSelfCheck", ""},
            // },

            // {
            //     "vacuum-extend:dry-stop-remind",
            //     {4, 55, "dry-stop-remind", "", kValueTypeNumber, Permission::NONE, "setDryStopRemind", ""},
            // },

            // {
            //     "vacuum-extend:int-message-prompt",
            //     {4, 56, "int-message-prompt", "", kValueTypeNumber, Permission::NONE, "setIntMessagePrompt", ""},
            // },

            // {
            //     "do-not-disturb:enable",
            //     {5, 1, "enable", "", kValueTypeBoolean, Permission::READ | Permission::WRITE, "setEnable", ""},
            // },

            // {
            //     "do-not-disturb:start-time",
            //     {5, 2, "start-time", "", kValueTypeString, Permission::READ | Permission::WRITE, "setStartTime", ""},
            // },

            // {
            //     "do-not-disturb:end-time",
            //     {5, 3, "end-time", "", kValueTypeString, Permission::READ | Permission::WRITE, "setEndTime", ""},
            // },

            // {
            //     "map:map-data",
            //     {6, 1, "map-data", "", kValueTypeString, Permission::NONE, "setMapData", ""},
            // },

            // {
            //     "map:frame-info",
            //     {6, 2, "frame-info", "", kValueTypeString, Permission::NONE, "setFrameInfo", ""},
            // },

            // {
            //     "map:object-name",
            //     {6, 3, "object-name", "", kValueTypeString, Permission::NONE, "setObjectName", ""},
            // },

            // {
            //     "map:map-extend-data",
            //     {6, 4, "map-extend-data", "", kValueTypeString, Permission::NONE, "setMapExtendData", ""},
            // },

            // {
            //     "map:robot-time",
            //     {6, 5, "robot-time", "", {{kValue}}, Permission::NONE, "setRobotTime", ""},
            // },

            // {
            //     "map:result-code",
            //     {6, 6, "result-code", "", {{kValue}}, Permission::NONE, "setResultCode", ""},
            // },

            // {
            //     "audio:volume",
            //     {7, 1, "volume", "", kValueTypeNumber, Permission::READ | Permission::WRITE, "setVolume", ""},
            // },

            // {
            //     "audio:voice-packet-id",
            //     {7, 2, "voice-packet-id", "", kValueTypeString, Permission::READ | Permission::WRITE, "setVoicePacketId", ""},
            // },

            // {
            //     "audio:voice-change-state",
            //     {7, 3, "voice-change-state", "", kValueTypeString, Permission::READ, "setVoiceChangeState", ""},
            // },

            // {
            //     "audio:set-voice",
            //     {7, 4, "set-voice", "", kValueTypeString, Permission::WRITE, "setSetVoice", ""},
            // },

            // {
            //     "time:time-zone",
            //     {8, 1, "time-zone", "", kValueTypeString, Permission::READ, "setTimeZone", ""},
            // },

            // {
            //     "time:timer-clean",
            //     {8, 2, "timer-clean", "", kValueTypeString, Permission::READ | Permission::WRITE, "setTimerClean", ""},
            // },

            // {
            //     "time:timer-id",
            //     {8, 3, "timer-id", "", kValueTypeString, Permission::NONE, "setTimerId", ""},
            // },

            // {
            //     "time:timer-cancel-reason",
            //     {8, 4, "timer-cancel-reason", "", kValueTypeNumber, Permission::NONE, "setTimerCancelReason", ""},
            // },

            // {
            //     "brush-cleaner:brush-left-time",
            //     {9, 1, "brush-left-time", "", kValueTypeNumber, Permission::READ, "setBrushLeftTime", "Brush Left Time"},
            // },

            // {
            //     "brush-cleaner:brush-life-level",
            //     {9, 2, "brush-life-level", "", kValueTypeNumber, Permission::READ, "setBrushLifeLevel", "Brush Life Level"},
            // },

            // {
            //     "brush-cleaner:brush-left-time",
            //     {10, 1, "brush-left-time", "", kValueTypeNumber, Permission::READ, "setBrushLeftTime", "Brush Left Time"},
            // },

            // {
            //     "brush-cleaner:brush-life-level",
            //     {10, 2, "brush-life-level", "", kValueTypeNumber, Permission::READ, "setBrushLifeLevel", "Brush Life Level"},
            // },

            // {
            //     "filter:filter-life-level",
            //     {11, 1, "filter-life-level", "", kValueTypeNumber, Permission::READ, "setFilterLifeLevel", "Filter Life Level"},
            // },

            // {
            //     "filter:filter-left-time",
            //     {11, 2, "filter-left-time", "", kValueTypeNumber, Permission::READ, "setFilterLeftTime", "Filter Left Time"},
            // },

            // {
            //     "clean-logs:first-clean-time",
            //     {12, 1, "first-clean-time", "", kValueTypeNumber, Permission::READ, "setFirstCleanTime", ""},
            // },

            // {
            //     "clean-logs:total-clean-time",
            //     {12, 2, "total-clean-time", "", kValueTypeNumber, Permission::READ, "setTotalCleanTime", "total-clean-time"},
            // },

            // {
            //     "clean-logs:total-clean-times",
            //     {12, 3, "total-clean-times", "", kValueTypeNumber, Permission::READ, "setTotalCleanTimes", ""},
            // },

            // {
            //     "clean-logs:total-clean-area",
            //     {12, 4, "total-clean-area", "", kValueTypeNumber, Permission::READ, "setTotalCleanArea", ""},
            // },

            // {
            //     "mop:mop-life-level",
            //     {18, 1, "mop-life-level", "", kValueTypeNumber, Permission::READ, "setMopLifeLevel", "Mop Life Level"},
            // },

            // {
            //     "mop:mop-left-time",
            //     {18, 2, "mop-left-time", "", kValueTypeNumber, Permission::READ, "setMopLeftTime", "Mop Left Time"},
            // },

            // {
            //     "no-disturb:no-disturb",
            //     {21, 1, "no-disturb", "", kValueTypeBoolean, Permission::READ | Permission::WRITE, "setNoDisturb", "No Disturb"},
            // },

            // {
            //     "alarm:alarm",
            //     {22, 1, "alarm", "", kValueTypeBoolean, Permission::READ | Permission::WRITE, "setAlarm", "Alarm"},
            // },

            // {
            //     "alarm:volume",
            //     {22, 2, "volume", "", kValueTypeNumber, Permission::READ | Permission::WRITE, "setVolume", "Volume"},
            // },

        };
        std::map<std::string, SpecAction> miotSpecAction = {

            {
                "vacuum:start-sweep",
                {2, 1, "StartSweep", "开始打扫", {}},
            },

            {
                "vacuum:stop-sweeping",
                {2, 2, "StopSweeping", "停止打扫", {}},
            },

            // {
            //     "vacuum:start-room-sweep",
            //     {2, 3, "StartRoomSweep", "Start Room Sweep", {SpecActionParam(4, "value4", "参数描述", kValueTypeNumber)}},
            // },

            // {
            //     "vacuum:start-dust-arrest",
            //     {2, 4, "StartDustArrest", "Start Dust Arrest", {}},
            // },

            // {
            //     "vacuum:start-mop-wash",
            //     {2, 6, "StartMopWash", "Start Mop Wash", {}},
            // },

            // {
            //     "vacuum:start-dry",
            //     {2, 8, "StartDry", "Start Dry", {}},
            // },

            // {
            //     "vacuum:stop-dry",
            //     {2, 9, "StopDry", "Stop Dry", {}},
            // },

            // {
            //     "vacuum:start-eject",
            //     {2, 10, "StartEject", "Start Eject", {}},
            // },

            // {
            //     "vacuum:start-call-clean",
            //     {2, 11, "StartCallClean", "Start Call Clean", {}},
            // },

            // {
            //     "vacuum:start-vacuum-room-sweep",
            //     {2, 12, "StartVacuumRoomSweep", "Start Vacuum Room Sweep", {SpecActionParam(7, "value7", "参数描述", kValueTypeNumber)}},
            // },

            // {
            //     "vacuum:start-build-map",
            //     {2, 13, "StartBuildMap", "Start Build Map", {}},
            // },

            // {
            //     "vacuum:pause-build-map",
            //     {2, 14, "PauseBuildMap", "Pause Build Map", {}},
            // },

            // {
            //     "vacuum:continue-build-map",
            //     {2, 15, "ContinueBuildMap", "Continue Build Map", {}},
            // },

            // {
            //     "vacuum:stop-build-map-and-gocharge",
            //     {2, 16, "StopBuildMapAndGocharge", "Stop Build Map And Gocharge", {}},
            // },

            // {
            //     "vacuum:stop-mop-wash",
            //     {2, 17, "StopMopWash", "Stop Mop Wash", {}},
            // },

            // {
            //     "vacuum:continue-sweep",
            //     {2, 18, "ContinueSweep", "Continue Sweep", {}},
            // },

            // {
            //     "vacuum:start-furniture-cleaning",
            //     {2, 19, "StartFurnitureCleaning", "Start Furniture Cleaning", {SpecActionParam(9, "value9", "参数描述", kValueTypeNumber)}},
            // },

            {
                "battery:start-charge",
                {3, 1, "StartCharge", "去充电", {}},
            },

            // {
            //     "vacuum-extend:start-clean",
            //     {4, 1, "StartClean", "start-clean", {SpecActionParam(10, "value10", "参数描述", kValueTypeNumber)}},
            // },

            // {
            //     "vacuum-extend:stop-clean",
            //     {4, 2, "StopClean", "stop-clean", {}},
            // },

            // {
            //     "map:map-req",
            //     {6, 1, "MapReq", "map-req", {SpecActionParam(2, "value2", "参数描述", kValueTypeNumber)}},
            // },

            // {
            //     "map:update-map",
            //     {6, 2, "UpdateMap", "update-map", {SpecActionParam(4, "value4", "参数描述", kValueTypeNumber)}},
            // },

            // {
            //     "audio:position",
            //     {7, 1, "Position", "position", {}},
            // },

            // {
            //     "audio:play-sound",
            //     {7, 2, "PlaySound", "play-sound", {}},
            // },

            // {
            //     "time:delete-timer",
            //     {8, 1, "DeleteTimer", "delete-timer", {SpecActionParam(3, "value3", "参数描述", kValueTypeNumber)}},
            // },

            // {
            //     "brush-cleaner:reset-brush-life",
            //     {9, 1, "ResetBrushLife", "Reset Brush Life", {}},
            // },

            // {
            //     "brush-cleaner:reset-brush-life",
            //     {10, 1, "ResetBrushLife", "Reset Brush Life", {}},
            // },

            // {
            //     "filter:reset-filter-life",
            //     {11, 1, "ResetFilterLife", "Reset Filter Life", {}},
            // },

            // {
            //     "mop:reset-mop-life",
            //     {18, 1, "ResetMopLife", "Reset Mop Life", {}},
            // },

        };

    public:
        XIAOMI_VACUUM_D110CH() : Thing("米家扫拖机器人 M40", "")
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

DECLARE_THING(XIAOMI_VACUUM_D110CH);