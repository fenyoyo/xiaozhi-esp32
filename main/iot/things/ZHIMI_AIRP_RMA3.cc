#include "iot/thing.h"
#include "iot/miot.h"
#include <esp_log.h>
#include "iot/protocol.h"
#include "application.h"
#include "board.h"
#include "iot/miot_device.h"

#define TAG "ZHIMI_AIRP_RMA3"

namespace iot
{
    // 这里仅定义 Lamp 的属性和方法，不包含具体的实现
    class ZHIMI_AIRP_RMA3 : public Thing
    {
    private:
        bool id_ = 2;
        bool power_ = false;
        bool mode_ = 0;
        uint8_t fault = 100;

        std::string ip_;
        std::string token_;
        uint32_t deviceID = 0;
        uint32_t deviceStamp = 0;
        MiotDevice miotDevice;
        // fault 滤芯
        // mode 模式 0自动 1睡眠 2喜爱
        std::map<std::string, SIID_PIID> miotSpec = {
            {"air-purifier:on", {2, 1}},
            {"air-purifier:fault", {2, 2}},
            {"air-purifier:mode", {2, 4}},
            {"air-purifier:relative-humidity", {3, 1}},
            {"air-purifier:pm2.5-density", {3, 4}},
            {"air-purifier:temperature", {3, 7}},
            {"air-purifier:air-quality", {3, 8}},
            {"air-purifier:brightness", {2, 12}},
            {"air-purifier:toggle", {2, 1}}};

    public:
        void initMiot(const std::string &ip, const std::string &token, const std::string &name) override
        {
            // ESP_LOGI(TAG, "initMiot ZHIMI_AIRP_RMA3");
            ip_ = ip;
            token_ = token;
            set_name(name);
            miotDevice = MiotDevice(ip_, token_);
        }

        ZHIMI_AIRP_RMA3() : Thing("ZHIMI_AIRP_RMA3", "空气净化器"), power_(false)
        {

            properties_.AddBooleanProperty("power", "空气净化器是否打开", [this]() -> bool
                                           {  
                                            auto spec = miotSpec.find("air-purifier:on");
                                            std::string name = spec->first;
                                            SIID_PIID sp = spec->second;
                                            auto response=  miotDevice.getProperty(name, sp.siid, sp.piid);
                                            ESP_LOGI(TAG, "power response:%s", response.c_str());
                                            if(response.empty())
                                            {
                                                return power_;
                                            }
                                            uint8_t value;
                                            miotDevice.parseJsonGetValue(response, &value);
                                            power_ = value;
                                            return power_; });
            // air-purifier:on 不生效
            methods_.AddMethod("Toggle", "切换开关状态", ParameterList(), [this](const ParameterList &parameters)
                               {
                                   //    power_ = true;
                                   auto spec = miotSpec.find("air-purifier:toggle");
                                   std::string did = spec->first;
                                   SIID_PIID sp = spec->second;
                                   auto res = miotDevice.callAction(sp.siid, sp.piid);
                                   ESP_LOGI(TAG, "power response:%s", res.c_str());
                                   //    auto res = miotDevice.setProperty(did, sp.siid, sp.piid, 1);
                                   if (res.empty())
                                   {
                                       return;
                                   }
                                   int8_t error;
                                   miotDevice.parseJsonHasError(res, &error);
                                   if (error == -1)
                                   {
                                       return;
                                   }
                                   uint8_t code;
                                   miotDevice.parseCallGetCode(res, &code);
                                   if (code == 0)
                                   {
                                       power_ = !power_;
                                   } //
                               });

            methods_.AddMethod("SetMode", "切换电风扇吹风模式", ParameterList({Parameter("mode", "自动模式=0,睡眠模式=1,最爱模式=2", kValueTypeNumber, true)}), [this](const ParameterList &parameters)
                               {
                                   auto tmp_mode_ = static_cast<int8_t>(parameters["mode"].number());
                                   auto spec = miotSpec.find("air-purifier:mode");
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
            // methods_.AddMethod("SetOffDelayTime", "定时关闭", ParameterList({Parameter("minutes", "0-480分钟", kValueTypeNumber, true)}), [this](const ParameterList &parameters)
            //                    {
            //                        auto tmp_minutes = static_cast<int8_t>(parameters["minutes"].number());
            //                        auto spec = miotSpec.find("fan:off-delay-time");
            //                        std::string did = spec->first;
            //                        SIID_PIID sp = spec->second;
            //                        auto res = miotDevice.setProperty(did, sp.siid, sp.piid, tmp_minutes);
            //                        if (res.empty())
            //                        {
            //                            return;
            //                        }
            //                        uint8_t code;
            //                        miotDevice.parseJsonGetCode(res, 0, &code);
            //                        if (code == 0)
            //                        {
            //                            minutes = tmp_minutes;
            //                        }
            //                        //
            //                    });
        }
    };

} // namespace iot

DECLARE_THING(ZHIMI_AIRP_RMA3);
