#include "iot/thing.h"
#include "iot/miot.h"
#include <esp_log.h>
#include "iot/protocol.h"
#include "application.h"
#include "board.h"
#include "iot/miot_device.h"

#define TAG "DMAKER_FAN_P8"

namespace iot
{
    // 这里仅定义 Lamp 的属性和方法，不包含具体的实现
    class DMAKER_FAN_P8 : public Thing
    {
    private:
        bool id_ = 2;
        bool power_ = false;
        bool mode_ = 0;
        uint8_t level_ = 1;
        uint8_t minutes = 0;
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
            ESP_LOGI(TAG, "initMiot DMAKER_FAN_P8");
            ip_ = ip;
            token_ = token;
            set_name(name);
            miotDevice = MiotDevice(ip_, token_);
        }

        DMAKER_FAN_P8() : Thing("DMAKER_FAN_P8", "电风扇"), power_(false), level_(1)
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
                                            uint8_t value;
                                            miotDevice.parseJsonGetValue(response, &value);
                                            power_ = value;
                                            return power_; });

            properties_.AddNumberProperty("level", "风速", [this]() -> int
                                          {
                                              auto spec = miotSpec.find("fan:fan-level");
                                              if (spec == miotSpec.end())
                                              {
                                                  return level_;
                                              }
                                              std::string name = spec->first;
                                              SIID_PIID sp = spec->second;
                                              auto response = miotDevice.getProperty(name, sp.siid, sp.piid);
                                              if (response.empty())
                                              {
                                                  return level_;
                                              }
                                              miotDevice.parseJsonGetValue(response, &level_);
                                              return level_;
                                              //
                                          });

            methods_.AddMethod("TurnOn", "打开电风扇", ParameterList(), [this](const ParameterList &parameters)
                               {
                                   //    power_ = true;
                                   auto spec = miotSpec.find("fan:on");
                                   std::string did = spec->first;
                                   SIID_PIID sp = spec->second;
                                   auto res = miotDevice.setProperty(did, sp.siid, sp.piid, 1);
                                   if (res.empty())
                                   {
                                       return;
                                   }
                                   cJSON *root = cJSON_Parse(res.data());
                                   cJSON *result = cJSON_GetObjectItem(root, "result");
                                   cJSON *item = cJSON_GetArrayItem(result, 0);
                                   cJSON *code = cJSON_GetObjectItem(item, "code");
                                   if (code->valueint != 0)
                                   {
                                       power_ = true;
                                   } //
                               });

            methods_.AddMethod("TurnOff", "关闭电风扇", ParameterList(), [this](const ParameterList &parameters)
                               {
                                   //    power_ = false;
                                   auto spec = miotSpec.find("fan:on");
                                   std::string did = spec->first;
                                   SIID_PIID sp = spec->second;
                                   auto res = miotDevice.setProperty(did, sp.siid, sp.piid, 0);
                                   if (res.empty())
                                   {
                                       return;
                                   }
                                   cJSON *root = cJSON_Parse(res.data());
                                   cJSON *result = cJSON_GetObjectItem(root, "result");
                                   cJSON *item = cJSON_GetArrayItem(result, 0);
                                   cJSON *code = cJSON_GetObjectItem(item, "code");
                                   if (code->valueint == 0)
                                   {
                                       power_ = false;
                                   } //
                               });
            methods_.AddMethod("SetLevel", "风速", ParameterList({Parameter("level", "1-3档,一档风力最小", kValueTypeNumber, true)}), [this](const ParameterList &parameters)
                               {
                                   auto tmp_level_ = static_cast<int8_t>(parameters["level"].number());
                                   auto spec = miotSpec.find("fan:fan-level");
                                   std::string did = spec->first;
                                   SIID_PIID sp = spec->second;
                                   auto res = miotDevice.setProperty(did, sp.siid, sp.piid, tmp_level_);
                                   if (res.empty())
                                   {
                                       return;
                                   }
                                   uint8_t code;
                                   miotDevice.parseJsonGetCode(res, 0, &code);
                                   if (code == 0)
                                   {
                                       level_ = tmp_level_;
                                   } //
                               });
            methods_.AddMethod("SetHorizontal", "开启或关闭左右摇摆", ParameterList({Parameter("horizontal", "开启或关闭", kValueTypeBoolean, true)}), [this](const ParameterList &parameters)
                               {
                                   auto tmp_horizontal_ = parameters["horizontal"].boolean();
                                   auto spec = miotSpec.find("fan:horizontal-swing");
                                   std::string did = spec->first;
                                   SIID_PIID sp = spec->second;
                                   auto res = miotDevice.setProperty(did, sp.siid, sp.piid, tmp_horizontal_);
                                   if (res.empty())
                                   {
                                       return;
                                   }
                                   uint8_t code;
                                   miotDevice.parseJsonGetCode(res, 0, &code);
                                   if (code == 0)
                                   {
                                       horizontal = tmp_horizontal_;
                                   }
                                   //
                               });

            methods_.AddMethod("SetMode", "切换电风扇吹风模式", ParameterList({Parameter("mode", "直吹模式=0,睡眠模式=1", kValueTypeNumber, true)}), [this](const ParameterList &parameters)
                               {
                                   auto tmp_mode_ = static_cast<int8_t>(parameters["mode"].number());
                                   auto spec = miotSpec.find("fan:mode");
                                   std::string did = spec->first;
                                   SIID_PIID sp = spec->second;
                                   auto res = miotDevice.setProperty(did, sp.siid, sp.piid, tmp_mode_);
                                   if (res.empty())
                                   {
                                       return;
                                   }
                                   uint8_t code;
                                   miotDevice.parseJsonGetCode(res, 0, &code);
                                   if (code == 0)
                                   {
                                       mode_ = tmp_mode_;
                                   }
                                   //
                               });
            methods_.AddMethod("SetOffDelayTime", "定时关闭", ParameterList({Parameter("minutes", "0-480分钟", kValueTypeNumber, true)}), [this](const ParameterList &parameters)
                               {
                                   auto tmp_minutes = static_cast<int8_t>(parameters["minutes"].number());
                                   auto spec = miotSpec.find("fan:off-delay-time");
                                   std::string did = spec->first;
                                   SIID_PIID sp = spec->second;
                                   auto res = miotDevice.setProperty(did, sp.siid, sp.piid, tmp_minutes);
                                   if (res.empty())
                                   {
                                       return;
                                   }
                                   uint8_t code;
                                   miotDevice.parseJsonGetCode(res, 0, &code);
                                   if (code == 0)
                                   {
                                       minutes = tmp_minutes;
                                   }
                                   //
                               });
        }
    };

} // namespace iot

DECLARE_THING(DMAKER_FAN_P8);
