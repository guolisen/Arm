#include "armwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    //a.setWindowIcon(QIcon("hands.ico"));

    QCoreApplication::setOrganizationName("DELLEMC");
    QCoreApplication::setOrganizationDomain("FILETEAM");
    QCoreApplication::setApplicationName("ARM");

    ArmWindow w;
    w.init();
    w.show();

    return a.exec();
}
