
// Pelco_ServerDlg.h : 头文件
//


#pragma once


#include <thread>
#include <string>
#include <vector>
#include <chrono>
#include <algorithm>

#include <Sensapi.h>     
#include <Wininet.h>  
#include <Windows.h>

#include <mysql.h>
#include "HandleTrack.h"
#include "Point3D.h"
#include "Logger.h"

#include "ping.h"
#include "hezhong.h"
#include "MatrixStatic.h"
#include "dhnetsdk.h"

#pragma comment(lib, "Sensapi.lib")  
#pragma comment(lib, "Wininet.lib") 
#pragma comment(lib, "lib/dhnetsdk.lib")
#pragma comment(lib, "lib/libmysql.lib")

struct PreposEventTaskJob                           //预置位，事件编码，作业编码，任务编码
{
  int prepos;
  std::string event;
  std::string task;
  std::string job;
};

// CPelco_ServerDlg 对话框
class CPelco_ServerDlg : public CDialogEx
{
  // 构造
public:
  CPelco_ServerDlg(CWnd* pParent = NULL);	// 标准构造函数

  // 对话框数据

  enum { IDD = IDD_PELCO_SERVER_DIALOG };

protected:
  virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


  // 实现
protected:
  HICON m_hIcon;

  char* soap_1;
  char* soap_2;

  char* para1;
  char* para2;
  char* para7;

  char* camip1;
  char* camport1;
  char* camnum1;
  char* myip;
  char* myexip;
  char* midip;
  int m_time;
  char* exipaddress;

  NOTIFYICONDATA m_nid;

  bool m_connected_middleware;
  bool neterror;
  CString midip_s;
  bool b_show;

  // 生成的消息映射函数
  virtual BOOL OnInitDialog();
  afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
  afx_msg void OnPaint();
  afx_msg HCURSOR OnQueryDragIcon();
  DECLARE_MESSAGE_MAP()
private:
  SOCKET m_sockSrv;
  MYSQL* m_conn;
  CHandleTrack m_track_0;
  CHandleTrack m_track_1;
  CHandleTrack m_track_2;
  CHandleTrack m_track_3;
  CHandleTrack m_track_4;
  CHandleTrack m_track_5;

public:
  CLogger m_asyncFile;

public:
  BOOL initSocket();
  int initDatabase();
  int displayData(unsigned char* dataBuf, int len);
  int ackConnect(SOCKET sockConn, unsigned char* dataBuf, int len);
  int ackAllCall(SOCKET sockConn, unsigned char* dataBuf, int len);

private:
  char* m_ip;
  char* m_user;
  char* m_passwd;
  char* m_database;
  char* m_sitename;
  long m_port;

  char* m_ip_middleware;
  char* m_ip_video;
  char* m_ip_net_disk;

  Chezhong	m_chezhongr;

public:
  int ackExecuteYuzhiOperation(SOCKET sockConn, unsigned char* dataBuf, int len);
  int ackWriteDatabases(SOCKET sockConn, unsigned char* dataBuf, int len);
  int ackExecuteSQL(SOCKET sockConn, unsigned char* dataBuf, int len);
  int ackYuzhiOperation(SOCKET sockConn, unsigned char* dataBuf, int len);                //预置位设置  by zhaokui 2016.09.23
  int ackWriteYuzhiOperation(SOCKET sockConn, unsigned char* dataBuf, int len);
  int ackHandOperation(SOCKET sockConn, unsigned char* dataBuf, int len);                 //手动控制
  int ackQueryHistoryVideo(SOCKET sockConn, unsigned char* dataBuf, int len);             //从站返回查询历史视频后的参数
  int initConnectTrack();                                                                 //初始化与轨道机的连接
  CHandleTrack& getMemberTrack(const int num = 0);                                        //获取成员变量m_track_*
  const CHandleTrack& getMemberTrack(const char num = '0');
  int connectPatternRecognition(SOCKET& sockPattern);                                     //与模式识别建立连接
  int disconnectPatternRecognition(SOCKET& sockPattern);                                  //与模式识别断开连接

  static int ackExecuteInspection(SOCKET sockConn, unsigned char* dataBuf, int len, CPelco_ServerDlg* dlg);

  // 分割字符串
  std::vector<std::string> Split(const std::string& str, const std::string& delim, const bool trim_empty = false);
  std::string getStringFromCoordinate(double x, double y, double z);
  std::string getStringFromCoordinate(Point3D point3d);

  void setSendSeqAndRecvSeq(unsigned char* dataBuf);
  void recvSeqAdd1(unsigned char* dataBuf);
  afx_msg void OnTimer(UINT_PTR nIDEvent);

  int SlaveExecuteCMD(char* msoap, char* para1, char * para2);
  char* GetIp();
  char* GetExIp();
  char* join(char *s1, char *s2);
};