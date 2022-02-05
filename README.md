# PluginTrayIcon
Rainmeter plugin to create tray icon for your skin

## Restrictions:
Due to my lack of knowledge, I couldn't find a way to load images of other formats. So only `.ico` files are supported.

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
