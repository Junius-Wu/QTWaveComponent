#ifndef DATABASECACHE_H
#define DATABASECACHE_H
#include <vector>
#include <qstring.h>
/**
  *    列表数据存盘缓冲区
  */
#define MAX_CAP 50 // 单个缓冲区的最大容量
int pCount = 0; // 打印计数

bool cacheSwitch = false;
std::vector<QString> listDataCache0;
std::vector<QString> listDataCache1;

class ListDataCache
{
public:

    /**
     * @brief 向缓冲区插入数据
     * @param data
     */
    static void insertData(QString data)
    {

        if(!cacheSwitch) {
            listDataCache0.push_back(data);
            qDebug("%d 写入一个到缓冲区0000000000此时0区的大小：%d", pCount++, listDataCache0.size());
            if(listDataCache0.size() >= MAX_CAP) {
                cacheSwitch = !cacheSwitch;
            }
        } else {
            listDataCache1.push_back(data);
            qDebug("%d 写入一个到缓冲区111111111此时1区的大小：%d", pCount++, listDataCache1.size());
            if(listDataCache1.size() >= MAX_CAP) {
                cacheSwitch = !cacheSwitch;
            }
        }
    }


    static std::vector<QString> getCache0()
    {
        return listDataCache0;
    }

    static std::vector<QString> getCache1()
    {
        return listDataCache1;
    }

    /**
     * @brief 获取每一个缓冲区的最大容量
     * @return
     */
    static size_t getMAX_CAP()
    {
        return MAX_CAP;
    }

    /**
     * @brief 获取开关状态 false代表cache0可写 true代表cache1可写
     * @return
     */
    static bool getCacheSwitch()
    {
        return cacheSwitch;
    }

    /**
     * @brief 改变开关的状态 切换插入的缓冲区
     * @return
     */
    static bool changeCacheSwitch()
    {
        cacheSwitch = !cacheSwitch;
    }

    /**
     * @brief 清空cache0 释放内存
     */
    static void clearCache0()
    {
        //for(int i=0 ; i<cache0.size(); i++) delete  cache0[i];
        listDataCache0.clear();
    }

    /**
     * @brief 清空cache1 释放内存
     */
    static void clearCache1()
    {
        //for(int i=0 ; i<cache1.size(); i++) delete  cache1[i];
        listDataCache1.clear();
    }
};

#endif // DATABASECACHE_H
