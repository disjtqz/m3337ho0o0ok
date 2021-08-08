#include "snaphakalgo_predef.hpp"
#include "snaphakalgo.hpp"
#include <Windows.h>
#include <ntsecapi.h>
#include <ntstatus.h>
#include <Sddl.h>

/*
struct snaphak_virtmemroutines_t {
	void* (*m_allocate_rw)(size_t size);
	void (*m_release_rw)(void* mem);
	void (*m_discard_contents)(void* mem, size_t size);
	void (*m_prefetch_for_seq_access)(void* mem, size_t size);
};
*/
SNAPHAK_SHARED_SEG
static 
void* cs_vmem_allocate_rw(size_t size) {
	return VirtualAlloc(nullptr, size, MEM_COMMIT, PAGE_READWRITE);
}
SNAPHAK_SHARED_SEG
static 
void* cs_vmem_allocate_rw_absolute(size_t size, void* where) {
	void* res = VirtualAlloc(where, size, MEM_COMMIT, PAGE_READWRITE);
	if(res !=where)
		return nullptr;
	return res;
}
SNAPHAK_SHARED_SEG
static 
void* cs_vmem_allocate_rwx_absolute(size_t size, void* where) {
	void* res = VirtualAlloc(where, size, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
	if(res !=where)
		return nullptr;
	return res;
}


SNAPHAK_SHARED_SEG
static 
void cs_vmem_release(void* mem) {
	VirtualFree(mem, 0, MEM_RELEASE);
}
SNAPHAK_SHARED_SEG
static 
void cs_vmem_discard(void* mem, size_t size) {
	DiscardVirtualMemory(mem, size);
}

SNAPHAK_SHARED_SEG
static 
void cs_vmem_prefetch(void* mem, size_t size){
	WIN32_MEMORY_RANGE_ENTRY entry{};
	entry.NumberOfBytes = size;
	entry.VirtualAddress = mem;
	
	PrefetchVirtualMemory((HANDLE)-1i64, 1, &entry, 0);
}


/*
	the following is from https://stackoverflow.com/questions/42354504/enable-large-pages-in-windows-programmatically
    code is from stack overflow user Andrey Moiseev, tyvm 
*/


static void InitLsaString(PLSA_UNICODE_STRING LsaString, LPWSTR String)
{
    DWORD StringLength;

    if (String == NULL) {
        LsaString->Buffer = NULL;
        LsaString->Length = 0;
        LsaString->MaximumLength = 0;
        return;
    }

    StringLength = wcslen(String);
    LsaString->Buffer = String;
    LsaString->Length = (USHORT)StringLength * sizeof(WCHAR);
    LsaString->MaximumLength = (USHORT)(StringLength + 1) * sizeof(WCHAR);
}

static NTSTATUS OpenPolicy(LPWSTR ServerName, DWORD DesiredAccess, PLSA_HANDLE PolicyHandle)
{
    LSA_OBJECT_ATTRIBUTES ObjectAttributes;
    LSA_UNICODE_STRING ServerString;
    PLSA_UNICODE_STRING Server = NULL;

    // 
    // Always initialize the object attributes to all zeroes.
    // 
    ZeroMemory(&ObjectAttributes, sizeof(ObjectAttributes));

    if (ServerName != NULL) {
        // 
        // Make a LSA_UNICODE_STRING out of the LPWSTR passed in
        // 
        InitLsaString(&ServerString, ServerName);
        Server = &ServerString;
    }

    // 
    // Attempt to open the policy.
    // 
    return LsaOpenPolicy(
        Server,
        &ObjectAttributes,
        DesiredAccess,
        PolicyHandle
    );
}

static NTSTATUS SetPrivilegeOnAccount(LSA_HANDLE PolicyHandle, PSID AccountSid, LPWSTR PrivilegeName, BOOL bEnable)
{
    LSA_UNICODE_STRING PrivilegeString;

    // 
    // Create a LSA_UNICODE_STRING for the privilege name.
    // 
    InitLsaString(&PrivilegeString, PrivilegeName);

    // 
    // grant or revoke the privilege, accordingly
    // 
    if (bEnable) {
        return LsaAddAccountRights(
            PolicyHandle,       // open policy handle
            AccountSid,         // target SID
            &PrivilegeString,   // privileges
            1                   // privilege count
        );
    }
    else {
        return LsaRemoveAccountRights(
            PolicyHandle,       // open policy handle
            AccountSid,         // target SID
            FALSE,              // do not disable all rights
            &PrivilegeString,   // privileges
            1                   // privilege count
        );
    }
}

static int enable_huge_pages()
{
    HANDLE hToken = NULL;

    if (!OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &hToken))
    {

        return -1;
    }

    DWORD dwBufferSize = 0;

    // Probe the buffer size reqired for PTOKEN_USER structure
    if (!GetTokenInformation(hToken, TokenUser, NULL, 0, &dwBufferSize) &&
        (GetLastError() != ERROR_INSUFFICIENT_BUFFER))
    {
        //applog(LOG_INFO, "GetTokenInformation failed. GetLastError returned: %d\n", GetLastError());

        // Cleanup
        CloseHandle(hToken);
        hToken = NULL;

        return -1;
    }

    PTOKEN_USER pTokenUser = (PTOKEN_USER)malloc(dwBufferSize);

    // Retrieve the token information in a TOKEN_USER structure
    if (!GetTokenInformation(
        hToken,
        TokenUser,
        pTokenUser,
        dwBufferSize,
        &dwBufferSize))
    {
       // applog(LOG_INFO, "GetTokenInformation failed. GetLastError returned: %d\n", GetLastError());

        // Cleanup
        CloseHandle(hToken);
        hToken = NULL;

        return -1;
    }

    // Print SID string
    LPWSTR strsid;
    ConvertSidToStringSid(pTokenUser->User.Sid, &strsid);
   // applog(LOG_INFO, "User SID: %S\n", strsid);

    // Cleanup
    CloseHandle(hToken);
    hToken = NULL;

    NTSTATUS status;
    LSA_HANDLE policyHandle;

    if (status = OpenPolicy(NULL, POLICY_CREATE_ACCOUNT | POLICY_LOOKUP_NAMES, &policyHandle))
    {
        //applog(LOG_INFO, "OpenPolicy %d", status);
    }

    // Add new privelege to the account
    if (status = SetPrivilegeOnAccount(policyHandle, pTokenUser->User.Sid, SE_LOCK_MEMORY_NAME, TRUE))
    {
        //applog(LOG_INFO, "OpenPSetPrivilegeOnAccountolicy %d", status);
    }

    // Enable this priveledge for the current process
    hToken = NULL;
    TOKEN_PRIVILEGES tp;

    if (!OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY | TOKEN_ADJUST_PRIVILEGES, &hToken))
    {
      //  applog(LOG_INFO, "OpenProcessToken #2 failed. GetLastError returned: %d\n", GetLastError());
        return -1;
    }

    tp.PrivilegeCount = 1;
    tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

    if (!LookupPrivilegeValue(NULL, SE_LOCK_MEMORY_NAME, &tp.Privileges[0].Luid))
    {
       //applog(LOG_INFO, "LookupPrivilegeValue failed. GetLastError returned: %d\n", GetLastError());
        return -1;
    }

    BOOL result = AdjustTokenPrivileges(hToken, FALSE, &tp, 0, (PTOKEN_PRIVILEGES)NULL, 0);
    DWORD error = GetLastError();

    if (!result || (error != ERROR_SUCCESS))
    {
        //applog(LOG_INFO, "AdjustTokenPrivileges failed. GetLastError returned: %d\n", error);
        return -1;
    }

    // Cleanup
    CloseHandle(hToken);
    hToken = NULL;
    return 0;
    //SIZE_T pageSize = GetLargePageMinimum();

    // Finally allocate the memory
   /*char* largeBuffer = VirtualAlloc(NULL, pageSize * N_PAGES_TO_ALLOC, MEM_RESERVE | MEM_COMMIT | MEM_LARGE_PAGES, PAGE_READWRITE);
    if (largeBuffer)
    {
        //applog(LOG_INFO, "VirtualAlloc failed, error 0x%x", GetLastError());
    }*/
}
void vmemalgos_init(snaphak_virtmemroutines_t* routines) {
	routines->m_allocate_rw = cs_vmem_allocate_rw;
	routines->m_release_rw = cs_vmem_release;
	routines->m_discard_contents = cs_vmem_discard;
	routines->m_prefetch_for_seq_access = cs_vmem_prefetch;
	routines->m_allocate_rwx_absolute = cs_vmem_allocate_rwx_absolute;
	routines->m_allocate_rw_absolute = cs_vmem_allocate_rw_absolute;
    routines->m_try_enable_hugepages = enable_huge_pages;
}