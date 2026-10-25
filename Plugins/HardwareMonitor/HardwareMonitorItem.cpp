#include "stdafx.h"
#include "HardwareMonitorItem.h"
#include "HardwareMonitor.h"
#include "HardwareMonitorHelper.h"
#include "Common.h"

using namespace LibreHardwareMonitor::Hardware;

namespace HardwareMonitor
{
    CHardwareMonitorItem::CHardwareMonitorItem(const std::wstring& _identifier, const std::wstring& _item_name, const std::wstring& _label_text)
        : identifier(_identifier), item_name(_item_name), label_text(_label_text)
    {
    }

    const wchar_t* CHardwareMonitorItem::GetItemName() const
    {
        return item_name.c_str();
    }

    const wchar_t* CHardwareMonitorItem::GetItemId() const
    {
        static std::wstring item_id;
        item_id = L"7KrUeFVl_" + identifier;
        return item_id.c_str();
    }

    const wchar_t* CHardwareMonitorItem::GetItemLableText() const
    {
        return label_text.c_str();
    }

    const wchar_t* CHardwareMonitorItem::GetItemValueText() const
    {
        if (CHardwareMonitor::GetInstance()->m_tm_settings.is_taskbar)
            return item_value_taskbar.c_str();
        else
            return item_value_main_wnd.c_str();
    }

    const wchar_t* CHardwareMonitorItem::GetItemValueSampleText() const
    {
        static std::wstring sample_text;

        //显存项目由两个数值组成，单独生成示例文本
        if (HardwareMonitorHelper::IsGpuMemoryItemIdentifier(identifier))
        {
            const ItemInfo& item_info{ CHardwareMonitor::GetInstance()->m_settings.FindItemInfo(identifier) };
            sample_text.clear();
            if (!item_info.specify_value_width)
            {
                //不指定数值宽度时，使用当前的实际数值作为示例文本，使显示区域刚好足够宽
                sample_text = GetItemValueText();
                if (sample_text.empty())
                {
                    sample_text = L"0/0";
                    if (item_info.show_unit)
                    {
                        if (CHardwareMonitor::GetInstance()->m_tm_settings.sperate_with_space())
                            sample_text += L' ';
                        sample_text += item_info.unit.empty() ? L"GB" : item_info.unit;
                    }
                }
                return sample_text.c_str();
            }
            //指定数值宽度时，根据实际的总显存估算示例数值。
            //已占用显存不会超过总显存，这样可以保证显示区域不会被过宽的示例文本撑开
            float used_gb{};
            float total_gb{};
            if (HardwareMonitorHelper::GetGpuMemoryValues(gcnew String(identifier.c_str()), used_gb, total_gb))
            {
                String^ unit = item_info.unit.empty() ? gcnew String(L"GB") : gcnew String(item_info.unit.c_str());
                bool use_mb = unit->Equals(gcnew String(L"MB"), StringComparison::OrdinalIgnoreCase);
                float total = use_mb ? total_gb * 1024.0f : total_gb;
                String^ used_format = String::Format("F{0}", item_info.decimal_places);
                sample_text = Common::StringToStdWstring(total.ToString(used_format));
                sample_text += L'/';
                //总显存显示为整数
                sample_text += Common::StringToStdWstring(total.ToString("F0"));
            }
            else
            {
                std::wstring number_sample(item_info.value_width, L'0');
                if (item_info.decimal_places > 0)
                {
                    number_sample += L'.';
                    number_sample += std::wstring(item_info.decimal_places, L'0');
                }
                sample_text = number_sample;
                sample_text += L'/';
                sample_text += std::wstring(item_info.value_width, L'0');
            }
            if (item_info.show_unit)
            {
                if (CHardwareMonitor::GetInstance()->m_tm_settings.sperate_with_space())
                    sample_text += L' ';
                sample_text += item_info.unit.empty() ? L"GB" : item_info.unit;
            }
            return sample_text.c_str();
        }

        const ItemInfo& item_info{ CHardwareMonitor::GetInstance()->m_settings.FindItemInfo(identifier) };
        if (item_info.specify_value_width)
        {
            sample_text = std::wstring(item_info.value_width, L'0');
            if (item_info.decimal_places > 0)
            {
                sample_text += L'.';
                sample_text += std::wstring(item_info.decimal_places, L'0');
            }
            if (item_info.show_unit)
            {
                if (CHardwareMonitor::GetInstance()->m_tm_settings.sperate_with_space())
                    sample_text += L' ';
                SensorType type = static_cast<SensorType>(sensor_type);
                std::wstring unit;
                if (!item_info.unit.empty())
                    unit = item_info.unit;
                else
                    unit = Common::StringToStdWstring(HardwareMonitorHelper::GetSensorTypeDefaultUnit(type));
                sample_text += unit;
            }
        }
        else
        {
            std::wstring item_value(GetItemValueText());
            if (item_value.empty())
                sample_text = L"0.00";
            else
                sample_text = item_value;
        }
        return sample_text.c_str();
    }

    int CHardwareMonitorItem::IsDrawResourceUsageGraph() const
    {
        //显存项目绘制占用背景条，数值为 已占用显存/总显存，范围 0.0~1.0
        if (HardwareMonitorHelper::IsGpuMemoryItemIdentifier(identifier))
            return 1;
        SensorType type = static_cast<SensorType>(sensor_type);
        return type == SensorType::Temperature || type == SensorType::Load;
    }

    float CHardwareMonitorItem::GetResourceUsageGraphValue() const
    {
        if (HardwareMonitorHelper::IsGpuMemoryItemIdentifier(identifier))
        {
            float used_gb;
            float total_gb;
            if (HardwareMonitorHelper::GetGpuMemoryValues(gcnew String(identifier.c_str()), used_gb, total_gb) && total_gb > 0)
                return used_gb / total_gb;
            return 0.0f;
        }
        SensorType type = static_cast<SensorType>(sensor_type);
        if (type == SensorType::Temperature || type == SensorType::Load)
        {
            return item_value_num / 100.0f;
        }
        return 0.0f;
    }

    void CHardwareMonitorItem::UpdateValue()
    {
        //显存项目不是一个真实的传感器，它的数值由“已占用显存”和“总显存”两个传感器组合而成
        if (HardwareMonitorHelper::IsGpuMemoryItemIdentifier(identifier))
        {
            const ItemInfo& item_info{ CHardwareMonitor::GetInstance()->m_settings.FindItemInfo(identifier) };
            String^ unit = item_info.unit.empty() ? gcnew String(L"GB") : gcnew String(item_info.unit.c_str());
            String^ item_identifier = gcnew String(identifier.c_str());
            const auto& cfg{ CHardwareMonitor::GetInstance()->m_tm_settings };
            item_value_main_wnd = Common::StringToStdWstring(HardwareMonitorHelper::GetGpuMemoryValueText(item_identifier, unit, item_info.decimal_places, item_info.show_unit, cfg.main_wnd_sperate_with_space));
            item_value_taskbar = Common::StringToStdWstring(HardwareMonitorHelper::GetGpuMemoryValueText(item_identifier, unit, item_info.decimal_places, item_info.show_unit, cfg.taskbar_sperate_with_space));
            sensor_type = static_cast<int>(SensorType::Data);
            return;
        }

        ISensor^ sensor = HardwareMonitorHelper::FindSensorByIdentifyer(gcnew String(identifier.c_str()));
        if (sensor != nullptr)
        {
            const ItemInfo& item_info{ CHardwareMonitor::GetInstance()->m_settings.FindItemInfo(identifier) };
            String^ unit;
            if (!item_info.unit.empty())
                unit = gcnew String(item_info.unit.c_str());
            else
                unit = HardwareMonitorHelper::GetSensorTypeDefaultUnit(sensor->SensorType);
            const auto& cfg{ CHardwareMonitor::GetInstance()->m_tm_settings };
            item_value_main_wnd = Common::StringToStdWstring(HardwareMonitorHelper::GetSensorValueText(sensor, unit, item_info.decimal_places, item_info.show_unit, cfg.main_wnd_sperate_with_space));
            item_value_taskbar = Common::StringToStdWstring(HardwareMonitorHelper::GetSensorValueText(sensor, unit, item_info.decimal_places, item_info.show_unit, cfg.taskbar_sperate_with_space));
            try
            {
                item_value_num = sensor->Value.Value;
            }
            catch (System::Exception^)
            {
                item_value_num = 0;
            }
            sensor_type = static_cast<int>(sensor->SensorType);

            //判断是否要显示通知
            item_value_with_unit = HardwareMonitorHelper::GetSensorValue(sensor, unit);
            if (item_info.show_notify)
            {
                if (last_item_value_with_unit < item_info.notify_value && item_value_with_unit >= item_info.notify_value)
                {
                    String^ format_str = MonitorGlobal::Instance()->GetString(L"ValueExceedNotifyMsg");
                    String^ value_with_unit = HardwareMonitorHelper::GetSensorValueText(sensor, unit);
                    String^ str_msg = String::Format(format_str, gcnew String(item_name.c_str()), value_with_unit);
                    CHardwareMonitor::GetInstance()->GetMainApp()->ShowNotifyMessage(Common::StringToStdWstring(str_msg).c_str());
                    last_item_value_with_unit = item_value_with_unit;
                }
            }
            last_item_value_with_unit = item_value_with_unit;
        }
    }

    const std::wstring& CHardwareMonitorItem::GetIdentifier() const
    {
        return identifier;
    }
}