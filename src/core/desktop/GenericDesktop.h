#pragma once
#include "interfaces/IDesktopIntegration.h"

namespace logitune {

class GenericDesktop : public IDesktopIntegration {
    Q_OBJECT
public:
    explicit GenericDesktop(QObject *parent = nullptr);

    void start() override;
    bool available() const override;
    QString desktopName() const override;
    QStringList detectedCompositors() const override;
    void blockGlobalShortcuts(bool block) override;
    QVariantList runningApplications() const override;
};

} // namespace logitune
