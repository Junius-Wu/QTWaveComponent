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


 // QString pathx = "/users/jun/Desktop/1/2/4/5/";

    ListDataSaveThread *listDataSaveThread = new ListDataSaveThread();
    listDataSaveThread->start();

    int index = 1;
    int n = 50000;
    while(n--) {
        char *s;
         sprintf(s, "%d", index++);
        QString ss(s);
        usleep(100 * 1000);
        ListDataCache::insertData(ss);
    }


    return a.exec();
}
