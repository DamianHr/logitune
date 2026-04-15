#include <gtest/gtest.h>
#include <QSignalSpy>
#include <QTemporaryDir>
#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <QJsonDocument>
#include "EditorModel.h"
#include "DeviceRegistry.h"

namespace {
QString writeMinimalDescriptor(const QString &dir) {
    QDir().mkpath(dir);
    QFile f(dir + QStringLiteral("/descriptor.json"));
    f.open(QIODevice::WriteOnly | QIODevice::Truncate);
    f.write(R"({
  "name": "Tester",
  "status": "community-local",
  "productIds": ["0xffff"],
  "features": {},
  "controls": [],
  "hotspots": {"buttons": [], "scroll": []},
  "images": {},
  "easySwitchSlots": [
    {"xPct": 0.10, "yPct": 0.20},
    {"xPct": 0.30, "yPct": 0.40}
  ]
})");
    f.close();
    return QFileInfo(dir).canonicalFilePath();
}
}

TEST(EditorModel, EditingFlagAndInitialState) {
    logitune::DeviceRegistry reg;
    logitune::EditorModel m(&reg, /*editing=*/true);
    EXPECT_TRUE(m.editing());
    EXPECT_FALSE(m.hasUnsavedChanges());
    EXPECT_FALSE(m.canUndo());
    EXPECT_FALSE(m.canRedo());
    EXPECT_TRUE(m.activeDevicePath().isEmpty());
}

TEST(EditorModel, ActiveDevicePathSetterEmitsSignals) {
    logitune::DeviceRegistry reg;
    logitune::EditorModel m(&reg, true);
    QSignalSpy pathSpy(&m, &logitune::EditorModel::activeDevicePathChanged);
    QSignalSpy dirtySpy(&m, &logitune::EditorModel::dirtyChanged);
    m.setActiveDevicePath(QStringLiteral("/tmp/foo"));
    EXPECT_EQ(pathSpy.count(), 1);
    EXPECT_EQ(dirtySpy.count(), 1);
    EXPECT_EQ(m.activeDevicePath(), QStringLiteral("/tmp/foo"));
}

TEST(EditorModel, UpdateSlotPositionMutatesPendingAndPushesUndo) {
    QTemporaryDir tmp;
    ASSERT_TRUE(tmp.isValid());
    const QString path = writeMinimalDescriptor(tmp.path() + QStringLiteral("/dev"));

    logitune::DeviceRegistry reg;
    logitune::EditorModel m(&reg, true);
    m.setActiveDevicePath(path);

    QSignalSpy dirtySpy(&m, &logitune::EditorModel::dirtyChanged);
    QSignalSpy undoSpy(&m, &logitune::EditorModel::undoStateChanged);

    m.updateSlotPosition(1, 0.50, 0.60);

    EXPECT_TRUE(m.hasUnsavedChanges());
    EXPECT_TRUE(m.canUndo());
    EXPECT_FALSE(m.canRedo());
    EXPECT_GE(dirtySpy.count(), 1);
    EXPECT_GE(undoSpy.count(), 1);
}
