#pragma once
#include "interfaces/IDesktopIntegration.h"
#include <QDBusInterface>
#include <QTimer>

namespace logitune {

class KDeDesktop : public IDesktopIntegration {
    Q_OBJECT
public:
    explicit KDeDesktop(QObject *parent = nullptr);

    void start() override;
    bool available() const override;
    QString desktopName() const override;
    QStringList detectedCompositors() const override;
    void blockGlobalShortcuts(bool block) override;
    QVariantList runningApplications() const override;

public slots:
    // Called by KWin focus watcher script via D-Bus
    void focusChanged(const QString &wmClass, const QString &title);

private slots:
    void onActiveWindowChanged();
    void pollActiveWindow();

private:
    QDBusInterface *m_kwin = nullptr;
    QTimer *m_pollTimer = nullptr;
    QString m_lastWmClass;
    bool m_available = false;
};

} // namespace logitune
