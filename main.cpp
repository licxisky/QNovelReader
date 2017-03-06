#include "qnovelreader.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QNovelReader w;
    w.showMaximized();

    return a.exec();
}
