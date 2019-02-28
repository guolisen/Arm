#include "armdialog.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    ArmDialog w;
    w.show();

    return a.exec();
}
