#include "MainWindow.h"
#include <QtWidgets/QApplication>
#include <QtGui/QIcon>

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);
    app.setWindowIcon(QIcon(":/Main/icon.ico"));

    MainWindow window;
    window.show();
    return app.exec();
}
