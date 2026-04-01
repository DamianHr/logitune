# Getting Started

## Installation

### Flatpak (Recommended)

Download the latest `.flatpak` bundle from the [Releases](https://github.com/logitune/logitune/releases) page:

```bash
flatpak install --user logitune.flatpak
flatpak run com.logitune.Logitune
```

The Flatpak manifest (`com.logitune.Logitune.yml`) uses the KDE 6.10 runtime and SDK. It requests access to `--device=all` for hidraw and registers the D-Bus name `com.logitune.app` for KWin focus tracking.

### From Source

See [Building](Building) for full instructions. The short version:

```bash
git clone https://github.com/logitune/logitune.git
cd logitune
make build
make run
```

### Arch Linux (AUR)

An AUR package is planned for future releases.

## First Run

### udev Rules

Logitune needs read/write access to hidraw devices and uinput for keystroke injection. A udev rules file is included at `data/71-logitune.rules`:

```
SUBSYSTEM=="hidraw", ATTRS{idVendor}=="046d", TAG+="uaccess"
KERNEL=="uinput", SUBSYSTEM=="misc", TAG+="uaccess"
```

Install it:

```bash
sudo cp data/71-logitune.rules /etc/udev/rules.d/
sudo udevadm control --reload-rules
sudo udevadm trigger
```

The `uaccess` tag grants access to the currently logged-in user without requiring `root` or group membership. After installing the rules, unplug and replug your device (or reboot).

### Permissions Check

If Logitune starts but shows "No device connected":

1. **Check hidraw access**: `ls -la /dev/hidraw*` — your user should have `rw` access to Logitech devices
2. **Check uinput access**: `ls -la /dev/uinput` — needed for keystroke injection
3. **Check the device is detected**: Run with `--debug` flag to see device scanning logs

```bash
./build/src/app/logitune --debug
```

### Single-Instance Guard

Logitune uses a lock file at `$TMPDIR/logitune.lock` to prevent two instances from fighting over the device. If a previous instance crashed without cleanup, delete the lock file manually:

```bash
rm /tmp/logitune.lock
```

## UI Overview

The application window is organized into four main areas:

### Sidebar Navigation

A vertical navigation bar on the left with icons for each page:

| Icon | Page | Description |
|------|------|-------------|
| Mouse | **Point & Scroll** | DPI slider, SmartShift toggle/threshold, hi-res scroll, natural scrolling |
| Grid | **Buttons** | Interactive device render with hotspot callouts for each button, plus action picker panel |
| Switch | **Easy-Switch** | View paired hosts, active slot |
| Gear | **Settings** | Debug logging toggle, bug report button, about info |

### Device Render

The Buttons page shows a rendered image of the device (front view by default, side view available). Interactive hotspots are overlaid at positions defined by the device descriptor — clicking a hotspot opens the action picker for that button.

### Profile Bar

A horizontal tab bar at the bottom of the window:

- **Default** tab is always present — this profile is used for applications without a specific binding
- **App profiles** appear as additional tabs with the application's icon
- The **hardware-active** profile (the one currently applied to the device) is indicated with a highlight
- The **display** profile (the one you're currently viewing/editing) may differ from the hardware-active profile
- Click the **+** button to add a new app profile from the list of installed applications

### Settings Storage

Profiles are stored per-device under:

```
~/.config/Logitune/devices/<device-serial>/profiles/
```

Each profile is a `.conf` file (QSettings INI format). App bindings are stored in `app-bindings.conf` in the same directory.

## Tray Icon

Logitune runs as a tray application — closing the window hides it to the tray rather than quitting. The tray menu shows:

- **Battery level** — e.g., "Battery: 85%"
- **Show** — brings the window back
- **Quit** — exits the application

The application sets `quitOnLastWindowClosed(false)` so the tray icon keeps the event loop alive.

## Command-Line Options

| Flag | Description |
|------|-------------|
| `--debug` | Enable debug logging to file and console at startup |

Debug logging can also be toggled at runtime from the Settings page.

## What Happens at Startup

1. **Single-instance check** — attempts to acquire `logitune.lock`
2. **Log manager init** — sets up Qt logging categories, optional file output
3. **Crash recovery** — checks for previous unclean shutdown, offers to file a bug report
4. **AppController init** — creates DeviceManager, ProfileEngine, models, wires signals
5. **QML engine load** — registers model singletons, loads `Main.qml`
6. **Start monitoring** — DeviceManager begins udev scanning and KDE desktop starts focus tracking
7. **Device connect** — on first hidraw match, enumerates HID++ features, reads state, creates command queue
8. **Profile load** — loads or seeds the default profile, applies settings to hardware
9. **Tray icon** — system tray icon appears with battery info
