#include "stdafx.h"
#include "HandleTrack.h"
#include <utility>


#define INSTRUCTION_SIZE 7
#define PI 3.14159265358979323846

Point3D A(-838.2, 415.707, -89);
Point3D B(654.7, 415.707, -89);
Point3D C(654.7, 415.707, -364.1);
Point3D D(396.2, 415.707, -364.1);

const double r_LEN = 50;
const double LINE1_LEN = sqrt((B.x - A.x - 50)*(B.x - A.x - 50) + (B.z - A.z)*(B.z - A.z));
const double LINE2_LEN = sqrt((C.x - B.x)*(C.x - B.x) + (C.z - B.z + 100)*(C.z - B.z + 100));
const double LINE3_LEN = sqrt((D.x - C.x + 50)*(D.x - C.x + 50) + (D.z - C.z)*(D.z - C.z));
const double CIRCLE_LEN = PI*r_LEN / 2.0;
const double TOTAL_LEN = LINE1_LEN + LINE2_LEN + LINE3_LEN + CIRCLE_LEN*2.0;

const double udMinOff = 166.307;
const double g_y_start_point = 415.707;

//Point3D point3dNums[30] = {
//    { -53.5, 415.707, -92.52733 },
//    { -107.3, 415.707, -92.52733 },
//    { -112.9158, 403.8022, -92.52733 },
//    { -120.8, 415.707, -92.52733 },
//    { -146.9, 415.707, -92.52733 },
//    { -178.1, 415.707, -92.52733 },
//    { -190, 415.707, -92.52733 },
//    { -210, 415.707, -92.52733 },
//    { -230, 415.707, -92.52733 },
//    { -250, 415.707, -92.52733 },
//    { -270, 415.707, -92.52733 },
//    { -290, 415.707, -92.52733 },
//    { -295.3464, 407.3415, -92.52933 },
//    { -294.2594, 385.1408, -92.52933 },      //�ټ�һ��������
//    { 75.94963, 415.707, -92.52733 },
//    { -295.3464, 407.3415, -92.52933 },
//    { -290, 415.707, -92.52733 },
//    { -270, 415.707, -92.52733 },
//    { -250, 415.707, -92.52733 },
//    { -230, 415.707, -92.52733 },
//    { -210, 415.707, -92.52733 },
//    { -190, 415.707, -92.52733 },
//    { -178.1, 415.707, -92.52733 },
//    { -146.9, 415.707, -92.52733 },
//    { -120.8, 415.707, -92.52733 },
//    { -112.9158, 403.8022, -92.52733 },
//    { -107.3, 415.707, -92.52733 },
//    { -53.5, 415.707, -92.52733 },
//    { 75.94963, 415.707, -92.52733 }
//};

CHandleTrack::CHandleTrack(const SOCKET& sockPelcoLeftRight, const SOCKET& sockPelcoUpDown) 
    :m_sockPelcoLeftRight(sockPelcoLeftRight), m_sockPelcoUpDown(sockPelcoUpDown)
{
    //size_t q_size = 1024;                //1Mջ�ռ�
    //spdlog::set_async_mode(q_size);
    //m_async_file = spdlog::daily_logger_mt("handletrack_logger", "logs_track/handletrack_log", 0, 0, true);
}

CHandleTrack& CHandleTrack::operator=(const CHandleTrack& rhs)
{
    CHandleTrack tmp(rhs);
    std::swap(m_sockPelcoLeftRight, tmp.m_sockPelcoLeftRight);
    std::swap(m_sockPelcoUpDown, tmp.m_sockPelcoUpDown);

    return *this;
}

CHandleTrack::~CHandleTrack()
{
}

Point3D CHandleTrack::getCoordinatePoint3D()                                        //��ȡ3D����    
{
    //writeLog("getCoordinatePoint3D() ������");
    int lroffsetLen = getLeftRightOffsetLen();                                      //��ȡ ���� ƫ�Ƶĳ���
    Point3D resultPoint = getPointByDist(static_cast<double>(lroffsetLen));
    resultPoint.y -= getUpDownOffsetLen();

    return resultPoint;
}

int CHandleTrack::getLeftRightOffsetLen()                                                    //��ȡ ���� ƫ�Ƶĳ���
{
    //writeLog("getLeftRightOffsetLen() ������");
    int ret = 0;
    unsigned char instructionBuf[7] = { 0xaa, 0, 0x03, 0, 0, 0, 0 };                         //pelcoָ��ֻ��7���ֽ�
    instructionBuf[6] = (unsigned char)getCheckBit(instructionBuf, INSTRUCTION_SIZE);

    ret = send(m_sockPelcoLeftRight, (char*)instructionBuf, INSTRUCTION_SIZE, 0);                     // ��ѯ��ǰλ�õ� �� ��λ����
    if (ret != INSTRUCTION_SIZE)
    {
        AfxMessageBox(_T("getLeftRightOffsetLen() query offsetlen failed!"));
        return -1;
    }
    //writeLog("getLeftRightOffsetLen() send "+std::to_string(ret) +" bytes");

    ret = recv(m_sockPelcoLeftRight, (char*)instructionBuf, INSTRUCTION_SIZE, 0);                    // ���յ�ǰλ�õ� �� ��λ����
    if (ret != INSTRUCTION_SIZE)
    {
        AfxMessageBox(_T("getLeftRightOffsetLen() recv offsetlen failed!"));
        return -1;
    }
    //writeLog("getLeftRightOffsetLen() recv " + std::to_string(ret) + " bytes");

    unsigned int res = 0;
    for (int i = 3; i < 6; ++i)
    {
        res <<= 8;
        res += instructionBuf[i];
    }
    //writeLog("getLeftRightOffsetLen() offsetlen = " + std::to_string(res));

    return res;
}

int CHandleTrack::getUpDownOffsetLen()      //��ȡƫ�Ƶĳ���
{
    //writeLog("getUpDownOffsetLen() ������");
    int ret = 0;

    unsigned char instructionBuf[7] = { 0xaa, 0, 0x03, 0, 0, 0, 0 };                                  //pelcoָ��ֻ��7���ֽ�
    instructionBuf[6] = (unsigned char)getCheckBit(instructionBuf, INSTRUCTION_SIZE);

    ret = send(m_sockPelcoUpDown, (char*)instructionBuf, INSTRUCTION_SIZE, 0);                     // ��ѯ��ǰλ�õ� �� ��λ����
    if (ret != INSTRUCTION_SIZE)
    {
        AfxMessageBox(_T("getUpDownOffsetLen() query offsetlen failed!"));
        return -1;
    }

    ret = recv(m_sockPelcoUpDown, (char*)instructionBuf, INSTRUCTION_SIZE, 0);                    // ���յ�ǰλ�õ� �� ��λ����
    if (ret != INSTRUCTION_SIZE)
    {
        AfxMessageBox(_T("getUpDownOffsetLen() recv offsetlen failed!"));
        return -1;
    }

    unsigned int res = 0;
    for (int i = 3; i < 6; ++i)
    {
        res <<= 8;
        res += instructionBuf[i];
    }

    return res;
}

int CHandleTrack::getCheckBit(unsigned char * instructionBuf, int len)       //У��λ������
{
    unsigned int res = 0;
    for (int i = 1; i < 6; ++i)
        res += instructionBuf[i];
    res %= 256;

    return res;
}

Point3D CHandleTrack::getLinePoint(double dist)
{
    Point3D ret;
    if (dist < LINE1_LEN)                                    //��һ���߶�
    {
        ret = A;
        ret.x += dist;
    }
    else if (dist > LINE1_LEN + CIRCLE_LEN && dist < LINE1_LEN + LINE2_LEN + CIRCLE_LEN)
    {
        ret = B;
        ret.z -= 50;                                        //�ڶ����߶ε���ʼ��
        dist = dist - LINE1_LEN - CIRCLE_LEN;
        ret.z -= dist;
    }
    else if (dist > LINE1_LEN + LINE2_LEN + CIRCLE_LEN * 2 && dist < LINE1_LEN + LINE2_LEN + LINE3_LEN + CIRCLE_LEN * 2)
    {
        ret = C;
        ret.x -= 50;                                        //�ڶ����߶ε���ʼ��
        dist = dist - LINE1_LEN - LINE2_LEN - CIRCLE_LEN * 2;
        ret.x -= dist;
    }

    return ret;
}

Point3D CHandleTrack::get_point_in_circle(double dist)
{
    Point3D ret = B;
    if (dist > LINE1_LEN && dist < LINE1_LEN + CIRCLE_LEN)                    //��һ��Բ��
    {
        dist -= LINE1_LEN;
        Point3D start_circle1(B.x - r_LEN, B.y, B.z);

        double angle = dist / r_LEN;                                          //��û�����Ӧ�ĽǶ�       
        ret.x = start_circle1.x + sin(angle) * r_LEN;
        ret.z = start_circle1.z - (r_LEN - cos(angle) * r_LEN);
    }
    else if (dist > LINE1_LEN + LINE2_LEN + CIRCLE_LEN                        //��ڶ���Բ��
        && dist < LINE1_LEN + LINE2_LEN + CIRCLE_LEN * 2)
    {
        dist = dist - LINE1_LEN - LINE2_LEN - CIRCLE_LEN;
        Point3D start_circle2(C.x, C.y, C.z + r_LEN);

        double angle = dist / r_LEN;                                          //��û�����Ӧ�ĽǶ�           
        ret.x = start_circle2.x - (r_LEN - cos(angle) * r_LEN);
        ret.z = start_circle2.z - sin(angle) * r_LEN;
    }

    return ret;
}

Point3D CHandleTrack::getPointByDist(double dist)
{
    if (dist < 0.0 || fabs(dist - 0.0) < 1e-7)                                        //1
        return A;
    else if (fabs(dist - LINE1_LEN) < 1e-7)                                           //2
        return Point3D(B.x - 50, B.y, B.z);
    else if (fabs(dist - LINE1_LEN - CIRCLE_LEN) < 1e-7)                              //3
        return Point3D(B.x, B.y, B.z - 50);
    else if (fabs(dist - LINE1_LEN - LINE2_LEN - CIRCLE_LEN) < 1e-7)                  //4
        return Point3D(C.x, C.y, C.z + 50);
    else if (fabs(dist - LINE1_LEN - LINE2_LEN - CIRCLE_LEN * 2) < 1e-7)              //5
        return Point3D(C.x - 50, C.y, C.z);
    else if (dist > LINE1_LEN + LINE2_LEN + LINE3_LEN + CIRCLE_LEN * 2
        || fabs(dist - LINE1_LEN - LINE2_LEN - LINE3_LEN - CIRCLE_LEN * 2) < 1e-7)    //6
        return Point3D(D.x, D.y, D.z);

    else if (dist < LINE1_LEN
        || (dist> LINE1_LEN + CIRCLE_LEN && dist < LINE1_LEN + LINE2_LEN + CIRCLE_LEN)
        || dist>LINE1_LEN + LINE2_LEN + CIRCLE_LEN * 2)
        return getLinePoint(dist);
    else
        return get_point_in_circle(dist);
}

int CHandleTrack::callPropos(int lowPos, int highPos)                                        //����Ԥ��λ
{
    //writeLog("callPropos() ������");
    int ret = 0;
    unsigned char instructionBuf[7] = { 0xff, 0, 0, 0x07, 0, 0, 0 };                         //pelcoָ��ֻ��7���ֽ�
    instructionBuf[5] = (unsigned char)lowPos;
    instructionBuf[4] = (unsigned char)highPos;
    instructionBuf[6] = (unsigned char)getCheckBit(instructionBuf, INSTRUCTION_SIZE);

    ret = send(m_sockPelcoLeftRight, (char*)instructionBuf, INSTRUCTION_SIZE, 0);            //��������
    if (ret != INSTRUCTION_SIZE)
    {
        AfxMessageBox(_T("callPropos() query offsetlen failed!"));
        return -1;
    }

    ret = send(m_sockPelcoUpDown, (char*)instructionBuf, INSTRUCTION_SIZE, 0);               //��������
    if (ret != INSTRUCTION_SIZE)
    {
        AfxMessageBox(_T("callPropos() query offsetlen failed!"));
        return -1;
    }

    //writeLog("callPropos() ������ send " +std::to_string(ret)+" bytes");

    return 0;
}

int CHandleTrack::setPropos(int lowPos, int highPos)                                         //����Ԥ��λ
{
    //writeLog("setPropos() ������");
    int ret = 0;
    unsigned char instructionBuf[7] = { 0xff, 0, 0, 0x03, 0, 0, 0 };                         //pelcoָ��ֻ��7���ֽ�
    instructionBuf[5] = (unsigned char)lowPos;
    instructionBuf[4] = (unsigned char)highPos;
    instructionBuf[6] = (unsigned char)getCheckBit(instructionBuf, INSTRUCTION_SIZE);

    ret = send(m_sockPelcoLeftRight, (char*)instructionBuf, INSTRUCTION_SIZE, 0);            //��������
    if (ret != INSTRUCTION_SIZE)
    {
        AfxMessageBox(_T("setPropos() query offsetlen failed!"));
        return -1;
    }

    ret = send(m_sockPelcoUpDown, (char*)instructionBuf, INSTRUCTION_SIZE, 0);              //��������
    if (ret != INSTRUCTION_SIZE)
    {
        AfxMessageBox(_T("setPropos() query offsetlen failed!"));
        return -1;
    }
    //writeLog("setPropos() ������ send " + std::to_string(ret) + " bytes");

    return 0;
}

int CHandleTrack::clearPropos(int lowPos, int highPos)                                        //����Ԥ��λ
{
    //writeLog("clearPropos() ������");
    int ret = 0;
    unsigned char instructionBuf[7] = { 0xff, 0, 0, 0x05, 0, 0, 0 };                          //pelcoָ��ֻ��7���ֽ�
    instructionBuf[5] = (unsigned char)lowPos;
    instructionBuf[4] = (unsigned char)highPos;
    instructionBuf[6] = (unsigned char)getCheckBit(instructionBuf, INSTRUCTION_SIZE);

    ret = send(m_sockPelcoLeftRight, (char*)instructionBuf, INSTRUCTION_SIZE, 0);
    if (ret != INSTRUCTION_SIZE)
    {
        AfxMessageBox(_T("clearPropos() query offsetlen failed!"));
        return -1;
    }

    ret = send(m_sockPelcoUpDown, (char*)instructionBuf, INSTRUCTION_SIZE, 0);                //��������
    if (ret != INSTRUCTION_SIZE)
    {
        AfxMessageBox(_T("clearPropos() query offsetlen failed!"));
        return -1;
    }

    //writeLog("clearPropos() ������ send " + std::to_string(ret) + " bytes");

    return 0;
}

int CHandleTrack::startLeft()                                                //��ʼ����ǰ�����������Ҷ�Ϊ���
{
    //writeLog("startLeft() ������");
    int ret = 0;
    unsigned char instructionBuf[7] = { 0xff, 0, 0, 0x09, 0x20, 1, 0 };      //pelcoָ��ֻ��7���ֽ�
    instructionBuf[6] = (unsigned char)getCheckBit(instructionBuf, INSTRUCTION_SIZE);

    ret = send(m_sockPelcoLeftRight, (char*)instructionBuf, INSTRUCTION_SIZE, 0);
    if (ret != INSTRUCTION_SIZE)
    {
        AfxMessageBox(_T("startLeft() send startLeft() failed!"));
        return -1;
    }
    //writeLog("startLeft() ������ send " + std::to_string(ret) + " bytes");

    return 0;
}

int CHandleTrack::startRight()                                               //��ʼ���ң����ˣ�
{
    //writeLog("startRight() ������");
    int ret = 0;
    unsigned char instructionBuf[7] = { 0xff, 0, 0, 0x09, 0x20, 2, 0 };      //pelcoָ��ֻ��7���ֽ�
    instructionBuf[6] = (unsigned char)getCheckBit(instructionBuf, INSTRUCTION_SIZE);

    ret = send(m_sockPelcoLeftRight, (char*)instructionBuf, INSTRUCTION_SIZE, 0);
    if (ret != INSTRUCTION_SIZE)
    {
        AfxMessageBox(_T("startRight() send startRight() failed!"));
        return -1;
    }
    //writeLog("startRight() ������ send " + std::to_string(ret) + " bytes");

    return 0;
}

int CHandleTrack::startUp()                                                  //��ʼ���ϣ����ȼ���
{
    //writeLog("startUp() ������");
    int ret = 0;
    unsigned char instructionBuf[7] = { 0xff, 0, 0, 0x09, 0x20, 5, 0 };      //pelcoָ��ֻ��7���ֽ�
    instructionBuf[6] = (unsigned char)getCheckBit(instructionBuf, INSTRUCTION_SIZE);

    ret = send(m_sockPelcoUpDown, (char*)instructionBuf, INSTRUCTION_SIZE, 0);
    if (ret != INSTRUCTION_SIZE)
    {
        AfxMessageBox(_T("startUp() send failed!"));
        return -1;
    }
    //writeLog("startUp() ������ send " + std::to_string(ret) + " bytes");

    return 0;
}

int CHandleTrack::startDown()                                                //��ʼ���£����ȼӣ�
{
    //writeLog("startDown() ������");
    int ret = 0;
    unsigned char instructionBuf[7] = { 0xff, 0, 0, 0x09, 0x20, 6, 0 };      //pelcoָ��ֻ��7���ֽ�
    instructionBuf[6] = (unsigned char)getCheckBit(instructionBuf, INSTRUCTION_SIZE);

    ret = send(m_sockPelcoUpDown, (char*)instructionBuf, INSTRUCTION_SIZE, 0);
    if (ret != INSTRUCTION_SIZE)
    {
        AfxMessageBox(_T("startDown() send failed!"));
        return -1;
    }
    //writeLog("startDown() ������ send " + std::to_string(ret) + " bytes");

    return 0;
}

int CHandleTrack::stopLeft()                                                 //ֹͣ������
{
    //writeLog("stopLeft() ������");
    int ret = 0;
    unsigned char instructionBuf[7] = { 0xff, 0, 0, 0x0B, 0, 1, 0 };      //pelcoָ��ֻ��7���ֽ�
    instructionBuf[6] = (unsigned char)getCheckBit(instructionBuf, INSTRUCTION_SIZE);

    ret = send(m_sockPelcoLeftRight, (char*)instructionBuf, INSTRUCTION_SIZE, 0);
    if (ret != INSTRUCTION_SIZE)
    {
        AfxMessageBox(_T("startLeft() send failed!"));
        return -1;
    }
    //writeLog("startLeft() ������ send " + std::to_string(ret) + " bytes");

    return 0;
}

int CHandleTrack::stopRight()                                                //ֹͣ������
{
    //writeLog("stopRight() ������");
    int ret = 0;
    unsigned char instructionBuf[7] = { 0xff, 0, 0, 0x0B, 0, 2, 0 };        //pelcoָ��ֻ��7���ֽ�
    instructionBuf[6] = (unsigned char)getCheckBit(instructionBuf, INSTRUCTION_SIZE);

    ret = send(m_sockPelcoLeftRight, (char*)instructionBuf, INSTRUCTION_SIZE, 0);
    if (ret != INSTRUCTION_SIZE)
    {
        AfxMessageBox(_T("startRight() send failed!"));
        return -1;
    }
    //writeLog("startRight() ������ send " + std::to_string(ret) + " bytes");

    return 0;
}

int CHandleTrack::stopUp()                                                   //ֹͣ������
{
    //writeLog("stopUp() ������");
    int ret = 0;
    unsigned char instructionBuf[7] = { 0xff, 0, 0, 0x0B, 0, 5, 0 };         //pelcoָ��ֻ��7���ֽ�
    instructionBuf[6] = (unsigned char)getCheckBit(instructionBuf, INSTRUCTION_SIZE);

    ret = send(m_sockPelcoUpDown, (char*)instructionBuf, INSTRUCTION_SIZE, 0);
    if (ret != INSTRUCTION_SIZE)
    {
        AfxMessageBox(_T("startUp() send failed!"));
        return -1;
    }
    //writeLog("startUp() ������ send " + std::to_string(ret) + " bytes");

    return 0;
}

int CHandleTrack::stopDown()                                                 //ֹͣ������
{
    //writeLog("stopDown() ������");
    int ret = 0;
    unsigned char instructionBuf[7] = { 0xff, 0, 0, 0x0B, 0, 6, 0 };        //pelcoָ��ֻ��7���ֽ�
    instructionBuf[6] = (unsigned char)getCheckBit(instructionBuf, INSTRUCTION_SIZE);

    ret = send(m_sockPelcoUpDown, (char*)instructionBuf, INSTRUCTION_SIZE, 0);
    if (ret != INSTRUCTION_SIZE)
    {
        AfxMessageBox(_T("startDown() send failed!"));
        return -1;
    }
    //writeLog("stopDown() ������ send " + std::to_string(ret) + " bytes");
    
    return 0;
}
