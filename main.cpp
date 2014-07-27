#include "stringtojson.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    stringToJSON w;
    w.show();

    return a.exec();
}
