// ColorButton.cpp : implementation file
//

#include "stdafx.h"
#include "ColorButton.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CColorButton

CColorButton::CColorButton()
{
	m_color = RGB(0, 255, 0);
}

CColorButton::~CColorButton()
{
}


BEGIN_MESSAGE_MAP(CColorButton, CButton)
	//{{AFX_MSG_MAP(CColorButton)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CColorButton message handlers



void CColorButton::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
	UINT uStyle = DFCS_BUTTONPUSH;
	
	// This code only works with buttons.
	ASSERT(lpDrawItemStruct->CtlType == ODT_BUTTON);
	
	// If drawing selected, add the pushed style to DrawFrameControl.
	if (lpDrawItemStruct->itemState & ODS_SELECTED)
		uStyle |= DFCS_PUSHED;
	SetBkMode(lpDrawItemStruct->hDC, TRANSPARENT);

	CBrush brush(m_color);
	CRect rect;
	rect.bottom = lpDrawItemStruct->rcItem.bottom-1;
	rect.top = lpDrawItemStruct->rcItem.top+1;
	rect.left = lpDrawItemStruct->rcItem.left+1;
	rect.right = lpDrawItemStruct->rcItem.right-1;

	// Draw the button frame.
	::DrawFrameControl(lpDrawItemStruct->hDC, &lpDrawItemStruct->rcItem, 
		DFC_BUTTON, uStyle);
	
	FillRect(lpDrawItemStruct->hDC, &rect, (HBRUSH)brush.GetSafeHandle());
	// Get the button's text.
	CString strText;
	GetWindowText(strText);

	if (lpDrawItemStruct->itemState & ODS_SELECTED)
	{
		rect.top += 1;
		rect.bottom += 1;
		rect.left += 1;
		rect.right += 1;
	}

	CDC* pDC = CDC::FromHandle(lpDrawItemStruct->hDC);
	// Draw the button text using the text color red.
	CSize Extent = pDC->GetTextExtent(strText);
	if (Extent.cx>rect.Width())
	{
		if (Extent.cx/1.5 > rect.Width())
		{
			//  数值显示
			int nHeight = rect.Height()/4;
			CFont font, *pFontOld;
			font.CreateFont (nHeight, 0, 0, 0, 400,
				FALSE, FALSE, 0, ANSI_CHARSET,
				OUT_DEFAULT_PRECIS, 
				CLIP_DEFAULT_PRECIS,
				DEFAULT_QUALITY, 
				DEFAULT_PITCH|FF_SWISS, "Arial") ;
			pFontOld = pDC->SelectObject(&font);
// 			pDC->SetTextAlign(TA_TOP|TA_CENTER);
			pDC->SetBkMode(TRANSPARENT);
			// 	pDC->TextOut(pttemp.x, pttemp.y-m_rectCtrl.Height()*0.3, _T("左　　右"));
			// 恢复字体和背景色
			rect.top += 5;
			pDC->DrawText(strText, strText.GetLength(), &rect, DT_CENTER|DT_WORDBREAK);
			pDC->SelectObject(pFontOld);
		}
		else
		{
			rect.top += 5;
			pDC->DrawText(strText, strText.GetLength(), 
				&rect, DT_CENTER|DT_WORDBREAK);
		}

// 		COLORREF crOldColor = ::SetTextColor(lpDrawItemStruct->hDC, RGB(255,0,0));
// 		::SetTextColor(lpDrawItemStruct->hDC, crOldColor);
	}
	else
	{
// 		COLORREF crOldColor = ::SetTextColor(lpDrawItemStruct->hDC, RGB(255,0,0));
// 		rect.top += 5;
		::DrawText(lpDrawItemStruct->hDC, strText, strText.GetLength(), 
			&rect, DT_CENTER|DT_VCENTER|DT_SINGLELINE);
// 		::SetTextColor(lpDrawItemStruct->hDC, crOldColor);
	}
}

void CColorButton::SetColor(COLORREF color)
{
	m_color = color;
	if (GetSafeHwnd())
	{
		Invalidate();
	}
}

