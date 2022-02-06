# Plugin TrayIcon
Rainmeter plugin to create tray icon for your skin

## Limitations:
- There can only be one tray icon per skin.
- Only `.ico` files are supported.

## Usage:
```ini
[MeasureTray]
Measure=Plugin
Plugin=TrayIcon
; Only .ico files
IconName=path\to\icon.ico
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
