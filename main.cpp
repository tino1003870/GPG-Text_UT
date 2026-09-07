#include <QQmlEngine>
#include <QQmlError>
#include <QApplication>
#include <QQuickView>
#include <QQmlContext>
#include <QDebug>
#include <QScreen>

#include "GpgBackend.h"

int main(int argc, char *argv[])
{
    qDebug() << "### MAIN.CPP START ###";

    QApplication app(argc, argv);

    qDebug() << "### PRIMARY SCREEN ###"
              << QGuiApplication::primaryScreen();

    if (QGuiApplication::primaryScreen()) {
        QScreen *screen = QGuiApplication::primaryScreen();

        qDebug() << "### SCREEN NAME ###"
                 << screen->name();

        qDebug() << "### SCREEN SIZE ###"
                 << screen->size();

        qDebug() << "### AVAILABLE GEOMETRY ###"
                 << screen->availableGeometry();

        qDebug() << "### DEVICE PIXEL RATIO ###"
                 << screen->devicePixelRatio();
    }

    GpgBackend gpgBackend;

    QQuickView view;

    qDebug() << "### QQUICKVIEW CREATED ###";

    view.engine()->rootContext()->setContextProperty(
        "gpgBackend",
        &gpgBackend
    );

    // QML Qt.quit() mit QGuiApplication::quit() verbinden
    QObject::connect(
        view.engine(),
        &QQmlEngine::quit,
        &app,
        &QCoreApplication::quit
    );

    qDebug() << "### QML QUIT CONNECTED ###";

    const QUrl url(QStringLiteral("qrc:/qml/Main.qml"));

    qDebug() << "### BEFORE SET SOURCE ###"
              << url;

    view.setSource(url);

    qDebug() << "### AFTER SET SOURCE ###";
    qDebug() << "### STATUS ###"
             << view.status();
    qDebug() << "### SIZE BEFORE SHOW ###"
             << view.size();
    qDebug() << "### WIDTH BEFORE SHOW ###"
             << view.width();
    qDebug() << "### HEIGHT BEFORE SHOW ###";
    qDebug() << view.height();

    if (view.status() == QQuickView::Error) {

        qCritical() << "### QML LOAD ERROR ###";

        const auto errors = view.errors();

        for (const auto &error : errors)
            qCritical().noquote() << error.toString();

        return -1;
    }

    /*
     * Explizit auf die verfügbare Bildschirmgröße setzen.
     */
    if (QGuiApplication::primaryScreen()) {

        const QRect geometry =
            QGuiApplication::primaryScreen()->availableGeometry();

        qDebug() << "### SET WINDOW GEOMETRY ###"
                 << geometry;

        view.setGeometry(geometry);

        qDebug() << "### SIZE AFTER SET GEOMETRY ###"
                 << view.size();
    }

    /*
     * Maximiertes Fenster erzwingen.
     */
    qDebug() << "### SHOW FULLSCREEN ###";

    view.showFullScreen();

    qDebug() << "### WINDOW SHOWN ###";
    qDebug() << "### FINAL SIZE ###"
             << view.size();
    qDebug() << "### FINAL WIDTH ###"
             << view.width();
    qDebug() << "### FINAL HEIGHT ###"
             << view.height();
    qDebug() << "### WINDOW ID ###"
             << view.winId();
    qDebug() << "### ENTER EVENT LOOP ###";

    return app.exec();
}
