#ifndef LISTDATASAVETHREAD_H
#define LISTDATASAVETHREAD_H
#include <QThread.h>
#include "ListDataCache.h"
#include "QTime"
#include "qdir.h"
#include "qfileinfo.h"
#include "QFileInfoList"
#include "algorithm"
// 每隔两个小时存一个文件：7200s
#define ONE_FILE_DURATION_SEC 7200
// 当存满84 * 2个文件时，清空前84个文件
#define MAX_FILE_LIMIT 84
// 文件的名称格式
#define FILE_NAME_FORMAT "yyyy-MM-dd hh mm ss"
//排列判断
bool compareDate(const QFileInfo &file1, const QFileInfo &file2)
{
    if (file1.created() > file2.created())
    {
        return true;
    }
    return false;
}
class ListDataSaveThread : public QThread
{
private:
    QString fileName;// 当前文件名
    QTime dTime;

    QString prePath;
    long long int oneFileDurationSEC;
    int maxFileLimt;
public:

    ListDataSaveThread(QString _prePath,
            long long int _oneFileDurationSEC = ONE_FILE_DURATION_SEC,
            int _maxFileLimt = MAX_FILE_LIMIT) {
        this->prePath = _prePath;
        this->oneFileDurationSEC = _oneFileDurationSEC;
        this->maxFileLimt = _maxFileLimt;
    }

    QString getTimeString() {
        QDateTime time = QDateTime::currentDateTime();//获取系统现在的时间
        QString timeString = time.toString(FILE_NAME_FORMAT); //设置显示格式
        return timeString;
    }

    /**
     * @brief 写入文件
     * @param cache
     * @param fileName
     * @return
     */
    int writeToFile(const std::vector<QString> &cache, QString &fileName) {

        QDir dir(prePath);
        if(!dir.exists()) {// 没有目录创建目录
            dir.mkpath(prePath);
        }

        QFile f(fileName);

        if(!f.open(QIODevice::WriteOnly | QIODevice::Text |  QFile::Append))
        {
            qDebug("文件打开失败" );
            qDebug("%s ", qPrintable( fileName ) );
            return -1;
        }

        // 打开成功
        QTextStream txtOutput(&f);
        for(int i = 0; i < cache.size(); i++) {
            txtOutput << cache[i] << endl;
        }

        f.close();
        return 0;
    }




    /**
     * @brief 删除上个星期的文件
     * @param folderDir
     * @return
     */
    bool removeFilesLastWeek(const QString &folderDir)
    {
        QDir dir(folderDir);
        QFileInfoList fileList;
        QFileInfo curFile;
        if(!dir.exists())  {return false;}//文件不存，则返回false
        fileList=dir.entryInfoList(QDir::Files
                                   //                                   |QDir::Dirs
                                   //                                   |QDir::Readable
                                   //                                   |QDir::Writable
                                   //                                   |QDir::Hidden
                                   //                                   |QDir::NoDotAndDotDot
                                   ,QDir::Name);

        int infoNum=fileList.size();
        if(infoNum < maxFileLimt * 2) return false;// 只有当文件数量大于设定时才进行删除


        // 排序
        qSort(fileList.begin(), fileList.end(), compareDate);

        for(int i = 0; i < fileList.size(); i++) {
            qDebug("%s ", qPrintable( fileList[i].fileName() ) );
        }

        for(int i = fileList.size() - 1; i >= maxFileLimt ;i--)
        {
            curFile=fileList[i];// 每次都是删除第0个 删除maxFileLimt次
            qDebug("删除 %s ", qPrintable( curFile.fileName() ) );

            if(curFile.isFile())//如果是文件，删除文件
            {
                QFile fileTemp(curFile.filePath());
                fileTemp.remove();
                fileList.removeAt(i);
            }
        }

        return true;
    }


    void setOneFileDurationSEC(long long value)
    {
        oneFileDurationSEC = value;
    }


    void setMaxFileLimt(int value)
    {
        maxFileLimt = value;
    }

    void setPrePath(const QString &value)
    {
        prePath = value;
    }

protected:
    void run() {

        dTime.start();

        fileName = prePath + getTimeString() + ".txt";

        bool switchT = ListDataCache::getCacheSwitch();// 当前的开关
        while(1) {

            sleep(1);

            // 当缓冲区0 满了 开关状态发生了变化
            if(switchT != ListDataCache::getCacheSwitch() &&
                    ListDataCache::getCache0().size() >= ListDataCache::getMAX_CAP()) {
                switchT = !switchT;
                qDebug("0 满了  写入文件");
                writeToFile(ListDataCache::getCache0(),fileName);
                ListDataCache::clearCache0();
            }

            // 当缓冲区1 满了 开关状态发生了变化
            if(switchT != ListDataCache::getCacheSwitch() &&
                    ListDataCache::getCache1().size() >= ListDataCache::getMAX_CAP()){
                switchT = !switchT;
                qDebug("1 满了  写入文件");
                writeToFile(ListDataCache::getCache1(),fileName);
                ListDataCache::clearCache1();

            }

            // 时间大于单个文件规定的时间
            if(dTime.elapsed()/1000.0 > oneFileDurationSEC) {
                if(ListDataCache::getCache0().size() != 0) {
                    switchT = !switchT;
                    ListDataCache::changeCacheSwitch();// 将要将数据写入文件，此时切换缓冲区，让数据插入到另一个去
                    qDebug("时间到了 0有数据 写入文件  此时文件名发生改变");
                    writeToFile(ListDataCache::getCache0(), fileName);// 写入
                    ListDataCache::clearCache0();// 清空缓冲区
                    fileName = prePath + getTimeString() + ".txt";// 改变文件名
                    removeFilesLastWeek(prePath);
                } else if(ListDataCache::getCache1().size() != 0) {
                    switchT = !switchT;
                    ListDataCache::changeCacheSwitch();
                    qDebug("时间到了 1有数据 写入文件  此时文件名发生改变");
                    writeToFile(ListDataCache::getCache1(), fileName);
                    ListDataCache::clearCache1();
                    fileName = prePath + getTimeString() + ".txt";
                    removeFilesLastWeek(prePath);
                }

                dTime.restart();// 重置时间
            }



        }

    }

};

#endif // LISTDATASAVETHREAD_H


