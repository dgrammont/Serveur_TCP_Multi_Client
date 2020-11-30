#include "serveurwidget.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    serveurWidget w;
    w.show();
    return a.exec();
}
