#include "iot/thing.h"
#include "iot/miot.h"
#include <esp_log.h>
#include "iot/protocol.h"
#include "application.h"
#include "board.h"
#include "iot/miot_device.h"
#define TAG "Fan"

namespace iot
{
    // 这里仅定义 Lamp 的属性和方法，不包含具体的实现
    class Fan : public Thing
    {
    private:
        bool id_ = 2;
        bool power_ = false;
        bool mode_ = 0;
        int8_t level_ = 1;
        bool horizontal = false;

        std::string ip_;
        std::string token_;
        uint32_t deviceID = 0;
        uint32_t deviceStamp = 0;
        MiotDevice miotDevice;
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
        void initMiot(const std::string &ip, const std::string &token, const std::string &name) override
        {
            // ESP_LOGI(TAG, "initMiot");
            // 初始化miot 从服务器上获取设备的属性
            ip_ = ip;
            token_ = token;
            set_name(name);
            miotDevice = MiotDevice(ip_, token_);
            // auto http = Board::GetInstance().CreateHttp();
            // std::string method = "GET";
            // if (!http->Open(method, "http://192.168.2.105:8080/", ""))
            // {
            //     ESP_LOGE(TAG, "Failed to open HTTP connection");
            //     delete http;
            // }
            // auto response = http->GetBody();
            // ESP_LOGI(TAG, "response:%s", response.c_str());
            // http->Close();
            // delete http;
        }

        Fan() : Thing("Fan", "电风扇"), power_(false), level_(1)
        {

            properties_.AddBooleanProperty("power", "电风扇是否打开", [this]() -> bool
                                           {  
                                            auto spec = miotSpec.find("fan:on");
                                            std::string name = spec->first;
                                            SIID_PIID sp = spec->second;
                                            auto response=  miotDevice.getProperty(name, sp.siid, sp.piid);
                                            if(response.empty())
                                            {
                                                return power_;
                                            }
                                            cJSON *root = cJSON_Parse(response.data());
                                            cJSON *result = cJSON_GetObjectItem(root, "result");
                                            cJSON *item = cJSON_GetArrayItem(result, 0);
                                            cJSON *value = cJSON_GetObjectItem(item, "value");
                                            power_ = value->valueint;
                                            return power_; });
            properties_.AddBooleanProperty("level", "风速", [this]() -> bool
                                           { return level_; });

            // 定义设备可以被远程执行的指令
            // methods_.AddMethod("TurnOn", "打开电风扇", ParameterList(), [this](const ParameterList &parameters)
            //                    {
            //                        power_ = true;
            //                        auto spec = miotSpec.find("fan:on");
            //                        std::string name = spec->first;
            //                        SIID_PIID sp = spec->second;
            //                        Parameters params = {
            //                            .did = name,
            //                            .siid = sp.siid,
            //                            .piid = sp.piid,
            //                            .value = 1};
            //                        SetProperty(params);
            //                        // return true;
            //                    });

            // methods_.AddMethod("TurnOff", "关闭电风扇", ParameterList(), [this](const ParameterList &parameters)
            //                    {
            //                        power_ = false;
            //                        auto spec = miotSpec.find("fan:on");
            //                        std::string name = spec->first;
            //                        SIID_PIID sp = spec->second;
            //                        Parameters params = {
            //                            .did = name,
            //                            .siid = sp.siid,
            //                            .piid = sp.piid,
            //                            .value = 0};
            //                        SetProperty(params); });
            // methods_.AddMethod("SetLevel", "风速", ParameterList({Parameter("level", "1-3档,一档风力最小", kValueTypeNumber, true)}), [this](const ParameterList &parameters)
            //                    {
            //                        level_ = static_cast<int8_t>(parameters["level"].number());
            //                        auto spec = miotSpec.find("fan:fan-level");
            //                        std::string name = spec->first;
            //                        SIID_PIID sp = spec->second;
            //                        Parameters params = {
            //                            .did = name,
            //                            .siid = sp.siid,
            //                            .piid = sp.piid,
            //                            .value = level_};
            //                        SetProperty(params);
            //                        // return false;
            //                    });
            // methods_.AddMethod("SetHorizontal", "开启或关闭左右摇摆", ParameterList({Parameter("horizontal", "开启或关闭", kValueTypeBoolean, true)}), [this](const ParameterList &parameters)
            //                    {
            //                     horizontal = parameters["horizontal"].boolean();

            //                     auto spec = miotSpec.find("fan:horizontal-swing");
            //                     std::string name = spec->first;
            //                     SIID_PIID sp = spec->second;
            //                     Parameters params = {
            //                         .did = name,
            //                         .siid = sp.siid,
            //                         .piid = sp.piid,
            //                         .value = horizontal};
            //                     SetProperty(params); });

            // methods_.AddMethod("SetMode", "切换电风扇吹风模式", ParameterList({Parameter("mode", "直吹模式=0,睡眠模式=1", kValueTypeNumber, true)}), [this](const ParameterList &parameters)
            //                    {
            //                     mode_ = static_cast<int8_t>(parameters["mode"].number());

            //                     auto spec = miotSpec.find("fan:mode");
            //                     std::string name = spec->first;
            //                     SIID_PIID sp = spec->second;
            //                     Parameters params = {
            //                         .did = name,
            //                         .siid = sp.siid,
            //                         .piid = sp.piid,
            //                         .value = mode_};
            //                     SetProperty(params); });
        }

        // void SetProperty(Parameters parameters)
        // {
        //     std::string jsonStr = "[{\"did\": \"" + parameters.did + "\",";
        //     jsonStr += "\"siid\": " + std::to_string(parameters.siid) + ",";
        //     jsonStr += "\"piid\": " + std::to_string(parameters.piid) + ",";
        //     jsonStr += "\"value\": " + std::to_string(parameters.value);
        //     jsonStr += "}]";
        //     Application::GetInstance().Schedule([this, jsonStr]()
        //                                         { 
        //                                             Miot miot(ip_, ip_);
        //                                             miot.Send("set_properties", jsonStr); });
        // };

        // void GetProperty(Parameters parameters)
        // {
        //     std::string jsonStr = "[{\"did\": \"" + parameters.did + "\",";
        //     jsonStr += "\"siid\": " + std::to_string(parameters.siid) + ",";
        //     jsonStr += "\"piid\": " + std::to_string(parameters.piid);
        //     jsonStr += "}]";
        //     Application::GetInstance().Schedule([this, jsonStr]()
        //                                         { 
        //                                             Miot miot(ip_, ip_);
        //                                             miot.Send("get_properties", jsonStr); });
        // };
    };

} // namespace iot

DECLARE_THING(Fan);
