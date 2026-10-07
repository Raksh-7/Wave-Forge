#include <QApplication>
#include <QWidget>

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);

    QWidget window;
    window.resize(700, 500);
    window.setWindowTitle("Waveline");

    window.show();

    return app.exec();
}