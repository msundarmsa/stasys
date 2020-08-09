#include <QQmlApplicationEngine>
#include <QApplication>
#include <QQmlContext>
#include "qmlcppbridge.h"
#include <QDebug>

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QApplication app(argc, argv);
    qmlRegisterType<QMLCppBridge>("com.mrmmsmsa", 1, 0, "QMLCppBridge");
    qmlRegisterType(QUrl("qrc:/ToastManager.qml"), "com.mrmmsmsa", 1, 0, "ToastManager");

    QQmlApplicationEngine engine;
    const QUrl url(QStringLiteral("qrc:/main.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);
    engine.load(url);

    /*QObject *rootObject = qobject_cast<QObject*>(engine.rootObjects().first());
    QMetaObject::invokeMethod(rootObject, "addShotStats",
            Q_ARG(QVariant, 0), Q_ARG(QVariant, 0), Q_ARG(QVariant, 9.9), Q_ARG(QVariant, 60), Q_ARG(QVariant, 5.55), Q_ARG(QVariant, 1.25));
    QMetaObject::invokeMethod(rootObject, "addShotStats",
            Q_ARG(QVariant, 10), Q_ARG(QVariant, 0), Q_ARG(QVariant, 9.9), Q_ARG(QVariant, 60), Q_ARG(QVariant, 5.55), Q_ARG(QVariant, 1.25));
    QMetaObject::invokeMethod(rootObject, "addShotStats",
            Q_ARG(QVariant, 10), Q_ARG(QVariant, -10), Q_ARG(QVariant, 9.9), Q_ARG(QVariant, 60), Q_ARG(QVariant, 5.55), Q_ARG(QVariant, 1.25));
    QMetaObject::invokeMethod(rootObject, "addShotStats",
            Q_ARG(QVariant, 0), Q_ARG(QVariant, -10), Q_ARG(QVariant, 9.9), Q_ARG(QVariant, 60), Q_ARG(QVariant, 5.55), Q_ARG(QVariant, 1.25));
    QMetaObject::invokeMethod(rootObject, "addShotStats",
            Q_ARG(QVariant, -10), Q_ARG(QVariant, -10), Q_ARG(QVariant, 9.9), Q_ARG(QVariant, 60), Q_ARG(QVariant, 5.55), Q_ARG(QVariant, 1.25));
    QMetaObject::invokeMethod(rootObject, "addShotStats",
            Q_ARG(QVariant, -10), Q_ARG(QVariant, 0), Q_ARG(QVariant, 9.9), Q_ARG(QVariant, 60), Q_ARG(QVariant, 5.55), Q_ARG(QVariant, 1.25));
    QMetaObject::invokeMethod(rootObject, "addShotStats",
            Q_ARG(QVariant, -10), Q_ARG(QVariant, 10), Q_ARG(QVariant, 9.9), Q_ARG(QVariant, 60), Q_ARG(QVariant, 5.55), Q_ARG(QVariant, 1.25));
    QMetaObject::invokeMethod(rootObject, "addShotStats",
            Q_ARG(QVariant, 0), Q_ARG(QVariant, 10), Q_ARG(QVariant, 9.9), Q_ARG(QVariant, 60), Q_ARG(QVariant, 5.55), Q_ARG(QVariant, 1.25));
    QMetaObject::invokeMethod(rootObject, "addShotStats",
            Q_ARG(QVariant, 10), Q_ARG(QVariant, 10), Q_ARG(QVariant, 9.9), Q_ARG(QVariant, 60), Q_ARG(QVariant, 5.55), Q_ARG(QVariant, 1.25));
    QMetaObject::invokeMethod(rootObject, "addShotStats",
            Q_ARG(QVariant, -30), Q_ARG(QVariant, 10), Q_ARG(QVariant, 9.9), Q_ARG(QVariant, 60), Q_ARG(QVariant, 5.55), Q_ARG(QVariant, 1.25));*/

    return app.exec();
}
