#include <windows.h>

static void get_priv() {
	HANDLE tok; LUID val; TOKEN_PRIVILEGES tp;
	OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &tok);
	LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &val);
	tp.PrivilegeCount = 1;
	tp.Privileges[0].Luid = val;
	tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
	AdjustTokenPrivileges(tok, FALSE, &tp, sizeof(TOKEN_PRIVILEGES), NULL, NULL);
	CloseHandle(tok);
}

static void inject_dll(const char *dll, const int pid) {
	void *lla = GetProcAddress(GetModuleHandleA("KERNEL32"), "LoadLibraryA");
	HANDLE proc = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
	void *smem = VirtualAllocEx(proc, NULL, strlen(dll) + 1, MEM_COMMIT, PAGE_READWRITE);
	WriteProcessMemory(proc, smem, dll, strlen(dll) + 1, NULL);
	HANDLE thread = CreateRemoteThread(proc, NULL, 0, lla, smem, 0, NULL);
	WaitForSingleObject(thread, INFINITE);
	CloseHandle(thread);
	VirtualFreeEx(proc, smem, 0, MEM_RELEASE);
	CloseHandle(proc);
}

int main(int argc, char **argv) {
	if (argc != 3) return 1;
	get_priv();
	inject_dll(argv[1], strtoul(argv[2], NULL, 10));
	return 0;
}
