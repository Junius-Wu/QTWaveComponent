#include "mainwindow.h"

#include <qfile.h>
#include <QTextStream>
#include <QApplication>

#include "ListDataSaveThread.h"
#include "ListDataCache.h"
#include "qdebug.h"
#define qout qDebug()


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();


    ListDataSaveThread *listDataSaveThread = new ListDataSaveThread("/users/jun/Desktop/1/", 5, 3);
    listDataSaveThread->start();

    int index = 1;
    int n = 50000;
    while(n--) {
        QString s = QString::number(index++, 10);
        usleep(100 * 1000);
        ListDataCache::insertData(s);
    }


    return a.exec();
}
