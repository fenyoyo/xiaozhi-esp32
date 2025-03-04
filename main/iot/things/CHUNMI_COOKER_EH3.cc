#include "iot/thing.h"
#include "iot/miot.h"
#include <esp_log.h>
#include "iot/protocol.h"
#include "application.h"
#include "iot/miot_device.h"
#include "board.h"

#define TAG "CHUNMI_COOKER_EH3"

namespace iot
{
    // 这里仅定义 Lamp 的属性和方法，不包含具体的实现
    class CHUNMI_COOKER_EH3 : public Thing
    {
    private:
        bool id_ = 2;
        bool power_ = false;
        // bool mode_ = 0;
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
            {"cooker:status", {2, 1, MOIT_PROPERTY_INT, 0, "工作状态:1=闲置,2=工作中,3=任务工作中,4=保暖,5=错误,6=更新中,7=烹煮完成"}},
            {"cooker:cook-mode", {2, 5, MOIT_PROPERTY_INT, 0, "模式:1=精细煮,2=快速煮,3=煮粥,4=保暖,5=定制"}},
        };

        std::map<std::string, SIID_AIID> miotAction = {
            {"cooker:start-cook", {2, 1, 5, MOIT_PROPERTY_INT, 0, "开始煮"}},
            {"cooker:cancel-cooking", {2, 2, 0, MOIT_PROPERTY_INT, 0, "取消"}},
        };
        // TODO 把属性和方法分离出来

    public:
        void initMiot(const std::string &ip, const std::string &token, const std::string &name) override
        {
            ip_ = ip;
            token_ = token;
            set_name(name);
            miotDevice = MiotDevice(ip_, token_);
        }

        CHUNMI_COOKER_EH3() : Thing("CHUNMI_COOKER_EH3", "电饭煲"), power_(false)
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

            methods_.AddMethod("startCook", "设置烹煮模式,并开始煮", ParameterList({Parameter("mode", "1=快速煮,2=精细煮,3=煮粥,4=保暖,5=定制", kValueTypeNumber, true)}), [this](const ParameterList &parameters)
                               {
                                   //    power_ = true;
                                   auto mode_ = static_cast<int8_t>(parameters["mode"].number());
                                   auto spec = miotAction.find("cooker:start-cook");
                                   std::string did = spec->first;
                                   SIID_AIID sp = spec->second;
                                   auto res = miotDevice.callAction(sp.siid, sp.aiid, sp.piid, mode_);
                                   ESP_LOGI(TAG, "startCook response:%s", res.c_str());
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
                                       spec->second.value = mode_;
                                   } //
                               });

            methods_.AddMethod("cancelCook", "取消烹煮", ParameterList(), [this](const ParameterList &parameters)
                               {
                                   auto spec = miotAction.find("cooker:cancel-cooking");
                                   std::string did = spec->first;
                                   SIID_AIID sp = spec->second;
                                   auto res = miotDevice.callAction(sp.siid, sp.aiid);
                                   ESP_LOGI(TAG, "cooker:start-cook response:%s", res.c_str());
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
                                       spec->second.value = 0;
                                   } //
                               });
        }
    };

} // namespace iot

DECLARE_THING(CHUNMI_COOKER_EH3);
