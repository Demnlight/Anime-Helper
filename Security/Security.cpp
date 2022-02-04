#include "Security.h"
#include "../Backend/Globals.h"

#include <thread>
#include <tlhelp32.h>
#include <winternl.h>

#ifndef _WIN64
PPEB pPeb = (PPEB)__readfsdword(0x30);
#else
PPEB pPeb = (PPEB)__readgsqword(0x60);
#endif // _WIN64
void EraseMemory()
{
	auto base_addr = static_cast<void*>(GetCommandLineA());
	VirtualProtect(base_addr, sizeof(base_addr), 0u, nullptr);
	SecureZeroMemory(base_addr, sizeof(base_addr));
}
inline bool Int2DCheck()
{
	__try
	{
		__asm
		{
			int 0x2d
			xor eax, eax
			add eax, 2
		}
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		return false;
	}

	return true;
}
inline bool IsDbgPresentPrefixCheck()
{
	__try
	{
		__asm __emit 0xF3 // 0xF3 0x64 disassembles as PREFIX REP:
		__asm __emit 0x64
		__asm __emit 0xF1 // One byte INT 1
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		return false;
	}

	return true;
}
inline bool checkGlobalFlags()
{
	DWORD NtGlobalFlags;
	__asm
	{
		mov eax, fs: [30h]
		mov eax, [eax + 68h]
		mov NtGlobalFlags, eax
	}

	if (NtGlobalFlags & 0x70)
		return true;

	return false;
}
BOOL(WINAPI* RegisterServiceProcess)(DWORD dwProcessId, DWORD dwType);
DWORD GetProcessPID(char* ProcName)
{
	PROCESSENTRY32 lppe;
	long PID = 0, Result = 0;
	HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
	if (hSnap)
	{
		lppe.dwSize = sizeof(PROCESSENTRY32);
		Result = Process32First(hSnap, &lppe);
		while (Result)
		{
			if (strcmp(lppe.szExeFile, ProcName) == NULL)
			{
				PID = lppe.th32ProcessID;
				break;
			}
			Result = Process32Next(hSnap, &lppe);
		}
		CloseHandle(hSnap);
	}
	return PID;
}
BOOL WINAPI IsDebug()
{
	HANDLE hProcess = NULL;
	PROCESSENTRY32 Pe32 = { 0 };
	HANDLE hProcessSnap = ::CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hProcessSnap == INVALID_HANDLE_VALUE)
	{
		return FALSE;
	}
	Pe32.dwSize = sizeof(PROCESSENTRY32);

	if (Process32First(hProcessSnap, &Pe32))
	{
		do
		{
			if (_stricmp("csrss.exe", Pe32.szExeFile) == 0)
			{
				HANDLE hProcess = OpenProcess(
					PROCESS_ALL_ACCESS,
					FALSE,
					Pe32.th32ProcessID
				);

				if (hProcess)
				{
					return TRUE;
				}
				else
				{
					return FALSE;
				}
				CloseHandle(hProcess);
			}
		} while (Process32Next(hProcessSnap, &Pe32));
	}
	CloseHandle(hProcessSnap);
}
DWORD __forceinline IsInsideVPC_exceptionFilter(LPEXCEPTION_POINTERS ep)
{
	PCONTEXT ctx = ep->ContextRecord;

	ctx->Ebx = -1;
	ctx->Eip += 4;
	return EXCEPTION_CONTINUE_EXECUTION;
}
bool IsInsideVPC()
{
	bool rc = false;

	__try
	{
		_asm push ebx
		_asm mov  ebx, 0
		_asm mov  eax, 1


		_asm __emit 0Fh
		_asm __emit 3Fh
		_asm __emit 07h
		_asm __emit 0Bh

		_asm test ebx, ebx
		_asm setz[rc]
			_asm pop ebx
	}

	__except (IsInsideVPC_exceptionFilter(GetExceptionInformation()))
	{
	}

	return rc;
}
bool IsInsideVMWare()
{
	bool rc = true;

	__try
	{
		__asm
		{
			push   edx
			push   ecx
			push   ebx

			mov    eax, 'VMXh'
			mov    ebx, 0
			mov    ecx, 10
			mov    edx, 'VX'

			in     eax, dx

			cmp    ebx, 'VMXh'
			setz[rc]

			pop    ebx
			pop    ecx
			pop    edx
		}
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		rc = false;
	}

	return rc;
}
bool VMCheck()
{
	unsigned int time1 = 0;
	unsigned int time2 = 0;
	__asm
	{
		RDTSC
		MOV time1, EAX
		RDTSC
		MOV time2, EAX

	}
	if ((time2 - time1) > 100)
	{
		return true;
	}

	return false;
}
BOOL AD_CheckRemoteDebuggerPresent()
{
	FARPROC Func_addr;
	HMODULE hModule = GetModuleHandle("kernel32.dll");

	if (hModule == INVALID_HANDLE_VALUE)
		return false;

	(FARPROC&)Func_addr = GetProcAddress(hModule, "CheckRemoteDebuggerPresent");

	if (Func_addr != NULL) {
		__asm {
			push  eax;
			push  esp;
			push  0xffffffff;
			call  Func_addr;
			test  eax, eax;
			je    choke_false;
			pop    eax;
			test  eax, eax
				je    choke_false;
			jmp    choke_true;
		}
	}

choke_true:
	return true;

choke_false:
	return false;
}
BOOL AD_PEB_NtGlobalFlags()
{
	__asm {
		mov eax, fs: [30h]
		mov eax, [eax + 68h]
		and eax, 0x70
	}
}
BOOL AD_PEB_IsDebugged()
{
	__asm {
		xor eax, eax
		mov ebx, fs: [30h]
		mov al, byte ptr[ebx + 2]
	}
}
bool SecurityCheck()
{
	if (IsDebuggerPresent())
		return false;

	if (IsDebug())
		return false;

	if (Int2DCheck())
		return false;

	if (AD_PEB_IsDebugged())
		return false;

	if (AD_PEB_NtGlobalFlags())
		return false;

	if (AD_CheckRemoteDebuggerPresent())
		return false;

	if (IsInsideVPC())
		return false;

	if (IsInsideVMWare())
		return false;

	if (VMCheck())
		return false;

	return true;
}
__forceinline std::uintptr_t HideThread()
{
	using NtSetInformationThreadFn = NTSTATUS(NTAPI*)
		(
			HANDLE ThreadHandle,
			ULONG  ThreadInformationClass,
			PVOID  ThreadInformation,
			ULONG  ThreadInformationLength
			);

	const ULONG hide_thread_from_debugger = 0x11;

	NtSetInformationThreadFn NtSetInformationThread = (NtSetInformationThreadFn)GetProcAddress
	(
		GetModuleHandleA("ntdll.dll"),
		"NtSetInformationThread"
	);

	NTSTATUS status = NtSetInformationThread
	(
		(HANDLE)0xFFFFFFFE,
		hide_thread_from_debugger,
		NULL,
		NULL
	);

	if (status != 0x0)
	{
		std::cout << ("[ Aimware Antidebug ] NtSetInformationThread | 0x0\n\n");
		std::cout << ("[ Aimware Antidebug ] Starting execute syscall\n");

		const auto Wow64AllocMemory = VirtualAlloc
		(
			0x0,
			0x1000,
			MEM_COMMIT | MEM_RESERVE,
			PAGE_EXECUTE_READWRITE
		);

		__asm
		{
			mov edx, dword ptr fs : [0xC0]
			movups xmm0, xmmword ptr ds : [edx]
			mov eax, dword ptr ds : [Wow64AllocMemory]
			mov dword ptr ss : [ebp - 0x8] , eax
			movups xmmword ptr ds : [eax] , xmm0
		}

		__asm
		{
			push 0x0
			push 0x0
			push 0x11
			push 0xFFFFFFFE

			call $ + 5

			mov eax, 0xD

			call dword ptr ds : [Wow64AllocMemory]
		}

		return status;
	}

	std::cout << ("[ Aimware Antidebug ] NtSetInformationThread | 0x1\n");

	return status;
}

void DoCheck()
{
	while (true)
	{
		if (pPeb->BeingDebugged)
		{
			MessageBox(NULL, "pPeb->BeingDebugged", "Error", MB_OKCANCEL);
			ExitProcess(0);
		}
		if (!SecurityCheck())
		{
			MessageBox(NULL, "SecurityCheck", "Error", MB_OKCANCEL);
			ExitProcess(0);
		}
		Sleep(500);
	}
}
void CSeciruty::Init()
{
	//EraseMemory();
	//HideThread();
	std::thread checks(DoCheck);
	checks.join();
}