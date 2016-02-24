#include "logiccalcwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    LogicCalcWindow w;
    w.show();

    return a.exec();
}
