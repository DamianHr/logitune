#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include "DeviceManager.h"
#include "models/DeviceModel.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);
    app.setOrganizationName("Logitune");
    app.setApplicationName("Logitune");

    // Create backend
    logitune::DeviceManager deviceManager;
    logitune::DeviceModel deviceModel;
    deviceModel.setDeviceManager(&deviceManager);

    QQmlApplicationEngine engine;

    // Register singleton — provide the existing instance
    qmlRegisterSingletonInstance("Logitune", 1, 0, "DeviceModel", &deviceModel);

    engine.loadFromModule("Logitune", "Main");

    if (engine.rootObjects().isEmpty())
        return -1;

    // Start device monitoring after QML is loaded
    deviceManager.start();

    return app.exec();
}
