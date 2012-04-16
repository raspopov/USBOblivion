.model flat, C

AfxActivateActCtxWrapper PROTO STDCALL :DWORD,:DWORD
AfxDeactivateActCtx PROTO STDCALL :DWORD,:DWORD

.data
__imp__EncodePointer@4 dd dummy
__imp__DecodePointer@4 dd dummy
__imp__ActivateActCtx@8 dd ActActCtx
__imp__DeactivateActCtx@8 dd DeacActCtx

EXTERNDEF __imp__EncodePointer@4 : DWORD
EXTERNDEF __imp__DecodePointer@4 : DWORD
EXTERNDEF __imp__ActivateActCtx@8 : DWORD
EXTERNDEF __imp__DeactivateActCtx@8 : DWORD

.code
dummy proc
mov eax, [esp+4]
ret 4
dummy endp

ActActCtx proc uses ebx ecx hActCtx:DWORD,lpCookie:DWORD
invoke AfxActivateActCtxWrapper, hActCtx, lpCookie
ret 8
ActActCtx endp

DeacActCtx proc uses ebx ecx dwFlags:DWORD,ulCookie:DWORD
invoke AfxDeactivateActCtx, dwFlags, ulCookie
ret 8
DeacActCtx endp

end