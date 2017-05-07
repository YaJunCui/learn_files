/*
 *  控制轨道器的类
 */
class CHandleTrack 
{
public:
  /*
   *  构造函数
   *  sockTrack：控制轨道机的socket
   */
  CHandleTrack(const SOCKET &sockTrack = NULL);     
  /*
   *  析构函数
   */                     
  ~CHandleTrack();
   /*
   *  赋值操作符
   */
  CHandleTrack &operator=(const CHandleTrack &rhs);

  /*
   *  获取轨道机的总长度
   *  loc：轨道的地址，默认为1，即左右方向
   */
  int getTotalLen(int loc = 1);                                

  /*
   *  获取轨道机左右的偏移长度
   */
  int getLeftRightOffsetLen();                                 

  /*
   *  获取轨道机上下的偏移长度
   */
  int getUpDownOffsetLen();                                     

  /*
   *  调用预置位
   *  lowPos：预置位的低位
   *  highPos：预置位的高位
   *  loc：轨道的地址，默认为1，即左右方向
   */
  int callPropos(int lowPos = 0, int highPos = 0, int loc = 1); 

  /*
   *  设置预置位
   *  lowPos：预置位的低位
   *  highPos：预置位的高位
   *  loc：轨道的地址，默认为1，即左右方向
   */
  int setPropos(int lowPos = 0, int highPos = 0, int loc = 1);   

  /*
   *  清除预置位
   *  lowPos：预置位的低位
   *  highPos：预置位的高位
   *  loc：轨道的地址，默认为1，即左右方向
   */
  int clearPropos(int lowPos = 0, int highPos = 0, int loc = 1); 

  int startLeft();  //开始：左
  int startRight(); //开始：右
  int startUp();    //开始：上
  int startDown();  //开始：下

  int stopLeft();   //停止：左、右、上、下
  int stopRight();
  int stopUp();
  int stopDown();

  /*
   *  获取三维坐标
   */
  Point3D getCoordinatePoint3D();     

  /*
   *  根据左右偏移的长度，获取三维坐标(根据获取的计算结果，在计算上下的偏移，最终得到真实的坐标)
   *  dist：为左右偏移的长度
   */
  Point3D getPointByDist(double dist); 

private:
  /*
   *  校验位的设置
   *  instructionBuf：pelco_d 指令
   *  len：指令的长度
   */
  int getCheckBit(unsigned char *instructionBuf, int len); //

  /*
   *  获取轨道  直线上  的坐标
   *  dist：为左右偏移的长度
   */
  Point3D getLinePoint(double dist);                     

  /*
   *  获取轨道  弯道上  的坐标
   *  dist：为左右偏移的长度
   */
  Point3D get_point_in_circle(double dist);                

public:
  SOCKET m_sockTrack;                          //轨道机对应的socket

private:
  int m_lrTotalLen;                            //左右方向轨道机的总长度
  int m_udTotalLen;                            //上下方向轨道机的总长度
};
