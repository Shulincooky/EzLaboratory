#include "EzLaboratory.h"
#include <QtWidgets/QApplication>

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);
    EzLaboratory window;
    window.show();
    return app.exec();
}