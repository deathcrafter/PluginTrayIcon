# PluginTrayIcon
Rainmeter plugin to create tray icon for your skin

## Usage:
```ini
[MeasureTray]
Measure=Plugin
Plugin=TrayIcon
IconName=path\to\icon
ToolTipText=TrayIcon Plugin Example
LeftMouseUpAction=[!ToggleFade]
RightMouseUpAction=[!SkinMenu]
DynamicVariables=1
```

## Build Instructions:
- Clone [rainmeter-plugins-sdk](https://github.com/rainmeter/rainmeter-plugin-sdk)
- Place the source files in PluginEmpty under C++.
- Rename the PluginTrayIcon.cpp to PluginEmpty.cpp.
- Change build target names.
- Build.
