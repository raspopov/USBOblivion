#ifndef _WIN64

#define DELETE_EXCEPTION(e) do { if(e) { e->Delete(); } } while (0)

class CArchiveStream;

#include <afxpriv2.h>

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
