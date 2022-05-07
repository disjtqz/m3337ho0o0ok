#if defined(ENABLE_SYSCALL_STUFF)
#include "syscall_list.hpp"

#include <array>
#include <Windows.h>

constexpr unsigned hash_string(const char* s) {
	
	unsigned r = 0;
	while(*s) {
		unsigned bleh = *s;
		r*=31;
		r+= bleh;
		s++;
	}
	return r;
}

struct syscall_translator_t {
	unsigned hashcode;
	unsigned swicode;

	constexpr syscall_translator_t(unsigned swic, const char* nm) : hashcode(hash_string(nm)), swicode(swic){}
};



#define DECL_SYSCALL(...)		syscall_translator_t(__VA_ARGS__)

static constexpr syscall_translator_t g_syscalls_translation[] = {
DECL_SYSCALL(_ZwAccessCheck,"ZwAccessCheck"),DECL_SYSCALL(_NtWorkerFactoryWorkerReady,"NtWorkerFactoryWorkerReady"),DECL_SYSCALL(_NtAcceptConnectPort,"NtAcceptConnectPort"),DECL_SYSCALL(_ZwMapUserPhysicalPagesScatter,"ZwMapUserPhysicalPagesScatter"),DECL_SYSCALL(_NtWaitForSingleObject,"NtWaitForSingleObject"),DECL_SYSCALL(_ZwCallbackReturn,"ZwCallbackReturn"),DECL_SYSCALL(_NtReadFile,"NtReadFile"),DECL_SYSCALL(_NtDeviceIoControlFile,"NtDeviceIoControlFile"),DECL_SYSCALL(_NtWriteFile,"NtWriteFile"),DECL_SYSCALL(_ZwRemoveIoCompletion,"ZwRemoveIoCompletion"),DECL_SYSCALL(_ZwReleaseSemaphore,"ZwReleaseSemaphore"),DECL_SYSCALL(_ZwReplyWaitReceivePort,"ZwReplyWaitReceivePort"),DECL_SYSCALL(_NtReplyPort,"NtReplyPort"),DECL_SYSCALL(_NtSetInformationThread,"NtSetInformationThread"),DECL_SYSCALL(_ZwSetEvent,"ZwSetEvent"),DECL_SYSCALL(_NtClose,"NtClose"),DECL_SYSCALL(_ZwQueryObject,"ZwQueryObject"),DECL_SYSCALL(_NtQueryInformationFile,"NtQueryInformationFile"),DECL_SYSCALL(_NtOpenKey,"NtOpenKey"),DECL_SYSCALL(_ZwEnumerateValueKey,"ZwEnumerateValueKey"),DECL_SYSCALL(_ZwFindAtom,"ZwFindAtom"),DECL_SYSCALL(_NtQueryDefaultLocale,"NtQueryDefaultLocale"),DECL_SYSCALL(_ZwQueryKey,"ZwQueryKey"),DECL_SYSCALL(_NtQueryValueKey,"NtQueryValueKey"),DECL_SYSCALL(_ZwAllocateVirtualMemory,"ZwAllocateVirtualMemory"),DECL_SYSCALL(_NtQueryInformationProcess,"NtQueryInformationProcess"),DECL_SYSCALL(_ZwWaitForMultipleObjects32,"ZwWaitForMultipleObjects32"),DECL_SYSCALL(_ZwWriteFileGather,"ZwWriteFileGather"),DECL_SYSCALL(_NtSetInformationProcess,"NtSetInformationProcess"),DECL_SYSCALL(_ZwCreateKey,"ZwCreateKey"),DECL_SYSCALL(_ZwFreeVirtualMemory,"ZwFreeVirtualMemory"),DECL_SYSCALL(_ZwImpersonateClientOfPort,"ZwImpersonateClientOfPort"),DECL_SYSCALL(_ZwReleaseMutant,"ZwReleaseMutant"),DECL_SYSCALL(_NtQueryInformationToken,"NtQueryInformationToken"),DECL_SYSCALL(_ZwRequestWaitReplyPort,"ZwRequestWaitReplyPort"),DECL_SYSCALL(_ZwQueryVirtualMemory,"ZwQueryVirtualMemory"),DECL_SYSCALL(_NtOpenThreadToken,"NtOpenThreadToken"),DECL_SYSCALL(_ZwQueryInformationThread,"ZwQueryInformationThread"),DECL_SYSCALL(_NtOpenProcess,"NtOpenProcess"),DECL_SYSCALL(_ZwSetInformationFile,"ZwSetInformationFile"),DECL_SYSCALL(_ZwMapViewOfSection,"ZwMapViewOfSection"),DECL_SYSCALL(_ZwAccessCheckAndAuditAlarm,"ZwAccessCheckAndAuditAlarm"),DECL_SYSCALL(_NtUnmapViewOfSection,"NtUnmapViewOfSection"),DECL_SYSCALL(_ZwReplyWaitReceivePortEx,"ZwReplyWaitReceivePortEx"),DECL_SYSCALL(_ZwTerminateProcess,"ZwTerminateProcess"),DECL_SYSCALL(_ZwSetEventBoostPriority,"ZwSetEventBoostPriority"),DECL_SYSCALL(_ZwReadFileScatter,"ZwReadFileScatter"),DECL_SYSCALL(_NtOpenThreadTokenEx,"NtOpenThreadTokenEx"),DECL_SYSCALL(_NtOpenProcessTokenEx,"NtOpenProcessTokenEx"),DECL_SYSCALL(_NtQueryPerformanceCounter,"NtQueryPerformanceCounter"),DECL_SYSCALL(_NtEnumerateKey,"NtEnumerateKey"),DECL_SYSCALL(_NtOpenFile,"NtOpenFile"),DECL_SYSCALL(_ZwDelayExecution,"ZwDelayExecution"),DECL_SYSCALL(_NtQueryDirectoryFile,"NtQueryDirectoryFile"),DECL_SYSCALL(_NtQuerySystemInformation,"NtQuerySystemInformation"),DECL_SYSCALL(_NtOpenSection,"NtOpenSection"),DECL_SYSCALL(_ZwQueryTimer,"ZwQueryTimer"),DECL_SYSCALL(_ZwFsControlFile,"ZwFsControlFile"),DECL_SYSCALL(_NtWriteVirtualMemory,"NtWriteVirtualMemory"),DECL_SYSCALL(_ZwCloseObjectAuditAlarm,"ZwCloseObjectAuditAlarm"),DECL_SYSCALL(_ZwDuplicateObject,"ZwDuplicateObject"),DECL_SYSCALL(_ZwQueryAttributesFile,"ZwQueryAttributesFile"),DECL_SYSCALL(_NtClearEvent,"NtClearEvent"),DECL_SYSCALL(_ZwReadVirtualMemory,"ZwReadVirtualMemory"),DECL_SYSCALL(_NtOpenEvent,"NtOpenEvent"),DECL_SYSCALL(_NtAdjustPrivilegesToken,"NtAdjustPrivilegesToken"),DECL_SYSCALL(_NtDuplicateToken,"NtDuplicateToken"),DECL_SYSCALL(_ZwContinue,"ZwContinue"),DECL_SYSCALL(_ZwQueryDefaultUILanguage,"ZwQueryDefaultUILanguage"),DECL_SYSCALL(_NtQueueApcThread,"NtQueueApcThread"),DECL_SYSCALL(_ZwYieldExecution,"ZwYieldExecution"),DECL_SYSCALL(_ZwAddAtom,"ZwAddAtom"),DECL_SYSCALL(_ZwCreateEvent,"ZwCreateEvent"),DECL_SYSCALL(_ZwQueryVolumeInformationFile,"ZwQueryVolumeInformationFile"),DECL_SYSCALL(_NtCreateSection,"NtCreateSection"),DECL_SYSCALL(_ZwFlushBuffersFile,"ZwFlushBuffersFile"),DECL_SYSCALL(_NtApphelpCacheControl,"NtApphelpCacheControl"),DECL_SYSCALL(_ZwCreateProcessEx,"ZwCreateProcessEx"),DECL_SYSCALL(_NtCreateThread,"NtCreateThread"),DECL_SYSCALL(_NtIsProcessInJob,"NtIsProcessInJob"),DECL_SYSCALL(_ZwProtectVirtualMemory,"ZwProtectVirtualMemory"),DECL_SYSCALL(_NtQuerySection,"NtQuerySection"),DECL_SYSCALL(_ZwResumeThread,"ZwResumeThread"),DECL_SYSCALL(_NtTerminateThread,"NtTerminateThread"),DECL_SYSCALL(_NtReadRequestData,"NtReadRequestData"),DECL_SYSCALL(_ZwCreateFile,"ZwCreateFile"),DECL_SYSCALL(_NtQueryEvent,"NtQueryEvent"),DECL_SYSCALL(_ZwWriteRequestData,"ZwWriteRequestData"),DECL_SYSCALL(_ZwOpenDirectoryObject,"ZwOpenDirectoryObject"),DECL_SYSCALL(_ZwAccessCheckByTypeAndAuditAlarm,"ZwAccessCheckByTypeAndAuditAlarm"),DECL_SYSCALL(_NtWaitForMultipleObjects,"NtWaitForMultipleObjects"),DECL_SYSCALL(_NtSetInformationObject,"NtSetInformationObject"),DECL_SYSCALL(_ZwCancelIoFile,"ZwCancelIoFile"),DECL_SYSCALL(_NtTraceEvent,"NtTraceEvent"),DECL_SYSCALL(_NtPowerInformation,"NtPowerInformation"),DECL_SYSCALL(_ZwSetValueKey,"ZwSetValueKey"),DECL_SYSCALL(_NtCancelTimer,"NtCancelTimer"),DECL_SYSCALL(_ZwSetTimer,"ZwSetTimer"),DECL_SYSCALL(_ZwAccessCheckByType,"ZwAccessCheckByType"),DECL_SYSCALL(_ZwAccessCheckByTypeResultList,"ZwAccessCheckByTypeResultList"),DECL_SYSCALL(_ZwAccessCheckByTypeResultListAndAuditAlarm,"ZwAccessCheckByTypeResultListAndAuditAlarm"),DECL_SYSCALL(_ZwAccessCheckByTypeResultListAndAuditAlarmByHandle,"ZwAccessCheckByTypeResultListAndAuditAlarmByHandle"),DECL_SYSCALL(_ZwAcquireProcessActivityReference,"ZwAcquireProcessActivityReference"),DECL_SYSCALL(_ZwAddAtomEx,"ZwAddAtomEx"),DECL_SYSCALL(_ZwAddBootEntry,"ZwAddBootEntry"),DECL_SYSCALL(_NtAddDriverEntry,"NtAddDriverEntry"),DECL_SYSCALL(_NtAdjustGroupsToken,"NtAdjustGroupsToken"),DECL_SYSCALL(_ZwAdjustTokenClaimsAndDeviceGroups,"ZwAdjustTokenClaimsAndDeviceGroups"),DECL_SYSCALL(_ZwAlertResumeThread,"ZwAlertResumeThread"),DECL_SYSCALL(_NtAlertThread,"NtAlertThread"),DECL_SYSCALL(_ZwAlertThreadByThreadId,"ZwAlertThreadByThreadId"),DECL_SYSCALL(_ZwAllocateLocallyUniqueId,"ZwAllocateLocallyUniqueId"),DECL_SYSCALL(_NtAllocateReserveObject,"NtAllocateReserveObject"),DECL_SYSCALL(_ZwAllocateUserPhysicalPages,"ZwAllocateUserPhysicalPages"),DECL_SYSCALL(_ZwAllocateUuids,"ZwAllocateUuids"),DECL_SYSCALL(_NtAllocateVirtualMemoryEx,"NtAllocateVirtualMemoryEx"),DECL_SYSCALL(_ZwAlpcAcceptConnectPort,"ZwAlpcAcceptConnectPort"),DECL_SYSCALL(_NtAlpcCancelMessage,"NtAlpcCancelMessage"),DECL_SYSCALL(_NtAlpcConnectPort,"NtAlpcConnectPort"),DECL_SYSCALL(_NtAlpcConnectPortEx,"NtAlpcConnectPortEx"),DECL_SYSCALL(_NtAlpcCreatePort,"NtAlpcCreatePort"),DECL_SYSCALL(_ZwAlpcCreatePortSection,"ZwAlpcCreatePortSection"),DECL_SYSCALL(_ZwAlpcCreateResourceReserve,"ZwAlpcCreateResourceReserve"),DECL_SYSCALL(_NtAlpcCreateSectionView,"NtAlpcCreateSectionView"),DECL_SYSCALL(_NtAlpcCreateSecurityContext,"NtAlpcCreateSecurityContext"),DECL_SYSCALL(_NtAlpcDeletePortSection,"NtAlpcDeletePortSection"),DECL_SYSCALL(_NtAlpcDeleteResourceReserve,"NtAlpcDeleteResourceReserve"),DECL_SYSCALL(_ZwAlpcDeleteSectionView,"ZwAlpcDeleteSectionView"),DECL_SYSCALL(_ZwAlpcDeleteSecurityContext,"ZwAlpcDeleteSecurityContext"),DECL_SYSCALL(_NtAlpcDisconnectPort,"NtAlpcDisconnectPort"),DECL_SYSCALL(_NtAlpcImpersonateClientContainerOfPort,"NtAlpcImpersonateClientContainerOfPort"),DECL_SYSCALL(_NtAlpcImpersonateClientOfPort,"NtAlpcImpersonateClientOfPort"),DECL_SYSCALL(_NtAlpcOpenSenderProcess,"NtAlpcOpenSenderProcess"),DECL_SYSCALL(_NtAlpcOpenSenderThread,"NtAlpcOpenSenderThread"),DECL_SYSCALL(_ZwAlpcQueryInformation,"ZwAlpcQueryInformation"),DECL_SYSCALL(_ZwAlpcQueryInformationMessage,"ZwAlpcQueryInformationMessage"),DECL_SYSCALL(_NtAlpcRevokeSecurityContext,"NtAlpcRevokeSecurityContext"),DECL_SYSCALL(_ZwAlpcSendWaitReceivePort,"ZwAlpcSendWaitReceivePort"),DECL_SYSCALL(_NtAlpcSetInformation,"NtAlpcSetInformation"),DECL_SYSCALL(_ZwAreMappedFilesTheSame,"ZwAreMappedFilesTheSame"),DECL_SYSCALL(_NtAssignProcessToJobObject,"NtAssignProcessToJobObject"),DECL_SYSCALL(_ZwAssociateWaitCompletionPacket,"ZwAssociateWaitCompletionPacket"),DECL_SYSCALL(_ZwCallEnclave,"ZwCallEnclave"),DECL_SYSCALL(_ZwCancelIoFileEx,"ZwCancelIoFileEx"),DECL_SYSCALL(_ZwCancelSynchronousIoFile,"ZwCancelSynchronousIoFile"),DECL_SYSCALL(_ZwCancelTimer2,"ZwCancelTimer2"),DECL_SYSCALL(_ZwCancelWaitCompletionPacket,"ZwCancelWaitCompletionPacket"),DECL_SYSCALL(_NtCommitComplete,"NtCommitComplete"),DECL_SYSCALL(_ZwCommitEnlistment,"ZwCommitEnlistment"),DECL_SYSCALL(_NtCommitRegistryTransaction,"NtCommitRegistryTransaction"),DECL_SYSCALL(_ZwCommitTransaction,"ZwCommitTransaction"),DECL_SYSCALL(_NtCompactKeys,"NtCompactKeys"),DECL_SYSCALL(_NtCompareObjects,"NtCompareObjects"),DECL_SYSCALL(_NtCompareSigningLevels,"NtCompareSigningLevels"),DECL_SYSCALL(_ZwCompareTokens,"ZwCompareTokens"),DECL_SYSCALL(_NtCompleteConnectPort,"NtCompleteConnectPort"),DECL_SYSCALL(_NtCompressKey,"NtCompressKey"),DECL_SYSCALL(_ZwConnectPort,"ZwConnectPort"),DECL_SYSCALL(_ZwConvertBetweenAuxiliaryCounterAndPerformanceCounter,"ZwConvertBetweenAuxiliaryCounterAndPerformanceCounter"),DECL_SYSCALL(_NtCreateDebugObject,"NtCreateDebugObject"),DECL_SYSCALL(_NtCreateDirectoryObject,"NtCreateDirectoryObject"),DECL_SYSCALL(_ZwCreateDirectoryObjectEx,"ZwCreateDirectoryObjectEx"),DECL_SYSCALL(_ZwCreateEnclave,"ZwCreateEnclave"),DECL_SYSCALL(_NtCreateEnlistment,"NtCreateEnlistment"),DECL_SYSCALL(_ZwCreateEventPair,"ZwCreateEventPair"),DECL_SYSCALL(_ZwCreateIRTimer,"ZwCreateIRTimer"),DECL_SYSCALL(_NtCreateIoCompletion,"NtCreateIoCompletion"),DECL_SYSCALL(_ZwCreateJobObject,"ZwCreateJobObject"),DECL_SYSCALL(_ZwCreateJobSet,"ZwCreateJobSet"),DECL_SYSCALL(_ZwCreateKeyTransacted,"ZwCreateKeyTransacted"),DECL_SYSCALL(_ZwCreateKeyedEvent,"ZwCreateKeyedEvent"),DECL_SYSCALL(_NtCreateLowBoxToken,"NtCreateLowBoxToken"),DECL_SYSCALL(_ZwCreateMailslotFile,"ZwCreateMailslotFile"),DECL_SYSCALL(_NtCreateMutant,"NtCreateMutant"),DECL_SYSCALL(_NtCreateNamedPipeFile,"NtCreateNamedPipeFile"),DECL_SYSCALL(_NtCreatePagingFile,"NtCreatePagingFile"),DECL_SYSCALL(_ZwCreatePartition,"ZwCreatePartition"),DECL_SYSCALL(_ZwCreatePort,"ZwCreatePort"),DECL_SYSCALL(_ZwCreatePrivateNamespace,"ZwCreatePrivateNamespace"),DECL_SYSCALL(_NtCreateProcess,"NtCreateProcess"),DECL_SYSCALL(_ZwCreateProfile,"ZwCreateProfile"),DECL_SYSCALL(_ZwCreateProfileEx,"ZwCreateProfileEx"),DECL_SYSCALL(_ZwCreateRegistryTransaction,"ZwCreateRegistryTransaction"),DECL_SYSCALL(_NtCreateResourceManager,"NtCreateResourceManager"),DECL_SYSCALL(_NtCreateSectionEx,"NtCreateSectionEx"),DECL_SYSCALL(_NtCreateSemaphore,"NtCreateSemaphore"),DECL_SYSCALL(_ZwCreateSymbolicLinkObject,"ZwCreateSymbolicLinkObject"),DECL_SYSCALL(_NtCreateThreadEx,"NtCreateThreadEx"),DECL_SYSCALL(_ZwCreateTimer,"ZwCreateTimer"),DECL_SYSCALL(_NtCreateTimer2,"NtCreateTimer2"),DECL_SYSCALL(_NtCreateToken,"NtCreateToken"),DECL_SYSCALL(_NtCreateTokenEx,"NtCreateTokenEx"),DECL_SYSCALL(_NtCreateTransaction,"NtCreateTransaction"),DECL_SYSCALL(_ZwCreateTransactionManager,"ZwCreateTransactionManager"),DECL_SYSCALL(_NtCreateUserProcess,"NtCreateUserProcess"),DECL_SYSCALL(_NtCreateWaitCompletionPacket,"NtCreateWaitCompletionPacket"),DECL_SYSCALL(_NtCreateWaitablePort,"NtCreateWaitablePort"),DECL_SYSCALL(_NtCreateWnfStateName,"NtCreateWnfStateName"),DECL_SYSCALL(_NtCreateWorkerFactory,"NtCreateWorkerFactory"),DECL_SYSCALL(_NtDebugActiveProcess,"NtDebugActiveProcess"),DECL_SYSCALL(_NtDebugContinue,"NtDebugContinue"),DECL_SYSCALL(_NtDeleteAtom,"NtDeleteAtom"),DECL_SYSCALL(_NtDeleteBootEntry,"NtDeleteBootEntry"),DECL_SYSCALL(_ZwDeleteDriverEntry,"ZwDeleteDriverEntry"),DECL_SYSCALL(_NtDeleteFile,"NtDeleteFile"),DECL_SYSCALL(_NtDeleteKey,"NtDeleteKey"),DECL_SYSCALL(_ZwDeleteObjectAuditAlarm,"ZwDeleteObjectAuditAlarm"),DECL_SYSCALL(_NtDeletePrivateNamespace,"NtDeletePrivateNamespace"),DECL_SYSCALL(_ZwDeleteValueKey,"ZwDeleteValueKey"),DECL_SYSCALL(_NtDeleteWnfStateData,"NtDeleteWnfStateData"),DECL_SYSCALL(_ZwDeleteWnfStateName,"ZwDeleteWnfStateName"),DECL_SYSCALL(_NtDisableLastKnownGood,"NtDisableLastKnownGood"),DECL_SYSCALL(_ZwDisplayString,"ZwDisplayString"),DECL_SYSCALL(_NtDrawText,"NtDrawText"),DECL_SYSCALL(_NtEnableLastKnownGood,"NtEnableLastKnownGood"),DECL_SYSCALL(_NtEnumerateBootEntries,"NtEnumerateBootEntries"),DECL_SYSCALL(_ZwEnumerateDriverEntries,"ZwEnumerateDriverEntries"),DECL_SYSCALL(_NtEnumerateSystemEnvironmentValuesEx,"NtEnumerateSystemEnvironmentValuesEx"),DECL_SYSCALL(_NtEnumerateTransactionObject,"NtEnumerateTransactionObject"),DECL_SYSCALL(_NtExtendSection,"NtExtendSection"),DECL_SYSCALL(_ZwFilterBootOption,"ZwFilterBootOption"),DECL_SYSCALL(_ZwFilterToken,"ZwFilterToken"),DECL_SYSCALL(_ZwFilterTokenEx,"ZwFilterTokenEx"),DECL_SYSCALL(_ZwFlushBuffersFileEx,"ZwFlushBuffersFileEx"),DECL_SYSCALL(_NtFlushInstallUILanguage,"NtFlushInstallUILanguage"),DECL_SYSCALL(_NtFlushInstructionCache,"NtFlushInstructionCache"),DECL_SYSCALL(_ZwFlushKey,"ZwFlushKey"),DECL_SYSCALL(_ZwFlushProcessWriteBuffers,"ZwFlushProcessWriteBuffers"),DECL_SYSCALL(_ZwFlushVirtualMemory,"ZwFlushVirtualMemory"),DECL_SYSCALL(_NtFlushWriteBuffer,"NtFlushWriteBuffer"),DECL_SYSCALL(_ZwFreeUserPhysicalPages,"ZwFreeUserPhysicalPages"),DECL_SYSCALL(_NtFreezeRegistry,"NtFreezeRegistry"),DECL_SYSCALL(_ZwFreezeTransactions,"ZwFreezeTransactions"),DECL_SYSCALL(_NtGetCachedSigningLevel,"NtGetCachedSigningLevel"),DECL_SYSCALL(_NtGetCompleteWnfStateSubscription,"NtGetCompleteWnfStateSubscription"),DECL_SYSCALL(_ZwGetContextThread,"ZwGetContextThread"),DECL_SYSCALL(_ZwGetCurrentProcessorNumber,"ZwGetCurrentProcessorNumber"),DECL_SYSCALL(_NtGetCurrentProcessorNumberEx,"NtGetCurrentProcessorNumberEx"),DECL_SYSCALL(_ZwGetDevicePowerState,"ZwGetDevicePowerState"),DECL_SYSCALL(_ZwGetMUIRegistryInfo,"ZwGetMUIRegistryInfo"),DECL_SYSCALL(_NtGetNextProcess,"NtGetNextProcess"),DECL_SYSCALL(_ZwGetNextThread,"ZwGetNextThread"),DECL_SYSCALL(_ZwGetNlsSectionPtr,"ZwGetNlsSectionPtr"),DECL_SYSCALL(_NtGetNotificationResourceManager,"NtGetNotificationResourceManager"),DECL_SYSCALL(_ZwGetWriteWatch,"ZwGetWriteWatch"),DECL_SYSCALL(_ZwImpersonateAnonymousToken,"ZwImpersonateAnonymousToken"),DECL_SYSCALL(_ZwImpersonateThread,"ZwImpersonateThread"),DECL_SYSCALL(_ZwInitializeEnclave,"ZwInitializeEnclave"),DECL_SYSCALL(_NtInitializeNlsFiles,"NtInitializeNlsFiles"),DECL_SYSCALL(_NtInitializeRegistry,"NtInitializeRegistry"),DECL_SYSCALL(_NtInitiatePowerAction,"NtInitiatePowerAction"),DECL_SYSCALL(_ZwIsSystemResumeAutomatic,"ZwIsSystemResumeAutomatic"),DECL_SYSCALL(_NtIsUILanguageComitted,"NtIsUILanguageComitted"),DECL_SYSCALL(_NtListenPort,"NtListenPort"),DECL_SYSCALL(_ZwLoadDriver,"ZwLoadDriver"),DECL_SYSCALL(_NtLoadEnclaveData,"NtLoadEnclaveData"),DECL_SYSCALL(_NtLoadKey,"NtLoadKey"),DECL_SYSCALL(_ZwLoadKey2,"ZwLoadKey2"),DECL_SYSCALL(_NtLoadKeyEx,"NtLoadKeyEx"),DECL_SYSCALL(_ZwLockFile,"ZwLockFile"),DECL_SYSCALL(_NtLockProductActivationKeys,"NtLockProductActivationKeys"),DECL_SYSCALL(_ZwLockRegistryKey,"ZwLockRegistryKey"),DECL_SYSCALL(_NtLockVirtualMemory,"NtLockVirtualMemory"),DECL_SYSCALL(_NtMakePermanentObject,"NtMakePermanentObject"),DECL_SYSCALL(_NtMakeTemporaryObject,"NtMakeTemporaryObject"),DECL_SYSCALL(_ZwManageHotPatch,"ZwManageHotPatch"),DECL_SYSCALL(_ZwManagePartition,"ZwManagePartition"),DECL_SYSCALL(_NtMapCMFModule,"NtMapCMFModule"),DECL_SYSCALL(_NtMapUserPhysicalPages,"NtMapUserPhysicalPages"),DECL_SYSCALL(_ZwMapViewOfSectionEx,"ZwMapViewOfSectionEx"),DECL_SYSCALL(_NtModifyBootEntry,"NtModifyBootEntry"),DECL_SYSCALL(_ZwModifyDriverEntry,"ZwModifyDriverEntry"),DECL_SYSCALL(_NtNotifyChangeDirectoryFile,"NtNotifyChangeDirectoryFile"),DECL_SYSCALL(_NtNotifyChangeDirectoryFileEx,"NtNotifyChangeDirectoryFileEx"),DECL_SYSCALL(_ZwNotifyChangeKey,"ZwNotifyChangeKey"),DECL_SYSCALL(_NtNotifyChangeMultipleKeys,"NtNotifyChangeMultipleKeys"),DECL_SYSCALL(_NtNotifyChangeSession,"NtNotifyChangeSession"),DECL_SYSCALL(_NtOpenEnlistment,"NtOpenEnlistment"),DECL_SYSCALL(_ZwOpenEventPair,"ZwOpenEventPair"),DECL_SYSCALL(_NtOpenIoCompletion,"NtOpenIoCompletion"),DECL_SYSCALL(_NtOpenJobObject,"NtOpenJobObject"),DECL_SYSCALL(_NtOpenKeyEx,"NtOpenKeyEx"),DECL_SYSCALL(_ZwOpenKeyTransacted,"ZwOpenKeyTransacted"),DECL_SYSCALL(_ZwOpenKeyTransactedEx,"ZwOpenKeyTransactedEx"),DECL_SYSCALL(_NtOpenKeyedEvent,"NtOpenKeyedEvent"),DECL_SYSCALL(_NtOpenMutant,"NtOpenMutant"),DECL_SYSCALL(_ZwOpenObjectAuditAlarm,"ZwOpenObjectAuditAlarm"),DECL_SYSCALL(_NtOpenPartition,"NtOpenPartition"),DECL_SYSCALL(_NtOpenPrivateNamespace,"NtOpenPrivateNamespace"),DECL_SYSCALL(_NtOpenProcessToken,"NtOpenProcessToken"),DECL_SYSCALL(_ZwOpenRegistryTransaction,"ZwOpenRegistryTransaction"),DECL_SYSCALL(_NtOpenResourceManager,"NtOpenResourceManager"),DECL_SYSCALL(_ZwOpenSemaphore,"ZwOpenSemaphore"),DECL_SYSCALL(_ZwOpenSession,"ZwOpenSession"),DECL_SYSCALL(_ZwOpenSymbolicLinkObject,"ZwOpenSymbolicLinkObject"),DECL_SYSCALL(_ZwOpenThread,"ZwOpenThread"),DECL_SYSCALL(_ZwOpenTimer,"ZwOpenTimer"),DECL_SYSCALL(_ZwOpenTransaction,"ZwOpenTransaction"),DECL_SYSCALL(_NtOpenTransactionManager,"NtOpenTransactionManager"),DECL_SYSCALL(_ZwPlugPlayControl,"ZwPlugPlayControl"),DECL_SYSCALL(_ZwPrePrepareComplete,"ZwPrePrepareComplete"),DECL_SYSCALL(_NtPrePrepareEnlistment,"NtPrePrepareEnlistment"),DECL_SYSCALL(_ZwPrepareComplete,"ZwPrepareComplete"),DECL_SYSCALL(_ZwPrepareEnlistment,"ZwPrepareEnlistment"),DECL_SYSCALL(_ZwPrivilegeCheck,"ZwPrivilegeCheck"),DECL_SYSCALL(_NtPrivilegeObjectAuditAlarm,"NtPrivilegeObjectAuditAlarm"),DECL_SYSCALL(_ZwPrivilegedServiceAuditAlarm,"ZwPrivilegedServiceAuditAlarm"),DECL_SYSCALL(_NtPropagationComplete,"NtPropagationComplete"),DECL_SYSCALL(_NtPropagationFailed,"NtPropagationFailed"),DECL_SYSCALL(_ZwPulseEvent,"ZwPulseEvent"),DECL_SYSCALL(_NtQueryAuxiliaryCounterFrequency,"NtQueryAuxiliaryCounterFrequency"),DECL_SYSCALL(_ZwQueryBootEntryOrder,"ZwQueryBootEntryOrder"),DECL_SYSCALL(_NtQueryBootOptions,"NtQueryBootOptions"),DECL_SYSCALL(_ZwQueryDebugFilterState,"ZwQueryDebugFilterState"),DECL_SYSCALL(_NtQueryDirectoryFileEx,"NtQueryDirectoryFileEx"),DECL_SYSCALL(_ZwQueryDirectoryObject,"ZwQueryDirectoryObject"),DECL_SYSCALL(_NtQueryDriverEntryOrder,"NtQueryDriverEntryOrder"),DECL_SYSCALL(_ZwQueryEaFile,"ZwQueryEaFile"),DECL_SYSCALL(_NtQueryFullAttributesFile,"NtQueryFullAttributesFile"),DECL_SYSCALL(_NtQueryInformationAtom,"NtQueryInformationAtom"),DECL_SYSCALL(_NtQueryInformationByName,"NtQueryInformationByName"),DECL_SYSCALL(_NtQueryInformationEnlistment,"NtQueryInformationEnlistment"),DECL_SYSCALL(_NtQueryInformationJobObject,"NtQueryInformationJobObject"),DECL_SYSCALL(_NtQueryInformationPort,"NtQueryInformationPort"),DECL_SYSCALL(_NtQueryInformationResourceManager,"NtQueryInformationResourceManager"),DECL_SYSCALL(_ZwQueryInformationTransaction,"ZwQueryInformationTransaction"),DECL_SYSCALL(_NtQueryInformationTransactionManager,"NtQueryInformationTransactionManager"),DECL_SYSCALL(_ZwQueryInformationWorkerFactory,"ZwQueryInformationWorkerFactory"),DECL_SYSCALL(_NtQueryInstallUILanguage,"NtQueryInstallUILanguage"),DECL_SYSCALL(_NtQueryIntervalProfile,"NtQueryIntervalProfile"),DECL_SYSCALL(_NtQueryIoCompletion,"NtQueryIoCompletion"),DECL_SYSCALL(_ZwQueryLicenseValue,"ZwQueryLicenseValue"),DECL_SYSCALL(_ZwQueryMultipleValueKey,"ZwQueryMultipleValueKey"),DECL_SYSCALL(_ZwQueryMutant,"ZwQueryMutant"),DECL_SYSCALL(_NtQueryOpenSubKeys,"NtQueryOpenSubKeys"),DECL_SYSCALL(_NtQueryOpenSubKeysEx,"NtQueryOpenSubKeysEx"),DECL_SYSCALL(_NtQueryPortInformationProcess,"NtQueryPortInformationProcess"),DECL_SYSCALL(_NtQueryQuotaInformationFile,"NtQueryQuotaInformationFile"),DECL_SYSCALL(_ZwQuerySecurityAttributesToken,"ZwQuerySecurityAttributesToken"),DECL_SYSCALL(_NtQuerySecurityObject,"NtQuerySecurityObject"),DECL_SYSCALL(_NtQuerySecurityPolicy,"NtQuerySecurityPolicy"),DECL_SYSCALL(_ZwQuerySemaphore,"ZwQuerySemaphore"),DECL_SYSCALL(_NtQuerySymbolicLinkObject,"NtQuerySymbolicLinkObject"),DECL_SYSCALL(_NtQuerySystemEnvironmentValue,"NtQuerySystemEnvironmentValue"),DECL_SYSCALL(_NtQuerySystemEnvironmentValueEx,"NtQuerySystemEnvironmentValueEx"),DECL_SYSCALL(_NtQuerySystemInformationEx,"NtQuerySystemInformationEx"),DECL_SYSCALL(_ZwQueryTimerResolution,"ZwQueryTimerResolution"),DECL_SYSCALL(_ZwQueryWnfStateData,"ZwQueryWnfStateData"),DECL_SYSCALL(_NtQueryWnfStateNameInformation,"NtQueryWnfStateNameInformation"),DECL_SYSCALL(_NtQueueApcThreadEx,"NtQueueApcThreadEx"),DECL_SYSCALL(_ZwRaiseException,"ZwRaiseException"),DECL_SYSCALL(_NtRaiseHardError,"NtRaiseHardError"),DECL_SYSCALL(_ZwReadOnlyEnlistment,"ZwReadOnlyEnlistment"),DECL_SYSCALL(_ZwRecoverEnlistment,"ZwRecoverEnlistment"),DECL_SYSCALL(_ZwRecoverResourceManager,"ZwRecoverResourceManager"),DECL_SYSCALL(_NtRecoverTransactionManager,"NtRecoverTransactionManager"),DECL_SYSCALL(_NtRegisterProtocolAddressInformation,"NtRegisterProtocolAddressInformation"),DECL_SYSCALL(_NtRegisterThreadTerminatePort,"NtRegisterThreadTerminatePort"),DECL_SYSCALL(_NtReleaseKeyedEvent,"NtReleaseKeyedEvent"),DECL_SYSCALL(_NtReleaseWorkerFactoryWorker,"NtReleaseWorkerFactoryWorker"),DECL_SYSCALL(_ZwRemoveIoCompletionEx,"ZwRemoveIoCompletionEx"),DECL_SYSCALL(_ZwRemoveProcessDebug,"ZwRemoveProcessDebug"),DECL_SYSCALL(_NtRenameKey,"NtRenameKey"),DECL_SYSCALL(_NtRenameTransactionManager,"NtRenameTransactionManager"),DECL_SYSCALL(_NtReplaceKey,"NtReplaceKey"),DECL_SYSCALL(_NtReplacePartitionUnit,"NtReplacePartitionUnit"),DECL_SYSCALL(_NtReplyWaitReplyPort,"NtReplyWaitReplyPort"),DECL_SYSCALL(_NtRequestPort,"NtRequestPort"),DECL_SYSCALL(_ZwResetEvent,"ZwResetEvent"),DECL_SYSCALL(_ZwResetWriteWatch,"ZwResetWriteWatch"),DECL_SYSCALL(_NtRestoreKey,"NtRestoreKey"),DECL_SYSCALL(_ZwResumeProcess,"ZwResumeProcess"),DECL_SYSCALL(_ZwRevertContainerImpersonation,"ZwRevertContainerImpersonation"),DECL_SYSCALL(_ZwRollbackComplete,"ZwRollbackComplete"),DECL_SYSCALL(_NtRollbackEnlistment,"NtRollbackEnlistment"),DECL_SYSCALL(_ZwRollbackRegistryTransaction,"ZwRollbackRegistryTransaction"),DECL_SYSCALL(_ZwRollbackTransaction,"ZwRollbackTransaction"),DECL_SYSCALL(_NtRollforwardTransactionManager,"NtRollforwardTransactionManager"),DECL_SYSCALL(_NtSaveKey,"NtSaveKey"),DECL_SYSCALL(_NtSaveKeyEx,"NtSaveKeyEx"),DECL_SYSCALL(_ZwSaveMergedKeys,"ZwSaveMergedKeys"),DECL_SYSCALL(_NtSecureConnectPort,"NtSecureConnectPort"),DECL_SYSCALL(_ZwSerializeBoot,"ZwSerializeBoot"),DECL_SYSCALL(_ZwSetBootEntryOrder,"ZwSetBootEntryOrder"),DECL_SYSCALL(_NtSetBootOptions,"NtSetBootOptions"),DECL_SYSCALL(_ZwSetCachedSigningLevel,"ZwSetCachedSigningLevel"),DECL_SYSCALL(_NtSetCachedSigningLevel2,"NtSetCachedSigningLevel2"),DECL_SYSCALL(_NtSetContextThread,"NtSetContextThread"),DECL_SYSCALL(_ZwSetDebugFilterState,"ZwSetDebugFilterState"),DECL_SYSCALL(_NtSetDefaultHardErrorPort,"NtSetDefaultHardErrorPort"),DECL_SYSCALL(_NtSetDefaultLocale,"NtSetDefaultLocale"),DECL_SYSCALL(_ZwSetDefaultUILanguage,"ZwSetDefaultUILanguage"),DECL_SYSCALL(_NtSetDriverEntryOrder,"NtSetDriverEntryOrder"),DECL_SYSCALL(_ZwSetEaFile,"ZwSetEaFile"),DECL_SYSCALL(_NtSetHighEventPair,"NtSetHighEventPair"),DECL_SYSCALL(_ZwSetHighWaitLowEventPair,"ZwSetHighWaitLowEventPair"),DECL_SYSCALL(_NtSetIRTimer,"NtSetIRTimer"),DECL_SYSCALL(_NtSetInformationDebugObject,"NtSetInformationDebugObject"),DECL_SYSCALL(_NtSetInformationEnlistment,"NtSetInformationEnlistment"),DECL_SYSCALL(_ZwSetInformationJobObject,"ZwSetInformationJobObject"),DECL_SYSCALL(_ZwSetInformationKey,"ZwSetInformationKey"),DECL_SYSCALL(_NtSetInformationResourceManager,"NtSetInformationResourceManager"),DECL_SYSCALL(_NtSetInformationSymbolicLink,"NtSetInformationSymbolicLink"),DECL_SYSCALL(_NtSetInformationToken,"NtSetInformationToken"),DECL_SYSCALL(_NtSetInformationTransaction,"NtSetInformationTransaction"),DECL_SYSCALL(_ZwSetInformationTransactionManager,"ZwSetInformationTransactionManager"),DECL_SYSCALL(_ZwSetInformationVirtualMemory,"ZwSetInformationVirtualMemory"),DECL_SYSCALL(_NtSetInformationWorkerFactory,"NtSetInformationWorkerFactory"),DECL_SYSCALL(_NtSetIntervalProfile,"NtSetIntervalProfile"),DECL_SYSCALL(_ZwSetIoCompletion,"ZwSetIoCompletion"),DECL_SYSCALL(_ZwSetIoCompletionEx,"ZwSetIoCompletionEx"),DECL_SYSCALL(_ZwSetLdtEntries,"ZwSetLdtEntries"),DECL_SYSCALL(_ZwSetLowEventPair,"ZwSetLowEventPair"),DECL_SYSCALL(_ZwSetLowWaitHighEventPair,"ZwSetLowWaitHighEventPair"),DECL_SYSCALL(_ZwSetQuotaInformationFile,"ZwSetQuotaInformationFile"),DECL_SYSCALL(_NtSetSecurityObject,"NtSetSecurityObject"),DECL_SYSCALL(_ZwSetSystemEnvironmentValue,"ZwSetSystemEnvironmentValue"),DECL_SYSCALL(_ZwSetSystemEnvironmentValueEx,"ZwSetSystemEnvironmentValueEx"),DECL_SYSCALL(_ZwSetSystemInformation,"ZwSetSystemInformation"),DECL_SYSCALL(_ZwSetSystemPowerState,"ZwSetSystemPowerState"),DECL_SYSCALL(_ZwSetSystemTime,"ZwSetSystemTime"),DECL_SYSCALL(_NtSetThreadExecutionState,"NtSetThreadExecutionState"),DECL_SYSCALL(_ZwSetTimer2,"ZwSetTimer2"),DECL_SYSCALL(_ZwSetTimerEx,"ZwSetTimerEx"),DECL_SYSCALL(_NtSetTimerResolution,"NtSetTimerResolution"),DECL_SYSCALL(_NtSetUuidSeed,"NtSetUuidSeed"),DECL_SYSCALL(_ZwSetVolumeInformationFile,"ZwSetVolumeInformationFile"),DECL_SYSCALL(_NtSetWnfProcessNotificationEvent,"NtSetWnfProcessNotificationEvent"),DECL_SYSCALL(_ZwShutdownSystem,"ZwShutdownSystem"),DECL_SYSCALL(_ZwShutdownWorkerFactory,"ZwShutdownWorkerFactory"),DECL_SYSCALL(_ZwSignalAndWaitForSingleObject,"ZwSignalAndWaitForSingleObject"),DECL_SYSCALL(_ZwSinglePhaseReject,"ZwSinglePhaseReject"),DECL_SYSCALL(_ZwStartProfile,"ZwStartProfile"),DECL_SYSCALL(_ZwStopProfile,"ZwStopProfile"),DECL_SYSCALL(_NtSubscribeWnfStateChange,"NtSubscribeWnfStateChange"),DECL_SYSCALL(_ZwSuspendProcess,"ZwSuspendProcess"),DECL_SYSCALL(_NtSuspendThread,"NtSuspendThread"),DECL_SYSCALL(_ZwSystemDebugControl,"ZwSystemDebugControl"),DECL_SYSCALL(_NtTerminateEnclave,"NtTerminateEnclave"),DECL_SYSCALL(_NtTerminateJobObject,"NtTerminateJobObject"),DECL_SYSCALL(_ZwTestAlert,"ZwTestAlert"),DECL_SYSCALL(_ZwThawRegistry,"ZwThawRegistry"),DECL_SYSCALL(_ZwThawTransactions,"ZwThawTransactions"),DECL_SYSCALL(_NtTraceControl,"NtTraceControl"),DECL_SYSCALL(_NtTranslateFilePath,"NtTranslateFilePath"),DECL_SYSCALL(_ZwUmsThreadYield,"ZwUmsThreadYield"),DECL_SYSCALL(_ZwUnloadDriver,"ZwUnloadDriver"),DECL_SYSCALL(_NtUnloadKey,"NtUnloadKey"),DECL_SYSCALL(_NtUnloadKey2,"NtUnloadKey2"),DECL_SYSCALL(_ZwUnloadKeyEx,"ZwUnloadKeyEx"),DECL_SYSCALL(_ZwUnlockFile,"ZwUnlockFile"),DECL_SYSCALL(_ZwUnlockVirtualMemory,"ZwUnlockVirtualMemory"),DECL_SYSCALL(_NtUnmapViewOfSectionEx,"NtUnmapViewOfSectionEx"),DECL_SYSCALL(_NtUnsubscribeWnfStateChange,"NtUnsubscribeWnfStateChange"),DECL_SYSCALL(_ZwUpdateWnfStateData,"ZwUpdateWnfStateData"),DECL_SYSCALL(_ZwVdmControl,"ZwVdmControl"),DECL_SYSCALL(_NtWaitForAlertByThreadId,"NtWaitForAlertByThreadId"),DECL_SYSCALL(_ZwWaitForDebugEvent,"ZwWaitForDebugEvent"),DECL_SYSCALL(_NtWaitForKeyedEvent,"NtWaitForKeyedEvent"),DECL_SYSCALL(_ZwWaitForWorkViaWorkerFactory,"ZwWaitForWorkViaWorkerFactory"),DECL_SYSCALL(_ZwWaitHighEventPair,"ZwWaitHighEventPair"),DECL_SYSCALL(_NtWaitLowEventPair,"NtWaitLowEventPair"),DECL_SYSCALL(_ZwLoadKey3,"ZwLoadKey3")
};

static const syscall_translator_t* find_syscall(const char* rvaname) {
	unsigned hcode = hash_string(rvaname);
#pragma clang loop unroll(disable)
	for(auto&& xlat : g_syscalls_translation) {
		if(xlat.hashcode == hcode) {
			return &xlat;
		}
	}
	return nullptr;
}

static unsigned g_translated_syscalls[sizeof(g_syscalls_translation) / sizeof(syscall_translator_t)] = {};

 template<unsigned char... values>
static bool is_seq_at(unsigned char* addr) {
	std::array<unsigned char, sizeof...(values)> vs {values...};
	for(unsigned i = 0; i < sizeof...(values);++i)	{
		if(addr[i]!=vs[i])
			return false;
	}
	return true;
}

static int extract_syscall(unsigned* start) {
	if(*start == 0xB8D18B4C) {
		unsigned char* pptr = (unsigned char*)(start+2);
#pragma clang loop unroll(disable)
		while(!is_seq_at<0xcd, 0x2e, 0xc3>(pptr)) {
		++pptr;
		if(pptr-(unsigned char*)start > 0x20)
			return -1;			
		}
		return start[1];
	}
	return -1;
}

static bool g_is_available = true;



void win_syscalls::initialize_syscall_table() {
	HANDLE ntdll = GetModuleHandleA("ntdll.dll");
	if(!ntdll) {
		g_is_available = false;
		return;
	}
	char* ntdllc = reinterpret_cast<char*>(ntdll);

	auto base = reinterpret_cast<IMAGE_DOS_HEADER*>(ntdll);
	auto winheader = reinterpret_cast<IMAGE_NT_HEADERS*>( ntdllc + base->e_lfanew);

	auto exports_ptr = ntdllc + winheader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress;

	auto exps  = reinterpret_cast<_IMAGE_EXPORT_DIRECTORY*>(exports_ptr);

	auto names = reinterpret_cast<unsigned*>(exps->AddressOfNames + ntdllc);
	auto funcs = reinterpret_cast<unsigned*>(exps->AddressOfFunctions + ntdllc);

	
	unsigned nfuncs = exps->NumberOfFunctions;
	unsigned nnames = exps->NumberOfNames;

	unsigned short* ords = reinterpret_cast<unsigned short*>(exps->AddressOfNameOrdinals + ntdllc);


	for(unsigned i = 0; i < nnames; ++i) {
		char* nam = names[i] + ntdllc;
		auto xlat = find_syscall(nam);
		if(xlat == nullptr)
			continue;
		unsigned ord = ords[i];

		auto funcptr = funcs[ord] + ntdllc;

		int swi = extract_syscall((unsigned int*)funcptr);
		g_translated_syscalls[xlat->swicode] = swi;//get_syscall_ptr_of_code(swi);
	}


}
//not available on linux under wine
bool win_syscalls::syscall_iface_available() {
	return g_is_available;
}

unsigned win_syscalls::get_syscall_code(unsigned swi) {
	return g_translated_syscalls[swi];
}

extern "C"
void perform_syscall_impl();

void* win_syscalls::get_syscall_func_ptr(unsigned untranslated_swi) {
	return reinterpret_cast<void*>((g_translated_syscalls[untranslated_swi] * 16 ) + reinterpret_cast<char*>(&perform_syscall_impl));
}

#endif