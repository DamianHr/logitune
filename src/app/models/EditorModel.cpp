#include "EditorModel.h"
#include "DeviceRegistry.h"
#include <QFile>
#include <QJsonDocument>
#include <QJsonArray>

namespace logitune {

EditorModel::EditorModel(DeviceRegistry *registry, bool editing, QObject *parent)
    : QObject(parent), m_registry(registry), m_editing(editing) {}

bool EditorModel::canUndo() const {
    auto it = m_undoStacks.find(m_activeDevicePath);
    return it != m_undoStacks.end() && !it->isEmpty();
}

bool EditorModel::canRedo() const {
    auto it = m_redoStacks.find(m_activeDevicePath);
    return it != m_redoStacks.end() && !it->isEmpty();
}

void EditorModel::setActiveDevicePath(const QString &path) {
    if (path == m_activeDevicePath) return;
    m_activeDevicePath = path;
    emit activeDevicePathChanged();
    emit dirtyChanged();
    emit undoStateChanged();
}

void EditorModel::ensurePending(const QString &path) {
    if (m_pendingEdits.contains(path)) return;
    QFile f(path + QStringLiteral("/descriptor.json"));
    if (!f.open(QIODevice::ReadOnly)) return;
    m_pendingEdits[path] = QJsonDocument::fromJson(f.readAll()).object();
}

void EditorModel::pushCommand(EditCommand cmd) {
    m_undoStacks[m_activeDevicePath].push(std::move(cmd));
    m_redoStacks[m_activeDevicePath].clear();
    m_dirty.insert(m_activeDevicePath);
    emit dirtyChanged();
    emit undoStateChanged();
}

void EditorModel::updateSlotPosition(int idx, double xPct, double yPct) {
    if (m_activeDevicePath.isEmpty()) return;
    ensurePending(m_activeDevicePath);
    QJsonObject &obj = m_pendingEdits[m_activeDevicePath];
    QJsonArray slotsArr = obj.value(QStringLiteral("easySwitchSlots")).toArray();
    if (idx < 0 || idx >= slotsArr.size()) return;

    EditCommand cmd;
    cmd.kind = EditCommand::SlotMove;
    cmd.index = idx;
    cmd.before = slotsArr[idx];

    QJsonObject slot = slotsArr[idx].toObject();
    slot[QStringLiteral("xPct")] = xPct;
    slot[QStringLiteral("yPct")] = yPct;
    slotsArr[idx] = slot;
    obj[QStringLiteral("easySwitchSlots")] = slotsArr;
    cmd.after = slotsArr[idx];

    pushCommand(std::move(cmd));
}

void EditorModel::updateHotspot(int idx, double xPct, double yPct,
                                 const QString &side, double labelOffsetYPct) {
    if (m_activeDevicePath.isEmpty()) return;
    ensurePending(m_activeDevicePath);
    QJsonObject &obj = m_pendingEdits[m_activeDevicePath];
    QJsonObject hotspots = obj.value(QStringLiteral("hotspots")).toObject();
    QJsonArray buttons = hotspots.value(QStringLiteral("buttons")).toArray();
    if (idx < 0 || idx >= buttons.size()) return;

    EditCommand cmd;
    cmd.kind = EditCommand::HotspotMove;
    cmd.index = idx;
    cmd.before = buttons[idx];

    QJsonObject hs = buttons[idx].toObject();
    hs[QStringLiteral("xPct")] = xPct;
    hs[QStringLiteral("yPct")] = yPct;
    hs[QStringLiteral("side")] = side;
    hs[QStringLiteral("labelOffsetYPct")] = labelOffsetYPct;
    buttons[idx] = hs;
    hotspots[QStringLiteral("buttons")] = buttons;
    obj[QStringLiteral("hotspots")] = hotspots;
    cmd.after = buttons[idx];

    pushCommand(std::move(cmd));
}

} // namespace logitune
