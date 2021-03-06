# Plugin TrayIcon
Rainmeter plugin to create tray icon for your skin

## Limitations:
- There can only be one tray icon per skin.
- Only `.ico` files are supported.

## Options:
- `IconName`: path to the icon file to be shown
- `ToolTipText`: text to be shown when tray icon is hovered
- `LeftMouseUpAction`
- `RightMouseUpAction`
- `MouseScrollUpAction`
- `MouseScrollDownAction`

## Example:
```ini
[MeasureTray]
Measure=Plugin
Plugin=TrayIcon
; Only .ico files
IconName=path\to\icon.ico
ToolTipText=TrayIcon Plugin Example
LeftMouseUpAction=[!Log "left click"]
RightMouseUpAction=[!Log "right click"]
MouseScrollUpAction=[!Log "scroll up"]
MouseScrollDownAction=[!Log "scroll down"]
DynamicVariables=1
```

## Build Instructions:
- Clone the repo.
- Download Visual Studio 2019 or up (if you don't have it already)
- Open PluginTrayIcon.sln and build the PluginTrayIcon project.
