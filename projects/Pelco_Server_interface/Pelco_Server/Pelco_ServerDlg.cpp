
// Pelco_ServerDlg.cpp : 实现文件
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


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
  CAboutDlg();

  // 对话框数据
  enum { IDD = IDD_ABOUTBOX };

protected:
  virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

  // 实现
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


// CPelco_ServerDlg 对话框


int startServer(CPelco_ServerDlg* dlg, SOCKET& sockSrv)              //启动服务端
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
    //等待客户端请求的到来
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

      //接收数据
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
        dlg->GetDlgItemText(IDC_EDIT_FRAME_NAME, tmp);                       //显示报文名
        strName += tmp;
        dlg->SetDlgItemText(IDC_EDIT_FRAME_NAME, strName);

        strName = _T("startServer : the connection has been closed!\r\n");
        dlg->GetDlgItemText(IDC_EDIT_FRAME_MESSAGE, tmp);                       //显示报文名
        strName += tmp;
        dlg->SetDlgItemText(IDC_EDIT_FRAME_MESSAGE, strName);

        break;
      }
      if (ret < 0 || recvBuf[0] != 0x68)
      {
        strName = _T("startServer : recv data has abnormal!\r\n");
        dlg->GetDlgItemText(IDC_EDIT_FRAME_NAME, tmp);                          //显示报文名
        strName += tmp;
        dlg->SetDlgItemText(IDC_EDIT_FRAME_NAME, strName);

        strName = _T("startServer : recv data has abnormal!\r\n");
        dlg->GetDlgItemText(IDC_EDIT_FRAME_MESSAGE, tmp);                       //显示报文名
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

        if (divBuf[1] == 0x04)                        //u或者s格式的数据帧
          dlg->ackConnect(sockConn, divBuf, len);
        else if (divBuf[6] == 0x64)                   //客户端请求总召
          dlg->ackAllCall(sockConn, divBuf, len);
        else if (divBuf[6] == 0x55)                   //巡检方案设置
          dlg->ackWriteDatabases(sockConn, divBuf, len);
        else if (divBuf[6] == 0x66)                    //巡检方案执行
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
          if (divBuf[12] == 0x11)                     //执行预置位
            dlg->ackExecuteYuzhiOperation(sockConn, divBuf, len);
          else  if (divBuf[12] == 0x12)               //预置位 数据准备
            dlg->ackYuzhiOperation(sockConn, divBuf, len);
          else if (divBuf[12] == 0x14)                //预置位 设置预置位
            dlg->ackWriteYuzhiOperation(sockConn, divBuf, len);
          else                                        //手动控制
            dlg->ackHandOperation(sockConn, divBuf, len);
        }
        else if (divBuf[6] == 0x11)                     //执行SQL语句
          dlg->ackExecuteSQL(sockConn, divBuf, len);
        else if (divBuf[6] == 0x67)                     //从站返回查询历史视频后的参数
          dlg->ackQueryHistoryVideo(sockConn, divBuf, len);
        else                                            //dlg->MessageBox(_T("没有匹配的类型!"));                    
          dlg->m_asyncFile.writeLog("startServer 没有匹配的类型!");
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


// CPelco_ServerDlg 消息处理程序

BOOL CPelco_ServerDlg::OnInitDialog()
{
  CDialogEx::OnInitDialog();

  // 将“关于...”菜单项添加到系统菜单中。

  // IDM_ABOUTBOX 必须在系统命令范围内。
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

  // 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
  //  执行此操作
  SetIcon(m_hIcon, TRUE);			// 设置大图标
  SetIcon(m_hIcon, FALSE);		// 设置小图标

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

  // TODO:  在此添加额外的初始化代码
  char    HostName[100];
  gethostname(HostName, sizeof(HostName));                              // 获得本机主机名.
  hostent* hn;
  hn = gethostbyname(HostName);                                         //根据本机主机名得到本机ip
  CString strIPAddr = inet_ntoa(*(struct in_addr *)hn->h_addr_list[0]); //把ip换成字符串形式

  if (initDatabase() != 0)                         //初始化MySQL数据库
  {
    MessageBox(_T("初始化数据库失败！"));
    return FALSE;
  }

  CString str_name;
  str_name = m_sitename;
  str_name += "-";
  str_name += strIPAddr;
  SetWindowText(str_name);

  if (!initSocket())                           //初始化套接字，并启动监听模式
  {
    MessageBox(_T("加载套接字库失败！"));
    return FALSE;
  }

  std::thread t(startServer, this, std::ref(m_sockSrv)); //创建 服务 线程
  t.detach();

  SetTimer(1, m_time * 60, NULL);                            //启动时打开定时器
  return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
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

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CPelco_ServerDlg::OnPaint()
{
  if (IsIconic())
  {
    CPaintDC dc(this); // 用于绘制的设备上下文

    SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

    // 使图标在工作区矩形中居中
    int cxIcon = GetSystemMetrics(SM_CXICON);
    int cyIcon = GetSystemMetrics(SM_CYICON);
    CRect rect;
    GetClientRect(&rect);
    int x = (rect.Width() - cxIcon + 1) / 2;
    int y = (rect.Height() - cyIcon + 1) / 2;

    // 绘制图标
    dc.DrawIcon(x, y, m_hIcon);
  }
  else
  {
    CDialogEx::OnPaint();
  }
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CPelco_ServerDlg::OnQueryDragIcon()
{
  return static_cast<HCURSOR>(m_hIcon);
}


BOOL CPelco_ServerDlg::initSocket()                      //初始化套接字
{
  //创建用于监听的套接字
  m_sockSrv = socket(AF_INET, SOCK_STREAM, 0);

  SOCKADDR_IN addrSrv;
  addrSrv.sin_addr.S_un.S_addr = htonl(ADDR_ANY);
  addrSrv.sin_family = AF_INET;
  addrSrv.sin_port = htons(2404);

  //绑定套接字
  int ret = bind(m_sockSrv, (SOCKADDR*)&addrSrv, sizeof(SOCKADDR));
  if (ret != 0)
  {
    MessageBox(_T("bind sockSrv failed！"));
    return FALSE;
  }

  //将套接字设为监听模式，等待客户端请求
  listen(m_sockSrv, 10);

  return TRUE;
}


int CPelco_ServerDlg::displayData(unsigned char* dataBuf, int len) //将数据显示到对话框
{
  char buf[1024] = { 0 };
  CString strName, strMsg, tmp;
  std::string logType;

  if (dataBuf[1] == 0x04)                                        //u或者s格式的数据帧
    strName = _T("客户端：u或者s格式的数据帧！");
  else if (dataBuf[6] == 0x64)                                   //客户端请求总召
    strName = _T("客户端：请求总召！");
  else if (dataBuf[6] == 0x33)                                   //客户端视频请求
    strName = _T("客户端：视频请求！");
  else if (dataBuf[6] == 0xff)                                   //客户端轨道控制
    strName = _T("客户端：轨道控制！");
  else if (dataBuf[6] == 0xaa)                                   //客户端轨道信息查询
    strName = _T("客户端：轨道信息查询！");
  else if (dataBuf[6] == 0x55)
    strName = _T("客户端：写数据库的关系表！");
  else if (dataBuf[6] == 0x66)
    strName = _T("客户端：执行巡检方案！");
  else if (dataBuf[6] == 0xac)
  {
    if (dataBuf[12] == 0x11)
      strName = _T("客户端：调用固定预置位！");
    else if (dataBuf[12] == 0x12)
      strName = _T("客户端：数据准备完毕");
    else if (dataBuf[12] == 0x13)
      strName = _T("服务端：返回数据校验");
    else if (dataBuf[12] == 0x14)
      strName = _T("客户端：执行数据");
    else if (dataBuf[12] == 0x15)
      strName = _T("服务端：确认执行数据");
    else if (dataBuf[13] == 0x07)
      strName = _T("客户端：手动模式：传预置位定位到表");
    else if (dataBuf[13] == 0x09)
    {
      if (dataBuf[15] == 0x01)
        strName = _T("客户端：手动模式：左行开始");
      else if (dataBuf[15] == 0x02)
        strName = _T("客户端：手动模式：右行开始");
      else if (dataBuf[15] == 0x05)
        strName = _T("客户端：手动模式：上行开始");
      else if (dataBuf[15] == 0x06)
        strName = _T("客户端：手动模式：下行开始");
      else
        MessageBox(_T("0xAC 0x09 没有匹配的开始!"));
    }
    else if (dataBuf[13] == 0x0b)
    {
      if (dataBuf[15] == 0x01)
        strName = _T("客户端：手动模式：左行结束");
      else if (dataBuf[15] == 0x02)
        strName = _T("客户端：手动模式：右行结束");
      else if (dataBuf[15] == 0x05)
        strName = _T("客户端：手动模式：上行结束");
      else if (dataBuf[15] == 0x06)
        strName = _T("客户端：手动模式：下行结束");
      else
        MessageBox(_T("0xAC 0x0B 没有匹配的结束!"));
    }
    else
      MessageBox(_T("0xAC 没有匹配的命令!"));
  }
  else if (dataBuf[6] == 0x11)
    strName = _T("客户端：执行SQL语句！");


  logType = strName;
  GetDlgItemText(IDC_EDIT_FRAME_NAME, tmp);                      //显示报文名
  strName += "\r\n";
  strName += tmp;
  SetDlgItemText(IDC_EDIT_FRAME_NAME, strName);

  for (int i = 0; i < len; ++i)                                  //显示报文
  {
    sprintf(buf + strlen(buf), "%02x ", dataBuf[i]);
  }

  m_asyncFile.writeLog(logType + buf);                           /******写日志******/
  strMsg += buf;
  strMsg += "\r\n";
  GetDlgItemText(IDC_EDIT_FRAME_MESSAGE, tmp);
  strMsg += tmp;
  SetDlgItemText(IDC_EDIT_FRAME_MESSAGE, strMsg);

  return 0;
}


int CPelco_ServerDlg::ackConnect(SOCKET sockConn, unsigned char* dataBuf, int len)            //u或者s格式的数据帧
{
  int ret = 0;

  displayData(dataBuf, len);                                 //显示数据
  recvSeqAdd1(dataBuf);                                      //接收序号+1

  dataBuf[2] = 0x0b;                                         //确认连接或者确认测试

  ret = send(sockConn, (char*)dataBuf, len, 0);
  if (ret != len)
  {
    MessageBox(_T("ack connnect failed!"));
    return -1;
  }

  return 0;
}


int CPelco_ServerDlg::ackAllCall(SOCKET sockConn, unsigned char* dataBuf, int len)                         //客户端请求总召
{
  int ret = 0;
  unsigned char recvBuf[BUF_SIZE] = { '\0' };
  displayData(dataBuf, len);                                                  //显示客户端总召请求报文名
  recvSeqAdd1(dataBuf);                                                       //接收序号+1
  dataBuf[8] = 0x07;                                                          //总召确认(I格式)
  setSendSeqAndRecvSeq(dataBuf);                                              //设置收发序号
  ret = send(sockConn, (char*)dataBuf, len, 0);
  if (ret != len)
  {
    MessageBox(_T("all call ack failed! ack allcall !"));
    return -1;
  }

  char dataVideo[] = { 0x68, 0x10, 0, 0, 0, 0, 0x33, 0, 0, 0, 0, 0, 0xc0, 0xa8, 0x83, 0x9a, 0x22, 0x75 };   //视频地址(I格式)
  setSendSeqAndRecvSeq(dataBuf);                                                                            //设置收发序号
  ret = send(sockConn, dataVideo, sizeof(dataVideo), 0);
  if (ret != sizeof(dataVideo))
  {
    MessageBox(_T("all call ack failed, video ack fialed!"));
    return -1;
  }
  //***************************** 待修改 **************************************//
  CHandleTrack track = getMemberTrack('0');

  std::string strCoordinate = getStringFromCoordinate(track.getCoordinatePoint3D());           //坐标
  m_asyncFile.writeLog(strCoordinate);

  time_t now_time;                                                           //当前的时间
  now_time = time(NULL);
  tm* now = localtime(&now_time);
  char nowTime[32] = { '\0' };
  sprintf(nowTime, "%04d%02d%02d%02d%02d%02d", now->tm_year + 1900, now->tm_mon + 1,
          now->tm_mday, now->tm_hour, now->tm_min, now->tm_sec);

  char dataOrbit[64] = { 0x68, 0, 0, 0, 0, 0, 0xbc, 0, 0, 0, 0, 0 };         //轨道信息(I格式)
  sprintf(dataOrbit + 12, "%s%s", strCoordinate.c_str(), nowTime);
  int dataOrbitLen = 12 + strCoordinate.size() + strlen(nowTime);

  dataOrbit[1] = dataOrbitLen - 2;
  setSendSeqAndRecvSeq(dataBuf);                                              //设置收发序号
  ret = send(sockConn, dataOrbit, dataOrbitLen, 0);
  if (ret != dataOrbitLen)
  {
    MessageBox(_T("all call ack failed, orbit ack fialed!"));
    return -1;
  }

  char finishAllCall[] = { 0x68, 0x0e, 6, 0, 2, 0, 0x64, 1, 0x0a, 0, 0x33, 0, 0, 0, 0, 0 };    //总召结束(I格式)
  setSendSeqAndRecvSeq(dataBuf);                             //设置收发序号
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
    MessageBox(_T("数据库连接失败！"));
    mysql_close(m_conn);
    return -1;
  }
  mysql_query(m_conn, "set names gbk");

  return 0;
}

int CPelco_ServerDlg::ackExecuteSQL(SOCKET sockConn, unsigned char* dataBuf, int len)                  //执行SQL语句
{
  int ret = 0;
  displayData(dataBuf, len);
  recvSeqAdd1(dataBuf);                                               //接收序号+1
  unsigned char recvBuf[1024 * 5] = { 0 };
  dataBuf[len] = '\0';                                                //变成c风格字符串
  unsigned char start1 = dataBuf[2];
  unsigned char start2 = dataBuf[3];

  int getTimes = (int)(dataBuf[7]);
  m_asyncFile.writeLog(std::to_string(getTimes) + " 次循环");

  std::string strData, strTotal;
  strTotal = std::string((char*)dataBuf + 12);
  m_asyncFile.writeLog(strTotal);

  for (int i = 0; i < getTimes - 1;)
  {
    ret = recv(sockConn, (char*)recvBuf, BUF_SIZE, 0);                     //接收客户端S帧
    if (ret == 0)
    {
      MessageBox(_T("all ExecuteSQL ack : recv client ack failed!"));
      return -1;
    }

    int offsetLen = 0;
    recvBuf[ret] = '\0';
    while (ret > 0)                                                       //粘包处理
    {
      ++i;                                   //由于会出现粘包问题，不能简单在for循环中使i = i+1;

      unsigned int dataLen = (unsigned int)recvBuf[offsetLen + 1] + 2;
      displayData(recvBuf + offsetLen, dataLen);                            //界面显示S帧数据

      recvSeqAdd1(recvBuf + offsetLen);                                       //接收序号+1

      strData = std::string((char*)recvBuf + offsetLen + 12, (char*)recvBuf + offsetLen + dataLen);
      strTotal += strData;

      ret -= dataLen;
      offsetLen += dataLen;

      m_asyncFile.writeLog(strData);
    }
  }
  m_asyncFile.writeLog(strTotal);

  strTotal = strTotal.substr(0, strTotal.size() - 1);
  ret = mysql_query(m_conn, strTotal.c_str());                               //插入数据库

  do                                                                         //free释放
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
  if (ret != 0)                                          //插入失败
  {
    recvBuf[12] = 0;
  }
  else                                                   //插入成功
  {
    recvBuf[12] = 1;
  }
  setSendSeqAndRecvSeq(recvBuf);                             //设置收发序号
  displayData(recvBuf, sendLen);

  ret = send(sockConn, (char*)recvBuf, sendLen, 0);
  if (ret != sendLen)
  {
    MessageBox(_T("all ExecuteSQL ack failed!"));
    return -1;
  }

  return 0;
}

int CPelco_ServerDlg::ackWriteDatabases(SOCKET sockConn, unsigned char* dataBuf, int len)              //巡检方案设置
{
  int ret = 0;
  dataBuf[len] = '\0';                                                //变成c风格字符串
  std::string strData((char*)dataBuf + 12);                           //转为string类型，便于运算
  displayData(dataBuf, len);
  recvSeqAdd1(dataBuf);                                               //接收序号+1

  std::string databasesId, operatorId;                                //数据库标号,1为op_eventtask,2为op_taskjob
  std::string strId, strValue1, strValue2;
  std::vector<std::string> res = Split(strData, ";", true);

  databasesId = res[0];                                               //增加、删除和修改的数据
  operatorId = res[1];
  strId = res[2];
  strValue1 = res[3];
  strValue2 = res[4];

  /******************* 操作op_eventtask表格 ******************/
  if (databasesId == "1")
  {
    if (operatorId == "1")
    {
      std::string strInsert = "INSERT INTO op_eventtask(seq,eventcode,taskcode) VALUES('" + strId + "','" + strValue1 + "','" + strValue2 + "')";
      ret = mysql_query(m_conn, strInsert.c_str());
      if (ret != 0)
      {
        MessageBox(_T("操作op_eventtask表格 ： 写数据库失败！"));
      }
    }
    else if (operatorId == "2")                                               //修改数据表
    {
      std::string strInsert = "UPDATE op_eventtask SET eventcode = '" + strValue1 + "',taskcode='"
        + strValue2 + "'WHERE seq ='" + strId + "'";

      ret = mysql_query(m_conn, strInsert.c_str());
      if (ret != 0)
      {
        MessageBox(_T("操作op_eventtask表格 ： 修改数据表！"));
      }
    }
    else if (operatorId == "3")
    {
      std::string strInsert = "DELETE FROM op_eventtask WHERE seq='" + strId + "'";
      ret = mysql_query(m_conn, strInsert.c_str());
      if (ret != 0)
      {
        MessageBox(_T("操作op_eventtask表格 ： 删除数据库失败！"));
      }
    }
    else
      MessageBox(_T("操作op_eventtask表格 ： 没有相应的操作！"));
  }
  /******************* 操作op_taskjob表格******************/
  else if (databasesId == "2")
  {
    if (operatorId == "1")
    {
      std::string strInsert = "INSERT INTO op_taskjob(seq,taskcode,jobcode) VALUES('" + strId + "','" + strValue1 + "','" + strValue2 + "')";
      ret = mysql_query(m_conn, strInsert.c_str());
      if (ret != 0)
      {
        MessageBox(_T("操作op_taskjob表格 ： 写数据库失败！"));
      }
    }
    else if (operatorId == "2")
    {
      std::string strInsert = "UPDATE op_taskjob SET taskcode = '" + strValue1 + "',jobcode='"
        + strValue2 + "'WHERE seq='" + strId + "'";
      ret = mysql_query(m_conn, strInsert.c_str());
      if (ret != 0)
      {
        MessageBox(_T("操作op_eventtask表格 ： 修改数据表失败！"));
      }
    }
    else if (operatorId == "3")
    {
      std::string strInsert = "DELETE FROM op_taskjob WHERE seq='" + strId + "'";
      ret = mysql_query(m_conn, strInsert.c_str());
      if (ret != 0)
      {
        MessageBox(_T("操作op_taskjob表格 ： 删除数据库失败！"));
      }
    }
    else
      MessageBox(_T("操作op_taskjob表格 ： 没有相应的操作！"));
  }
  else
    MessageBox(_T("操作表格 ： 没有相应的表！"));

  dataBuf[8] = 0x07;                                         //巡检方案设置请求确认(I格式)
  setSendSeqAndRecvSeq(dataBuf);                             //设置收发序号
  ret = send(sockConn, (char*)dataBuf, len, 0);
  if (ret != len)
  {
    MessageBox(_T("orbit query ack failed!"));
    return -1;
  }

  return 0;
}

bool prepos_event_task_job_comp(const PreposEventTaskJob& lhs, const PreposEventTaskJob& rhs)                       //根据预置位+事件+任务+作业组成的字符串大小进行判断
{
  std::string str_lhs = std::to_string(lhs.prepos) + lhs.event + lhs.task + lhs.job;
  std::string str_rhs = std::to_string(rhs.prepos) + rhs.event + rhs.task + rhs.job;

  return str_lhs < str_rhs;
}

int CPelco_ServerDlg::ackExecuteInspection(SOCKET sockConn, unsigned char* dataBuf, int len, CPelco_ServerDlg* dlg)              //巡检方案执行
{
  if (dataBuf[6] != 0x66)
    return -1;
  dlg->displayData(dataBuf, len);                              //显示数据
  dlg->recvSeqAdd1(dataBuf);                                   //接收序号+1
  dataBuf[len] = '\0';                                         //变成c风格字符串
  std::string strEvent((char*)dataBuf + 12);
  std::vector<std::string> vecEventCode = dlg->Split(strEvent, ";");
  dlg->m_asyncFile.writeLog("event_code 1pos " + vecEventCode[0]);                                //打印日志

  MYSQL_RES* result;
  MYSQL_ROW row;

  for (int i = 0; i < vecEventCode.size(); ++i)
  {
    //***************************** 待修改 **************************************//
    CHandleTrack track = dlg->getMemberTrack('0');

    std::vector<PreposEventTaskJob> vec_prepos_event_task_job;                                    //PreposEventTaskJob:预置位，事件编码，任务编码，作业编码

    std::string event_code_str = vecEventCode[i];                                                 //事件编码
    dlg->m_asyncFile.writeLog("event_code 2pos " + event_code_str);                                //打印日志

    //查询该事件对应任务编码                                                                       
    std::string prepos_event_task_job_query_str =
      "SELECT base_job.preposition, op_eventtask.eventcode, op_eventtask.taskcode, op_taskjob.jobcode FROM base_job, op_eventtask, op_taskjob WHERE op_eventtask.taskcode = op_taskjob.taskcode AND op_taskjob.jobcode = base_job.jobcode AND op_eventtask.eventcode = '" + event_code_str + "'";

    dlg->m_asyncFile.writeLog(prepos_event_task_job_query_str);                                //打印日志

    int ret = mysql_query(dlg->m_conn, prepos_event_task_job_query_str.c_str());
    if (ret != 0)
    {
      dlg->MessageBox(_T("数据库查询失败！"));
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
    std::sort(vec_prepos_event_task_job.begin(), vec_prepos_event_task_job.end(), prepos_event_task_job_comp);                //预置位排序
    for (auto val_prepos_event_task_job : vec_prepos_event_task_job)
    {
      track.callPropos(val_prepos_event_task_job.prepos);                         //调用预置位

      posStr = std::to_string(val_prepos_event_task_job.prepos);
      dlg->m_asyncFile.writeLog(posStr);

      MYSQL_RES* resultPos;
      MYSQL_ROW rowPos;
      int lrpreposDist = 0, ud_prepos_dist = 0;
      //查询该预置位对应的偏移
      std::string prepos_query_str = "select lrdist,uddist from base_prepos where prepos = " + posStr;
      dlg->m_asyncFile.writeLog(prepos_query_str);

      ret = mysql_query(dlg->m_conn, prepos_query_str.c_str());
      if (ret != 0)
      {
        dlg->MessageBox(_T("数据库查询失败！"));
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


      //查询作业对应的主标识和辅助标识
      std::string pattern_query_str = "select RECOGNITIONTYPE from base_job where JOBCODE = '" + val_prepos_event_task_job.job + "'";
      dlg->m_asyncFile.writeLog(pattern_query_str);

      ret = mysql_query(dlg->m_conn, pattern_query_str.c_str());
      if (ret != 0)
      {
        dlg->MessageBox(_T("op_jobrecognition数据库查询失败！"));
      }

      MYSQL_RES* pattern_result;
      MYSQL_ROW pattern_row;
      std::string str_recognition;

      pattern_result = mysql_store_result(dlg->m_conn);
      while (pattern_row = mysql_fetch_row(pattern_result))
      {
        str_recognition = std::stoi(pattern_row[0]);
      }

      unsigned char start_pattern_buf[BUF_SIZE] = { 0x68, 0x00, 0, 0, 0, 0, 0x07, 0, 0, 0, 0, 0, 0x01, 0x01, 0x01 };     //PatternRecognition模式识别开始

      time_t now_time;                                                                          //当前的时间
      now_time = time(NULL);
      tm* now = localtime(&now_time);
      char return_pattern_id[512] = { '\0' };                                                  //模式识别ID
      sprintf(return_pattern_id, "%s%s%s%04d%02d%02d%02d%02d%02d", val_prepos_event_task_job.event, val_prepos_event_task_job.task, val_prepos_event_task_job.job,
              now->tm_year + 1900, now->tm_mon + 1, now->tm_mday, now->tm_hour, now->tm_min, now->tm_sec);
      sprintf((char*)start_pattern_buf + 13, "%s%s", str_recognition, return_pattern_id);
      int start_pattern_len = 13 + strlen((char*)start_pattern_buf);
      start_pattern_buf[1] = (unsigned char)start_pattern_len - 2;

      dlg->setSendSeqAndRecvSeq(start_pattern_buf);                                              //设置收发序号
      ret = send(sockConn, (char*)start_pattern_buf, start_pattern_len, 0);
      if (ret != start_pattern_len)
      {
        dlg->MessageBox(_T("all call ack failed, orbit ack fialed!"));
        return -1;
      }

      SOCKET sockPattern;                                                                        //与模式识别建立连接                            
      dlg->connectPatternRecognition(sockPattern);
      ret = send(sockPattern, (char*)start_pattern_buf, start_pattern_len, 0);
      if (ret != start_pattern_len)
      {
        dlg->MessageBox(_T("all call ack failed, orbit ack fialed!"));
        return -1;
      }

      std::this_thread::sleep_for(std::chrono::seconds(10));

      dlg->disconnectPatternRecognition(sockPattern);                                            //与模式识别断开连接

      unsigned char end_pattern_buf[BUF_SIZE] = { 0x68, 0x00, 0, 0, 0, 0, 0x07, 0, 0, 0, 0, 0, 0x02, 0x01, 0x01 };     //PatternRecognition模式识别结束

      now_time = time(NULL);                                                                  //当前的时间
      now = localtime(&now_time);
      sprintf(return_pattern_id, "%s%s%s%04d%02d%02d%02d%02d%02d", val_prepos_event_task_job.event, val_prepos_event_task_job.task, val_prepos_event_task_job.job,
              now->tm_year + 1900, now->tm_mon + 1, now->tm_mday, now->tm_hour, now->tm_min, now->tm_sec);
      sprintf((char*)end_pattern_buf + 13, "%s%s", str_recognition, return_pattern_id);
      int end_pattern_len = 13 + strlen((char*)start_pattern_buf);
      end_pattern_buf[1] = (unsigned char)end_pattern_len - 2;

      dlg->setSendSeqAndRecvSeq(end_pattern_buf);                                              //设置收发序号
      ret = send(sockConn, (char*)end_pattern_buf, end_pattern_len, 0);
      if (ret != end_pattern_len)
      {
        dlg->MessageBox(_T("all call ack failed, orbit ack fialed!"));
        return -1;
      }
    }
  }

  unsigned char sendBuf[13] = { 0x68, 0x0B, 0, 0, 0, 0, 0x66, 0, 0, 0, 0, 0, 0 };     //巡检方案结束
  dlg->setSendSeqAndRecvSeq(sendBuf);                             //设置收发序号
  int ret = send(sockConn, (char*)sendBuf, 13, 0);
  if (ret != 13)
  {
    dlg->MessageBox(_T("CPelco_ServerDlg::ackExecuteInspection ack failed!"));
    return -1;
  }
  dlg->m_asyncFile.writeLog(std::to_string(66));

  g_executing_inspection = false;                             //自动巡检结束

  return 0;
}

// 分割字符串
std::vector<std::string> CPelco_ServerDlg::Split(const std::string& str, const std::string& delim, const bool trim_empty)
{
  size_t pos, last_pos = 0, len;
  std::vector<std::string> tokens;

  while (true) {
    pos = str.find(delim, last_pos);                    //不存在
    if (pos == std::string::npos) {
      pos = str.size();                               //置为字符的长度
    }

    len = pos - last_pos;
    if (!trim_empty || len != 0) {                      //trim_empty是否允许字符串为空
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
  str_x = str_x.substr(0, pos + 3);             //x保留2位

  pos = str_y.find('.');
  str_y = str_y.substr(0, pos + 3);             //y保留2位

  pos = str_z.find('.');
  str_z = str_z.substr(0, pos + 3);             //z保留2位

  return str_x + ";" + str_y + ";" + str_z + ";";
}

std::string CPelco_ServerDlg::getStringFromCoordinate(Point3D point3d)
{
  std::string str_x = std::to_string(point3d.x);
  std::string str_y = std::to_string(point3d.y);
  std::string str_z = std::to_string(point3d.z);
  int pos = 0;
  pos = str_x.find('.');
  str_x = str_x.substr(0, pos + 3);             //x保留2位

  pos = str_y.find('.');
  str_y = str_y.substr(0, pos + 3);             //y保留2位

  pos = str_z.find('.');
  str_z = str_z.substr(0, pos + 3);             //z保留2位

  return str_x + ";" + str_y + ";" + str_z + ";";
}

void CPelco_ServerDlg::OnTimer(UINT_PTR nIDEvent)
{
  // TODO:  在此添加消息处理程序代码和/或调用默认值
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
      SetDlgItemText(IDC_EDIT_CONN_ORBITAL, _T("连通"));
    }
    else
    {
      SetDlgItemText(IDC_EDIT_CONN_ORBITAL, _T("未连通"));
    }
    break;

  case MIDDLEWARE:
    g_connected_middleware = p1.Ping1(m_ip_middleware, pr1);
    if (g_connected_middleware)
    {
      SetDlgItemText(IDC_EDIT_CONN_MIDDLEWARE, _T("连通"));
    }
    else
    {
      SetDlgItemText(IDC_EDIT_CONN_MIDDLEWARE, _T("未连通"));
    }
    break;

  case VIDEO:
    g_connected_video = p1.Ping1(m_ip_video, pr1);
    if (g_connected_video)
    {
      SetDlgItemText(IDC_EDIT_CONN_VIDEO, _T("连通"));
    }
    else
    {
      SetDlgItemText(IDC_EDIT_CONN_VIDEO, _T("未连通"));
    }
    break;

  case NET_DISK:
    g_connected_net_disk = p1.Ping1(m_ip_net_disk, pr1);;
    if (g_connected_net_disk)
    {
      SetDlgItemText(IDC_EDIT_CONN_NET_DISK, _T("连通"));
    }
    else
    {
      SetDlgItemText(IDC_EDIT_CONN_NET_DISK, _T("未连通"));
    }
    break;

  default:
    break;
  }

  CDialogEx::OnTimer(nIDEvent);
}

int CPelco_ServerDlg::ackHandOperation(SOCKET sockConn, unsigned char* dataBuf, int len)               //手动控制
{

  int ret = 0;
  displayData(dataBuf, len);                                 //显示数据
  recvSeqAdd1(dataBuf);                                      //接收序号+1

  dataBuf[2] = 0x0b;                                         //确认连接或者确认测试
  setSendSeqAndRecvSeq(dataBuf);                             //设置收发序号
  ret = send(sockConn, (char*)dataBuf, len, 0);
  if (ret != len)
  {
    MessageBox(_T("ack connnect failed!"));
    return -1;
  }
  //***************************** 待修改 **************************************//
  CHandleTrack track = getMemberTrack('0');

  if (dataBuf[13] == 0x09)                                  //开始
  {
    if (dataBuf[15] == 0x01)                              //手动模式：左行开始
      track.startLeft();
    else if (dataBuf[15] == 0x02)                         //手动模式：右行开始
      track.startRight();
    else if (dataBuf[15] == 0x05)                         //手动模式：上行开始
      track.startUp();
    else if (dataBuf[15] == 0x06)
      track.startDown();
    else
      MessageBox(_T("0xAC 0x09 没有匹配的开始!"));
  }

  if (dataBuf[13] == 0x0B)                                  //开始
  {
    if (dataBuf[15] == 0x01)                              //手动模式：左行停止
      track.stopLeft();
    else if (dataBuf[15] == 0x02)                         //手动模式：右行停止
      track.stopRight();
    else if (dataBuf[15] == 0x05)                         //手动模式：上行停止
      track.stopUp();
    else if (dataBuf[15] == 0x06)                         //手动模式：下行停止
      track.stopDown();
    else
      MessageBox(_T("0xAC 0x0B 没有匹配的开始!"));
  }

  std::string strCoordinate = getStringFromCoordinate(track.getCoordinatePoint3D());    //坐标 

  time_t now_time;                                                                      //当前的时间
  now_time = time(NULL);
  tm* now = localtime(&now_time);
  char nowTime[32] = { '\0' };
  sprintf(nowTime, "%04d%02d%02d%02d%02d%02d", now->tm_year + 1900, now->tm_mon + 1,
          now->tm_mday, now->tm_hour, now->tm_min, now->tm_sec);

  char dataOrbit[BUF_SIZE] = { 0x68, 0, 0, 0, 0, 0, 0xbc, 0, 0, 0, 0, 0 };         //轨道信息(I格式)
  sprintf(dataOrbit + 12, "%s%s", strCoordinate.c_str(), nowTime);
  int dataOrbitLen = 12 + strCoordinate.size() + strlen(nowTime);
  dataOrbit[1] = dataOrbitLen - 2;

  setSendSeqAndRecvSeq((unsigned char*)dataOrbit);                                  //设置收发序号
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
  if ((sendSeq2 & 0xFE) == 0xFE)             //增加发送序列
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

int CPelco_ServerDlg::ackYuzhiOperation(SOCKET sockConn, unsigned char* dataBuf, int len)            //预置位 数据准备
{
  /*
  by zhaokui 2016.09.23

  dataBuf[6]=0x12为数据准备，         返回的数据为：1：成功，0：失败
  dataBuf[6]=0x14为数据执行，
  dataBuf[12]以后数据为：作业编码；预置位；IP地址，
  0x09及0x12 后需要返回数据为 作业编码；预置位；IP地址；结果（1：成功，0：失败）。

  0x12是选择 0x13是返校
  0x14是执行 0x15是返回结果？
  */

  int ret = 0;
  displayData(dataBuf, len);                                 //显示数据
  recvSeqAdd1(dataBuf);                                      //接收序号+1

  dataBuf[12] = 0x13;                                         //反校    
  dataBuf[len] = ';';
  dataBuf[len + 1] = 'T';

  setSendSeqAndRecvSeq(dataBuf);                             //设置收发序号
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

int CPelco_ServerDlg::ackWriteYuzhiOperation(SOCKET sockConn, unsigned char* dataBuf, int len)             //预置位 设置预置位
{
  /*
  by zhaokui 2016.09.23

  dataBuf[6]=0x12为数据准备，         返回的数据为：1：成功，0：失败
  dataBuf[6]=0x14为数据执行，
  dataBuf[12]以后数据为：作业编码；预置位；IP地址，
  0x09及0x12 后需要返回数据为 作业编码；预置位；IP地址；结果（1：成功，0：失败）。

  0x12是选择 0x13是返校
  0x14是执行 0x15是返回结果？
  */

  int ret = 0;
  displayData(dataBuf, len);                                             //显示数据
  recvSeqAdd1(dataBuf);                                                  //接收序号+1

  if (dataBuf[12] != 0x14)                                               //执行
  {
    MessageBox(_T("ack yuzhiwei zhixing failed!"));
    return -1;
  }
  dataBuf[len] = '\0';
  std::string strData((char*)dataBuf + 13);                                //数据段：作业编码；预置位；IP地址；操作类别
  m_asyncFile.writeLog(strData);

  std::vector<std::string> vecData = Split(strData, ";");

  for (int i = 0; i < vecData.size(); ++i)                                 //打印解析的数据
  {
    m_asyncFile.writeLog(vecData[i]);
  }
  if (vecData[1].size() == 0)                                              //空字符置为NULL
    vecData[1] = "NULL";

  //***************************** 待修改 **************************************//
  CHandleTrack track = getMemberTrack('0');
  if (vecData[1] != "NULL")
  {
    track.setPropos(std::stoi(vecData[1]));                            //设置预置位

    std::string prepos_update_str = "UPDATE base_prepos SET lrdist = " + std::to_string(track.getLeftRightOffsetLen()) +        //本地数据库记录预置位当前的偏移
      " ,uddist = " + std::to_string(track.getUpDownOffsetLen()) + " where prepos = " + vecData[1];
    m_asyncFile.writeLog(prepos_update_str);
    ret = mysql_query(m_conn, prepos_update_str.c_str());
    if (ret != 0)
    {
      MessageBox(_T("base_prepos数据库更新失败！"));
      return -1;
    }
  }

  std::string strUpdate = "UPDATE base_job SET preposition = " + vecData[1] + " where jobcode ='" + vecData[0] + "'";
  m_asyncFile.writeLog(strUpdate);
  ret = mysql_query(m_conn, strUpdate.c_str());

  dataBuf[12] = 0x15;                                                   //预置位确认    
  dataBuf[len] = ';';
  if (ret != 0)
    dataBuf[len + 1] = 'F';
  else
    dataBuf[len + 1] = 'T';
  setSendSeqAndRecvSeq(dataBuf);                                       //设置收发序号
  dataBuf[1] = len;
  ret = send(sockConn, (char*)dataBuf, len + 2, 0);
  if (ret != len + 2)
  {
    MessageBox(_T("ack yuzhiwei queren failed!"));
    return -1;
  }
  displayData(dataBuf, len + 2);                                          //显示数据

  return 0;
}

int CPelco_ServerDlg::ackExecuteYuzhiOperation(SOCKET sockConn, unsigned char* dataBuf, int len)        //执行指定预置位
{
  int ret = 0;
  displayData(dataBuf, len);                                 //显示数据
  recvSeqAdd1(dataBuf);                                      //接收序号+1
  dataBuf[len] = '\0';

  int prepos = (unsigned int)dataBuf[13];
  m_asyncFile.writeLog(std::to_string(prepos));              //打印日志:预置位

  //***************************** 待修改 **************************************//
  CHandleTrack track = getMemberTrack('0');
  track.callPropos(prepos);                                  //调用预置位

  dataBuf[2] = 0x0b;                                         //确认连接或者确认测试
  setSendSeqAndRecvSeq(dataBuf);                             //设置收发序号
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
  soap_set_mode(soap, SOAP_C_UTFSTRING);  //设置soap编码为UTF-8，防止中文乱码

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
  strHostIp.Format("本机名：%s\n", szHostName);
  lpHostEnt = gethostbyname(szHostName);
  //	m_strText += strHostIp + _T("\r\n");

  int   i;
  for (i = 0; lpHostEnt->h_addr_list[i] != NULL; i++)
  {
    strcpy(szHostIp, inet_ntoa(*(in_addr   *)lpHostEnt->h_addr_list[i]));
    strHostIp.Format("本机IP:%s", szHostIp);
  }
  //	m_strText += strHostIp + _T("\r\n");

  WSACleanup();
  return szHostIp;
}
char* CPelco_ServerDlg::GetExIp()
{

  // 得到外网IP和地址
  TCHAR szTempPath[MAX_PATH];
  DWORD dwResult = ::GetTempPath(MAX_PATH, szTempPath);

  //创建临时文件
  TCHAR szTempFile[MAX_PATH];
  UINT nResult = GetTempFileName(szTempPath, _T("~ex"), 0, szTempFile);

  CString strText = _T("");

  //int ret=URLDownloadToFile(NULL,"http://www.ip138.com/ips.asp",szTempFile,BINDF_GETNEWESTVERSION,NULL);
  //int ret=URLDownloadToFile(NULL,"http://city.ip138.com/ip2city.asp",szTempFile,BINDF_GETNEWESTVERSION,NULL);
  //int ret=URLDownloadToFile(NULL,"http://1212.ip138.com/ic.asp",szTempFile,BINDF_GETNEWESTVERSION,NULL);

  //int ret=URLDownloadToFile(NULL,"http://www.ip.cn/",szTempFile,BINDF_GETNEWESTVERSION,NULL);

  int ret = URLDownloadToFile(NULL, exipaddress, szTempFile, BINDF_GETNEWESTVERSION, NULL);
  if (ret == S_OK) //如果下载成功
  {
    CString szFileName = szTempFile;

    CFile file;
    file.Open(szFileName, CFile::modeRead);
    CArchive archive(&file, CArchive::load);

    while (archive.ReadString(strText))
    {
      strText.TrimLeft();
      strText.TrimRight();
      //if (strText.Find(_T("您的IP是")) != -1)
      if (strText.Find(_T("您的IP地址是")) != -1)
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
    //strText = _T("外网未连接!"); 
    strText = _T("0");
  int index1 = strText.ReverseFind('[');
  int index2 = strText.ReverseFind(']');
  int index3 = index2 - index1;
  strText = strText.Mid(index1 + 1, index3 - 1);
  //m_strText += "外网IP:"+strText;
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

// 获取摄像头时间的全局函数
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

  //返回起始时间端点
  LLONG hPlayBack = 0;
  LLONG lLoginHandle = 0;
  int nChannelID = 0; // 预览通道号
  NET_DEVICEINFO stDevInfo = { 0 };
  int nError = 0;
  int nPort = 37777;

  //注册到设备，成功返回id，失败返回0
  lLoginHandle = CLIENT_Login(szDevIp, nPort, "admin", "admin", &stDevInfo, &nError);
  if (lLoginHandle != 0)
  {
    NET_RECORDFILE_INFO netFileInfo[30] = { 0 };

    //查找录像文件---成功：返回查询句柄；失败：返回0
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

    //将pstruFileData中当前位置后面的n个字节用 0替换
    memset(pstruFileData, 0, 5000 * sizeof(NET_RECORDFILE_INFO));

    int nFileIndex = 0;

    while (true)
    {
      //查询下一条录像文件记录----pstruFileData:录像文件记录缓冲，用于输出查询到的录像文件记录
      int result = CLIENT_FindNextFile(lFindHandle, &pstruFileData[nFileIndex]);
      if (result == 1)//取回一条录像文件信息
      {
        //将查询结果放入返回值
        //vec_time[count++] = pstruFileData[nFileIndex].starttime;
        vec_time.push_back(pstruFileData[nFileIndex].starttime);
        nFileIndex++;
        //存储录像文件
      }
      else if (result == 0)//录像文件信息数据取完
      {
        //printf("over \n");
        break;
      }
      else//参数出错
      {
        AfxMessageBox("find file fail for illegal param\n");
        break;
      }
    }

    //停止查找
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

int CPelco_ServerDlg::ackQueryHistoryVideo(SOCKET sockConn, unsigned char* dataBuf, int len)        //从站返回查询历史视频后的参数
{
  CLIENT_Init(DisConnectFunc, 0);

  dataBuf[len] = '\0';                                            //变成c风格字符串
  std::string str_data((char*)dataBuf + 12);
  m_asyncFile.writeLog(str_data);
  std::vector<std::string> vec_data = Split(str_data, ";");       // 摄像头ID;开始时间;结束时间;一串IP

  NET_TIME StartTime = { 0 };                                     // 开始时间
  NET_TIME StopTime = { 0 };                                      // 结束时间
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


  unsigned char sendBuf[512] = { 0x68, 0x00, 0, 0, 0, 0, 0x67, 0, 0, 0, 0, 0, 0 };     //巡检方案结束
  for (int i = 0; i < vec_time.size(); ++i)
  {
    setSendSeqAndRecvSeq(sendBuf);                                                       //设置收发序号
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


int CPelco_ServerDlg::initConnectTrack()                                //初始化与轨道机的连接
{
  CIni Ini("config.ini");
  int total_num = Ini.ReadInt("num_track", "total_num");         //获取轨道机的数目(注：一个轨道机意指同一轨道上的上下和左右轨道机)

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
    ip_left_right_track = Ini.ReadText("ip_track", ip_left_right_track_index);                  //获取 i 号轨道机的左右 ip
    port_left_right_track_index[strlen(port_left_right_track_index) - 1] = (i + '0');
    port_left_right_track = Ini.ReadInt("port_track", ip_left_right_track_index);               //获取 i 号轨道机的左右 port
    ///////////////////////////// 与轨道机的进程间通信 ： 左右 ///////////////////////////
    SOCKET sockPelcoLeftRight = socket(AF_INET, SOCK_STREAM, 0);
    SOCKADDR_IN addrPelcoSrvLeftRight;
    addrPelcoSrvLeftRight.sin_addr.S_un.S_addr = inet_addr(ip_left_right_track);
    addrPelcoSrvLeftRight.sin_family = AF_INET;
    addrPelcoSrvLeftRight.sin_port = htons(port_left_right_track);

    ret = connect(sockPelcoLeftRight, (SOCKADDR*)&addrPelcoSrvLeftRight, sizeof(SOCKADDR));
    while (ret != 0)                                                                  //如果为连接失败，继续尝试连接
    {
      strName.Format(_T("TrackLeftRight Server %d : the connection failed!\r\n"), i);
      GetDlgItemText(IDC_EDIT_FRAME_NAME, oldMsg);                                    //显示报文名
      strName += oldMsg;
      SetDlgItemText(IDC_EDIT_FRAME_NAME, strName);
      Sleep(1000);
      ret = connect(sockPelcoLeftRight, (SOCKADDR*)&addrPelcoSrvLeftRight, sizeof(SOCKADDR));
    }
    strName.Format(_T("TrackLeftRight Server %d : the connection succeed!\r\n"), i);
    GetDlgItemText(IDC_EDIT_FRAME_NAME, oldMsg);                                      //显示报文名
    strName += oldMsg;
    SetDlgItemText(IDC_EDIT_FRAME_NAME, strName);

    ip_up_down_track_index[strlen(ip_up_down_track_index) - 1] = (i + '0');
    ip_up_down_track = Ini.ReadText("ip_track", ip_up_down_track_index);              //获取 i 号轨道机的左右 ip
    port_up_down_track_index[strlen(port_up_down_track_index) - 1] = (i + '0');
    port_up_down_track = Ini.ReadInt("port_track", port_up_down_track_index);         //获取 i 号轨道机的左右 port
    ///////////////////////////// 与轨道机的进程间通信 ： 上下 ///////////////////////////
    SOCKET sockPelcoUpDown = socket(AF_INET, SOCK_STREAM, 0);
    SOCKADDR_IN addrPelcoSrvUpDown;
    addrPelcoSrvUpDown.sin_addr.S_un.S_addr = inet_addr(ip_up_down_track);
    addrPelcoSrvUpDown.sin_family = AF_INET;
    addrPelcoSrvUpDown.sin_port = htons(port_up_down_track);

    ret = connect(sockPelcoUpDown, (SOCKADDR*)&addrPelcoSrvUpDown, sizeof(SOCKADDR));
    while (ret != 0)                                                                  //如果为连接失败，继续尝试连接
    {
      strName.Format(_T("TrackUpDown Server %d : the connection failed!\r\n"), i);
      GetDlgItemText(IDC_EDIT_FRAME_NAME, oldMsg);                                    //显示报文名
      strName += oldMsg;
      SetDlgItemText(IDC_EDIT_FRAME_NAME, strName);
      Sleep(1000);
      ret = connect(sockPelcoUpDown, (SOCKADDR*)&addrPelcoSrvUpDown, sizeof(SOCKADDR));
    }
    strName.Format(_T("TrackUpDown Server %d : the connection succeed!\r\n"), i);
    GetDlgItemText(IDC_EDIT_FRAME_NAME, oldMsg);                                      //显示报文名
    strName += oldMsg;
    SetDlgItemText(IDC_EDIT_FRAME_NAME, strName);

    CHandleTrack track = getMemberTrack(i);                                         //为配置文件中的track初始化        
    track = CHandleTrack(sockPelcoLeftRight, sockPelcoUpDown);
  }

  return 0;
}


CHandleTrack& CPelco_ServerDlg::getMemberTrack(const int num)
{
  assert(num >= 0 && num <= 5);                                   //track的数据应该在[0,5]
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
  assert(num >= 0 && num <= 5);                                   //track的数据应该在['0','5']
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
  //********************** 需要修改 *****************************//
  addrPatternServer.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
  addrPatternServer.sin_family = AF_INET;
  addrPatternServer.sin_port = htons(9900);

  ret = connect(sockPattern, (SOCKADDR*)&addrPatternServer, sizeof(SOCKADDR));
  while (ret != 0)                                                                  //如果为连接失败，继续尝试连接
  {
    strName = _T("TrackLeftRight Server : the connection failed!\r\n");
    GetDlgItemText(IDC_EDIT_FRAME_NAME, oldMsg);                                    //显示报文名
    strName += oldMsg;
    SetDlgItemText(IDC_EDIT_FRAME_NAME, strName);
    Sleep(1000);
    ret = connect(sockPattern, (SOCKADDR*)&addrPatternServer, sizeof(SOCKADDR));
  }
  strName = _T("TrackLeftRight Server %d : the connection succeed!\r\n");
  GetDlgItemText(IDC_EDIT_FRAME_NAME, oldMsg);                                      //显示报文名
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