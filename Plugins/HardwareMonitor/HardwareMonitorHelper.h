#pragma once
#include <string>

namespace HardwareMonitor
{
    class HardwareMonitorHelper
    {
    public:
        //获取传感器类型名称
        static System::String^ GetSensorTypeName(LibreHardwareMonitor::Hardware::SensorType type);

        //根据硬件类型获取图标资源名称
        static System::String^ GetHardwareIconResName(LibreHardwareMonitor::Hardware::HardwareType type);

        //根据传感器类型获取数据的单位列表
        //可能会有多个可用单位，默认的单位总是为列表的第一个元素
        static System::Collections::Generic::List<System::String^>^ GetSensorTypeUnit(LibreHardwareMonitor::Hardware::SensorType type);

        //根据传感器类型获取数据的默认单位
        static System::String^ GetSensorTypeDefaultUnit(LibreHardwareMonitor::Hardware::SensorType type);

        //根据一个传感器的唯一标识符查找一个传感器
        static LibreHardwareMonitor::Hardware::ISensor^ FindSensorByIdentifyer(System::String^ identifyer);

        //根据指定单位，获取一个传感器数值
        static float GetSensorValue(LibreHardwareMonitor::Hardware::ISensor^ sensor, System::String^ unit);

        //获取一个传感器数值的文本，并指定小数位数
        static System::String^ GetSensorValueText(LibreHardwareMonitor::Hardware::ISensor^ sensor, System::String^ unit, int decimal_place = 2, bool show_unit = true, bool seperate_with_space = true);

        //获取一个传感器的显示名称（由硬件、传感器类型、传感器组成）
        static System::String^ GetSensorDisplayName(LibreHardwareMonitor::Hardware::ISensor^ sensor);

        //获取一个传感器的唯一标识符
        static System::String^ GetSensorIdentifyer(LibreHardwareMonitor::Hardware::ISensor^ sensor);

        //获取默认的监控项目
        static void GetDefaultMonitorItem(System::Collections::Generic::List<LibreHardwareMonitor::Hardware::ISensor^>^ default_sensors);

        //////////////////////////////////////////////////////////////////////////
        // 显卡显存项目（已占用显存/总显存）
        //////////////////////////////////////////////////////////////////////////

        //判断一个硬件是否为显卡
        static bool IsGpuHardware(LibreHardwareMonitor::Hardware::IHardware^ hardware);

        //获取显卡显存项目的标识符（它不是真实的传感器，而是一个虚拟的监控项目）
        static System::String^ GetGpuMemoryItemIdentifier(LibreHardwareMonitor::Hardware::IHardware^ hardware);

        //判断一个标识符是否为显卡显存项目
        static bool IsGpuMemoryItemIdentifier(System::String^ identifier);
        static bool IsGpuMemoryItemIdentifier(const std::wstring& identifier);

        //从显存项目的标识符中获取显卡硬件的标识符
        static System::String^ GetGpuMemoryHardwareIdentifier(System::String^ identifier);

        //根据硬件标识符查找硬件
        static LibreHardwareMonitor::Hardware::IHardware^ FindHardwareByIdentifier(System::String^ identifier);

        //判断一个显卡是否有可用的显存传感器
        static bool HasGpuMemorySensors(LibreHardwareMonitor::Hardware::IHardware^ hardware);

        //获取显卡的已占用显存和总显存，单位为GB
        static bool GetGpuMemoryValues(System::String^ identifier, float% used_gb, float% total_gb);

        //获取显存项目的数值文本，unit可以为GB或MB
        static System::String^ GetGpuMemoryValueText(System::String^ identifier, System::String^ unit, int decimal_place = 1, bool show_unit = true, bool seperate_with_space = true);

        //获取显存项目的显示名称，格式为“显卡名称/显存已占用/总显存”
        static System::String^ GetGpuMemoryDisplayName(System::String^ identifier);

        //获取显存项目的标签文本
        static System::String^ GetGpuMemoryLabel();
    };
}

