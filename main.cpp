#include <QQmlApplicationEngine>
#include <QApplication>
#include <QQmlContext>
#include "qmlcppbridge.h"
#include "cvcamview/CVCamera.h"

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QApplication app(argc, argv);
    qmlRegisterType<QMLCppBridge>("com.mrmmsmsa", 1, 0, "QMLCppBridge");
    qmlRegisterType(QUrl("qrc:/ToastManager.qml"), "com.mrmmsmsa", 1, 0, "ToastManager");
    qmlRegisterType<CVCamera>("com.mrmmsmsa", 1, 0, "CVCamera");

    QQmlApplicationEngine engine;
    const QUrl url(QStringLiteral("qrc:/main.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);
    engine.load(url);

    return app.exec();
}
