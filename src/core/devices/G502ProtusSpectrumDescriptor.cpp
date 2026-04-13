#include "G502ProtusSpectrumDescriptor.h"

namespace logitune {

QString G502ProtusSpectrumDescriptor::deviceName() const {
  return QStringLiteral("Logitech Gaming Mouse G502");
}

std::vector<uint16_t> G502ProtusSpectrumDescriptor::productIds() const {
  return {0xc332}; // USB wired only — no Bolt/BT variants
}

bool G502ProtusSpectrumDescriptor::matchesPid(uint16_t pid) const {
  for (auto id : productIds()) {
    if (id == pid)
      return true;
  }
  return false;
}

QList<ControlDescriptor> G502ProtusSpectrumDescriptor::controls() const {
  return {
      {0x0050, 0, QStringLiteral("Left click"), QStringLiteral("default"),
       false},
      {0x0051, 1, QStringLiteral("Right click"), QStringLiteral("default"),
       false},
      {0x0052, 2, QStringLiteral("Middle click"), QStringLiteral("default"),
       true},
      {0x0053, 3, QStringLiteral("Back"), QStringLiteral("default"), true},
      {0x0056, 4, QStringLiteral("Forward"), QStringLiteral("default"), true},
      {0x005b, 5, QStringLiteral("Scroll left tilt"), QStringLiteral("default"),
       true},
      {0x005d, 6, QStringLiteral("Scroll right tilt"),
       QStringLiteral("default"), true},
      {0x00c5, 7, QStringLiteral("Sniper button"), QStringLiteral("default"),
       true},
      {0x00ed, 8, QStringLiteral("DPI button"), QStringLiteral("default"),
       true},
      {0x00c4, 9, QStringLiteral("Shift wheel mode"),
       QStringLiteral("smartshift-toggle"), true},
  };
}

QList<HotspotDescriptor> G502ProtusSpectrumDescriptor::buttonHotspots() const {
  // TODO: update coords once real device images are added
  // w / h
  return {
      {2, 0.57, 0.32, QStringLiteral("right"), -0.1}, // Middle click
      {3, 0.19, 0.70, QStringLiteral("left"), 0.02},  // Back
      {4, 0.15, 0.55, QStringLiteral("left"), 0.02},  // Forward
      {5, 0.57, 0.32, QStringLiteral("left"), 0.0},   // Scroll left tilt
      {6, 0.57, 0.32, QStringLiteral("right"), 0.03}, // Scroll right tilt
      {7, 0.14, 0.42, QStringLiteral("left"), 0.0},   // Sniper button
      {8, 0.58, 0.53, QStringLiteral("right"), 0.1},  // DPI button
      {9, 0.58, 0.47, QStringLiteral("right"), 0.02}, // Shift wheel mode
  };
}

QList<HotspotDescriptor> G502ProtusSpectrumDescriptor::scrollHotspots() const {
  // TODO: update coords once real device images are added
  // -1 = main scroll wheel, no thumb wheel on G502
  return {
      {-1, 0.57, 0.32, QStringLiteral("right"), 0.0}, // Main scroll wheel
  };
}

FeatureSupport G502ProtusSpectrumDescriptor::features() const {
  FeatureSupport f;
  f.battery = false; // Wired — no battery
  f.adjustableDpi = true;
  f.smartShift = true;
  f.hiResWheel = true;
  f.thumbWheel = false; // No thumb wheel
  f.reprogControls = true;
  f.gestureV2 = false;
  f.smoothScroll = true;
  return f;
}

QString G502ProtusSpectrumDescriptor::frontImagePath() const {
  return QStringLiteral("qrc:/Logitune/qml/assets/g502-proteus-spectrum.png");
}

QString G502ProtusSpectrumDescriptor::sideImagePath() const {
  // Buttons page renders sideImage — use front view so hotspot coords map
  // correctly
  return QStringLiteral("qrc:/Logitune/qml/assets/g502-proteus-spectrum.png");
}

QString G502ProtusSpectrumDescriptor::backImagePath() const {
  return QStringLiteral(
      "qrc:/Logitune/qml/assets/g502-proteus-spectrum-back.png");
}

QMap<QString, ButtonAction>
G502ProtusSpectrumDescriptor::defaultGestures() const {
  return {}; // No gesture button on G502
}

int G502ProtusSpectrumDescriptor::minDpi() const { return 200; }
int G502ProtusSpectrumDescriptor::maxDpi() const { return 12000; }
int G502ProtusSpectrumDescriptor::dpiStep() const { return 50; }

QList<EasySwitchSlotPosition>
G502ProtusSpectrumDescriptor::easySwitchSlotPositions() const {
  return {}; // Wired — no Easy-Switch
}

} // namespace logitune
