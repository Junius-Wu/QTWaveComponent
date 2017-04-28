#ifndef LISTDATASAVETHREAD_H
#define LISTDATASAVETHREAD_H
#include <QThread.h>
#include "FileDataCache.h"
#include "QTime"

// 每隔两个小时存一个文件：7200000ms
#define ONE_FILE_DURATION_MS 9
// 当存满84 * 2个文件时，清空前84个文件
#define MAX_FILE_LIMIT 84



class ListDataSaveThread : public QThread
{
private:
    QString fileName;// 当前文件名
    QTime dTime;
public:
    ListDataSaveThread() {

    }

protected:
    void run() {

        dTime.start();

        bool switchT = FileDataCache::getCacheSwitch();// 当前的开关
        while(1) {


            // 当缓冲区0 满了 开关状态发生了变化
            if(switchT != FileDataCache::getCacheSwitch() &&
                    FileDataCache::getCache0().size() >= FileDataCache::getMAX_CAP()) {
                switchT = !switchT;
                qDebug("0 满了  写入文件");
                FileDataCache::clearCache0();
            }
            // 当缓冲区1 满了 开关状态发生了变化
            if(switchT != FileDataCache::getCacheSwitch() &&
                    FileDataCache::getCache1().size() >= FileDataCache::getMAX_CAP()){
                switchT = !switchT;
                qDebug("1 满了  写入文件");
                FileDataCache::clearCache1();
            }

            // 时间大于2小时
            if(dTime.elapsed()/1000.0 > ONE_FILE_DURATION_MS) {
                if(FileDataCache::getCache0().size() != 0) {
                    switchT = !switchT;
                    FileDataCache::changeCacheSwitch();// 将要将数据写入文件，此时切换缓冲区，让数据插入到另一个去
                    qDebug("时间到了 0有数据 写入文件  此时文件名发生改变");
                    FileDataCache::clearCache0();
                } else if(FileDataCache::getCache1().size() != 0) {
                    switchT = !switchT;
                    FileDataCache::changeCacheSwitch();
                    qDebug("时间到了 1有数据 写入文件  此时文件名发生改变");
                    FileDataCache::clearCache1();
                }

                dTime.restart();// 重置时间
            }

        }

    }

};

#endif // LISTDATASAVETHREAD_H
