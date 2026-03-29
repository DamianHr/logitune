#include "desktop/GenericDesktop.h"
#include <QDir>
#include <QFileInfo>
#include <QSettings>
#include <QSet>
#include <algorithm>

namespace logitune {

GenericDesktop::GenericDesktop(QObject *parent)
    : IDesktopIntegration(parent)
{
}

void GenericDesktop::start()
{
    // No-op: generic fallback has no window tracking
}

bool GenericDesktop::available() const
{
    return true;
}

QString GenericDesktop::desktopName() const
{
    return QStringLiteral("Generic");
}

QStringList GenericDesktop::detectedCompositors() const
{
    return {};
}

void GenericDesktop::blockGlobalShortcuts(bool)
{
    // No-op: generic fallback cannot block global shortcuts
}

QVariantList GenericDesktop::runningApplications() const
{
    // Same .desktop file scan works on any Linux DE
    QVariantList result;
    QSet<QString> seen;

    const QStringList dirs = {
        QStringLiteral("/usr/share/applications"),
        QDir::homePath() + QStringLiteral("/.local/share/applications"),
        QStringLiteral("/var/lib/flatpak/exports/share/applications"),
        QDir::homePath() + QStringLiteral("/.local/share/flatpak/exports/share/applications"),
        QStringLiteral("/var/lib/snapd/desktop/applications")
    };

    for (const QString &dir : dirs) {
        QDir d(dir);
        if (!d.exists()) continue;

        const QStringList files = d.entryList({QStringLiteral("*.desktop")}, QDir::Files);
        for (const QString &file : files) {
            QSettings desktop(d.filePath(file), QSettings::IniFormat);
            desktop.beginGroup(QStringLiteral("Desktop Entry"));

            QString type = desktop.value(QStringLiteral("Type")).toString();
            if (type != QStringLiteral("Application")) continue;
            if (desktop.value(QStringLiteral("NoDisplay")).toBool()) continue;

            QString name = desktop.value(QStringLiteral("Name")).toString();
            QString wmClass = desktop.value(QStringLiteral("StartupWMClass")).toString();
            QString icon = desktop.value(QStringLiteral("Icon")).toString();

            if (wmClass.isEmpty())
                wmClass = QFileInfo(file).completeBaseName();

            if (name.isEmpty() || seen.contains(wmClass.toLower()))
                continue;

            seen.insert(wmClass.toLower());
            QVariantMap entry;
            entry[QStringLiteral("wmClass")] = wmClass;
            entry[QStringLiteral("title")] = name;
            entry[QStringLiteral("icon")] = icon;
            result.append(entry);
        }
    }

    std::sort(result.begin(), result.end(), [](const QVariant &a, const QVariant &b) {
        return a.toMap()[QStringLiteral("title")].toString().toLower()
             < b.toMap()[QStringLiteral("title")].toString().toLower();
    });

    return result;
}

} // namespace logitune
