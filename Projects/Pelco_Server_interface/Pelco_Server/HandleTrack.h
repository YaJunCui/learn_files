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

    int getLeftRightOffsetLen();                                    //��ȡ ���� ƫ�Ƶĳ���
    int getUpDownOffsetLen();                                       //��ȡ ���� ƫ�Ƶĳ���
    int callPropos(int lowPos = 0,int highPos = 0);                 //����Ԥ��λ
    int setPropos(int lowPos = 0, int highPos = 0);                 //����Ԥ��λ
    int clearPropos(int lowPos = 0, int highPos = 0);               //����Ԥ��λ

    int startLeft();                                                //��ʼ����ǰ�����������Ҷ�Ϊ���
    int startRight();                                               //��ʼ���ң����ˣ�
    int startUp();                                                  //��ʼ���ϣ����ȼ���
    int startDown();                                                //��ʼ���£����ȼӣ�

    int stopLeft();                                                 //ֹͣ�����ҡ��ϡ���
    int stopRight();
    int stopUp();
    int stopDown();

    Point3D getCoordinatePoint3D();                                 //��ȡ3D����       
    Point3D getPointByDist(double dist);                            //��ȡ����ϵ�����

private:
    int getCheckBit(unsigned char * instructionBuf, int len);       //У��λ������
    Point3D getLinePoint(double dist);                              //��ȡ���  ֱ����  ������
    Point3D get_point_in_circle(double dist);                       //��ȡ���  �����  ������

    //bool writeLog(const std::string& str);

private:
    SOCKET m_sockPelcoLeftRight;
    SOCKET m_sockPelcoUpDown;
    //std::shared_ptr<spdlog::logger> m_async_file;
};
