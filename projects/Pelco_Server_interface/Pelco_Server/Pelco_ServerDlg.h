
// Pelco_ServerDlg.h : ͷ�ļ�
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

struct PreposEventTaskJob                           //Ԥ��λ���¼����룬��ҵ���룬�������
{
  int prepos;
  std::string event;
  std::string task;
  std::string job;
};

// CPelco_ServerDlg �Ի���
class CPelco_ServerDlg : public CDialogEx
{
  // ����
public:
  CPelco_ServerDlg(CWnd* pParent = NULL);	// ��׼���캯��

  // �Ի�������

  enum { IDD = IDD_PELCO_SERVER_DIALOG };

protected:
  virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


  // ʵ��
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

  // ���ɵ���Ϣӳ�亯��
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
  int ackYuzhiOperation(SOCKET sockConn, unsigned char* dataBuf, int len);                //Ԥ��λ����  by zhaokui 2016.09.23
  int ackWriteYuzhiOperation(SOCKET sockConn, unsigned char* dataBuf, int len);
  int ackHandOperation(SOCKET sockConn, unsigned char* dataBuf, int len);                 //�ֶ�����
  int ackQueryHistoryVideo(SOCKET sockConn, unsigned char* dataBuf, int len);             //��վ���ز�ѯ��ʷ��Ƶ��Ĳ���
  int initConnectTrack();                                                                 //��ʼ��������������
  CHandleTrack& getMemberTrack(const int num = 0);                                        //��ȡ��Ա����m_track_*
  const CHandleTrack& getMemberTrack(const char num = '0');
  int connectPatternRecognition(SOCKET& sockPattern);                                     //��ģʽʶ��������
  int disconnectPatternRecognition(SOCKET& sockPattern);                                  //��ģʽʶ��Ͽ�����

  static int ackExecuteInspection(SOCKET sockConn, unsigned char* dataBuf, int len, CPelco_ServerDlg* dlg);

  // �ָ��ַ���
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