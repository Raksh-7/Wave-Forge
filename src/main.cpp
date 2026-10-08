#include "app/WaveForgeMainWindow.h"

#include <QApplication>

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);
    QApplication::setApplicationName("WaveForge");
    QApplication::setOrganizationName("WaveForge");

    WaveForgeMainWindow clMainWindow;
    clMainWindow.show();

    return app.exec();
}
