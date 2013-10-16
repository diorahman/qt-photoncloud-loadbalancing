#include <QApplication>
#include <QDeclarativeContext>
#include "qmlapplicationviewer.h"

#include "driver.h"

Q_DECL_EXPORT int main(int argc, char *argv[])
{
    QScopedPointer<QApplication> app(createApplication(argc, argv));

    Driver * driver = new Driver();
    QmlApplicationViewer viewer;
    QDeclarativeContext * context = viewer.rootContext();
    context->setContextProperty("Driver", driver);

    viewer.addImportPath(QLatin1String("modules"));
    viewer.setOrientation(QmlApplicationViewer::ScreenOrientationAuto);
    viewer.setMainQmlFile(QLatin1String("qml/qt-photon/main.qml"));
    viewer.showExpanded();

    driver->drive();

    return app->exec();
}
