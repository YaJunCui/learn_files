// hezhong.cpp : implementation file
//

#include "stdafx.h"
#include "hezhong.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Chezhong

Chezhong::Chezhong()
{
	memset(switchs, 0 ,13*sizeof(bool));
	
	strings[0] = _T("录像");
	strings[1] = _T("视频");
	strings[2] = _T("轨道机");
	strings[3] = _T("前置");
	//strings[4] = _T("进一");
	//strings[5] = _T("接排");
	//strings[6] = _T("空车");
	//strings[7] = _T("停车");
	//strings[8] = _T("退一");
	//strings[9] = _T("退二");
	//strings[10] = _T("退三");
	//strings[11] = _T("错向");
	//strings[12] = _T("试灯");
	//strings[13] = _T("调光±");
	//strings[14] = _T("电源");
}

Chezhong::~Chezhong()
{
}


BEGIN_MESSAGE_MAP(Chezhong, CStatic)
	//{{AFX_MSG_MAP(Chezhong)
	ON_WM_PAINT()
	ON_COMMAND_RANGE(10010, 10010+14, OnButtonRange)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Chezhong message handlers

void Chezhong::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	// TODO: Add your message handler code here
	CRect ctrlrect;
	GetClientRect(&ctrlrect);
	CBrush* pbrush, poldbrush;
	pbrush = CBrush::FromHandle((HBRUSH)GetStockObject(NULL_BRUSH));
	dc.SetBkMode(TRANSPARENT);
	dc.SelectObject(pbrush);
	dc.Rectangle(ctrlrect);
	dc.SelectObject(poldbrush);
	int i;
	for (i=0; i<4; i++)  //13
	{
		if (switchs[i] == 0)
		{
			led[i].SwitchOff();
		} 
		else
		{
			led[i].SwitchOn();
		}
	}
	int x1;
	x1 = 10;
	if (!led[0].GetSafeHwnd())
	{
		for (i=0; i<4; i++)
		{
			//led[i].Create(NULL, WS_CHILD|WS_VISIBLE, CRect(x1, 30+25*i, x1+16, 30+25*i+16), this);
			led[i].Create(NULL, WS_CHILD | WS_VISIBLE, CRect(x1+70*i, 20 , x1 +70*i+ 16, 20 + 16), this);
		}
		for (i=0; i<4; i++)
		{
			button[i].m_color = RGB(43, 168, 128);
			//button[i].Create(NULL, WS_CHILD|WS_VISIBLE|BS_PUSHBUTTON|BS_OWNERDRAW, 
			//	CRect(x1+70,30+25*i,x1+90,30+25*i+20), this, 10010+i);
			button[i].Create(NULL, WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_OWNERDRAW,
				CRect(x1 + 68*i, 35+30 , x1 + 68*i+20, 35 + 30+20), this, 10010 + i);
			
		}
		///*for (i=5; i<8; i++)
		//{
		//	button[i].m_color = RGB(230, 207, 4);
		//	button[i].Create(NULL, WS_CHILD|WS_VISIBLE|BS_PUSHBUTTON|BS_OWNERDRAW, 
		//		CRect(x1+70,30+25*i,x1+90,30+25*i+20), this, 10010+i);
		//}
		//for (i=8; i<11; i++)
		//{
		//	button[i].m_color = RGB(214, 7, 100);
		//	button[i].Create(NULL, WS_CHILD|WS_VISIBLE|BS_PUSHBUTTON|BS_OWNERDRAW, 
		//		CRect(x1+70,30+25*i,x1+90,30+25*i+20), this, 10010+i);
		//}
		//led[11].Create(NULL, WS_CHILD|WS_VISIBLE, CRect(x1+118, 30, x1+134, 30+16), this);
		//led[12].Create(NULL, WS_CHILD|WS_VISIBLE, CRect(x1+118, 30+25*8, x1+134, 30+25*8+16), this);
		//button[11].m_color = RGB(43, 168, 128);
		//button[11].Create(NULL, WS_CHILD|WS_VISIBLE|BS_PUSHBUTTON|BS_OWNERDRAW, 
		//	CRect(x1+118,30+25*2,x1+138,30+25*2+20), this, 10010+11);
		//button[12].m_color = RGB(43, 168, 128);
		//button[12].Create(NULL, WS_CHILD|WS_VISIBLE|BS_PUSHBUTTON|BS_OWNERDRAW, 
		//	CRect(x1+118,30+25*4,x1+138,30+25*4+20), this, 10010+12);
		//button[13].m_color = RGB(43, 168, 128);
		//button[13].Create(NULL, WS_CHILD|WS_VISIBLE|BS_PUSHBUTTON|BS_OWNERDRAW, 
		//	CRect(x1+118,30+25*6,x1+138,30+25*6+20), this, 10010+13);
		//button[14].m_color = RGB(43, 168, 128);
		//button[14].Create(NULL, WS_CHILD|WS_VISIBLE|BS_PUSHBUTTON|BS_OWNERDRAW, 
		//	CRect(x1+118,30+25*10,x1+138,30+25*10+20), this, 10010+14);*/
	}
	for (i=0; i<4; i++)
	{
		led[i].ShowWindow(SW_SHOW);
		dc.TextOut(x1 + 68 * i, 30 + 15, strings[i]);
	}
/*	dc.TextOut(x1+110, 30+25, strings[11]);
	dc.TextOut(x1+110, 30+25*3, strings[12]);
	dc.TextOut(x1+110, 30+25*5, strings[13]);
	dc.TextOut(x1+110, 30+25*9, strings[14]);
	
	dc.SetTextAlign(TA_TOP|TA_CENTER);
	dc.TextOut(ctrlrect.Width()/2, 5, _T("YCZ-01传令车钟"));*/
	// Do not call CStatic::OnPaint() for painting messages
}

void Chezhong::OnButtonRange(UINT nID)
{
	int i;
	bool all_on = true;
	for (i=0; i<4; i++)
	{
		if (led[i].m_bOn == false)
		{
			all_on = false;
		}
	}
	if (nID<10010+11)
	{
		if (all_on == false)
		{
			for (i=0; i<4; i++)
			{
				if (i == nID-10010)
				{
					if (led[i].m_bOn == false)
					{
						led[i].SwitchOn();
						switchs[i] = true;
					} 
					else
					{
						led[i].SwitchOff();
						switchs[i] = false;
					}
				} 
				else
				{
					led[i].SwitchOff();
					switchs[i] = false;
				}
			}
		}
	}
	else if (nID == 10010+14)
	{
		if (all_on == false)
		{
			if (led[12].m_bOn == false)
			{
				led[12].SwitchOn();
				switchs[12] = true;
			} 
			else
			{
				led[12].SwitchOff();
				switchs[12] = false;
			}
		}
	}
	else if (nID == 10010+11)
	{
		if (all_on)
		{
			for (i=0; i<4; i++)
			{
				led[i].SwitchOff();
				switchs[i] = false;
			}
		} 
		else
		{
			for (i=0; i<4; i++)
			{
				led[i].SwitchOn();
				switchs[i] = true;
			}
		}
	}
	else if (nID == 10010+12)
	{
		for (i=0; i<4; i++)
		{
			led[i].SetLEDBrightness(led[i].GetLEDBrightness()+30);
		}
	}
	else if (nID == 10010+13)
	{
		for (i=0; i<4; i++)
		{
			led[i].SetLEDBrightness(led[i].GetLEDBrightness()-30);
		}
	}
}