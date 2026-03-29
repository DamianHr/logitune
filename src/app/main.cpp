#include <QApplication>
#include <QDir>
#include <QDirIterator>
#include <signal.h>
#include <QQmlApplicationEngine>
#include <QSystemTrayIcon>
#include <QMenu>
#include <QAction>
#include <QQuickWindow>
#include <QQuickImageProvider>
#include <QIcon>
#include <QDebug>
#include "AppController.h"

int main(int argc, char *argv[])
{
    // Ignore SIGPIPE — hidraw writes to wrong interface cause EPIPE,
    // and without this the process terminates on the first failed write.
    signal(SIGPIPE, SIG_IGN);

    qInstallMessageHandler([](QtMsgType type, const QMessageLogContext &ctx, const QString &msg) {
        fprintf(stderr, "[Qt %d] %s:%d: %s\n", type, ctx.file ? ctx.file : "?", ctx.line, qPrintable(msg));
    });
    fprintf(stderr, "[logitune] starting...\n");
    QApplication app(argc, argv);
    fprintf(stderr, "[logitune] QApplication created\n");
    app.setOrganizationName("Logitune");
    app.setApplicationName("Logitune");

    // Detect dark mode from system palette
    QColor windowBg = app.palette().window().color();
    double lum = windowBg.redF() * 0.299 + windowBg.greenF() * 0.587 + windowBg.blueF() * 0.114;
    bool isDark = lum < 0.5;
    fprintf(stderr, "[logitune] palette bg: r=%.2f g=%.2f b=%.2f lum=%.2f dark=%d\n",
            windowBg.redF(), windowBg.greenF(), windowBg.blueF(), lum, isDark);

    // ── AppController — owns all subsystems and business logic ───────────
    fprintf(stderr, "[logitune] creating AppController...\n");
    logitune::AppController controller;
    controller.init();

    // ── QML engine ──────────────────────────────────────────────────────
    fprintf(stderr, "[logitune] creating QML engine...\n");
    QQmlApplicationEngine engine;

    // Icon theme provider for QML — resolves icon names to pixmaps
    class IconProvider : public QQuickImageProvider {
    public:
        IconProvider() : QQuickImageProvider(QQuickImageProvider::Pixmap) {}
        QPixmap requestPixmap(const QString &id, QSize *size, const QSize &requestedSize) override {
            QIcon icon = QIcon::fromTheme(id);
            QSize s = requestedSize.isValid() ? requestedSize : QSize(22, 22);
            QPixmap pm = icon.pixmap(s);
            if (size) *size = pm.size();
            return pm;
        }
    };
    engine.addImageProvider(QStringLiteral("icon"), new IconProvider);

    qmlRegisterSingletonInstance("Logitune", 1, 0, "DeviceModel",  controller.deviceModel());
    qmlRegisterSingletonInstance("Logitune", 1, 0, "ButtonModel",  controller.buttonModel());
    qmlRegisterSingletonInstance("Logitune", 1, 0, "ActionModel",  controller.actionModel());
    qmlRegisterSingletonInstance("Logitune", 1, 0, "ProfileModel", controller.profileModel());

    QObject::connect(&engine, &QQmlApplicationEngine::objectCreationFailed,
        [](const QUrl &url) { fprintf(stderr, "QML CREATION FAILED: %s\n", qPrintable(url.toString())); });
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
        [](QObject *obj, const QUrl &url) { fprintf(stderr, "QML CREATED: %s obj=%p\n", qPrintable(url.toString()), obj); });

    // Debug: list QRC resources
    {
        QDirIterator it(":", QDirIterator::Subdirectories);
        while (it.hasNext()) {
            QString p = it.next();
            if (p.contains("Logitune") || p.contains("Main") || p.contains("qml"))
                fprintf(stderr, "  QRC: %s\n", qPrintable(p));
        }
    }
    fprintf(stderr, "[logitune] loading QML...\n");
    engine.load(QUrl(QStringLiteral("qrc:/Logitune/qml/Main.qml")));
    fprintf(stderr, "[logitune] QML loaded, root objects: %d\n", (int)engine.rootObjects().size());

    if (engine.rootObjects().isEmpty()) {
        qCritical() << "No root objects loaded — QML module failed to initialize";
        return -1;
    }

    // Set Theme.dark from detected system palette
    if (auto *theme = engine.singletonInstance<QObject*>("Logitune", "Theme")) {
        theme->setProperty("dark", isDark);
        qDebug() << "[main] Theme.dark set to" << isDark;
    }

    // Start device monitoring and window tracking after QML is loaded
    controller.startMonitoring();

    // ── System tray ─────────────────────────────────────────────────────
    QSystemTrayIcon trayIcon;
    trayIcon.setIcon(QIcon::fromTheme("input-mouse"));
    trayIcon.setToolTip("Logitune - MX Master 3S");

    QMenu trayMenu;
    QAction *showAction = trayMenu.addAction("Show Logitune");
    trayMenu.addSeparator();

    QAction *batteryAction = trayMenu.addAction("Battery: ---%");
    batteryAction->setEnabled(false);

    trayMenu.addSeparator();
    QAction *quitAction = trayMenu.addAction("Quit");

    trayIcon.setContextMenu(&trayMenu);
    trayIcon.show();

    auto showWindow = [&engine]() {
        for (auto *obj : engine.rootObjects()) {
            if (auto *window = qobject_cast<QQuickWindow*>(obj)) {
                window->show();
                window->raise();
                window->requestActivate();
            }
        }
    };

    QObject::connect(showAction, &QAction::triggered, showWindow);
    QObject::connect(quitAction, &QAction::triggered, &app, &QApplication::quit);

    QObject::connect(&trayIcon, &QSystemTrayIcon::activated,
        [showWindow](QSystemTrayIcon::ActivationReason reason) {
            if (reason == QSystemTrayIcon::Trigger)
                showWindow();
        });

    // Battery updates -> tray text
    QObject::connect(controller.deviceModel(), &logitune::DeviceModel::batteryLevelChanged,
        [batteryAction, dm = controller.deviceModel()]() {
            batteryAction->setText(dm->batteryStatusText());
        });

    return app.exec();
}
