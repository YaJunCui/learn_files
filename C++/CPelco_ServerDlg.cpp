/*
 *  全局函数：启动服务
 *  dlg：对话框句柄
 *  sockSrv：当前服务器绑定的socket
 */

int startServer(CPelco_ServerDlg* dlg, SOCKET& sockSrv)              //启动服务端



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
  // 生成的消息映射函数
  virtual BOOL OnInitDialog();
  afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
  afx_msg void OnPaint();
  afx_msg HCURSOR OnQueryDragIcon();
  DECLARE_MESSAGE_MAP()

public:
  BOOL initSocket();         //初始化socket                  
  int initDatabase();        //初始化数据库

  /*
   *  在对话框中显示数据
   *  dataBuf：需要显示的数据
   *  len：数据的长度
   */
  int displayData(unsigned char* dataBuf, int len);

  /*
   *  确认与客户端建立连接
   *  sockConn：客户端对应的socket
   *  dataBuf：客户端发送过来的请求连接数据
   *  len：数据的长度
   */
  int ackConnect(SOCKET sockConn, unsigned char* dataBuf, int len);

  /*
   *  确认总召：用于onTimer事件中
   */
  int ackAllCall();

  /*
   *  确认总召
   *  sockConn：客户端对应的socket
   *  dataBuf：客户端发送过来的请求总召数据
   *  len：数据的长度
   */
  int ackAllCall(SOCKET sockConn, unsigned char* dataBuf, int len);


public:
  /*
   *  响应执行预置位
   *  sockConn：客户端对应的socket
   *  dataBuf：客户端发送过来的请求执行预置位数据
   *  len：数据的长度
   */
  int ackExecuteYuzhiOperation(SOCKET sockConn, unsigned char* dataBuf, int len);     

  /*
   *  响应写数据库
   *  sockConn：客户端对应的socket
   *  dataBuf：客户端发送过来的请求写数据库数据
   *  len：数据的长度
   */ 
  int ackWriteDatabases(SOCKET sockConn, unsigned char* dataBuf, int len);             
  
  /*
   *  响应执行SQL
   *  sockConn：客户端对应的socket
   *  dataBuf：客户端发送过来的请求执行SQL
   *  len：数据的长度
   */ 
  int ackExecuteSQL(SOCKET sockConn, unsigned char* dataBuf, int len);                  

  /*
   *  预置位设置
   *  sockConn：客户端对应的socket
   *  dataBuf：客户端发送过来的请求预置位设置数据
   *  len：数据的长度
   */ 
  int ackYuzhiOperation(SOCKET sockConn, unsigned char* dataBuf, int len);           

  /*
   *  响应写预置位
   *  sockConn：客户端对应的socket
   *  dataBuf：客户端发送过来的请求写预置位数据
   *  len：数据的长度
   */ 
  int ackWriteYuzhiOperation(SOCKET sockConn, unsigned char* dataBuf, int len);        

  /*
   *  手动控制
   *  sockConn：客户端对应的socket
   *  dataBuf：客户端发送过来的请求手动控制数据
   *  len：数据的长度
   */ 
  int ackHandOperation(SOCKET sockConn, unsigned char* dataBuf, int len);                

  /*
   *  从站返回查询历史视频后的参数
   *  sockConn：客户端对应的socket
   *  dataBuf：客户端发送过来的请求查询历史视频
   *  len：数据的长度
   */ 
  int ackQueryHistoryVideo(SOCKET sockConn, unsigned char* dataBuf, int len);            


  int initConnectTrack();                                                                 //初始化与轨道机的连接


  CHandleTrack& getMemberTrack(const int num = 0);                                        //获取成员变量m_track_*，用于操作轨道机
  const CHandleTrack& getMemberTrack(const char num = '0');

  /*
   *  与模式识别建立连接
   *  sockPattern：模式识别对应的socket
   */ 
  int connectPatternRecognition(SOCKET& sockPattern);                                 

  /*
   *  与模式识别断开连接
   *  sockPattern：模式识别对应的socket
   */ 
  int disconnectPatternRecognition(SOCKET& sockPattern);           


  /*
   *  响应执行巡检方案：静态成员函数，用于创建执行巡检方案线程
   *  sockConn：客户端对应的socket
   *  dataBuf：客户端发送过来的请求执行巡检方案
   *  len：数据的长度
   *  dlg：对话框句柄
   */
  static int ackExecuteInspection(SOCKET sockConn, unsigned char* dataBuf, int len, CPelco_ServerDlg* dlg);

  /*
   *  分割字符串
   *  str：需要分割的字符串
   *  delim：需要分割的字符串的分隔符类型
   *  trim_empty：是否保留空字符串，默认保留
   */ 
  std::vector<std::string> Split(const std::string& str, const std::string& delim, const bool trim_empty = false);

  std::string getStringFromCoordinate(double x, double y, double z);                      //将坐标转换为字符串

  /*
   *  将Point3D坐标转为字符串
   *  point3d：需要进行转换的坐标
   */
  std::string getStringFromCoordinate(Point3D point3d);                                  

  void setSendSeqAndRecvSeq(unsigned char* dataBuf);                                      //设置收发序号
  void recvSeqAdd1(unsigned char* dataBuf);                                               //接收序列号加1
  
  afx_msg void OnTimer(UINT_PTR nIDEvent);                                                //定时器

  int SlaveExecuteCMD(char* msoap, char* para1, char * para2);                            //Slave控制
  char* GetIp();
  char* GetExIp();
  char* join(char *s1, char *s2);                                           
};