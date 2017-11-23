#include "ps.h"
#include <Psapi.h>

typedef enum _SYSTEM_INFORMATION_CLASS     //    Q S  
{
	SystemBasicInformation,                // 00 Y N  
	SystemProcessorInformation,            // 01 Y N  
	SystemPerformanceInformation,          // 02 Y N  
	SystemTimeOfDayInformation,            // 03 Y N  
	SystemNotImplemented1,                 // 04 Y N  
	SystemProcessesAndThreadsInformation,  // 05 Y N  
	SystemCallCounts,                      // 06 Y N  
	SystemConfigurationInformation,        // 07 Y N  
	SystemProcessorTimes,                  // 08 Y N  
	SystemGlobalFlag,                      // 09 Y Y  
	SystemNotImplemented2,                 // 10 Y N  
	SystemModuleInformation,               // 11 Y N  
	SystemLockInformation,                 // 12 Y N  
	SystemNotImplemented3,                 // 13 Y N  
	SystemNotImplemented4,                 // 14 Y N  
	SystemNotImplemented5,                 // 15 Y N  
	SystemHandleInformation,               // 16 Y N  
	SystemObjectInformation,               // 17 Y N  
	SystemPagefileInformation,             // 18 Y N  
	SystemInstructionEmulationCounts,      // 19 Y N  
	SystemInvalidInfoClass1,               // 20  
	SystemCacheInformation,                // 21 Y Y  
	SystemPoolTagInformation,              // 22 Y N  
	SystemProcessorStatistics,             // 23 Y N  
	SystemDpcInformation,                  // 24 Y Y  
	SystemNotImplemented6,                 // 25 Y N  
	SystemLoadImage,                       // 26 N Y  
	SystemUnloadImage,                     // 27 N Y  
	SystemTimeAdjustment,                  // 28 Y Y  
	SystemNotImplemented7,                 // 29 Y N  
	SystemNotImplemented8,                 // 30 Y N  
	SystemNotImplemented9,                 // 31 Y N  
	SystemCrashDumpInformation,            // 32 Y N  
	SystemExceptionInformation,            // 33 Y N  
	SystemCrashDumpStateInformation,       // 34 Y Y/N  
	SystemKernelDebuggerInformation,       // 35 Y N  
	SystemContextSwitchInformation,        // 36 Y N  
	SystemRegistryQuotaInformation,        // 37 Y Y  
	SystemLoadAndCallImage,                // 38 N Y  
	SystemPrioritySeparation,              // 39 N Y  
	SystemNotImplemented10,                // 40 Y N  
	SystemNotImplemented11,                // 41 Y N  
	SystemInvalidInfoClass2,               // 42  
	SystemInvalidInfoClass3,               // 43  
	SystemTimeZoneInformation,             // 44 Y N  
	SystemLookasideInformation,            // 45 Y N  
	SystemSetTimeSlipEvent,                // 46 N Y  
	SystemCreateSession,                   // 47 N Y  
	SystemDeleteSession,                   // 48 N Y  
	SystemInvalidInfoClass4,               // 49  
	SystemRangeStartInformation,           // 50 Y N  
	SystemVerifierInformation,             // 51 Y Y  
	SystemAddVerifier,                     // 52 N Y  
	SystemSessionProcessesInformation      // 53 Y N  
} SYSTEM_INFORMATION_CLASS;

typedef struct _SYSTEM_MODULE_INFORMATION  // Information Class 11  
{
	ULONG  Reserved[2];
	PVOID  Base;
	ULONG  Size;
	ULONG  Flags;
	USHORT Index;
	USHORT Unknown;
	USHORT LoadCount;
	USHORT ModuleNameOffset;
	CHAR   ImageName[256];
} SYSTEM_MODULE_INFORMATION, *PSYSTEM_MODULE_INFORMATION;


typedef struct tagPROCESS_BASIC_INFORMATION
{
	DWORD ExitStatus;
	DWORD PebBaseAddress;
	DWORD AffinityMask;
	DWORD BasePriority;
	ULONG UniqueProcessId;
	ULONG InheritedFromUniqueProcessId;
}PROCESS_BASIC_INFORMATION;


typedef struct _UNICODE_STRING {
	USHORT Length;
	USHORT MaximumLength;
	PWSTR  Buffer;                 //注意，这里为Unicode类型
} UNICODE_STRING, *PUNICODE_STRING;


typedef struct _SYSTEM_PROCESS_INFORMATION
{
	DWORD NextEntryDelta;
	DWORD dThreadCount;
	DWORD dReserved01;
	DWORD dReserved02;
	DWORD dReserved03;
	DWORD dReserved04;
	DWORD dReserved05;
	DWORD dReserved06;
	FILETIME ftCreateTime; /* relative to 01-01-1601 */
	FILETIME ftUserTime; /* 100 nsec units */
	FILETIME ftKernelTime; /* 100 nsec units */
	UNICODE_STRING ProcessName;      //这就是进程名
	DWORD BasePriority;
	DWORD dUniqueProcessId;            //进程ID
	DWORD dParentProcessID;
	DWORD dHandleCount;
	DWORD dReserved07;
	DWORD dReserved08;
	DWORD VmCounters;
	DWORD dCommitCharge;
	PVOID ThreadInfos[1];
} SYSTEM_PROCESS_INFORMATION, *PSYSTEM_PROCESS_INFORMATION;


typedef LONG		(__stdcall *PNTQUERYINFORMATIONPROCESS)	(HANDLE, UINT, PVOID, ULONG, PULONG);
//typedef NTSTATUS	(__stdcall *PZWQUERYSYSTEMINFORMATION)	(UINT, PVOID, ULONG, PULONG);
typedef NTSTATUS	(__stdcall *PZWQUERYSYSTEMINFORMATION)
(
	IN SYSTEM_INFORMATION_CLASS SystemInformationClass, 
	IN OUT PVOID SystemInformation, 
	IN ULONG SystemInformationLength, 
	OUT PULONG ReturnLength OPTIONAL
);

static PNTQUERYINFORMATIONPROCESS  NtQueryInformationProcess = nullptr; 
static PZWQUERYSYSTEMINFORMATION  ZwQuerySystemInformation = nullptr;


void init_ntdll()
{
	if (!NtQueryInformationProcess)
		NtQueryInformationProcess = (PNTQUERYINFORMATIONPROCESS)GetProcAddress(GetModuleHandle("ntdll"), "NtQueryInformationProcess");

	if (!ZwQuerySystemInformation)
		ZwQuerySystemInformation = (PZWQUERYSYSTEMINFORMATION)GetProcAddress(GetModuleHandle("ntdll"), "ZwQuerySystemInformation");
}

DWORD GetParantPid(DWORD dwId)
{
	LONG                      status;
	DWORD                     dwParentPID = 0;
	HANDLE                    hProcess;
	PROCESS_BASIC_INFORMATION pbi;

	hProcess = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, dwId);
	if (!hProcess)
		return -1;

	status = NtQueryInformationProcess(hProcess, SystemBasicInformation,
		(PVOID)&pbi, sizeof(PROCESS_BASIC_INFORMATION), NULL);

	if (!status)
		dwParentPID = pbi.InheritedFromUniqueProcessId;

	CloseHandle(hProcess);
	return dwParentPID;
}


void GetChildPids(DWORD pid, std::vector<DWORD>& vec_child)
{
	DWORD needed = 0;
	DWORD processid[1024] = { 0 };

	if (!EnumProcesses(processid, sizeof(processid), &needed))
		return;

	int processcount = needed / sizeof(DWORD);

	for (int i = 0; i < processcount; i++)
	{
		if (pid == GetParantPid(processid[i]))
			vec_child.push_back(processid[i]);
	}
}


DWORD RunCmd(const char* path, bool show_window)
{
	//SECURITY_ATTRIBUTES sa;

	//sa.nLength = sizeof(SECURITY_ATTRIBUTES);
	//sa.lpSecurityDescriptor = NULL;
	//sa.bInheritHandle = TRUE;

	STARTUPINFOA si;
	PROCESS_INFORMATION pi;

	si.cb = sizeof(STARTUPINFO);
	GetStartupInfoA(&si);
	si.wShowWindow = show_window ? SW_SHOW : SW_HIDE;
	si.dwFlags = STARTF_USESHOWWINDOW;

	if (!CreateProcessA(0,
		(char*)path,
		NULL, NULL, TRUE, NULL, NULL, NULL, &si, &pi))
	{
		return 0;
	}
	return pi.dwProcessId;
}

void ListModule(void)
{
	NTSTATUS                    status;
	PSYSTEM_MODULE_INFORMATION  module = NULL;
	ULONG                       n = 0;
	ULONG                       i = 0;
	void                       *buf = NULL;

	status = ZwQuerySystemInformation(SystemModuleInformation, buf, n, NULL);
	if (!BCRYPT_SUCCESS(status))
	{
		//PrintZwError("ZwQuerySystemInformation() failed", status);
		goto ListModule_return;
	}

	module = (PSYSTEM_MODULE_INFORMATION)((PULONG)buf + 1);
	n = *((PULONG)buf);
	for (i = 0; i < n; i++)
	{
		printf("0x%08X %s\n", module[i].Base, module[i].ImageName + module[i].ModuleNameOffset);
	}

ListModule_return:

	if (buf != NULL)
	{
		free(buf);
		buf = NULL;
	}
	return;
}

void list_process()
{
	NTSTATUS                    status;
	PSYSTEM_PROCESS_INFORMATION  pProcIndex = NULL;
	void                       *buf = NULL;
	DWORD dwNeedSize = 0;


	status = ZwQuerySystemInformation(SystemProcessesAndThreadsInformation, 0, 0, &dwNeedSize);
	if (!dwNeedSize)
	{
		return;
	}


	buf = new BYTE[dwNeedSize];
	status = ZwQuerySystemInformation(SystemProcessesAndThreadsInformation, buf, dwNeedSize, nullptr);

	if (BCRYPT_SUCCESS(status))
	{
		auto pProcIndex = (PSYSTEM_PROCESS_INFORMATION)buf;

		if (pProcIndex->dUniqueProcessId == 0)
			printf("PID 0 System Idle Process\n");

		do
		{
			pProcIndex = (PSYSTEM_PROCESS_INFORMATION)((char*)pProcIndex + pProcIndex->NextEntryDelta);
			if (pProcIndex->ProcessName.Buffer == NULL)
				pProcIndex->ProcessName.Buffer = L"NULL";

			printf("pid:%5u thd:%3d ppid:%5u name:%ws\n",
				pProcIndex->dUniqueProcessId,
				pProcIndex->dThreadCount,
				pProcIndex->dParentProcessID,
				pProcIndex->ProcessName.Buffer);
		} while (pProcIndex->NextEntryDelta != 0);
	}

	delete buf;	
}