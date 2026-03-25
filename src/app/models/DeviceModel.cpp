#include "DeviceModel.h"

namespace logitune {

DeviceModel::DeviceModel(QObject *parent)
    : QObject(parent)
{
}

void DeviceModel::setDeviceManager(DeviceManager *dm)
{
    m_dm = dm;

    connect(dm, &DeviceManager::deviceConnectedChanged,
            this, &DeviceModel::deviceConnectedChanged);
    connect(dm, &DeviceManager::deviceNameChanged,
            this, &DeviceModel::deviceNameChanged);
    connect(dm, &DeviceManager::batteryLevelChanged,
            this, &DeviceModel::batteryLevelChanged);
    connect(dm, &DeviceManager::batteryChargingChanged,
            this, &DeviceModel::batteryChargingChanged);
    connect(dm, &DeviceManager::connectionTypeChanged,
            this, &DeviceModel::connectionTypeChanged);
}

bool DeviceModel::deviceConnected() const
{
    return m_dm ? m_dm->deviceConnected() : false;
}

QString DeviceModel::deviceName() const
{
    return m_dm ? m_dm->deviceName() : QString();
}

int DeviceModel::batteryLevel() const
{
    return m_dm ? m_dm->batteryLevel() : 0;
}

bool DeviceModel::batteryCharging() const
{
    return m_dm ? m_dm->batteryCharging() : false;
}

QString DeviceModel::connectionType() const
{
    return m_dm ? m_dm->connectionType() : QString();
}

} // namespace logitune
