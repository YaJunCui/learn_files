
// Pelco_ServerDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "Pelco_Server.h"
#include "Pelco_ServerDlg.h"
#include "afxdialogex.h"
#include <SensAPI.h>
#include "ini.h"

#include "mainserviceSoap11Binding.nsmap"
#include <urlmon.h>
#include <Winsock2.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define BUF_SIZE 1024
#define NET_LOCAL 87
#define MIDDLEWARE 97
#define VIDEO 107
#define NET_DISK 117

unsigned char sendSeq2 = 0;
unsigned char sendSeq3 = 0;
unsigned char recvSeq4 = 0;
unsigned char recvSeq5 = 0;
bool flagAdd1 = true;

bool g_executing_inspection = false;
bool g_connected_local = true;
bool g_connected_middleware = true;
bool g_connected_video = true;
bool g_connected_net_disk = true;
DWORD g_dw_local;


// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialogEx
{
public:
  CAboutDlg();

  // �Ի�������
  enum { IDD = IDD_ABOUTBOX };

protected:
  virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

  // ʵ��
protected:
  DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
  CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CPelco_ServerDlg �Ի���


int startServer(CPelco_ServerDlg* dlg, SOCKET& sockSrv)              //���������
{
  int ret = 0;
  CString strName, strMsg, tmp;

  SOCKADDR_IN addrClient;
  int len = sizeof(SOCKADDR);
  SOCKET sockConn;
  unsigned char recvBuf[BUF_SIZE];
  unsigned char unionBuf[BUF_SIZE * 10];
  unsigned char divBuf[BUF_SIZE];
  int unionLen = 0;;

  dlg->SetTimer(NET_LOCAL, 2000, NULL);
  dlg->SetTimer(MIDDLEWARE, 2000, NULL);
  dlg->SetTimer(VIDEO, 2000, NULL);
  dlg->SetTimer(NET_DISK, 2000, NULL);

  while (true)
  {
    //�ȴ��ͻ�������ĵ���
    sockConn = accept(sockSrv, (SOCKADDR*)&addrClient, &len);

    sendSeq2 = 0;
    sendSeq3 = 0;
    recvSeq4 = 0;
    recvSeq5 = 0;

    //dlg->MessageBox("Accept");
    dlg->initConnectTrack();

    while (true)
    {
      if (!g_connected_middleware || !g_connected_video || !g_connected_net_disk)
        break;

      //��������
      ret = recv(sockConn, (char*)recvBuf, BUF_SIZE, 0);
      char buf[1024] = { '\0' };
      for (int i = 0; i < ret; ++i)
      {
        sprintf(buf + strlen(buf), "%02x ", recvBuf[i]);
      }
      sprintf(buf + strlen(buf), "recv: %d ", ret);
      dlg->m_asyncFile.writeLog(buf);

      if (ret == 0)
      {
        strName = _T("startServer : the connection has been closed!\r\n");
        dlg->GetDlgItemText(IDC_EDIT_FRAME_NAME, tmp);                       //��ʾ������
        strName += tmp;
        dlg->SetDlgItemText(IDC_EDIT_FRAME_NAME, strName);

        strName = _T("startServer : the connection has been closed!\r\n");
        dlg->GetDlgItemText(IDC_EDIT_FRAME_MESSAGE, tmp);                       //��ʾ������
        strName += tmp;
        dlg->SetDlgItemText(IDC_EDIT_FRAME_MESSAGE, strName);

        break;
      }
      if (ret < 0 || recvBuf[0] != 0x68)
      {
        strName = _T("startServer : recv data has abnormal!\r\n");
        dlg->GetDlgItemText(IDC_EDIT_FRAME_NAME, tmp);                          //��ʾ������
        strName += tmp;
        dlg->SetDlgItemText(IDC_EDIT_FRAME_NAME, strName);

        strName = _T("startServer : recv data has abnormal!\r\n");
        dlg->GetDlgItemText(IDC_EDIT_FRAME_MESSAGE, tmp);                       //��ʾ������
        strName += tmp;
        dlg->SetDlgItemText(IDC_EDIT_FRAME_MESSAGE, strName);

        break;
      }

      memcpy(unionBuf + unionLen, recvBuf, ret);
      unionLen = unionLen + ret;
      while (unionLen > 2 && unionBuf[1] <= unionLen - 2)
      {
        int len = unionBuf[1] + 2;
        memcpy(divBuf, unionBuf, len);

        if (divBuf[1] == 0x04)                        //u����s��ʽ������֡
          dlg->ackConnect(sockConn, divBuf, len);
        else if (divBuf[6] == 0x64)                   //�ͻ�����������
          dlg->ackAllCall(sockConn, divBuf, len);
        else if (divBuf[6] == 0x55)                   //Ѳ�췽������
          dlg->ackWriteDatabases(sockConn, divBuf, len);
        else if (divBuf[6] == 0x66)                    //Ѳ�췽��ִ��
        {
          if (!g_executing_inspection)
          {
            g_executing_inspection = true;
            int tmpLen = len;
            unsigned char thread_data[1024] = { 0 };
            memcpy(thread_data, divBuf, tmpLen);
            std::thread t1(CPelco_ServerDlg::ackExecuteInspection, sockConn, thread_data, tmpLen, dlg);
            t1.detach();
          }
        }
        else if (divBuf[6] == 0xac)
        {
          if (divBuf[12] == 0x11)                     //ִ��Ԥ��λ
            dlg->ackExecuteYuzhiOperation(sockConn, divBuf, len);
          else  if (divBuf[12] == 0x12)               //Ԥ��λ ����׼��
            dlg->ackYuzhiOperation(sockConn, divBuf, len);
          else if (divBuf[12] == 0x14)                //Ԥ��λ ����Ԥ��λ
            dlg->ackWriteYuzhiOperation(sockConn, divBuf, len);
          else                                        //�ֶ�����
            dlg->ackHandOperation(sockConn, divBuf, len);
        }
        else if (divBuf[6] == 0x11)                     //ִ��SQL���
          dlg->ackExecuteSQL(sockConn, divBuf, len);
        else if (divBuf[6] == 0x67)                     //��վ���ز�ѯ��ʷ��Ƶ��Ĳ���
          dlg->ackQueryHistoryVideo(sockConn, divBuf, len);
        else                                            //dlg->MessageBox(_T("û��ƥ�������!"));                    
          dlg->m_asyncFile.writeLog("startServer û��ƥ�������!");
        unionLen = unionLen - len;
        memcpy(unionBuf, unionBuf + len, unionLen);
      }
    }
    closesocket(sockConn);
  }

  return 0;
}

CPelco_ServerDlg::CPelco_ServerDlg(CWnd* pParent /*=NULL*/)
  : CDialogEx(CPelco_ServerDlg::IDD, pParent)
  , m_conn(NULL), m_asyncFile()
{
  m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
  b_show = false;
  neterror = false;
}

void CPelco_ServerDlg::DoDataExchange(CDataExchange* pDX)
{
  CDialogEx::DoDataExchange(pDX);
  //DDX_Control(pDX, IDC_STATIC10, m_chezhongr);
}

BEGIN_MESSAGE_MAP(CPelco_ServerDlg, CDialogEx)
  ON_WM_SYSCOMMAND()
  ON_WM_PAINT()
  ON_WM_QUERYDRAGICON()
  ON_WM_TIMER()
END_MESSAGE_MAP()


// CPelco_ServerDlg ��Ϣ�������

BOOL CPelco_ServerDlg::OnInitDialog()
{
  CDialogEx::OnInitDialog();

  // ��������...���˵�����ӵ�ϵͳ�˵��С�

  // IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
  ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
  ASSERT(IDM_ABOUTBOX < 0xF000);

  CMenu* pSysMenu = GetSystemMenu(FALSE);
  if (pSysMenu != NULL)
  {
    BOOL bNameValid;
    CString strAboutMenu;
    bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
    ASSERT(bNameValid);
    if (!strAboutMenu.IsEmpty())
    {
      pSysMenu->AppendMenu(MF_SEPARATOR);
      pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
    }
  }

  // ���ô˶Ի����ͼ�ꡣ  ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
  //  ִ�д˲���
  SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
  SetIcon(m_hIcon, FALSE);		// ����Сͼ��

  m_connected_middleware = true;

  CIni Ini("config.ini");
  soap_1 = Ini.ReadText("index", "soap");
  soap_2 = Ini.ReadText("index", "soap2");
  para1 = Ini.ReadText("index", "id");
  camip1 = Ini.ReadText("index", "camip");
  camport1 = Ini.ReadText("index", "camport");
  camnum1 = Ini.ReadText("index", "camnum");
  m_time = Ini.ReadInt("index", "time");
  exipaddress = Ini.ReadText("index", "exipaddress");
  midip = Ini.ReadText("index", "midip");

  char* tempip = GetIp();
  myip = (char*)malloc(strlen(tempip) + 1);
  strcpy(myip, tempip);
  char* tempexip = GetExIp();
  myexip = (char*)malloc(strlen(tempexip) + 1);
  strcpy(myexip, tempexip);

  CString tempstr = soap_1;
  SetDlgItemText(IDC_STATIC_SOAP, tempstr);

  tempstr = para1;
  SetDlgItemText(IDC_STATIC_ID, tempstr);

  tempstr = camip1;
  SetDlgItemText(IDC_STATIC_IP, tempstr);

  tempstr = camport1;
  SetDlgItemText(IDC_STATIC_PORT, tempstr);

  tempstr = camnum1;
  SetDlgItemText(IDC_STATIC_NUM, tempstr);

  //tempstr.Format("%d", m_time);
  tempstr = "180 000ms";
  SetDlgItemText(IDC_STATIC_TIME, tempstr);

  tempstr = exipaddress;
  SetDlgItemText(IDC_STATIC_PAGE, tempstr);

  tempstr = myip;
  SetDlgItemText(IDC_STATIC_MYIP, tempstr);

  tempstr = myexip;
  SetDlgItemText(IDC_STATIC_EXIP, tempstr);

  SetDlgItemText(IDC_STATIC_12, _T("Start Out IP .."));
  SetDlgItemText(IDC_STATIC_T, _T("IP SEND MESSAGE PROGRAM"));

  tempstr = midip;
  SetDlgItemText(IDC_STATIC_SOAPIPD, tempstr);
  midip_s = midip;

  para2 = join("REPORTIP|", myip);                           //myip	
  char* para3 = join("REPORTCAM|", camip1);
  char* para4 = join(para3, "|");
  char* para5 = join(para4, camport1);
  char* para6 = join(para5, "|");
  para7 = join(para6, camnum1);

  m_ip = Ini.ReadText("index", "ip");
  m_user = Ini.ReadText("index", "user");
  m_passwd = Ini.ReadText("index", "passwd");
  m_database = Ini.ReadText("index", "database");
  m_port = Ini.ReadInt("index", "port");
  m_sitename = Ini.ReadText("index", "site");

  m_ip_middleware = Ini.ReadText("index", "ip_middleware");
  m_ip_video = Ini.ReadText("index", "ip_video");
  m_ip_net_disk = Ini.ReadText("index", "ip_net_disk");
  //m_ip_left_right_track = Ini.ReadText("index", "ip_left_right_track");
  //m_ip_up_down_track = Ini.ReadText("index", "ip_up_down_track");

  // TODO:  �ڴ���Ӷ���ĳ�ʼ������
  char    HostName[100];
  gethostname(HostName, sizeof(HostName));                              // ��ñ���������.
  hostent* hn;
  hn = gethostbyname(HostName);                                         //���ݱ����������õ�����ip
  CString strIPAddr = inet_ntoa(*(struct in_addr *)hn->h_addr_list[0]); //��ip�����ַ�����ʽ

  if (initDatabase() != 0)                         //��ʼ��MySQL���ݿ�
  {
    MessageBox(_T("��ʼ�����ݿ�ʧ�ܣ�"));
    return FALSE;
  }

  CString str_name;
  str_name = m_sitename;
  str_name += "-";
  str_name += strIPAddr;
  SetWindowText(str_name);

  if (!initSocket())                           //��ʼ���׽��֣�����������ģʽ
  {
    MessageBox(_T("�����׽��ֿ�ʧ�ܣ�"));
    return FALSE;
  }

  std::thread t(startServer, this, std::ref(m_sockSrv)); //���� ���� �߳�
  t.detach();

  SetTimer(1, m_time * 60, NULL);                            //����ʱ�򿪶�ʱ��
  return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

void CPelco_ServerDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
  if ((nID & 0xFFF0) == IDM_ABOUTBOX)
  {
    CAboutDlg dlgAbout;
    dlgAbout.DoModal();
  }
  else
  {
    CDialogEx::OnSysCommand(nID, lParam);
  }
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ  ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CPelco_ServerDlg::OnPaint()
{
  if (IsIconic())
  {
    CPaintDC dc(this); // ���ڻ��Ƶ��豸������

    SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

    // ʹͼ���ڹ����������о���
    int cxIcon = GetSystemMetrics(SM_CXICON);
    int cyIcon = GetSystemMetrics(SM_CYICON);
    CRect rect;
    GetClientRect(&rect);
    int x = (rect.Width() - cxIcon + 1) / 2;
    int y = (rect.Height() - cyIcon + 1) / 2;

    // ����ͼ��
    dc.DrawIcon(x, y, m_hIcon);
  }
  else
  {
    CDialogEx::OnPaint();
  }
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CPelco_ServerDlg::OnQueryDragIcon()
{
  return static_cast<HCURSOR>(m_hIcon);
}


BOOL CPelco_ServerDlg::initSocket()                      //��ʼ���׽���
{
  //�������ڼ������׽���
  m_sockSrv = socket(AF_INET, SOCK_STREAM, 0);

  SOCKADDR_IN addrSrv;
  addrSrv.sin_addr.S_un.S_addr = htonl(ADDR_ANY);
  addrSrv.sin_family = AF_INET;
  addrSrv.sin_port = htons(2404);

  //���׽���
  int ret = bind(m_sockSrv, (SOCKADDR*)&addrSrv, sizeof(SOCKADDR));
  if (ret != 0)
  {
    MessageBox(_T("bind sockSrv failed��"));
    return FALSE;
  }

  //���׽�����Ϊ����ģʽ���ȴ��ͻ�������
  listen(m_sockSrv, 10);

  return TRUE;
}


int CPelco_ServerDlg::displayData(unsigned char* dataBuf, int len) //��������ʾ���Ի���
{
  char buf[1024] = { 0 };
  CString strName, strMsg, tmp;
  std::string logType;

  if (dataBuf[1] == 0x04)                                        //u����s��ʽ������֡
    strName = _T("�ͻ��ˣ�u����s��ʽ������֡��");
  else if (dataBuf[6] == 0x64)                                   //�ͻ�����������
    strName = _T("�ͻ��ˣ��������٣�");
  else if (dataBuf[6] == 0x33)                                   //�ͻ�����Ƶ����
    strName = _T("�ͻ��ˣ���Ƶ����");
  else if (dataBuf[6] == 0xff)                                   //�ͻ��˹������
    strName = _T("�ͻ��ˣ�������ƣ�");
  else if (dataBuf[6] == 0xaa)                                   //�ͻ��˹����Ϣ��ѯ
    strName = _T("�ͻ��ˣ������Ϣ��ѯ��");
  else if (dataBuf[6] == 0x55)
    strName = _T("�ͻ��ˣ�д���ݿ�Ĺ�ϵ��");
  else if (dataBuf[6] == 0x66)
    strName = _T("�ͻ��ˣ�ִ��Ѳ�췽����");
  else if (dataBuf[6] == 0xac)
  {
    if (dataBuf[12] == 0x11)
      strName = _T("�ͻ��ˣ����ù̶�Ԥ��λ��");
    else if (dataBuf[12] == 0x12)
      strName = _T("�ͻ��ˣ�����׼�����");
    else if (dataBuf[12] == 0x13)
      strName = _T("����ˣ���������У��");
    else if (dataBuf[12] == 0x14)
      strName = _T("�ͻ��ˣ�ִ������");
    else if (dataBuf[12] == 0x15)
      strName = _T("����ˣ�ȷ��ִ������");
    else if (dataBuf[13] == 0x07)
      strName = _T("�ͻ��ˣ��ֶ�ģʽ����Ԥ��λ��λ����");
    else if (dataBuf[13] == 0x09)
    {
      if (dataBuf[15] == 0x01)
        strName = _T("�ͻ��ˣ��ֶ�ģʽ�����п�ʼ");
      else if (dataBuf[15] == 0x02)
        strName = _T("�ͻ��ˣ��ֶ�ģʽ�����п�ʼ");
      else if (dataBuf[15] == 0x05)
        strName = _T("�ͻ��ˣ��ֶ�ģʽ�����п�ʼ");
      else if (dataBuf[15] == 0x06)
        strName = _T("�ͻ��ˣ��ֶ�ģʽ�����п�ʼ");
      else
        MessageBox(_T("0xAC 0x09 û��ƥ��Ŀ�ʼ!"));
    }
    else if (dataBuf[13] == 0x0b)
    {
      if (dataBuf[15] == 0x01)
        strName = _T("�ͻ��ˣ��ֶ�ģʽ�����н���");
      else if (dataBuf[15] == 0x02)
        strName = _T("�ͻ��ˣ��ֶ�ģʽ�����н���");
      else if (dataBuf[15] == 0x05)
        strName = _T("�ͻ��ˣ��ֶ�ģʽ�����н���");
      else if (dataBuf[15] == 0x06)
        strName = _T("�ͻ��ˣ��ֶ�ģʽ�����н���");
      else
        MessageBox(_T("0xAC 0x0B û��ƥ��Ľ���!"));
    }
    else
      MessageBox(_T("0xAC û��ƥ�������!"));
  }
  else if (dataBuf[6] == 0x11)
    strName = _T("�ͻ��ˣ�ִ��SQL��䣡");


  logType = strName;
  GetDlgItemText(IDC_EDIT_FRAME_NAME, tmp);                      //��ʾ������
  strName += "\r\n";
  strName += tmp;
  SetDlgItemText(IDC_EDIT_FRAME_NAME, strName);

  for (int i = 0; i < len; ++i)                                  //��ʾ����
  {
    sprintf(buf + strlen(buf), "%02x ", dataBuf[i]);
  }

  m_asyncFile.writeLog(logType + buf);                           /******д��־******/
  strMsg += buf;
  strMsg += "\r\n";
  GetDlgItemText(IDC_EDIT_FRAME_MESSAGE, tmp);
  strMsg += tmp;
  SetDlgItemText(IDC_EDIT_FRAME_MESSAGE, strMsg);

  return 0;
}


int CPelco_ServerDlg::ackConnect(SOCKET sockConn, unsigned char* dataBuf, int len)            //u����s��ʽ������֡
{
  int ret = 0;

  displayData(dataBuf, len);                                 //��ʾ����
  recvSeqAdd1(dataBuf);                                      //�������+1

  dataBuf[2] = 0x0b;                                         //ȷ�����ӻ���ȷ�ϲ���

  ret = send(sockConn, (char*)dataBuf, len, 0);
  if (ret != len)
  {
    MessageBox(_T("ack connnect failed!"));
    return -1;
  }

  return 0;
}


int CPelco_ServerDlg::ackAllCall(SOCKET sockConn, unsigned char* dataBuf, int len)                         //�ͻ�����������
{
  int ret = 0;
  unsigned char recvBuf[BUF_SIZE] = { '\0' };
  displayData(dataBuf, len);                                                  //��ʾ�ͻ���������������
  recvSeqAdd1(dataBuf);                                                       //�������+1
  dataBuf[8] = 0x07;                                                          //����ȷ��(I��ʽ)
  setSendSeqAndRecvSeq(dataBuf);                                              //�����շ����
  ret = send(sockConn, (char*)dataBuf, len, 0);
  if (ret != len)
  {
    MessageBox(_T("all call ack failed! ack allcall !"));
    return -1;
  }

  char dataVideo[] = { 0x68, 0x10, 0, 0, 0, 0, 0x33, 0, 0, 0, 0, 0, 0xc0, 0xa8, 0x83, 0x9a, 0x22, 0x75 };   //��Ƶ��ַ(I��ʽ)
  setSendSeqAndRecvSeq(dataBuf);                                                                            //�����շ����
  ret = send(sockConn, dataVideo, sizeof(dataVideo), 0);
  if (ret != sizeof(dataVideo))
  {
    MessageBox(_T("all call ack failed, video ack fialed!"));
    return -1;
  }
  //***************************** ���޸� **************************************//
  CHandleTrack track = getMemberTrack('0');

  std::string strCoordinate = getStringFromCoordinate(track.getCoordinatePoint3D());           //����
  m_asyncFile.writeLog(strCoordinate);

  time_t now_time;                                                           //��ǰ��ʱ��
  now_time = time(NULL);
  tm* now = localtime(&now_time);
  char nowTime[32] = { '\0' };
  sprintf(nowTime, "%04d%02d%02d%02d%02d%02d", now->tm_year + 1900, now->tm_mon + 1,
          now->tm_mday, now->tm_hour, now->tm_min, now->tm_sec);

  char dataOrbit[64] = { 0x68, 0, 0, 0, 0, 0, 0xbc, 0, 0, 0, 0, 0 };         //�����Ϣ(I��ʽ)
  sprintf(dataOrbit + 12, "%s%s", strCoordinate.c_str(), nowTime);
  int dataOrbitLen = 12 + strCoordinate.size() + strlen(nowTime);

  dataOrbit[1] = dataOrbitLen - 2;
  setSendSeqAndRecvSeq(dataBuf);                                              //�����շ����
  ret = send(sockConn, dataOrbit, dataOrbitLen, 0);
  if (ret != dataOrbitLen)
  {
    MessageBox(_T("all call ack failed, orbit ack fialed!"));
    return -1;
  }

  char finishAllCall[] = { 0x68, 0x0e, 6, 0, 2, 0, 0x64, 1, 0x0a, 0, 0x33, 0, 0, 0, 0, 0 };    //���ٽ���(I��ʽ)
  setSendSeqAndRecvSeq(dataBuf);                             //�����շ����
  ret = send(sockConn, finishAllCall, sizeof(finishAllCall), 0);
  if (ret != sizeof(finishAllCall))
  {
    MessageBox(_T("all call ack failed!"));
    return -1;
  }

  return 0;
}

int CPelco_ServerDlg::initDatabase()
{
  m_conn = mysql_init(NULL);
  char value = 1;
  mysql_options(m_conn, MYSQL_OPT_RECONNECT, (char *)&value);
  if (!mysql_real_connect(m_conn, m_ip, m_user, m_passwd, m_database, m_port, NULL, CLIENT_MULTI_STATEMENTS))
  {
    MessageBox(_T("���ݿ�����ʧ�ܣ�"));
    mysql_close(m_conn);
    return -1;
  }
  mysql_query(m_conn, "set names gbk");

  return 0;
}

int CPelco_ServerDlg::ackExecuteSQL(SOCKET sockConn, unsigned char* dataBuf, int len)                  //ִ��SQL���
{
  int ret = 0;
  displayData(dataBuf, len);
  recvSeqAdd1(dataBuf);                                               //�������+1
  unsigned char recvBuf[1024 * 5] = { 0 };
  dataBuf[len] = '\0';                                                //���c����ַ���
  unsigned char start1 = dataBuf[2];
  unsigned char start2 = dataBuf[3];

  int getTimes = (int)(dataBuf[7]);
  m_asyncFile.writeLog(std::to_string(getTimes) + " ��ѭ��");

  std::string strData, strTotal;
  strTotal = std::string((char*)dataBuf + 12);
  m_asyncFile.writeLog(strTotal);

  for (int i = 0; i < getTimes - 1;)
  {
    ret = recv(sockConn, (char*)recvBuf, BUF_SIZE, 0);                     //���տͻ���S֡
    if (ret == 0)
    {
      MessageBox(_T("all ExecuteSQL ack : recv client ack failed!"));
      return -1;
    }

    int offsetLen = 0;
    recvBuf[ret] = '\0';
    while (ret > 0)                                                       //ճ������
    {
      ++i;                                   //���ڻ����ճ�����⣬���ܼ���forѭ����ʹi = i+1;

      unsigned int dataLen = (unsigned int)recvBuf[offsetLen + 1] + 2;
      displayData(recvBuf + offsetLen, dataLen);                            //������ʾS֡����

      recvSeqAdd1(recvBuf + offsetLen);                                       //�������+1

      strData = std::string((char*)recvBuf + offsetLen + 12, (char*)recvBuf + offsetLen + dataLen);
      strTotal += strData;

      ret -= dataLen;
      offsetLen += dataLen;

      m_asyncFile.writeLog(strData);
    }
  }
  m_asyncFile.writeLog(strTotal);

  strTotal = strTotal.substr(0, strTotal.size() - 1);
  ret = mysql_query(m_conn, strTotal.c_str());                               //�������ݿ�

  do                                                                         //free�ͷ�
  {
    MYSQL_RES* res = mysql_store_result(m_conn);
    mysql_free_result(res);
  } while ((0 == mysql_next_result(m_conn)));

  unsigned int sequence_number = ((unsigned char)start1 >> 1) + (start2 << 7);
  recvBuf[13] = (unsigned char)sequence_number;
  recvBuf[14] = sequence_number >> 8;
  recvBuf[15] = '\0';
  int sendLen = 15;
  recvBuf[1] = 0x0d;
  char* sendData = NULL;
  if (ret != 0)                                          //����ʧ��
  {
    recvBuf[12] = 0;
  }
  else                                                   //����ɹ�
  {
    recvBuf[12] = 1;
  }
  setSendSeqAndRecvSeq(recvBuf);                             //�����շ����
  displayData(recvBuf, sendLen);

  ret = send(sockConn, (char*)recvBuf, sendLen, 0);
  if (ret != sendLen)
  {
    MessageBox(_T("all ExecuteSQL ack failed!"));
    return -1;
  }

  return 0;
}

int CPelco_ServerDlg::ackWriteDatabases(SOCKET sockConn, unsigned char* dataBuf, int len)              //Ѳ�췽������
{
  int ret = 0;
  dataBuf[len] = '\0';                                                //���c����ַ���
  std::string strData((char*)dataBuf + 12);                           //תΪstring���ͣ���������
  displayData(dataBuf, len);
  recvSeqAdd1(dataBuf);                                               //�������+1

  std::string databasesId, operatorId;                                //���ݿ���,1Ϊop_eventtask,2Ϊop_taskjob
  std::string strId, strValue1, strValue2;
  std::vector<std::string> res = Split(strData, ";", true);

  databasesId = res[0];                                               //���ӡ�ɾ�����޸ĵ�����
  operatorId = res[1];
  strId = res[2];
  strValue1 = res[3];
  strValue2 = res[4];

  /******************* ����op_eventtask��� ******************/
  if (databasesId == "1")
  {
    if (operatorId == "1")
    {
      std::string strInsert = "INSERT INTO op_eventtask(seq,eventcode,taskcode) VALUES('" + strId + "','" + strValue1 + "','" + strValue2 + "')";
      ret = mysql_query(m_conn, strInsert.c_str());
      if (ret != 0)
      {
        MessageBox(_T("����op_eventtask��� �� д���ݿ�ʧ�ܣ�"));
      }
    }
    else if (operatorId == "2")                                               //�޸����ݱ�
    {
      std::string strInsert = "UPDATE op_eventtask SET eventcode = '" + strValue1 + "',taskcode='"
        + strValue2 + "'WHERE seq ='" + strId + "'";

      ret = mysql_query(m_conn, strInsert.c_str());
      if (ret != 0)
      {
        MessageBox(_T("����op_eventtask��� �� �޸����ݱ�"));
      }
    }
    else if (operatorId == "3")
    {
      std::string strInsert = "DELETE FROM op_eventtask WHERE seq='" + strId + "'";
      ret = mysql_query(m_conn, strInsert.c_str());
      if (ret != 0)
      {
        MessageBox(_T("����op_eventtask��� �� ɾ�����ݿ�ʧ�ܣ�"));
      }
    }
    else
      MessageBox(_T("����op_eventtask��� �� û����Ӧ�Ĳ�����"));
  }
  /******************* ����op_taskjob���******************/
  else if (databasesId == "2")
  {
    if (operatorId == "1")
    {
      std::string strInsert = "INSERT INTO op_taskjob(seq,taskcode,jobcode) VALUES('" + strId + "','" + strValue1 + "','" + strValue2 + "')";
      ret = mysql_query(m_conn, strInsert.c_str());
      if (ret != 0)
      {
        MessageBox(_T("����op_taskjob��� �� д���ݿ�ʧ�ܣ�"));
      }
    }
    else if (operatorId == "2")
    {
      std::string strInsert = "UPDATE op_taskjob SET taskcode = '" + strValue1 + "',jobcode='"
        + strValue2 + "'WHERE seq='" + strId + "'";
      ret = mysql_query(m_conn, strInsert.c_str());
      if (ret != 0)
      {
        MessageBox(_T("����op_eventtask��� �� �޸����ݱ�ʧ�ܣ�"));
      }
    }
    else if (operatorId == "3")
    {
      std::string strInsert = "DELETE FROM op_taskjob WHERE seq='" + strId + "'";
      ret = mysql_query(m_conn, strInsert.c_str());
      if (ret != 0)
      {
        MessageBox(_T("����op_taskjob��� �� ɾ�����ݿ�ʧ�ܣ�"));
      }
    }
    else
      MessageBox(_T("����op_taskjob��� �� û����Ӧ�Ĳ�����"));
  }
  else
    MessageBox(_T("������� �� û����Ӧ�ı�"));

  dataBuf[8] = 0x07;                                         //Ѳ�췽����������ȷ��(I��ʽ)
  setSendSeqAndRecvSeq(dataBuf);                             //�����շ����
  ret = send(sockConn, (char*)dataBuf, len, 0);
  if (ret != len)
  {
    MessageBox(_T("orbit query ack failed!"));
    return -1;
  }

  return 0;
}

bool prepos_event_task_job_comp(const PreposEventTaskJob& lhs, const PreposEventTaskJob& rhs)                       //����Ԥ��λ+�¼�+����+��ҵ��ɵ��ַ�����С�����ж�
{
  std::string str_lhs = std::to_string(lhs.prepos) + lhs.event + lhs.task + lhs.job;
  std::string str_rhs = std::to_string(rhs.prepos) + rhs.event + rhs.task + rhs.job;

  return str_lhs < str_rhs;
}

int CPelco_ServerDlg::ackExecuteInspection(SOCKET sockConn, unsigned char* dataBuf, int len, CPelco_ServerDlg* dlg)              //Ѳ�췽��ִ��
{
  if (dataBuf[6] != 0x66)
    return -1;
  dlg->displayData(dataBuf, len);                              //��ʾ����
  dlg->recvSeqAdd1(dataBuf);                                   //�������+1
  dataBuf[len] = '\0';                                         //���c����ַ���
  std::string strEvent((char*)dataBuf + 12);
  std::vector<std::string> vecEventCode = dlg->Split(strEvent, ";");
  dlg->m_asyncFile.writeLog("event_code 1pos " + vecEventCode[0]);                                //��ӡ��־

  MYSQL_RES* result;
  MYSQL_ROW row;

  for (int i = 0; i < vecEventCode.size(); ++i)
  {
    //***************************** ���޸� **************************************//
    CHandleTrack track = dlg->getMemberTrack('0');

    std::vector<PreposEventTaskJob> vec_prepos_event_task_job;                                    //PreposEventTaskJob:Ԥ��λ���¼����룬������룬��ҵ����

    std::string event_code_str = vecEventCode[i];                                                 //�¼�����
    dlg->m_asyncFile.writeLog("event_code 2pos " + event_code_str);                                //��ӡ��־

    //��ѯ���¼���Ӧ�������                                                                       
    std::string prepos_event_task_job_query_str =
      "SELECT base_job.preposition, op_eventtask.eventcode, op_eventtask.taskcode, op_taskjob.jobcode FROM base_job, op_eventtask, op_taskjob WHERE op_eventtask.taskcode = op_taskjob.taskcode AND op_taskjob.jobcode = base_job.jobcode AND op_eventtask.eventcode = '" + event_code_str + "'";

    dlg->m_asyncFile.writeLog(prepos_event_task_job_query_str);                                //��ӡ��־

    int ret = mysql_query(dlg->m_conn, prepos_event_task_job_query_str.c_str());
    if (ret != 0)
    {
      dlg->MessageBox(_T("���ݿ��ѯʧ�ܣ�"));
    }
    result = mysql_store_result(dlg->m_conn);
    while (row = mysql_fetch_row(result))
    {
      PreposEventTaskJob tmp;
      tmp.prepos = std::stoi(row[0]);
      tmp.event = row[1];
      tmp.task = row[2];
      tmp.job = row[3];
      vec_prepos_event_task_job.push_back(tmp);
    }


    std::string posStr;
    std::sort(vec_prepos_event_task_job.begin(), vec_prepos_event_task_job.end(), prepos_event_task_job_comp);                //Ԥ��λ����
    for (auto val_prepos_event_task_job : vec_prepos_event_task_job)
    {
      track.callPropos(val_prepos_event_task_job.prepos);                         //����Ԥ��λ

      posStr = std::to_string(val_prepos_event_task_job.prepos);
      dlg->m_asyncFile.writeLog(posStr);

      MYSQL_RES* resultPos;
      MYSQL_ROW rowPos;
      int lrpreposDist = 0, ud_prepos_dist = 0;
      //��ѯ��Ԥ��λ��Ӧ��ƫ��
      std::string prepos_query_str = "select lrdist,uddist from base_prepos where prepos = " + posStr;
      dlg->m_asyncFile.writeLog(prepos_query_str);

      ret = mysql_query(dlg->m_conn, prepos_query_str.c_str());
      if (ret != 0)
      {
        dlg->MessageBox(_T("���ݿ��ѯʧ�ܣ�"));
      }
      resultPos = mysql_store_result(dlg->m_conn);
      while (rowPos = mysql_fetch_row(resultPos))
      {
        lrpreposDist = std::stoi(rowPos[0]);
        ud_prepos_dist = std::stoi(rowPos[1]);
      }
      dlg->m_asyncFile.writeLog(std::to_string(lrpreposDist) + " " + std::to_string(ud_prepos_dist));

      while (abs(lrpreposDist - track.getLeftRightOffsetLen()) > 1)
      {
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
      }
      while (abs(ud_prepos_dist - track.getUpDownOffsetLen()) > 1)
      {
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
      }


      //��ѯ��ҵ��Ӧ������ʶ�͸�����ʶ
      std::string pattern_query_str = "select RECOGNITIONTYPE from base_job where JOBCODE = '" + val_prepos_event_task_job.job + "'";
      dlg->m_asyncFile.writeLog(pattern_query_str);

      ret = mysql_query(dlg->m_conn, pattern_query_str.c_str());
      if (ret != 0)
      {
        dlg->MessageBox(_T("op_jobrecognition���ݿ��ѯʧ�ܣ�"));
      }

      MYSQL_RES* pattern_result;
      MYSQL_ROW pattern_row;
      std::string str_recognition;

      pattern_result = mysql_store_result(dlg->m_conn);
      while (pattern_row = mysql_fetch_row(pattern_result))
      {
        str_recognition = std::stoi(pattern_row[0]);
      }

      unsigned char start_pattern_buf[BUF_SIZE] = { 0x68, 0x00, 0, 0, 0, 0, 0x07, 0, 0, 0, 0, 0, 0x01, 0x01, 0x01 };     //PatternRecognitionģʽʶ��ʼ

      time_t now_time;                                                                          //��ǰ��ʱ��
      now_time = time(NULL);
      tm* now = localtime(&now_time);
      char return_pattern_id[512] = { '\0' };                                                  //ģʽʶ��ID
      sprintf(return_pattern_id, "%s%s%s%04d%02d%02d%02d%02d%02d", val_prepos_event_task_job.event, val_prepos_event_task_job.task, val_prepos_event_task_job.job,
              now->tm_year + 1900, now->tm_mon + 1, now->tm_mday, now->tm_hour, now->tm_min, now->tm_sec);
      sprintf((char*)start_pattern_buf + 13, "%s%s", str_recognition, return_pattern_id);
      int start_pattern_len = 13 + strlen((char*)start_pattern_buf);
      start_pattern_buf[1] = (unsigned char)start_pattern_len - 2;

      dlg->setSendSeqAndRecvSeq(start_pattern_buf);                                              //�����շ����
      ret = send(sockConn, (char*)start_pattern_buf, start_pattern_len, 0);
      if (ret != start_pattern_len)
      {
        dlg->MessageBox(_T("all call ack failed, orbit ack fialed!"));
        return -1;
      }

      SOCKET sockPattern;                                                                        //��ģʽʶ��������                            
      dlg->connectPatternRecognition(sockPattern);
      ret = send(sockPattern, (char*)start_pattern_buf, start_pattern_len, 0);
      if (ret != start_pattern_len)
      {
        dlg->MessageBox(_T("all call ack failed, orbit ack fialed!"));
        return -1;
      }

      std::this_thread::sleep_for(std::chrono::seconds(10));

      dlg->disconnectPatternRecognition(sockPattern);                                            //��ģʽʶ��Ͽ�����

      unsigned char end_pattern_buf[BUF_SIZE] = { 0x68, 0x00, 0, 0, 0, 0, 0x07, 0, 0, 0, 0, 0, 0x02, 0x01, 0x01 };     //PatternRecognitionģʽʶ�����

      now_time = time(NULL);                                                                  //��ǰ��ʱ��
      now = localtime(&now_time);
      sprintf(return_pattern_id, "%s%s%s%04d%02d%02d%02d%02d%02d", val_prepos_event_task_job.event, val_prepos_event_task_job.task, val_prepos_event_task_job.job,
              now->tm_year + 1900, now->tm_mon + 1, now->tm_mday, now->tm_hour, now->tm_min, now->tm_sec);
      sprintf((char*)end_pattern_buf + 13, "%s%s", str_recognition, return_pattern_id);
      int end_pattern_len = 13 + strlen((char*)start_pattern_buf);
      end_pattern_buf[1] = (unsigned char)end_pattern_len - 2;

      dlg->setSendSeqAndRecvSeq(end_pattern_buf);                                              //�����շ����
      ret = send(sockConn, (char*)end_pattern_buf, end_pattern_len, 0);
      if (ret != end_pattern_len)
      {
        dlg->MessageBox(_T("all call ack failed, orbit ack fialed!"));
        return -1;
      }
    }
  }

  unsigned char sendBuf[13] = { 0x68, 0x0B, 0, 0, 0, 0, 0x66, 0, 0, 0, 0, 0, 0 };     //Ѳ�췽������
  dlg->setSendSeqAndRecvSeq(sendBuf);                             //�����շ����
  int ret = send(sockConn, (char*)sendBuf, 13, 0);
  if (ret != 13)
  {
    dlg->MessageBox(_T("CPelco_ServerDlg::ackExecuteInspection ack failed!"));
    return -1;
  }
  dlg->m_asyncFile.writeLog(std::to_string(66));

  g_executing_inspection = false;                             //�Զ�Ѳ�����

  return 0;
}

// �ָ��ַ���
std::vector<std::string> CPelco_ServerDlg::Split(const std::string& str, const std::string& delim, const bool trim_empty)
{
  size_t pos, last_pos = 0, len;
  std::vector<std::string> tokens;

  while (true) {
    pos = str.find(delim, last_pos);                    //������
    if (pos == std::string::npos) {
      pos = str.size();                               //��Ϊ�ַ��ĳ���
    }

    len = pos - last_pos;
    if (!trim_empty || len != 0) {                      //trim_empty�Ƿ������ַ���Ϊ��
      tokens.push_back(str.substr(last_pos, len));
    }

    if (pos == str.size()) {
      break;
    }
    else {
      last_pos = pos + delim.size();
    }
  }

  return tokens;
}


std::string CPelco_ServerDlg::getStringFromCoordinate(double x, double y, double z)
{
  std::string str_x = std::to_string(x);
  std::string str_y = std::to_string(y);
  std::string str_z = std::to_string(z);
  int pos = 0;
  pos = str_x.find('.');
  str_x = str_x.substr(0, pos + 3);             //x����2λ

  pos = str_y.find('.');
  str_y = str_y.substr(0, pos + 3);             //y����2λ

  pos = str_z.find('.');
  str_z = str_z.substr(0, pos + 3);             //z����2λ

  return str_x + ";" + str_y + ";" + str_z + ";";
}

std::string CPelco_ServerDlg::getStringFromCoordinate(Point3D point3d)
{
  std::string str_x = std::to_string(point3d.x);
  std::string str_y = std::to_string(point3d.y);
  std::string str_z = std::to_string(point3d.z);
  int pos = 0;
  pos = str_x.find('.');
  str_x = str_x.substr(0, pos + 3);             //x����2λ

  pos = str_y.find('.');
  str_y = str_y.substr(0, pos + 3);             //y����2λ

  pos = str_z.find('.');
  str_z = str_z.substr(0, pos + 3);             //z����2λ

  return str_x + ";" + str_y + ";" + str_z + ";";
}

void CPelco_ServerDlg::OnTimer(UINT_PTR nIDEvent)
{
  // TODO:  �ڴ������Ϣ�����������/�����Ĭ��ֵ
  CPing p1;
  CPingReply pr1;

  switch (nIDEvent)
  {
  case 1:
  {
    m_connected_middleware = p1.Ping1(midip_s, pr1);
    if (m_connected_middleware)
    {
      neterror = false;
      SetDlgItemText(IDC_STATIC_12, _T("Start Inner IP ..."));
      SlaveExecuteCMD(soap_1, para1, para2);
      SlaveExecuteCMD(soap_1, para1, para7);
    }
    else
    {
      char* tempexip = GetExIp();
      myexip = (char*)malloc(strlen(tempexip) + 1);
      strcpy(myexip, tempexip);
      para2 = join("REPORTIP|", myexip);              //myip	
      char* para3 = join("REPORTCAM|", myexip);
      char* para4 = join(para3, "|");
      char* para5 = join(para4, camport1);
      char* para6 = join(para5, "|");
      para7 = join(para6, camnum1);
      neterror = true;
      SetDlgItemText(IDC_STATIC_12, _T("Start Out IP .."));
      SlaveExecuteCMD(soap_2, para1, para2);
      SlaveExecuteCMD(soap_2, para1, para7);

    }
  }
  break;

  case NET_LOCAL:
    g_connected_local = ::IsNetworkAlive(&g_dw_local);
    if (g_connected_local)
    {
      g_connected_local = InternetGetConnectedState(&g_dw_local, 0);
    }
    if (g_connected_local)
    {
      SetDlgItemText(IDC_EDIT_CONN_ORBITAL, _T("��ͨ"));
    }
    else
    {
      SetDlgItemText(IDC_EDIT_CONN_ORBITAL, _T("δ��ͨ"));
    }
    break;

  case MIDDLEWARE:
    g_connected_middleware = p1.Ping1(m_ip_middleware, pr1);
    if (g_connected_middleware)
    {
      SetDlgItemText(IDC_EDIT_CONN_MIDDLEWARE, _T("��ͨ"));
    }
    else
    {
      SetDlgItemText(IDC_EDIT_CONN_MIDDLEWARE, _T("δ��ͨ"));
    }
    break;

  case VIDEO:
    g_connected_video = p1.Ping1(m_ip_video, pr1);
    if (g_connected_video)
    {
      SetDlgItemText(IDC_EDIT_CONN_VIDEO, _T("��ͨ"));
    }
    else
    {
      SetDlgItemText(IDC_EDIT_CONN_VIDEO, _T("δ��ͨ"));
    }
    break;

  case NET_DISK:
    g_connected_net_disk = p1.Ping1(m_ip_net_disk, pr1);;
    if (g_connected_net_disk)
    {
      SetDlgItemText(IDC_EDIT_CONN_NET_DISK, _T("��ͨ"));
    }
    else
    {
      SetDlgItemText(IDC_EDIT_CONN_NET_DISK, _T("δ��ͨ"));
    }
    break;

  default:
    break;
  }

  CDialogEx::OnTimer(nIDEvent);
}

int CPelco_ServerDlg::ackHandOperation(SOCKET sockConn, unsigned char* dataBuf, int len)               //�ֶ�����
{

  int ret = 0;
  displayData(dataBuf, len);                                 //��ʾ����
  recvSeqAdd1(dataBuf);                                      //�������+1

  dataBuf[2] = 0x0b;                                         //ȷ�����ӻ���ȷ�ϲ���
  setSendSeqAndRecvSeq(dataBuf);                             //�����շ����
  ret = send(sockConn, (char*)dataBuf, len, 0);
  if (ret != len)
  {
    MessageBox(_T("ack connnect failed!"));
    return -1;
  }
  //***************************** ���޸� **************************************//
  CHandleTrack track = getMemberTrack('0');

  if (dataBuf[13] == 0x09)                                  //��ʼ
  {
    if (dataBuf[15] == 0x01)                              //�ֶ�ģʽ�����п�ʼ
      track.startLeft();
    else if (dataBuf[15] == 0x02)                         //�ֶ�ģʽ�����п�ʼ
      track.startRight();
    else if (dataBuf[15] == 0x05)                         //�ֶ�ģʽ�����п�ʼ
      track.startUp();
    else if (dataBuf[15] == 0x06)
      track.startDown();
    else
      MessageBox(_T("0xAC 0x09 û��ƥ��Ŀ�ʼ!"));
  }

  if (dataBuf[13] == 0x0B)                                  //��ʼ
  {
    if (dataBuf[15] == 0x01)                              //�ֶ�ģʽ������ֹͣ
      track.stopLeft();
    else if (dataBuf[15] == 0x02)                         //�ֶ�ģʽ������ֹͣ
      track.stopRight();
    else if (dataBuf[15] == 0x05)                         //�ֶ�ģʽ������ֹͣ
      track.stopUp();
    else if (dataBuf[15] == 0x06)                         //�ֶ�ģʽ������ֹͣ
      track.stopDown();
    else
      MessageBox(_T("0xAC 0x0B û��ƥ��Ŀ�ʼ!"));
  }

  std::string strCoordinate = getStringFromCoordinate(track.getCoordinatePoint3D());    //���� 

  time_t now_time;                                                                      //��ǰ��ʱ��
  now_time = time(NULL);
  tm* now = localtime(&now_time);
  char nowTime[32] = { '\0' };
  sprintf(nowTime, "%04d%02d%02d%02d%02d%02d", now->tm_year + 1900, now->tm_mon + 1,
          now->tm_mday, now->tm_hour, now->tm_min, now->tm_sec);

  char dataOrbit[BUF_SIZE] = { 0x68, 0, 0, 0, 0, 0, 0xbc, 0, 0, 0, 0, 0 };         //�����Ϣ(I��ʽ)
  sprintf(dataOrbit + 12, "%s%s", strCoordinate.c_str(), nowTime);
  int dataOrbitLen = 12 + strCoordinate.size() + strlen(nowTime);
  dataOrbit[1] = dataOrbitLen - 2;

  setSendSeqAndRecvSeq((unsigned char*)dataOrbit);                                  //�����շ����
  ret = send(sockConn, dataOrbit, dataOrbitLen, 0);
  if (ret != dataOrbitLen)
  {
    MessageBox(_T("all call ack failed, orbit ack fialed!"));
    return -1;
  }
  m_asyncFile.writeLog(strCoordinate);

  return 0;
}

void CPelco_ServerDlg::setSendSeqAndRecvSeq(unsigned char* dataBuf)
{
  if ((sendSeq2 & 0xFE) == 0xFE)             //���ӷ�������
    sendSeq3 += 1;
  sendSeq2 += 2;

  dataBuf[2] = sendSeq2;
  dataBuf[3] = sendSeq3;
  dataBuf[4] = recvSeq4;
  dataBuf[5] = recvSeq5;
}


void CPelco_ServerDlg::recvSeqAdd1(unsigned char* dataBuf)
{
  if ((recvSeq4 & 0xFE) == 0xFE)
    recvSeq5 += 1;
  recvSeq4 += 2;
}

int CPelco_ServerDlg::ackYuzhiOperation(SOCKET sockConn, unsigned char* dataBuf, int len)            //Ԥ��λ ����׼��
{
  /*
  by zhaokui 2016.09.23

  dataBuf[6]=0x12Ϊ����׼����         ���ص�����Ϊ��1���ɹ���0��ʧ��
  dataBuf[6]=0x14Ϊ����ִ�У�
  dataBuf[12]�Ժ�����Ϊ����ҵ���룻Ԥ��λ��IP��ַ��
  0x09��0x12 ����Ҫ��������Ϊ ��ҵ���룻Ԥ��λ��IP��ַ�������1���ɹ���0��ʧ�ܣ���

  0x12��ѡ�� 0x13�Ƿ�У
  0x14��ִ�� 0x15�Ƿ��ؽ����
  */

  int ret = 0;
  displayData(dataBuf, len);                                 //��ʾ����
  recvSeqAdd1(dataBuf);                                      //�������+1

  dataBuf[12] = 0x13;                                         //��У    
  dataBuf[len] = ';';
  dataBuf[len + 1] = 'T';

  setSendSeqAndRecvSeq(dataBuf);                             //�����շ����
  dataBuf[1] = len;
  ret = send(sockConn, (char*)dataBuf, len + 2, 0);
  if (ret != len + 2)
  {
    MessageBox(_T("ack yuzhiwei fanjiao failed!"));
    return -1;
  }
  displayData(dataBuf, ret);

  return 0;
}

int CPelco_ServerDlg::ackWriteYuzhiOperation(SOCKET sockConn, unsigned char* dataBuf, int len)             //Ԥ��λ ����Ԥ��λ
{
  /*
  by zhaokui 2016.09.23

  dataBuf[6]=0x12Ϊ����׼����         ���ص�����Ϊ��1���ɹ���0��ʧ��
  dataBuf[6]=0x14Ϊ����ִ�У�
  dataBuf[12]�Ժ�����Ϊ����ҵ���룻Ԥ��λ��IP��ַ��
  0x09��0x12 ����Ҫ��������Ϊ ��ҵ���룻Ԥ��λ��IP��ַ�������1���ɹ���0��ʧ�ܣ���

  0x12��ѡ�� 0x13�Ƿ�У
  0x14��ִ�� 0x15�Ƿ��ؽ����
  */

  int ret = 0;
  displayData(dataBuf, len);                                             //��ʾ����
  recvSeqAdd1(dataBuf);                                                  //�������+1

  if (dataBuf[12] != 0x14)                                               //ִ��
  {
    MessageBox(_T("ack yuzhiwei zhixing failed!"));
    return -1;
  }
  dataBuf[len] = '\0';
  std::string strData((char*)dataBuf + 13);                                //���ݶΣ���ҵ���룻Ԥ��λ��IP��ַ���������
  m_asyncFile.writeLog(strData);

  std::vector<std::string> vecData = Split(strData, ";");

  for (int i = 0; i < vecData.size(); ++i)                                 //��ӡ����������
  {
    m_asyncFile.writeLog(vecData[i]);
  }
  if (vecData[1].size() == 0)                                              //���ַ���ΪNULL
    vecData[1] = "NULL";

  //***************************** ���޸� **************************************//
  CHandleTrack track = getMemberTrack('0');
  if (vecData[1] != "NULL")
  {
    track.setPropos(std::stoi(vecData[1]));                            //����Ԥ��λ

    std::string prepos_update_str = "UPDATE base_prepos SET lrdist = " + std::to_string(track.getLeftRightOffsetLen()) +        //�������ݿ��¼Ԥ��λ��ǰ��ƫ��
      " ,uddist = " + std::to_string(track.getUpDownOffsetLen()) + " where prepos = " + vecData[1];
    m_asyncFile.writeLog(prepos_update_str);
    ret = mysql_query(m_conn, prepos_update_str.c_str());
    if (ret != 0)
    {
      MessageBox(_T("base_prepos���ݿ����ʧ�ܣ�"));
      return -1;
    }
  }

  std::string strUpdate = "UPDATE base_job SET preposition = " + vecData[1] + " where jobcode ='" + vecData[0] + "'";
  m_asyncFile.writeLog(strUpdate);
  ret = mysql_query(m_conn, strUpdate.c_str());

  dataBuf[12] = 0x15;                                                   //Ԥ��λȷ��    
  dataBuf[len] = ';';
  if (ret != 0)
    dataBuf[len + 1] = 'F';
  else
    dataBuf[len + 1] = 'T';
  setSendSeqAndRecvSeq(dataBuf);                                       //�����շ����
  dataBuf[1] = len;
  ret = send(sockConn, (char*)dataBuf, len + 2, 0);
  if (ret != len + 2)
  {
    MessageBox(_T("ack yuzhiwei queren failed!"));
    return -1;
  }
  displayData(dataBuf, len + 2);                                          //��ʾ����

  return 0;
}

int CPelco_ServerDlg::ackExecuteYuzhiOperation(SOCKET sockConn, unsigned char* dataBuf, int len)        //ִ��ָ��Ԥ��λ
{
  int ret = 0;
  displayData(dataBuf, len);                                 //��ʾ����
  recvSeqAdd1(dataBuf);                                      //�������+1
  dataBuf[len] = '\0';

  int prepos = (unsigned int)dataBuf[13];
  m_asyncFile.writeLog(std::to_string(prepos));              //��ӡ��־:Ԥ��λ

  //***************************** ���޸� **************************************//
  CHandleTrack track = getMemberTrack('0');
  track.callPropos(prepos);                                  //����Ԥ��λ

  dataBuf[2] = 0x0b;                                         //ȷ�����ӻ���ȷ�ϲ���
  setSendSeqAndRecvSeq(dataBuf);                             //�����շ����
  ret = send(sockConn, (char*)dataBuf, len, 0);
  if (ret != len)
  {
    MessageBox(_T("ack prepos failed!"));
    return -1;
  }

  return 0;
}


int CPelco_ServerDlg::SlaveExecuteCMD(char* msoap, char* para1, char * para2)
{

  CString ch1;
  //	GetDlgItemText(IDC_EDIT1,ch1);  
  //char * ip;
  char ** ch;
  struct soap *soap = soap_new();

  char * url = "http://***/MessageService.asmx";
  //       struct _ns1__Connect connect;
  //		struct _ns1__ConnectResponse out;
  //struct _ns1__ExecuteCMD execute;
  //struct _ns1__ExecuteCMDResponse out1;

  struct _ns1__SlaveExecuteCMD report;
  //		struct __ns1__SlaveExecuteCMDResponse outt;

  ch = (char **)malloc(2 * sizeof(char *));
  *ch = ch1.GetBuffer(0);
  report.id = para1;
  report.content = para2;
  //		execute.__sizeobj=1;
  //		execute.obj=ch;
  //execute.obj= ch1.GetBuffer(0);;
  // connect.ip =  ch1.GetBuffer(0); //"192.168.131.45";
  soap_set_mode(soap, SOAP_C_UTFSTRING);  //����soap����ΪUTF-8����ֹ��������

  if (soap_call___ns1__SlaveExecuteCMD(soap, msoap, NULL, &report, NULL) == SOAP_OK)
  {
    if (!neterror)
      SetDlgItemText(IDC_STATIC_12, _T("Inner OK"));
    //m_lcddown12.SetText(_T("Inner OK .."));
    else
      SetDlgItemText(IDC_STATIC_12, _T("Out OK"));
    //m_lcddown12.SetText(_T("Out OK .."));
  }
  else
  {
    if (!neterror)
      SetDlgItemText(IDC_STATIC_12, "Inner SOAP error");
    //m_lcddown12.SetText(_T("Inner SOAP error .."));
    else
      SetDlgItemText(IDC_STATIC_12, "Out SOAP error");
    //m_lcddown12.SetText(_T("Out SOAP error .."));
  }


  return 0;
}

char* CPelco_ServerDlg::GetIp()
{
  WORD wVersionRequested = MAKEWORD(1, 1);
  WSADATA wsaData;
  if (WSAStartup(wVersionRequested, &wsaData))
    return NULL;

  LPHOSTENT   lpHostEnt;
  char   szHostName[128];
  static char szHostIp[128];
  CString   strHostIp;

  gethostname(szHostName, 128);
  strHostIp.Format("��������%s\n", szHostName);
  lpHostEnt = gethostbyname(szHostName);
  //	m_strText += strHostIp + _T("\r\n");

  int   i;
  for (i = 0; lpHostEnt->h_addr_list[i] != NULL; i++)
  {
    strcpy(szHostIp, inet_ntoa(*(in_addr   *)lpHostEnt->h_addr_list[i]));
    strHostIp.Format("����IP:%s", szHostIp);
  }
  //	m_strText += strHostIp + _T("\r\n");

  WSACleanup();
  return szHostIp;
}
char* CPelco_ServerDlg::GetExIp()
{

  // �õ�����IP�͵�ַ
  TCHAR szTempPath[MAX_PATH];
  DWORD dwResult = ::GetTempPath(MAX_PATH, szTempPath);

  //������ʱ�ļ�
  TCHAR szTempFile[MAX_PATH];
  UINT nResult = GetTempFileName(szTempPath, _T("~ex"), 0, szTempFile);

  CString strText = _T("");

  //int ret=URLDownloadToFile(NULL,"http://www.ip138.com/ips.asp",szTempFile,BINDF_GETNEWESTVERSION,NULL);
  //int ret=URLDownloadToFile(NULL,"http://city.ip138.com/ip2city.asp",szTempFile,BINDF_GETNEWESTVERSION,NULL);
  //int ret=URLDownloadToFile(NULL,"http://1212.ip138.com/ic.asp",szTempFile,BINDF_GETNEWESTVERSION,NULL);

  //int ret=URLDownloadToFile(NULL,"http://www.ip.cn/",szTempFile,BINDF_GETNEWESTVERSION,NULL);

  int ret = URLDownloadToFile(NULL, exipaddress, szTempFile, BINDF_GETNEWESTVERSION, NULL);
  if (ret == S_OK) //������سɹ�
  {
    CString szFileName = szTempFile;

    CFile file;
    file.Open(szFileName, CFile::modeRead);
    CArchive archive(&file, CArchive::load);

    while (archive.ReadString(strText))
    {
      strText.TrimLeft();
      strText.TrimRight();
      //if (strText.Find(_T("����IP��")) != -1)
      if (strText.Find(_T("����IP��ַ��")) != -1)
      {
        strText = strText.Right(strText.GetLength() - strText.Find(_T("[")));
        strText = strText.Left(strText.GetLength() - 15);
        break;
      }
    }
    archive.Close();
    file.Close();
  }
  else
    //strText = _T("����δ����!"); 
    strText = _T("0");
  int index1 = strText.ReverseFind('[');
  int index2 = strText.ReverseFind(']');
  int index3 = index2 - index1;
  strText = strText.Mid(index1 + 1, index3 - 1);
  //m_strText += "����IP:"+strText;
  //AfxMessageBox(m_strText);
  static char szTemp[128];
  memset(szTemp, 0, sizeof(szTemp));
  strcpy(szTemp, strText.GetBuffer(strText.GetLength()));

  return szTemp;
}
char*  CPelco_ServerDlg::join(char *s1, char *s2)
{
  char *result = (char*)malloc(strlen(s1) + strlen(s2) + 1);//+1 for the zero-terminator  
  //in real code you would check for errors in malloc here  
  if (result == NULL) exit(1);

  strcpy(result, s1);
  strcat(result, s2);

  return result;
}

// ��ȡ����ͷʱ���ȫ�ֺ���
void __stdcall DisConnectFunc(LONG lLoginID, char *pchDVRIP, LONG nDVRPort, DWORD dwUser)
{
  printf("Device disconn, IP=%s+++++++++++++++++++++++\n", pchDVRIP);
}

void __stdcall DownLoadPosCallBackFunc(LONG lPlayHandle, DWORD dwTotalSize, DWORD dwDownLoadSize, DWORD dwUser)
{
  if (dwDownLoadSize == -1)
  {
    printf("PlayBackPosCallBack: 100\n");
  }
  else if (dwTotalSize != 0)
  {
    //	printf("PlayBackPosCallBack: %d%\n", dwDownLoadSize*100/dwTotalSize);
  }
}

int __stdcall DataCallBackFunc(LONG lRealHandle, DWORD dwDataType, BYTE *pBuffer, DWORD dwBufSize, DWORD dwUser)
{
  FILE *file = (FILE *)dwUser;
  if (file)
  {
    fwrite(pBuffer, 1, dwBufSize, file);
  }

  printf("PlayBack: data++++, len=%d+++++++++++++++\n", dwBufSize);

  return 1;
}


int FindByTime(const char szDevIp[], NET_TIME StartTime, NET_TIME StopTime, std::vector<NET_TIME>& vec_time){

  //������ʼʱ��˵�
  LLONG hPlayBack = 0;
  LLONG lLoginHandle = 0;
  int nChannelID = 0; // Ԥ��ͨ����
  NET_DEVICEINFO stDevInfo = { 0 };
  int nError = 0;
  int nPort = 37777;

  //ע�ᵽ�豸���ɹ�����id��ʧ�ܷ���0
  lLoginHandle = CLIENT_Login(szDevIp, nPort, "admin", "admin", &stDevInfo, &nError);
  if (lLoginHandle != 0)
  {
    NET_RECORDFILE_INFO netFileInfo[30] = { 0 };

    //����¼���ļ�---�ɹ������ز�ѯ�����ʧ�ܣ�����0
    int lFindHandle = CLIENT_FindFile(lLoginHandle, nChannelID, 0, NULL, &StartTime, &StopTime, FALSE, 5000);
    if (lFindHandle == 0)
    {
      AfxMessageBox("find file fail,last error");
    }

    NET_RECORDFILE_INFO *pstruFileData = new NET_RECORDFILE_INFO[5000];
    if (pstruFileData == NULL)
    {
      AfxMessageBox("pstruFileData == null\n");
    }

    //��pstruFileData�е�ǰλ�ú����n���ֽ��� 0�滻
    memset(pstruFileData, 0, 5000 * sizeof(NET_RECORDFILE_INFO));

    int nFileIndex = 0;

    while (true)
    {
      //��ѯ��һ��¼���ļ���¼----pstruFileData:¼���ļ���¼���壬���������ѯ����¼���ļ���¼
      int result = CLIENT_FindNextFile(lFindHandle, &pstruFileData[nFileIndex]);
      if (result == 1)//ȡ��һ��¼���ļ���Ϣ
      {
        //����ѯ������뷵��ֵ
        //vec_time[count++] = pstruFileData[nFileIndex].starttime;
        vec_time.push_back(pstruFileData[nFileIndex].starttime);
        nFileIndex++;
        //�洢¼���ļ�
      }
      else if (result == 0)//¼���ļ���Ϣ����ȡ��
      {
        //printf("over \n");
        break;
      }
      else//��������
      {
        AfxMessageBox("find file fail for illegal param\n");
        break;
      }
    }

    //ֹͣ����
    if (lFindHandle != 0)
    {
      CLIENT_FindClose(lFindHandle);
    }

    memcpy(&netFileInfo[0], (void *)&pstruFileData[0], sizeof(NET_RECORDFILE_INFO));

    delete[]pstruFileData;

    FILE *file = fopen("PlayBack.dav", "wb");
    hPlayBack = CLIENT_PlayBackByRecordFileEx(lLoginHandle, &netFileInfo[0], NULL, DownLoadPosCallBackFunc, 0, DataCallBackFunc, (DWORD)file);
  }
  else
  {
    AfxMessageBox("CLIENT_LoginEx failed!");
  }

  return 0;
}

int CPelco_ServerDlg::ackQueryHistoryVideo(SOCKET sockConn, unsigned char* dataBuf, int len)        //��վ���ز�ѯ��ʷ��Ƶ��Ĳ���
{
  CLIENT_Init(DisConnectFunc, 0);

  dataBuf[len] = '\0';                                            //���c����ַ���
  std::string str_data((char*)dataBuf + 12);
  m_asyncFile.writeLog(str_data);
  std::vector<std::string> vec_data = Split(str_data, ";");       // ����ͷID;��ʼʱ��;����ʱ��;һ��IP

  NET_TIME StartTime = { 0 };                                     // ��ʼʱ��
  NET_TIME StopTime = { 0 };                                      // ����ʱ��
  StartTime.dwYear = std::stoi(vec_data[1].substr(0, 4));
  StartTime.dwMonth = std::stoi(vec_data[1].substr(4, 2));
  StartTime.dwDay = std::stoi(vec_data[1].substr(6, 2));
  StartTime.dwHour = std::stoi(vec_data[1].substr(8, 2));
  StartTime.dwMinute = std::stoi(vec_data[1].substr(10, 2));
  StartTime.dwSecond = std::stoi(vec_data[1].substr(12, 2));
  StopTime.dwYear = std::stoi(vec_data[2].substr(0, 4));
  StopTime.dwMonth = std::stoi(vec_data[2].substr(4, 2));
  StopTime.dwDay = std::stoi(vec_data[2].substr(6, 2));
  StopTime.dwHour = std::stoi(vec_data[2].substr(8, 2));
  StopTime.dwMinute = std::stoi(vec_data[2].substr(10, 2));
  StopTime.dwSecond = std::stoi(vec_data[2].substr(12, 2));

  std::vector<NET_TIME> vec_time;
  FindByTime(vec_data[3].c_str(), StartTime, StopTime, vec_time);


  unsigned char sendBuf[512] = { 0x68, 0x00, 0, 0, 0, 0, 0x67, 0, 0, 0, 0, 0, 0 };     //Ѳ�췽������
  for (int i = 0; i < vec_time.size(); ++i)
  {
    setSendSeqAndRecvSeq(sendBuf);                                                       //�����շ����
    sprintf((char*)sendBuf, "%04d%02d%02d%02d%02d%02d;%s", vec_time[i].dwYear, vec_time[i].dwMonth, vec_time[i].dwDay,
            vec_time[i].dwHour, vec_time[i].dwMinute, vec_time[i].dwSecond, vec_data[3]);

    std::string str_tmp((char*)sendBuf + 12);
    m_asyncFile.writeLog(str_tmp);

    int totalLen = 12 + strlen((char*)sendBuf + 12);
    sendBuf[2] = (unsigned int)totalLen - 2;

    int ret = send(sockConn, (char*)sendBuf, totalLen, 0);
    if (ret != totalLen)
    {
      MessageBox(_T("CPelco_ServerDlg::ackExecuteInspection ack failed!"));
      return -1;
    }
  }

  return 0;
}


int CPelco_ServerDlg::initConnectTrack()                                //��ʼ��������������
{
  CIni Ini("config.ini");
  int total_num = Ini.ReadInt("num_track", "total_num");         //��ȡ���������Ŀ(ע��һ���������ָͬһ����ϵ����º����ҹ����)

  char ip_left_right_track_index[] = "ip_left_right_track_0";
  char ip_up_down_track_index[] = "ip_up_down_track_0";

  char port_left_right_track_index[] = "port_left_right_track_0";
  char port_up_down_track_index[] = "port_up_down_track_0";

  char* ip_left_right_track = NULL;
  char* ip_up_down_track = NULL;
  int port_left_right_track = 0;
  int port_up_down_track = 0;

  int ret = 0;
  CString strName, oldMsg;

  for (int i = 0; i < total_num; ++i)
  {
    ip_left_right_track_index[strlen(ip_left_right_track_index) - 1] = (i + '0');
    ip_left_right_track = Ini.ReadText("ip_track", ip_left_right_track_index);                  //��ȡ i �Ź���������� ip
    port_left_right_track_index[strlen(port_left_right_track_index) - 1] = (i + '0');
    port_left_right_track = Ini.ReadInt("port_track", ip_left_right_track_index);               //��ȡ i �Ź���������� port
    ///////////////////////////// �������Ľ��̼�ͨ�� �� ���� ///////////////////////////
    SOCKET sockPelcoLeftRight = socket(AF_INET, SOCK_STREAM, 0);
    SOCKADDR_IN addrPelcoSrvLeftRight;
    addrPelcoSrvLeftRight.sin_addr.S_un.S_addr = inet_addr(ip_left_right_track);
    addrPelcoSrvLeftRight.sin_family = AF_INET;
    addrPelcoSrvLeftRight.sin_port = htons(port_left_right_track);

    ret = connect(sockPelcoLeftRight, (SOCKADDR*)&addrPelcoSrvLeftRight, sizeof(SOCKADDR));
    while (ret != 0)                                                                  //���Ϊ����ʧ�ܣ�������������
    {
      strName.Format(_T("TrackLeftRight Server %d : the connection failed!\r\n"), i);
      GetDlgItemText(IDC_EDIT_FRAME_NAME, oldMsg);                                    //��ʾ������
      strName += oldMsg;
      SetDlgItemText(IDC_EDIT_FRAME_NAME, strName);
      Sleep(1000);
      ret = connect(sockPelcoLeftRight, (SOCKADDR*)&addrPelcoSrvLeftRight, sizeof(SOCKADDR));
    }
    strName.Format(_T("TrackLeftRight Server %d : the connection succeed!\r\n"), i);
    GetDlgItemText(IDC_EDIT_FRAME_NAME, oldMsg);                                      //��ʾ������
    strName += oldMsg;
    SetDlgItemText(IDC_EDIT_FRAME_NAME, strName);

    ip_up_down_track_index[strlen(ip_up_down_track_index) - 1] = (i + '0');
    ip_up_down_track = Ini.ReadText("ip_track", ip_up_down_track_index);              //��ȡ i �Ź���������� ip
    port_up_down_track_index[strlen(port_up_down_track_index) - 1] = (i + '0');
    port_up_down_track = Ini.ReadInt("port_track", port_up_down_track_index);         //��ȡ i �Ź���������� port
    ///////////////////////////// �������Ľ��̼�ͨ�� �� ���� ///////////////////////////
    SOCKET sockPelcoUpDown = socket(AF_INET, SOCK_STREAM, 0);
    SOCKADDR_IN addrPelcoSrvUpDown;
    addrPelcoSrvUpDown.sin_addr.S_un.S_addr = inet_addr(ip_up_down_track);
    addrPelcoSrvUpDown.sin_family = AF_INET;
    addrPelcoSrvUpDown.sin_port = htons(port_up_down_track);

    ret = connect(sockPelcoUpDown, (SOCKADDR*)&addrPelcoSrvUpDown, sizeof(SOCKADDR));
    while (ret != 0)                                                                  //���Ϊ����ʧ�ܣ�������������
    {
      strName.Format(_T("TrackUpDown Server %d : the connection failed!\r\n"), i);
      GetDlgItemText(IDC_EDIT_FRAME_NAME, oldMsg);                                    //��ʾ������
      strName += oldMsg;
      SetDlgItemText(IDC_EDIT_FRAME_NAME, strName);
      Sleep(1000);
      ret = connect(sockPelcoUpDown, (SOCKADDR*)&addrPelcoSrvUpDown, sizeof(SOCKADDR));
    }
    strName.Format(_T("TrackUpDown Server %d : the connection succeed!\r\n"), i);
    GetDlgItemText(IDC_EDIT_FRAME_NAME, oldMsg);                                      //��ʾ������
    strName += oldMsg;
    SetDlgItemText(IDC_EDIT_FRAME_NAME, strName);

    CHandleTrack track = getMemberTrack(i);                                         //Ϊ�����ļ��е�track��ʼ��        
    track = CHandleTrack(sockPelcoLeftRight, sockPelcoUpDown);
  }

  return 0;
}


CHandleTrack& CPelco_ServerDlg::getMemberTrack(const int num)
{
  assert(num >= 0 && num <= 5);                                   //track������Ӧ����[0,5]
  switch (num)
  {
  case 0:
    return m_track_0;
    break;

  case 1:
    return m_track_1;
    break;

  case 2:
    return m_track_2;
    break;

  case 3:
    return m_track_3;
    break;

  case 4:
    return m_track_4;
    break;

  case 5:
    return m_track_5;
    break;

  default:
    break;
  }
}

const CHandleTrack& CPelco_ServerDlg::getMemberTrack(const char num)
{
  assert(num >= 0 && num <= 5);                                   //track������Ӧ����['0','5']
  switch (num)
  {
  case '0':
    return m_track_0;
    break;

  case '1':
    return m_track_1;
    break;

  case '2':
    return m_track_2;
    break;

  case '3':
    return m_track_3;
    break;

  case '4':
    return m_track_4;
    break;

  case '5':
    return m_track_5;
    break;

  default:
    break;
  }
}

int CPelco_ServerDlg::connectPatternRecognition(SOCKET& sockPattern)
{
  int ret = 0;
  CString strName, oldMsg;

  sockPattern = socket(AF_INET, SOCK_STREAM, 0);
  SOCKADDR_IN addrPatternServer;
  //********************** ��Ҫ�޸� *****************************//
  addrPatternServer.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
  addrPatternServer.sin_family = AF_INET;
  addrPatternServer.sin_port = htons(9900);

  ret = connect(sockPattern, (SOCKADDR*)&addrPatternServer, sizeof(SOCKADDR));
  while (ret != 0)                                                                  //���Ϊ����ʧ�ܣ�������������
  {
    strName = _T("TrackLeftRight Server : the connection failed!\r\n");
    GetDlgItemText(IDC_EDIT_FRAME_NAME, oldMsg);                                    //��ʾ������
    strName += oldMsg;
    SetDlgItemText(IDC_EDIT_FRAME_NAME, strName);
    Sleep(1000);
    ret = connect(sockPattern, (SOCKADDR*)&addrPatternServer, sizeof(SOCKADDR));
  }
  strName = _T("TrackLeftRight Server %d : the connection succeed!\r\n");
  GetDlgItemText(IDC_EDIT_FRAME_NAME, oldMsg);                                      //��ʾ������
  strName += oldMsg;
  SetDlgItemText(IDC_EDIT_FRAME_NAME, strName);

  return 0;
}


int CPelco_ServerDlg::disconnectPatternRecognition(SOCKET& sockPattern)
{
  if (sockPattern)
  {
    closesocket(sockPattern);
    sockPattern = 0;
  }
  return 0;
}