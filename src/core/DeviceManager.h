#pragma once
#include "hidpp/HidppTypes.h"
#include "hidpp/HidrawDevice.h"
#include "hidpp/Transport.h"
#include "hidpp/FeatureDispatcher.h"
#include <QObject>
#include <QThread>
#include <QSocketNotifier>
#include <memory>

struct udev;
struct udev_monitor;

namespace logitune {

class DeviceManager : public QObject {
    Q_OBJECT
    Q_PROPERTY(bool deviceConnected READ deviceConnected NOTIFY deviceConnectedChanged)
    Q_PROPERTY(QString deviceName READ deviceName NOTIFY deviceNameChanged)
    Q_PROPERTY(int batteryLevel READ batteryLevel NOTIFY batteryLevelChanged)
    Q_PROPERTY(bool batteryCharging READ batteryCharging NOTIFY batteryChargingChanged)
    Q_PROPERTY(QString connectionType READ connectionType NOTIFY connectionTypeChanged)

public:
    explicit DeviceManager(QObject *parent = nullptr);
    ~DeviceManager();

    void start();

    // Static helpers (testable without hardware)
    static bool isReceiver(uint16_t pid);
    static bool isDirectDevice(uint16_t pid);
    static uint8_t deviceIndexForDirect();
    static uint8_t deviceIndexForReceiver(int slot);

    // Properties
    bool deviceConnected() const;
    QString deviceName() const;
    int batteryLevel() const;
    bool batteryCharging() const;
    QString connectionType() const;

    // Access to internals for other components
    hidpp::FeatureDispatcher *features() const;
    hidpp::Transport *transport() const;
    uint8_t deviceIndex() const;

signals:
    void deviceConnectedChanged();
    void deviceNameChanged();
    void batteryLevelChanged();
    void batteryChargingChanged();
    void connectionTypeChanged();
    void deviceDisconnected();
    void transportSwitched(const QString &newType);
    void divertedButtonPressed(uint16_t controlId, bool pressed);
    void gestureEvent(int dx, int dy, bool released);
    void deviceWoke();

private slots:
    void onUdevReady();

private:
    void scanExistingDevices();
    void onUdevEvent(const QString &action, const QString &devNode);
    void probeDevice(const QString &devNode);
    void disconnectDevice();
    void handleNotification(const hidpp::Report &report);
    void startIoThread();
    void stopIoThread();
    void enumerateAndSetup();

    // Sleep/wake
    void checkSleepWake();
    qint64 m_lastResponseTime = 0;

    // Transport failover
    QStringList m_availableTransports;

    std::unique_ptr<hidpp::HidrawDevice> m_device;
    std::unique_ptr<hidpp::Transport> m_transport;
    std::unique_ptr<hidpp::FeatureDispatcher> m_features;
    QThread m_ioThread;

    struct udev *m_udev = nullptr;
    struct udev_monitor *m_udevMon = nullptr;
    QSocketNotifier *m_udevNotifier = nullptr;
    uint8_t m_deviceIndex = 0xFF;

    // Cached state
    bool m_connected = false;
    QString m_deviceName;
    int m_batteryLevel = 0;
    bool m_batteryCharging = false;
    QString m_connectionType; // "Bolt" or "Bluetooth"
};

} // namespace logitune
