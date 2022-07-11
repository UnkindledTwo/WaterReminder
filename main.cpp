#include "waterreminder.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    WaterReminder w;
    w.show();
    return a.exec();
}
