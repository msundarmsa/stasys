#include <QQmlApplicationEngine>
#include <QApplication>

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QApplication app(argc, argv);

    QQmlApplicationEngine engine;
    const QUrl url(QStringLiteral("qrc:/main.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);
    engine.load(url);

    QObject *rootObject = qobject_cast<QObject*>(engine.rootObjects().first());
    QMetaObject::invokeMethod(rootObject, "showStabDescAim",
            Q_ARG(QVariant, 60), Q_ARG(QVariant, 5.55), Q_ARG(QVariant, 1.25));

    return app.exec();
}
