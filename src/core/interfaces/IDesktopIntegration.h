#pragma once
#include <QObject>
#include <QString>
#include <QStringList>
#include <QVariantList>

namespace logitune {

class IDesktopIntegration : public QObject {
    Q_OBJECT
public:
    using QObject::QObject;
    virtual ~IDesktopIntegration() = default;

    virtual void start() = 0;
    virtual bool available() const = 0;
    virtual QString desktopName() const = 0;
    virtual QStringList detectedCompositors() const = 0;

    /// Block/unblock global shortcuts during keystroke capture.
    virtual void blockGlobalShortcuts(bool block) = 0;

    /// Return list of running graphical applications.
    /// Each entry is a QVariantMap with keys "wmClass" and "title".
    virtual QVariantList runningApplications() const = 0;

signals:
    void activeWindowChanged(const QString &wmClass, const QString &title);
};

} // namespace logitune
