#include "iot/thing.h"
#include "iot/miot.h"
#include <esp_log.h>
#include "iot/protocol.h"
#include "application.h"

#define TAG "Fan"

namespace iot
{
    struct Parameters
    {
        std::string did;
        int8_t siid;
        int8_t piid;
        int8_t value;
    };

    struct SIID_PIID
    {
        int8_t siid;
        int8_t piid;
    };

    // 这里仅定义 Lamp 的属性和方法，不包含具体的实现
    class Fan : public Thing
    {
    private:
        bool id_ = 2;
        bool power_ = false;
        bool mode_ = 0;
        int8_t level_ = 1;
        bool horizontal = false;

        std::string ip = "192.168.2.103";
        std::string token = "2c94fc01d4399baa4e6a18c076de918d";
        uint32_t deviceID = 0;

        std::map<std::string, SIID_PIID> miotSpec = {
            {"fan:on", {2, 1}},
            {"fan:fan-level", {2, 2}},
            {"fan:horizontal-swing", {2, 3}},
            {"fan:mode", {2, 7}},
            {"fan:off-delay-time", {2, 10}},
            {"fan:alarm", {2, 11}},
            {"fan:brightness", {2, 12}},
            {"physical-controls-locked:physical-controls-locked", {3, 1}},
            {"alarm:alarm", {4, 1}},
            {"indicator-light:on", {5, 1}}};

    public:
        Fan() : Thing("Fan", "电风扇"), power_(false), level_(1)
        {

            properties_.AddBooleanProperty("power", "电风扇是否打开", [this]() -> bool
                                           {  
                                            Miot::InitHandshake(ip);
                                            ESP_LOGI(TAG,"获取电风扇是否打开：%d",power_);
                                            return power_; });
            properties_.AddBooleanProperty("level", "风速", [this]() -> bool
                                           { return level_; });

            // 定义设备可以被远程执行的指令
            methods_.AddMethod("TurnOn", "打开电风扇", ParameterList(), [this](const ParameterList &parameters)
                               {
                                   power_ = true;
                                   auto spec = miotSpec.find("fan:on");
                                   std::string name = spec->first;
                                   SIID_PIID sp = spec->second;
                                   Parameters params = {
                                       .did = name,
                                       .siid = sp.siid,
                                       .piid = sp.piid,
                                       .value = 1};
                                   SetProperty(params);
                                   // return true;
                               });

            methods_.AddMethod("TurnOff", "关闭电风扇", ParameterList(), [this](const ParameterList &parameters)
                               {
                                   power_ = false;
                                   auto spec = miotSpec.find("fan:on");
                                   std::string name = spec->first;
                                   SIID_PIID sp = spec->second;
                                   Parameters params = {
                                       .did = name,
                                       .siid = sp.siid,
                                       .piid = sp.piid,
                                       .value = 0};
                                   SetProperty(params);
                                   // return false;
                               });
            methods_.AddMethod("SetLevel", "风速", ParameterList({Parameter("level", "1-3档", kValueTypeNumber, true)}), [this](const ParameterList &parameters)
                               {
                                   level_ = static_cast<int8_t>(parameters["level"].number());
                                   auto spec = miotSpec.find("fan:fan-level");
                                   std::string name = spec->first;
                                   SIID_PIID sp = spec->second;
                                   Parameters params = {
                                       .did = name,
                                       .siid = sp.siid,
                                       .piid = sp.piid,
                                       .value = level_};
                                   SetProperty(params);
                                   // return false;
                               });
            methods_.AddMethod("SetHorizontal", "开启或关闭左右摇摆", ParameterList({Parameter("horizontal", "开启或关闭", kValueTypeBoolean, true)}), [this](const ParameterList &parameters)
                               { 
                                horizontal = parameters["horizontal"].boolean();
                                
                                auto spec = miotSpec.find("fan:horizontal-swing");
                                std::string name = spec->first;
                                SIID_PIID sp = spec->second;
                                Parameters params = {
                                    .did = name,
                                    .siid = sp.siid,
                                    .piid = sp.piid,
                                    .value = horizontal};
                                SetProperty(params); });

            methods_.AddMethod("SetMode", "切换电风扇吹风模式", ParameterList({Parameter("mode", "直吹模式=0,睡眠模式=1", kValueTypeNumber, true)}), [this](const ParameterList &parameters)
                               { 
                                mode_ = static_cast<int8_t>(parameters["mode"].number());
                                
                                auto spec = miotSpec.find("fan:mode");
                                std::string name = spec->first;
                                SIID_PIID sp = spec->second;
                                Parameters params = {
                                    .did = name,
                                    .siid = sp.siid,
                                    .piid = sp.piid,
                                    .value = mode_};
                                SetProperty(params); });
        }

        void SetProperty(Parameters parameters)
        {
            std::string jsonStr = "[{\"did\": \"" + parameters.did + "\",";
            jsonStr += "\"siid\": " + std::to_string(parameters.siid) + ",";
            jsonStr += "\"piid\": " + std::to_string(parameters.piid) + ",";
            jsonStr += "\"value\": " + std::to_string(parameters.value);
            jsonStr += "}]";

            //
            // xTaskCreate([](void *arg)
            //             {
            //             Fan* fan = (Fan*)arg;
            //             fan->miot->Send("set_properties", jsonStr);
            // vTaskDelete(NULL); }, "main_loop", 4096 * 2, this, 2, nullptr);

            Application::GetInstance().Schedule([this, jsonStr]()
                                                { 
                                                    Miot miot(ip, token);
                                                    miot.Send("set_properties", jsonStr); });
        };
    };

} // namespace iot

DECLARE_THING(Fan);
