//
// vs2010.cpp
//
// Copyright (c) Nikolay Raspopov, 2009-2012.
// This file is part of USB Oblivion (http://code.google.com/p/usboblivion/)
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

#ifndef _WIN64

#define DELETE_EXCEPTION(e) do { if(e) { e->Delete(); } } while (0)

class CArchiveStream;

#include "afxpriv2.h"

HANDLE AFXAPI AfxCreateActCtxW(PCACTCTXW pActCtx);
void AFXAPI AfxReleaseActCtx(HANDLE hActCtx);
extern "C" BOOL AFXAPI AfxActivateActCtx(HANDLE hActCtx, ULONG_PTR *lpCookie);
extern "C" BOOL AFXAPI AfxDeactivateActCtx(DWORD dwFlags, ULONG_PTR ulCookie);
extern "C" eActCtxResult AFXAPI AfxActivateActCtxWrapper(HANDLE hActCtx, ULONG_PTR *lpCookie);

#define AFX_ACTCTX_API_INIT_PROCPTR(hKernel,name) pfn##name = (PFN_##name) GetProcAddress(hKernel, #name)
#define AFX_ACTCTX_API_PTR_DEFINE(name, type, params) typedef type (WINAPI* PFN_##name)params; PFN_##name pfn##name = NULL;
AFX_ACTCTX_API_PTR_DEFINE(CreateActCtxW, HANDLE, (PCACTCTXW));
AFX_ACTCTX_API_PTR_DEFINE(ReleaseActCtx, void, (HANDLE));
AFX_ACTCTX_API_PTR_DEFINE(ActivateActCtx, BOOL, (HANDLE, ULONG_PTR*));
AFX_ACTCTX_API_PTR_DEFINE(DeactivateActCtx, BOOL, (DWORD, ULONG_PTR));

AFX_STATIC void AFXAPI _AfxInitContextAPI()
{
	static HMODULE hKernel = NULL;
	if (hKernel == NULL)
	{
		hKernel = GetModuleHandle(_T("KERNEL32"));
		ENSURE(hKernel != NULL);
		AFX_ACTCTX_API_INIT_PROCPTR(hKernel,CreateActCtxW);
		AFX_ACTCTX_API_INIT_PROCPTR(hKernel,ReleaseActCtx);
		AFX_ACTCTX_API_INIT_PROCPTR(hKernel,ActivateActCtx);
		AFX_ACTCTX_API_INIT_PROCPTR(hKernel,DeactivateActCtx);
	}
}

eActCtxResult AFXAPI AfxActivateActCtxWrapper(HANDLE hActCtx, ULONG_PTR *lpCookie)
{
	ENSURE_ARG(lpCookie!=NULL);

	eActCtxResult eResult=ActCtxFailed;
	if (pfnActivateActCtx != 0)
	{
		eResult=AfxActivateActCtx(hActCtx, lpCookie) ? ActCtxSucceeded : ActCtxFailed;
	} else
	{
		eResult=ActCtxNoFusion;
	}

	return eResult;
}

class InitContext
{
public:
	InitContext() { _AfxInitContextAPI(); }
};

InitContext context;

#define AfxActivateActCtxWrapper AfxActivateActCtxWrapperVC10
#define ActivateActCtx(hActCtx, lpCookie) (pfnActivateActCtx != 0 ? pfnActivateActCtx(hActCtx, lpCookie) : FALSE)
#define DeactivateActCtx(dwFlags, ulCookie) (pfnDeactivateActCtx != 0 ? pfnDeactivateActCtx(dwFlags, ulCookie) : FALSE)
#define CreateActCtxW(pActCtx) (pfnCreateActCtxW != 0 ? pfnCreateActCtxW(pActCtx) : INVALID_HANDLE_VALUE)
#define ReleaseActCtx(hActCtx) if(pfnReleaseActCtx != 0) { pfnReleaseActCtx(hActCtx); }

#include "..\src\mfc\afxstate.cpp"

#endif
