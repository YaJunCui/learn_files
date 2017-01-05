#if !defined(AFX_HEZHONG_H__50CB64AE_0E60_423A_971A_937434CA602A__INCLUDED_)
#define AFX_HEZHONG_H__50CB64AE_0E60_423A_971A_937434CA602A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// hezhong.h : header file
//
#include "DynamicLED.h"
#include "ColorButton.h"
/////////////////////////////////////////////////////////////////////////////
// Chezhong window

class Chezhong : public CStatic
{
// Construction
public:
	Chezhong();

// Attributes
public:
	bool switchs[13];
	CDynamicLED led[13];
	CColorButton button[15];
	CString strings[15];

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(Chezhong)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~Chezhong();

	// Generated message map functions
protected:
	//{{AFX_MSG(Chezhong)
	afx_msg void OnPaint();
	afx_msg void OnButtonRange(UINT nID);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_HEZHONG_H__50CB64AE_0E60_423A_971A_937434CA602A__INCLUDED_)
