#include "iot/thing.h"
#include "iot/miot.h"
#include <esp_log.h>
#include "iot/protocol.h"
#include "application.h"
#include "iot/miot_device.h"
#include "board.h"

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
        uint8_t humidity_ = 0;
        uint16_t pm25_ = 0;
        double temperature_ = 0;
        uint8_t air_quality_ = 0;

        std::string ip_;
        std::string token_;
        uint32_t deviceID = 0;
        uint32_t deviceStamp = 0;
        MiotDevice miotDevice;
        // fault 滤芯
        // mode 模式 0自动 1睡眠 2喜爱
        std::map<std::string, SIID_PIID> miotSpec = {
            {"air-purifier:on", {2, 1, MOIT_PROPERTY_BOOL, 0, "开关"}},
            {"air-purifier:mode", {2, 4, MOIT_PROPERTY_INT, 0, "模式"}},
            {"air-purifier:relative-humidity", {3, 1, MOIT_PROPERTY_INT, 0, "空气湿度"}},
            {"air-purifier:pm2.5-density", {3, 4, MOIT_PROPERTY_INT, 0, "pm2.5"}},
            {"air-purifier:temperature", {3, 7, MOIT_PROPERTY_INT, 0, "温度"}},
            {"air-purifier:air-quality", {3, 8, MOIT_PROPERTY_INT, 0, "空气质量"}},
        };

    public:
        void initMiot(const std::string &ip, const std::string &token, const std::string &name) override
        {
            ip_ = ip;
            token_ = token;
            set_name(name);
            miotDevice = MiotDevice(ip_, token_);
        }

        ZHIMI_AIRP_RMA3() : Thing("ZHIMI_AIRP_RMA3", "空气净化器"), power_(false)
        {

            properties_.AddBooleanProperty("properties", "获取设备所有状态", [this]() -> bool
                                           {
                                               auto spec = miotDevice.getProperties(miotSpec);
                                               if (spec.empty())
                                               {
                                                   return false;
                                               }
                                               for (auto it = spec.begin(); it != spec.end(); ++it)
                                               {
                                                   auto property = miotSpec.find(it->first);
                                                   property->second.value = it->second;
                                               }
                                               return true; //
                                           });

            for (auto it = miotSpec.begin(); it != miotSpec.end(); ++it)
            {
                switch (it->second.type)
                {
                case MOIT_PROPERTY_BOOL:
                    properties_.AddBooleanProperty(it->first, it->second.description, [this, it]() -> bool
                                                   { return miotSpec.find(it->first)->second.value; });
                    break;
                case MOIT_PROPERTY_INT:
                    properties_.AddNumberProperty(it->first, it->second.description, [this, it]() -> int
                                                  { return miotSpec.find(it->first)->second.value; });
                    break;
                case MOIT_PROPERTY_STRING:
                default:
                    properties_.AddStringProperty(it->first, it->second.description, [this, it]() -> std::string
                                                  { return std::to_string(miotSpec.find(it->first)->second.value); });
                    break;
                }
            }
            //   auto spec = miotSpec.find("air-purifier:relative-humidity");
            //   std::string name = spec->first;
            //   SIID_PIID sp = spec->second;
            //   int value;
            //   auto err = miotDevice.getPropertyIntValue(name, sp.siid, sp.piid, &value);
            //   if (err != 0)
            //   {
            //       return humidity_;
            //   }
            //   // ESP_LOGI(TAG, "humidity response:%d", value);
            //   humidity_ = value;

            // properties_.AddBooleanProperty("power", "空气净化器是否打开", [this]() -> bool
            //                                {
            //                                 return miotSpec.find("air-purifier:on")->second.value;
            //                                 return power_; });
            // properties_.AddNumberProperty("humidity", "空气湿度", [this]() -> int
            //                               {
            //                                   return miotSpec.find("air-purifier:relative-humidity")->second.value;
            //                                   return humidity_; //
            //                               });
            // properties_.AddNumberProperty("pm25", "pm2.5污染", [this]() -> int
            //                               {
            //                                   return miotSpec.find("air-purifier:pm2.5-density")->second.value;
            //                                   return pm25_;
            //                                   //
            //                               });
            // properties_.AddStringProperty("temperature", "温度", [this]() -> std::string
            //                               {
            //                                   return std::to_string(miotSpec.find("air-purifier:temperature")->second.value);
            //                                   return std::to_string(temperature_);
            //                                   //
            //                               });
            // properties_.AddNumberProperty("air-quality", "空气质量:0=优秀,1=良好,2=中等,3=差,4=严重污染,5=危险", [this]() -> int
            //                               {
            //                                   return miotSpec.find("air-purifier:air-quality")->second.value;
            //                                   return air_quality_;
            //                                   //
            //                               });

            methods_.AddMethod("TurnOn", "打开空气净化器", ParameterList(), [this](const ParameterList &parameters)
                               {
                                   //    power_ = true;
                                   auto spec = miotSpec.find("air-purifier:on");
                                   std::string did = spec->first;
                                   SIID_PIID sp = spec->second;
                                   auto res = miotDevice.setProperty(did, sp.siid, sp.piid, MOIT_ON, true);
                                   if (res.empty())
                                   {
                                       return;
                                   }
                                   uint8_t code;
                                   miotDevice.parseJsonGetCode(res, 0, &code);
                                   if (code == 0)
                                   {
                                    power_ = true;
                                   } });
            methods_.AddMethod("TurnOff", "关闭空气净化器", ParameterList(), [this](const ParameterList &parameters)
                               {
                                       //    power_ = true;
                                       auto spec = miotSpec.find("air-purifier:on");
                                       std::string did = spec->first;
                                       SIID_PIID sp = spec->second;
                                       auto res = miotDevice.setProperty(did, sp.siid, sp.piid, MOIT_OFF, true);
                                       if (res.empty())
                                       {
                                           return;
                                       }
                                       uint8_t code;
                                       miotDevice.parseJsonGetCode(res, 0, &code);
                                       if (code == 0)
                                       {
                                        power_ = true;
                                       } });

            // methods_.AddMethod("Toggle", "切换开关状态", ParameterList(), [this](const ParameterList &parameters)
            //                    {
            //                        //    power_ = true;
            //                        auto spec = miotSpec.find("air-purifier:toggle");
            //                        std::string did = spec->first;
            //                        SIID_PIID sp = spec->second;
            //                        auto res = miotDevice.callAction(sp.siid, sp.piid);
            //                        ESP_LOGI(TAG, "power response:%s", res.c_str());
            //                        //    auto res = miotDevice.setProperty(did, sp.siid, sp.piid, 1);
            //                        if (res.empty())
            //                        {
            //                            return;
            //                        }
            //                        int8_t error;
            //                        miotDevice.parseJsonHasError(res, &error);
            //                        if (error == -1)
            //                        {
            //                            return;
            //                        }
            //                        uint8_t code;
            //                        miotDevice.parseCallGetCode(res, &code);
            //                        if (code == 0)
            //                        {
            //                            power_ = !power_;
            //                        } //
            //                    });

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
