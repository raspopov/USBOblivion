//
// CtrlsResize.h
//
// Copyright (c) Nikolay Raspopov, 2009-2016.
// This file is part of USB Oblivion (http://www.cherubicsoft.com/en/projects/usboblivion)
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along
// with this program; if not, write to the Free Software Foundation, Inc.,
// 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
//

#pragma once

const int BIND_TOP =	0x01;
const int BIND_LEFT =	0x02;
const int BIND_RIGHT =	0x04;
const int BIND_BOTTOM =	0x08;
const int BIND_ALL =	0x0F;
const int BIND_UNKNOWN = 0x00;

class CCtrlResize
{
public:
	CCtrlResize();
	~CCtrlResize();
	void Clear();
	void OnSize ();
	void SetParentWnd (CWnd *pWnd);
	int FixControls ();
	int AddControl (int _controlID, int _bindtype, const CRect& _rectInitial = CRect( 0, 0, 0, 0 ));
	int AddControl (CWnd *_pControl, int _bindtype, const CRect& _rectInitial = CRect( 0, 0, 0, 0 ));

	class CControlInfo  
	{
	public:
		CControlInfo ();
		CControlInfo (int _controlID, int _bindtype, const CRect& _rectInitial, CWnd* _m_pControlWnd = NULL);
		int controlID;
		int bindtype;
		CRect rectInitial;
		CWnd* m_pControlWnd;
	};

private:
	CWnd* m_pWnd;
	CArray <CControlInfo*, CControlInfo*> m_aCtrls;
	CRect m_rectInitialParent;
};
