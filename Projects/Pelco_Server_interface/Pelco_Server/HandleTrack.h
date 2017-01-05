#pragma once
#include <Windows.h>
#include "Point3D.h"
//#include "spdlog\spdlog.h"

class CHandleTrack
{
public:
    CHandleTrack(const SOCKET& sockPelcoLeftRight = NULL, const SOCKET& sockPelcoUpDown = NULL);
    ~CHandleTrack();
    CHandleTrack& operator=(const CHandleTrack& rhs);

    int getLeftRightOffsetLen();                                    //获取 左右 偏移的长度
    int getUpDownOffsetLen();                                       //获取 上下 偏移的长度
    int callPropos(int lowPos = 0,int highPos = 0);                 //调用预置位
    int setPropos(int lowPos = 0, int highPos = 0);                 //调用预置位
    int clearPropos(int lowPos = 0, int highPos = 0);               //调用预置位

    int startLeft();                                                //开始：左（前进），以最右端为起点
    int startRight();                                               //开始：右（后退）
    int startUp();                                                  //开始：上（长度减）
    int startDown();                                                //开始：下（长度加）

    int stopLeft();                                                 //停止：左、右、上、下
    int stopRight();
    int stopUp();
    int stopDown();

    Point3D getCoordinatePoint3D();                                 //获取3D坐标       
    Point3D getPointByDist(double dist);                            //获取轨道上的坐标

private:
    int getCheckBit(unsigned char * instructionBuf, int len);       //校验位的设置
    Point3D getLinePoint(double dist);                              //获取轨道  直线上  的坐标
    Point3D get_point_in_circle(double dist);                       //获取轨道  弯道上  的坐标

    //bool writeLog(const std::string& str);

private:
    SOCKET m_sockPelcoLeftRight;
    SOCKET m_sockPelcoUpDown;
    //std::shared_ptr<spdlog::logger> m_async_file;
};
