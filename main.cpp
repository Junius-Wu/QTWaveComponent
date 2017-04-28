#include "mainwindow.h"

#include <qfile.h>
#include <QTextStream>
#include <QApplication>

#include "listdatasavethread.h"
#include "FileDataCache.h"
#include "qdebug.h"
#define qout qDebug()


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    ListDataSaveThread *listDataSaveThread = new ListDataSaveThread();
    listDataSaveThread->start();

    QString ss("123");
    int n = 50000;
    while(n--) {
        usleep(100 * 1000);
        FileDataCache::insertData(ss);
    }


    return a.exec();
}
