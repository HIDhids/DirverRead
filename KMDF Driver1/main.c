#include<ntifs.h>
#include "common.h"
#include "main.h"
#include "进程保护.h"
#include"过保护读写.h"

typedef struct _DEVICE_EXTENSION {

	PDEVICE_OBJECT       kbdDeviceObject;        //键盘类设备对象
	PDEVICE_OBJECT       mouDeviceObject;        //鼠标类设备对象
	MY_KEYBOARDCALLBACK  My_KbdCallback;         //KeyboardClassServiceCallback函数 
	MY_MOUSECALLBACK     My_MouCallback;         //MouseClassServiceCallback函数

}DEVICE_EXTENSION, * PDEVICE_EXTENSION;

struct
{
	PDEVICE_OBJECT KdbDeviceObject;
	MY_KEYBOARDCALLBACK KeyboardClassServiceCallback;
	PDEVICE_OBJECT MouDeviceObject;
	MY_MOUSECALLBACK MouseClassServiceCallback;
}g_KoMCallBack;


NTSTATUS CreateDevice(PDRIVER_OBJECT driver);
void DriverUnLoad(PDRIVER_OBJECT pDriver);

#define 读测试   CTL_CODE(FILE_DEVICE_UNKNOWN, 	0x803, 	METHOD_BUFFERED,FILE_ANY_ACCESS) //控制码测试
#define 写测试   CTL_CODE(FILE_DEVICE_UNKNOWN, 	0x804, 	METHOD_BUFFERED,FILE_ANY_ACCESS) //控制码测试
#define 读写测试   CTL_CODE(FILE_DEVICE_UNKNOWN, 	0x805, 	METHOD_BUFFERED,FILE_ANY_ACCESS) //控制码测试
#define IO_添加受保护的PID   CTL_CODE(FILE_DEVICE_UNKNOWN, 0x806, 	METHOD_BUFFERED,FILE_ANY_ACCESS) //控制码测试
#define IO_删除受保护的PID   CTL_CODE(FILE_DEVICE_UNKNOWN, 0x807, 	METHOD_BUFFERED,FILE_ANY_ACCESS) //控制码测试
#define  CTL_IO_通过PID读取数据   CTL_CODE(FILE_DEVICE_UNKNOWN, 0x80A, 	METHOD_BUFFERED,FILE_ANY_ACCESS) //控制码测试
#define  CTL_IO_通过PID读取数据2   CTL_CODE(FILE_DEVICE_UNKNOWN, 0x80B, 	METHOD_BUFFERED,FILE_ANY_ACCESS) //控制码测试
#define  IO_MOUSE   CTL_CODE(FILE_DEVICE_UNKNOWN, 0x80C, 	METHOD_BUFFERED,FILE_ANY_ACCESS) //控制码测试
//int ReadProcessMemoryForPid2(UINT32 dwPid, PVOID pBase, PVOID lpBuffer, UINT32 nSize);
NTSTATUS  IRP_ReadProcessMemory2(/*PDEVICE_OBJECT device_Object,*/ PIRP pirp)
{

	//	UNREFERENCED_PARAMETER(device_Object); //未使用的参数 禁止警告
	//DbgPrint("yjx:sys64 %s 行号=%d %s", __FUNCDNAME__, __LINE__);
	NTSTATUS ntStatus = STATUS_SUCCESS;
	PIO_STACK_LOCATION     irpStack = NULL;
	irpStack = IoGetCurrentIrpStackLocation(pirp);

#pragma pack(push)
#pragma pack(8)
	typedef struct TINPUT_BUF
	{
		UINT64 dwPid;//目标进程PID
		PVOID pBase; //目标进程地址
		UINT64 nSize;//要读取的长度

	}TINPUT_BUF;
#pragma pack(pop)
	//结构

	//PVOID BaseAddress = NULL; //返回地址

	//PEPROCESS selectedprocess = NULL;//目标进程	
	//SIZE_T RegionSize = 0;//分配大小


	TINPUT_BUF* bufInput = (TINPUT_BUF*)(pirp->AssociatedIrp.SystemBuffer);
	//读“TINPUT_BUF 输入缓冲区” 
	//写  UserBuferr  		&返回数据,//返回缓冲区 UserBuffer=&返回数据

	ReadProcessMemoryForPid2((UINT32)bufInput->dwPid, bufInput->pBase, bufInput/*保存读取的数据*/, (UINT32)bufInput->nSize);

	//ENDCODE:
	//pirp->IoStatus.Status = STATUS_SUCCESS;//
	////pirp->IoStatus.Information = 4;//返回给DeviceIoControl中的 倒数第二个参数lpBytesReturned
	//IoCompleteRequest(pirp, IO_NO_INCREMENT);//调用方已完成所有I/O请求处理操作 并且不增加优先级 
	if (irpStack) //
	{
		if (ntStatus == STATUS_SUCCESS)
		{ //成功则返回 缓冲区大小
			pirp->IoStatus.Information = irpStack->Parameters.DeviceIoControl.OutputBufferLength;//DeviceIoControl
		}
		else
		{ //失败则不返回
			pirp->IoStatus.Information = 0;
		}
		//完成请求
		IoCompleteRequest(pirp, IO_NO_INCREMENT);
	}

	pirp->IoStatus.Status = ntStatus;
	return ntStatus;
}


void IRP_IO_通过PID读取数据(PIRP pirp)
{
	PIO_STACK_LOCATION irpStack = IoGetCurrentIrpStackLocation(pirp); //获取应用层传来的参数
	UINT64* 缓冲区 = (UINT64*)(pirp->AssociatedIrp.SystemBuffer);
	KdPrint(("yjx:%s 行号%d\n", __FUNCDNAME__, __LINE__));
	if (缓冲区)
	{
		//
		//int*p = (int*)缓冲区;
		UINT32 PID = (UINT32)(UINT64)缓冲区[0]; //传入数据
		PVOID pBase = (PVOID)(UINT64)缓冲区[1]; //传入数据
		//PVOID lpBuffer = (PVOID)(UINT64)缓冲区[2]; //传入数据
		UINT32 nSize = (UINT32)(UINT64)缓冲区[3]; //传入数据


		UINT32 ReadSize = ReadProcessMemoryForPid(PID, pBase, 缓冲区, nSize);

		ReadSize;
		pirp->IoStatus.Status = STATUS_SUCCESS;
		pirp->IoStatus.Information = nSize;//返回给DeviceIoControl中的 倒数第二个参数lpBytesReturned
		IoCompleteRequest(pirp, IO_NO_INCREMENT);//调用方已完成所有I/O请求处理操作 并且不增加优先级 
	}
	irpStack;
}

void IRP_IO_写测试(PIRP pirp)
{
	(pirp);

	//取出缓冲区数据 写测试 803
	//PIO_STACK_LOCATION irpStack = IoGetCurrentIrpStackLocation(pirp); //获取应用层传来的参数

}
void IRP_IO_读测试(PIRP pirp)
{
	KdPrint(("lkzs:IRP_IO_读测试 行号=%d", __LINE__));
	char* 缓冲区 = (char*)pirp->AssociatedIrp.SystemBuffer;
	//0x804
	//PIO_STACK_LOCATION irpStack = IoGetCurrentIrpStackLocation(pirp); //获取应用层传来的参数
	//向缓冲区写入
	char 返回字符串[] = "lkzs:SYSR0返回的字符串123456";
	ULONG 长度 = sizeof(返回字符串);
	memcpy_s(缓冲区, 长度, 返回字符串, 长度);

	KdPrint(("lkzs:IRP_IO_读测试  长度=%d 行号=%d", 长度, __LINE__));

	pirp->IoStatus.Status = STATUS_SUCCESS;
	pirp->IoStatus.Information = 长度;//返回给DeviceIoControl中的 倒数第二个参数lpBytesReturned
	IoCompleteRequest(pirp, IO_NO_INCREMENT);//调用方已完成所有I/O请求处理操作 并且不增加优先级 
	KdPrint(("lkzs:离开派遣函数"));

}

void IRP_IO_读写测试(PIRP pirp)
{
	int* 缓冲区 = (int*)pirp->AssociatedIrp.SystemBuffer;
	if (缓冲区)
	{
		int* p = 缓冲区;
		int 结果 = p[0] + p[1] + p[2];
		KdPrint(("lkzs: 结果=%d  (%d  %d  %d)", 结果, p[0], p[1], p[2]));
		*(int*)p = 结果;
		pirp->IoStatus.Status = STATUS_SUCCESS;
		pirp->IoStatus.Information = sizeof(int);//返回给DeviceIoControl中的 倒数第二个参数lpBytesReturned
		IoCompleteRequest(pirp, IO_NO_INCREMENT);//调用方已完成所有I/O请求处理操作 并且不增加优先级 

	}
}

void IRP_IO_添加受保护PID(PIRP pirp)
{
	PIO_STACK_LOCATION irpStack = IoGetCurrentIrpStackLocation(pirp); //获取应用层传来的参数
	int* 缓冲区 = (int*)pirp->AssociatedIrp.SystemBuffer;
	if (缓冲区)
	{
		//
		UINT32* pPID = (UINT32*)缓冲区;

		UINT32 pid = pPID[0];
		添加受保护的PID(pid);
		pirp->IoStatus.Status = STATUS_SUCCESS;
		pirp->IoStatus.Information = sizeof(int);//返回给DeviceIoControl中的 倒数第二个参数lpBytesReturned
		IoCompleteRequest(pirp, IO_NO_INCREMENT);//调用方已完成所有I/O请求处理操作 并且不增加优先级 
	}
	irpStack;
}

void IRP_IO_删除受保护PID(PIRP pirp)
{
	PIO_STACK_LOCATION irpStack = IoGetCurrentIrpStackLocation(pirp); //获取应用层传来的参数
	int* 缓冲区 = (int*)pirp->AssociatedIrp.SystemBuffer;
	if (缓冲区)
	{
		//
		UINT32* pPID = (UINT32*)缓冲区;

		UINT32 pid = pPID[0];
		删除受保护的PID(pid);
		pirp->IoStatus.Status = STATUS_SUCCESS;
		pirp->IoStatus.Information = sizeof(int);//返回给DeviceIoControl中的 倒数第二个参数lpBytesReturned
		IoCompleteRequest(pirp, IO_NO_INCREMENT);//调用方已完成所有I/O请求处理操作 并且不增加优先级 
	}
	irpStack;
}
NTSTATUS IRP_CALL(PDEVICE_OBJECT device, PIRP pirp)
{
	UNREFERENCED_PARAMETER(device); //等价于 (device);
	//aaaabbb(device);
	//device;
	PMOUSE_INPUT_DATA 	 MouseInputDataStart, MouseInputDataEnd;

	//KdPrint(("lkzs:进入派遣函数"));
	PIO_STACK_LOCATION irpStackL;
	ULONG 				 InputDataConsumed;
	PVOID				 ioBuffer;
	//	ULONG CtlCode;
	//	ULONG InputBuffLength;

	irpStackL = IoGetCurrentIrpStackLocation(pirp); //获取应用层传来的参数

	switch (irpStackL->MajorFunction)
	{
		case IRP_MJ_DEVICE_CONTROL: //DeviceIoControl
		{
			UINT32 控制码 = irpStackL->Parameters.DeviceIoControl.IoControlCode;
			switch (控制码)
			{
			case CTL_IO_通过PID读取数据:
			{
				IRP_IO_通过PID读取数据(pirp); //IRP_IO_通过PID读取数据
				return STATUS_SUCCESS;
				break;
			}
			case CTL_IO_通过PID读取数据2:
			{
				return IRP_ReadProcessMemory2(pirp);
				break;
			}
			case IO_MOUSE:
			{
				//PVOID ioBuffer;
				ioBuffer = pirp->AssociatedIrp.SystemBuffer;
				if (ioBuffer)
				{
					MOUSE_INPUT_DATA mid = *(PMOUSE_INPUT_DATA)ioBuffer;
					MouseInputDataStart = &mid;
					MouseInputDataEnd = MouseInputDataStart + 1;

					//deviceExtension->My_MouCallback(deviceExtension->mouDeviceObject,
					g_KoMCallBack.MouseClassServiceCallback(g_KoMCallBack.MouDeviceObject,
						MouseInputDataStart,
						MouseInputDataEnd,
						&InputDataConsumed);

					pirp->IoStatus.Status = STATUS_SUCCESS;
					//pirp->IoStatus.Information = nSize;//返回给DeviceIoControl中的 倒数第二个参数lpBytesReturned
					IoCompleteRequest(pirp, IO_NO_INCREMENT);//调用方已完成所有I/O请求处理操作 并且不增加优先级 
					return STATUS_SUCCESS;
				}
				break;
			}

				
			}


			//if (控制码 == 读测试)
			//{
			//	IRP_IO_读测试(pirp);
			//	return STATUS_SUCCESS;
			//}
			//else if (控制码 == 写测试)
			//{
			//	char* 缓冲区 = (char*)pirp->AssociatedIrp.SystemBuffer;
			//	缓冲区;
			//	//KdPrint(("R0 yjx:用户层调用了 DeviceIoControl 控制码=%X", 控制码));
			//	KdPrint(("R0 lkzs:用户层调用了 DeviceIoControl 控制码=%X 缓冲区=%s", 控制码, 缓冲区));
			//}
			//else if (控制码 == 读写测试)
			//{
			//	IRP_IO_读写测试(pirp);
			//	return STATUS_SUCCESS;

			//}
			//else if (控制码 == IO_添加受保护的PID)
			//{
			//	IRP_IO_添加受保护PID(pirp);
			//	//添加pid
			//	return STATUS_SUCCESS;
			//}
			//else if (控制码 == IO_删除受保护的PID)
			//{
			//	//删除pid
			//	IRP_IO_删除受保护PID(pirp);
			//	return STATUS_SUCCESS;
			//}
			//else if (CTL_IO_通过PID读取数据 == 控制码)
			//{
			//	IRP_IO_通过PID读取数据(pirp); //IRP_IO_通过PID读取数据
			//	return STATUS_SUCCESS;
			//}
			//else if (CTL_IO_通过PID读取数据2 == 控制码)
			//{
			//	return IRP_ReadProcessMemory2(pirp);
			//}
			break;
		}
	case IRP_MJ_CREATE: //CreateFile
	{
		KdPrint(("lkzs:用户层调用了 CreateFile"));
		break;
	}
	case IRP_MJ_CLOSE: //CloseHandle
	{
		KdPrint(("lkzs:用户层调用了 CloseHandle"));
		break;
	}
	//case IRP_MJ_READ:
	//{
	//	//char* 指针1 = (char*)pirp->AssociatedIrp.SystemBuffer;
	//	//char* 指针2 = (char*)pirp->MdlAddress; //
	//	char* 指针3 = (char*)pirp->UserBuffer;

	//	//KdPrint(("lkzs:SYS IRP_MJ_READ (%p,%p,%p)", 指针1, 指针2, 指针3));
	//	char 返回字符串[] = "SYS IRP_MJ_READ返回的字符串";
	//	ULONG 长度 = sizeof(返回字符串);
	//	if (指针3 && irpStackL->Parameters.Read.Length >= 长度)
	//	{
	//		//irpStackL->Parameters.Read.Length;

	//		RtlCopyMemory(指针3, 返回字符串, 长度);
	//	}
	//	pirp->IoStatus.Status = STATUS_SUCCESS;
	//	pirp->IoStatus.Information = 长度;//strlen // 对应 ReadFile第4个参数 len
	//	IoCompleteRequest(pirp, IO_NO_INCREMENT);//调用方已完成所有I/O请求处理操作 并且不增加优先级 
	//	KdPrint(("yjx IRP_MJ_READ 离开派遣函数"));
	//	return STATUS_SUCCESS;  //0 返回成功
	//	break;
	//}
	//case IRP_MJ_WRITE:
	//{
	//	//char* 指针1 = (char*)pirp->AssociatedIrp.SystemBuffer;
	//	//char* 指针2 = (char*)pirp->MdlAddress; //
	//	//char* 指针3 = (char*)pirp->UserBuffer;

	//	//KdPrint(("lkzs:SYS IRP_MJ_WRITE (%p,%p,%p)", 指针1, 指针2, 指针3));
	//	//KdPrint(("lkzs:SYS IRP_MJ_WRITE (%s,%s,%s)", 指针1, 指针2, 指针3));
	//	break;
	//}
	}

	pirp->IoStatus.Status = STATUS_SUCCESS;
	pirp->IoStatus.Information = 4;//返回给DeviceIoControl中的 倒数第二个参数lpBytesReturned
	IoCompleteRequest(pirp, IO_NO_INCREMENT);//调用方已完成所有I/O请求处理操作 并且不增加优先级 
	KdPrint(("lkzs:离开派遣函数"));
	return STATUS_SUCCESS;  //0 返回成功
}
void UnLoad(PDRIVER_OBJECT Driver)
{
	(Driver);
	KdPrint(("lkzs:进入卸载历程 Driver=%p\n", Driver));
	//卸载内存保护();
	DriverUnLoad(Driver);


}


typedef struct _KLDR_DATA_TABLE_ENTRY
{
	LIST_ENTRY listEntry;
	ULONG64 __Undefined1;
	ULONG64 __Undefined2;
	ULONG64 __Undefined3;
	ULONG64 NonPagedDebugInfo;
	ULONG64 DllBase;
	ULONG64 EntryPoint;
	ULONG SizeOfImage;
	UNICODE_STRING path;
	UNICODE_STRING name;
	ULONG   Flags;
	USHORT  LoadCount;
	USHORT  __Undefined5;
	ULONG64 __Undefined6;
	ULONG   CheckSum;
	ULONG   __padding1;
	ULONG   TimeDateStamp;
	ULONG   __padding2;
} KLDR_DATA_TABLE_ENTRY, * PKLDR_DATA_TABLE_ENTRY;



NTSTATUS SearchServiceFromMouExt(PDRIVER_OBJECT MouDriverObject, PDEVICE_OBJECT pPortDev)
{
	PDEVICE_OBJECT pTargetDeviceObject = NULL;
	UCHAR* DeviceExt;
	int i = 0;
	NTSTATUS status;
	PVOID KbdDriverStart;
	ULONG KbdDriverSize = 0;
	PDEVICE_OBJECT  pTmpDev;
	UNICODE_STRING  kbdDriName;

	KbdDriverStart = MouDriverObject->DriverStart;
	KbdDriverSize = MouDriverObject->DriverSize;

	status = STATUS_UNSUCCESSFUL;

	RtlInitUnicodeString(&kbdDriName, L"\\Driver\\mouclass");
	pTmpDev = pPortDev;
	while (pTmpDev->AttachedDevice != NULL)
	{
		KdPrint(("Att:  0x%x", pTmpDev->AttachedDevice));
		KdPrint(("Dri Name : %wZ", &pTmpDev->AttachedDevice->DriverObject->DriverName));
		if (RtlCompareUnicodeString(&pTmpDev->AttachedDevice->DriverObject->DriverName,
			&kbdDriName, TRUE) == 0)
		{
			KdPrint(("Find Object Device: "));
			break;
		}
		pTmpDev = pTmpDev->AttachedDevice;
	}
	if (pTmpDev->AttachedDevice == NULL)
	{
		return status;
	}
	pTargetDeviceObject = MouDriverObject->DeviceObject;
	while (pTargetDeviceObject)
	{
		if (pTmpDev->AttachedDevice != pTargetDeviceObject)
		{
			pTargetDeviceObject = pTargetDeviceObject->NextDevice;
			continue;
		}
		DeviceExt = (UCHAR*)pTmpDev->DeviceExtension;
		g_KoMCallBack.MouDeviceObject = NULL;
		//遍历我们先找到的端口驱动的设备扩展的每一个指针  
		for (i = 0; i < 4096; i++, DeviceExt++)
		{
			PVOID tmp;
			if (!MmIsAddressValid(DeviceExt))
			{
				break;
			}
			//找到后会填写到这个全局变量中，这里检查是否已经填好了  
			//如果已经填好了就不用继续找了，可以直接退出  
			if (g_KoMCallBack.MouDeviceObject && g_KoMCallBack.MouseClassServiceCallback)
			{
				status = STATUS_SUCCESS;
				break;
			}
			//在端口驱动的设备扩展里，找到了类驱动设备对象，填好类驱动设备对象后继续  
			tmp = *(PVOID*)DeviceExt;
			if (tmp == pTargetDeviceObject)
			{
				g_KoMCallBack.MouDeviceObject = pTargetDeviceObject;
				continue;
			}

			//如果在设备扩展中找到一个地址位于KbdClass这个驱动中，就可以认为，这就是我们要找的回调函数  
			if ((tmp > KbdDriverStart) && (tmp < (UCHAR*)KbdDriverStart + KbdDriverSize) &&
				(MmIsAddressValid(tmp)))
			{
				//将这个回调函数记录下来  
				g_KoMCallBack.MouseClassServiceCallback = (MY_MOUSECALLBACK)tmp;
				//g_KoMCallBack.MouSerCallAddr = (PVOID *)DeviceExt;
				status = STATUS_SUCCESS;
			}
		}
		if (status == STATUS_SUCCESS)
		{
			break;
		}
		//换成下一个设备，继续遍历  
		pTargetDeviceObject = pTargetDeviceObject->NextDevice;
	}
	return status;
}



NTSTATUS  SearchMouServiceCallBack(IN PDRIVER_OBJECT DriverObject)
{
	//定义用到的一组全局变量，这些变量大多数是顾名思义的  
	NTSTATUS status = STATUS_SUCCESS;
	UNICODE_STRING uniNtNameString;
	PDEVICE_OBJECT pTargetDeviceObject = NULL;
	PDRIVER_OBJECT KbdDriverObject = NULL;
	PDRIVER_OBJECT KbdhidDriverObject = NULL;
	PDRIVER_OBJECT Kbd8042DriverObject = NULL;
	PDRIVER_OBJECT UsingDriverObject = NULL;
	PDEVICE_OBJECT UsingDeviceObject = NULL;

	PVOID UsingDeviceExt = NULL;

	//这里的代码用来打开USB键盘端口驱动的驱动对象  
	RtlInitUnicodeString(&uniNtNameString, L"\\Driver\\mouhid");
	status = ObReferenceObjectByName(&uniNtNameString,
		OBJ_CASE_INSENSITIVE, NULL, 0,
		*IoDriverObjectType,
		KernelMode,
		NULL,
		(PVOID*)&KbdhidDriverObject);
	if (!NT_SUCCESS(status))
	{
		KdPrint(("Couldn't get the USB Mouse Object\n"));
	}
	else
	{
		ObDereferenceObject(KbdhidDriverObject);
		KdPrint(("get the USB Mouse Object\n"));
	}
	//打开PS/2键盘的驱动对象  
	RtlInitUnicodeString(&uniNtNameString, L"\\Driver\\i8042prt");
	status = ObReferenceObjectByName(&uniNtNameString,
		OBJ_CASE_INSENSITIVE,
		NULL, 0,
		*IoDriverObjectType,
		KernelMode,
		NULL,
		(PVOID*)&Kbd8042DriverObject);
	if (!NT_SUCCESS(status))
	{
		KdPrint(("Couldn't get the PS/2 Mouse Object %08x\n", status));
	}
	else
	{
		ObDereferenceObject(Kbd8042DriverObject);
		KdPrint(("get the PS/2 Mouse Object\n"));
	}
	//如果两个设备都没有找到  
	if (!Kbd8042DriverObject && !KbdhidDriverObject)
	{
		return STATUS_SUCCESS;
	}
	//如果USB键盘和PS/2键盘同时存在，使用USB鼠标
	if (KbdhidDriverObject)
	{
		UsingDriverObject = KbdhidDriverObject;
	}
	else
	{
		UsingDriverObject = Kbd8042DriverObject;
	}
	RtlInitUnicodeString(&uniNtNameString, L"\\Driver\\mouclass");
	status = ObReferenceObjectByName(&uniNtNameString,
		OBJ_CASE_INSENSITIVE, NULL,
		0,
		*IoDriverObjectType,
		KernelMode,
		NULL,
		(PVOID*)&KbdDriverObject);
	if (!NT_SUCCESS(status))
	{
		//如果没有成功，直接返回即可  
		KdPrint(("MyAttach: Coundn't get the Mouse driver Object\n"));
		return STATUS_UNSUCCESSFUL;
	}
	else
	{
		ObDereferenceObject(KbdDriverObject);
	}
	//遍历KbdDriverObject下的设备对象 
	UsingDeviceObject = UsingDriverObject->DeviceObject;
	while (UsingDeviceObject)
	{
		status = SearchServiceFromMouExt(KbdDriverObject, UsingDeviceObject);
		if (status == STATUS_SUCCESS)
		{
			break;
		}
		UsingDeviceObject = UsingDeviceObject->NextDevice;
	}
	if (g_KoMCallBack.MouDeviceObject && g_KoMCallBack.MouseClassServiceCallback)
	{
		KdPrint(("Find MouseClassServiceCallback\n"));
	}
	return status;
}





NTSTATUS
DriverEntry(
	_In_ PDRIVER_OBJECT  DriverObject,
	_In_ PUNICODE_STRING RegistryPath
)
{
	NTSTATUS           status;
	RegistryPath;
	//PKLDR_DATA_TABLE_ENTRY pobj = (PKLDR_DATA_TABLE_ENTRY)DriverObject->DriverSection;
	//pobj->Flags |= 0x20;
	KdPrint(("lkzs:my first dirver\n"));
	DriverObject->DriverUnload = UnLoad;
	DriverObject->MajorFunction[IRP_MJ_CREATE] = IRP_CALL;
	DriverObject->MajorFunction[IRP_MJ_CLOSE] = IRP_CALL;
	DriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = IRP_CALL;//DeviceIoControl
	//DriverObject->MajorFunction[IRP_MJ_WRITE] = IRP_CALL;
	//DriverObject->MajorFunction[IRP_MJ_READ] = IRP_CALL;
	CreateDevice(DriverObject);
	status = SearchMouServiceCallBack(DriverObject);
	if (!NT_SUCCESS(status))
	{
		KdPrint(("MOUSE_DEVICE ERROR, error = 0x%08lx\n", status));
		return status;
	}
	//安装内存保护();
	return 0;
}

