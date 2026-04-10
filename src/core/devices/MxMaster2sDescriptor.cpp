#include "MxMaster2sDescriptor.h"

namespace logitune {

QString MxMaster2sDescriptor::deviceName() const
{
    return QStringLiteral("MX Master 2S");
}

std::vector<uint16_t> MxMaster2sDescriptor::productIds() const
{
    return { 0xb034 };  // Bolt receiver-reported PID; BT PID discovered via device name
}

bool MxMaster2sDescriptor::matchesPid(uint16_t pid) const
{
    for (auto id : productIds()) {
        if (id == pid)
            return true;
    }
    return false;
}

QList<ControlDescriptor> MxMaster2sDescriptor::controls() const
{
    return {
        { 0x0050, 0, QStringLiteral("Left click"),        QStringLiteral("default"),          false },
        { 0x0051, 1, QStringLiteral("Right click"),       QStringLiteral("default"),          false },
        { 0x0052, 2, QStringLiteral("Middle click"),      QStringLiteral("default"),          true  },
        { 0x0053, 3, QStringLiteral("Back"),              QStringLiteral("default"),          true  },
        { 0x0056, 4, QStringLiteral("Forward"),           QStringLiteral("default"),          true  },
        { 0x00C3, 5, QStringLiteral("Gesture button"),    QStringLiteral("gesture-trigger"),  true  },
        { 0x00C4, 6, QStringLiteral("Shift wheel mode"),  QStringLiteral("smartshift-toggle"),true  },
        { 0x0000, 7, QStringLiteral("Thumb wheel"),       QStringLiteral("default"),          true  },
    };
}

QList<HotspotDescriptor> MxMaster2sDescriptor::buttonHotspots() const
{
    return {
        { 2, 0.71, 0.15,  QStringLiteral("right"), 0.0  },
        { 6, 0.81, 0.34,  QStringLiteral("right"), 0.0  },
        { 7, 0.55, 0.515, QStringLiteral("right"), 0.0  },
        { 4, 0.35, 0.43,  QStringLiteral("left"),  0.0  },
        { 3, 0.45, 0.60,  QStringLiteral("left"),  0.20 },
        { 5, 0.08, 0.58,  QStringLiteral("left"),  0.0  },
    };
}

QList<HotspotDescriptor> MxMaster2sDescriptor::scrollHotspots() const
{
    return {
        { -1, 0.73, 0.16, QStringLiteral("right"), 0.0 },
        { -2, 0.55, 0.51, QStringLiteral("left"),  0.0 },
        { -3, 0.83, 0.54, QStringLiteral("right"), 0.0 },
    };
}

FeatureSupport MxMaster2sDescriptor::features() const
{
    FeatureSupport f;
    f.battery        = true;
    f.adjustableDpi  = true;
    f.smartShift     = true;
    f.hiResWheel     = true;
    f.thumbWheel     = true;
    f.reprogControls = true;
    f.gestureV2      = false;
    return f;
}

QString MxMaster2sDescriptor::frontImagePath() const
{
    return QStringLiteral("qrc:/Logitune/qml/assets/mx-master-2s.png");
}

QString MxMaster2sDescriptor::sideImagePath() const
{
    return QStringLiteral("qrc:/Logitune/qml/assets/mx-master-2s-side.png");
}

QString MxMaster2sDescriptor::backImagePath() const
{
    return QStringLiteral("qrc:/Logitune/qml/assets/mx-master-2s-back.png");
}

QMap<QString, ButtonAction> MxMaster2sDescriptor::defaultGestures() const
{
    QMap<QString, ButtonAction> g;
    g[QStringLiteral("up")]    = { ButtonAction::Default,   {} };
    g[QStringLiteral("down")]  = { ButtonAction::Keystroke, QStringLiteral("Super+D") };
    g[QStringLiteral("left")]  = { ButtonAction::Keystroke, QStringLiteral("Ctrl+Super+Left") };
    g[QStringLiteral("right")] = { ButtonAction::Keystroke, QStringLiteral("Ctrl+Super+Right") };
    g[QStringLiteral("click")] = { ButtonAction::Keystroke, QStringLiteral("Super+W") };
    return g;
}

int MxMaster2sDescriptor::minDpi() const  { return 200; }
int MxMaster2sDescriptor::maxDpi() const  { return 4000; }
int MxMaster2sDescriptor::dpiStep() const { return 50; }

QList<EasySwitchSlotPosition> MxMaster2sDescriptor::easySwitchSlotPositions() const
{
    return {
        { 0.325, 0.658 }, // 1
        { 0.384, 0.642 }, // 2
        { 0.443, 0.643 }, // 3
    };
}

} // namespace logitune
