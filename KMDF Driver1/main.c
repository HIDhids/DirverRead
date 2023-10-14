#include<ntifs.h>
#include "common.h"
#include "main.h"
#include "���̱���.h"
#include"��������д.h"

typedef struct _DEVICE_EXTENSION {

	PDEVICE_OBJECT       kbdDeviceObject;        //�������豸����
	PDEVICE_OBJECT       mouDeviceObject;        //������豸����
	MY_KEYBOARDCALLBACK  My_KbdCallback;         //KeyboardClassServiceCallback���� 
	MY_MOUSECALLBACK     My_MouCallback;         //MouseClassServiceCallback����

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

#define ������   CTL_CODE(FILE_DEVICE_UNKNOWN, 	0x803, 	METHOD_BUFFERED,FILE_ANY_ACCESS) //���������
#define д����   CTL_CODE(FILE_DEVICE_UNKNOWN, 	0x804, 	METHOD_BUFFERED,FILE_ANY_ACCESS) //���������
#define ��д����   CTL_CODE(FILE_DEVICE_UNKNOWN, 	0x805, 	METHOD_BUFFERED,FILE_ANY_ACCESS) //���������
#define IO_����ܱ�����PID   CTL_CODE(FILE_DEVICE_UNKNOWN, 0x806, 	METHOD_BUFFERED,FILE_ANY_ACCESS) //���������
#define IO_ɾ���ܱ�����PID   CTL_CODE(FILE_DEVICE_UNKNOWN, 0x807, 	METHOD_BUFFERED,FILE_ANY_ACCESS) //���������
#define  CTL_IO_ͨ��PID��ȡ����   CTL_CODE(FILE_DEVICE_UNKNOWN, 0x80A, 	METHOD_BUFFERED,FILE_ANY_ACCESS) //���������
#define  CTL_IO_ͨ��PID��ȡ����2   CTL_CODE(FILE_DEVICE_UNKNOWN, 0x80B, 	METHOD_BUFFERED,FILE_ANY_ACCESS) //���������
#define  IO_MOUSE   CTL_CODE(FILE_DEVICE_UNKNOWN, 0x80C, 	METHOD_BUFFERED,FILE_ANY_ACCESS) //���������
//int ReadProcessMemoryForPid2(UINT32 dwPid, PVOID pBase, PVOID lpBuffer, UINT32 nSize);
NTSTATUS  IRP_ReadProcessMemory2(/*PDEVICE_OBJECT device_Object,*/ PIRP pirp)
{

	//	UNREFERENCED_PARAMETER(device_Object); //δʹ�õĲ��� ��ֹ����
	//DbgPrint("yjx:sys64 %s �к�=%d %s", __FUNCDNAME__, __LINE__);
	NTSTATUS ntStatus = STATUS_SUCCESS;
	PIO_STACK_LOCATION     irpStack = NULL;
	irpStack = IoGetCurrentIrpStackLocation(pirp);

#pragma pack(push)
#pragma pack(8)
	typedef struct TINPUT_BUF
	{
		UINT64 dwPid;//Ŀ�����PID
		PVOID pBase; //Ŀ����̵�ַ
		UINT64 nSize;//Ҫ��ȡ�ĳ���

	}TINPUT_BUF;
#pragma pack(pop)
	//�ṹ

	//PVOID BaseAddress = NULL; //���ص�ַ

	//PEPROCESS selectedprocess = NULL;//Ŀ�����	
	//SIZE_T RegionSize = 0;//�����С


	TINPUT_BUF* bufInput = (TINPUT_BUF*)(pirp->AssociatedIrp.SystemBuffer);
	//����TINPUT_BUF ���뻺������ 
	//д  UserBuferr  		&��������,//���ػ����� UserBuffer=&��������

	ReadProcessMemoryForPid2((UINT32)bufInput->dwPid, bufInput->pBase, bufInput/*�����ȡ������*/, (UINT32)bufInput->nSize);

	//ENDCODE:
	//pirp->IoStatus.Status = STATUS_SUCCESS;//
	////pirp->IoStatus.Information = 4;//���ظ�DeviceIoControl�е� �����ڶ�������lpBytesReturned
	//IoCompleteRequest(pirp, IO_NO_INCREMENT);//���÷����������I/O��������� ���Ҳ��������ȼ� 
	if (irpStack) //
	{
		if (ntStatus == STATUS_SUCCESS)
		{ //�ɹ��򷵻� ��������С
			pirp->IoStatus.Information = irpStack->Parameters.DeviceIoControl.OutputBufferLength;//DeviceIoControl
		}
		else
		{ //ʧ���򲻷���
			pirp->IoStatus.Information = 0;
		}
		//�������
		IoCompleteRequest(pirp, IO_NO_INCREMENT);
	}

	pirp->IoStatus.Status = ntStatus;
	return ntStatus;
}


void IRP_IO_ͨ��PID��ȡ����(PIRP pirp)
{
	PIO_STACK_LOCATION irpStack = IoGetCurrentIrpStackLocation(pirp); //��ȡӦ�ò㴫���Ĳ���
	UINT64* ������ = (UINT64*)(pirp->AssociatedIrp.SystemBuffer);
	KdPrint(("yjx:%s �к�%d\n", __FUNCDNAME__, __LINE__));
	if (������)
	{
		//
		//int*p = (int*)������;
		UINT32 PID = (UINT32)(UINT64)������[0]; //��������
		PVOID pBase = (PVOID)(UINT64)������[1]; //��������
		//PVOID lpBuffer = (PVOID)(UINT64)������[2]; //��������
		UINT32 nSize = (UINT32)(UINT64)������[3]; //��������


		UINT32 ReadSize = ReadProcessMemoryForPid(PID, pBase, ������, nSize);

		ReadSize;
		pirp->IoStatus.Status = STATUS_SUCCESS;
		pirp->IoStatus.Information = nSize;//���ظ�DeviceIoControl�е� �����ڶ�������lpBytesReturned
		IoCompleteRequest(pirp, IO_NO_INCREMENT);//���÷����������I/O��������� ���Ҳ��������ȼ� 
	}
	irpStack;
}

void IRP_IO_д����(PIRP pirp)
{
	(pirp);

	//ȡ������������ д���� 803
	//PIO_STACK_LOCATION irpStack = IoGetCurrentIrpStackLocation(pirp); //��ȡӦ�ò㴫���Ĳ���

}
void IRP_IO_������(PIRP pirp)
{
	KdPrint(("lkzs:IRP_IO_������ �к�=%d", __LINE__));
	char* ������ = (char*)pirp->AssociatedIrp.SystemBuffer;
	//0x804
	//PIO_STACK_LOCATION irpStack = IoGetCurrentIrpStackLocation(pirp); //��ȡӦ�ò㴫���Ĳ���
	//�򻺳���д��
	char �����ַ���[] = "lkzs:SYSR0���ص��ַ���123456";
	ULONG ���� = sizeof(�����ַ���);
	memcpy_s(������, ����, �����ַ���, ����);

	KdPrint(("lkzs:IRP_IO_������  ����=%d �к�=%d", ����, __LINE__));

	pirp->IoStatus.Status = STATUS_SUCCESS;
	pirp->IoStatus.Information = ����;//���ظ�DeviceIoControl�е� �����ڶ�������lpBytesReturned
	IoCompleteRequest(pirp, IO_NO_INCREMENT);//���÷����������I/O��������� ���Ҳ��������ȼ� 
	KdPrint(("lkzs:�뿪��ǲ����"));

}

void IRP_IO_��д����(PIRP pirp)
{
	int* ������ = (int*)pirp->AssociatedIrp.SystemBuffer;
	if (������)
	{
		int* p = ������;
		int ��� = p[0] + p[1] + p[2];
		KdPrint(("lkzs: ���=%d  (%d  %d  %d)", ���, p[0], p[1], p[2]));
		*(int*)p = ���;
		pirp->IoStatus.Status = STATUS_SUCCESS;
		pirp->IoStatus.Information = sizeof(int);//���ظ�DeviceIoControl�е� �����ڶ�������lpBytesReturned
		IoCompleteRequest(pirp, IO_NO_INCREMENT);//���÷����������I/O��������� ���Ҳ��������ȼ� 

	}
}

void IRP_IO_����ܱ���PID(PIRP pirp)
{
	PIO_STACK_LOCATION irpStack = IoGetCurrentIrpStackLocation(pirp); //��ȡӦ�ò㴫���Ĳ���
	int* ������ = (int*)pirp->AssociatedIrp.SystemBuffer;
	if (������)
	{
		//
		UINT32* pPID = (UINT32*)������;

		UINT32 pid = pPID[0];
		����ܱ�����PID(pid);
		pirp->IoStatus.Status = STATUS_SUCCESS;
		pirp->IoStatus.Information = sizeof(int);//���ظ�DeviceIoControl�е� �����ڶ�������lpBytesReturned
		IoCompleteRequest(pirp, IO_NO_INCREMENT);//���÷����������I/O��������� ���Ҳ��������ȼ� 
	}
	irpStack;
}

void IRP_IO_ɾ���ܱ���PID(PIRP pirp)
{
	PIO_STACK_LOCATION irpStack = IoGetCurrentIrpStackLocation(pirp); //��ȡӦ�ò㴫���Ĳ���
	int* ������ = (int*)pirp->AssociatedIrp.SystemBuffer;
	if (������)
	{
		//
		UINT32* pPID = (UINT32*)������;

		UINT32 pid = pPID[0];
		ɾ���ܱ�����PID(pid);
		pirp->IoStatus.Status = STATUS_SUCCESS;
		pirp->IoStatus.Information = sizeof(int);//���ظ�DeviceIoControl�е� �����ڶ�������lpBytesReturned
		IoCompleteRequest(pirp, IO_NO_INCREMENT);//���÷����������I/O��������� ���Ҳ��������ȼ� 
	}
	irpStack;
}
NTSTATUS IRP_CALL(PDEVICE_OBJECT device, PIRP pirp)
{
	UNREFERENCED_PARAMETER(device); //�ȼ��� (device);
	//aaaabbb(device);
	//device;
	PMOUSE_INPUT_DATA 	 MouseInputDataStart, MouseInputDataEnd;

	//KdPrint(("lkzs:������ǲ����"));
	PIO_STACK_LOCATION irpStackL;
	ULONG 				 InputDataConsumed;
	PVOID				 ioBuffer;
	//	ULONG CtlCode;
	//	ULONG InputBuffLength;

	irpStackL = IoGetCurrentIrpStackLocation(pirp); //��ȡӦ�ò㴫���Ĳ���

	switch (irpStackL->MajorFunction)
	{
		case IRP_MJ_DEVICE_CONTROL: //DeviceIoControl
		{
			UINT32 ������ = irpStackL->Parameters.DeviceIoControl.IoControlCode;
			switch (������)
			{
			case CTL_IO_ͨ��PID��ȡ����:
			{
				IRP_IO_ͨ��PID��ȡ����(pirp); //IRP_IO_ͨ��PID��ȡ����
				return STATUS_SUCCESS;
				break;
			}
			case CTL_IO_ͨ��PID��ȡ����2:
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
					//pirp->IoStatus.Information = nSize;//���ظ�DeviceIoControl�е� �����ڶ�������lpBytesReturned
					IoCompleteRequest(pirp, IO_NO_INCREMENT);//���÷����������I/O��������� ���Ҳ��������ȼ� 
					return STATUS_SUCCESS;
				}
				break;
			}

				
			}


			//if (������ == ������)
			//{
			//	IRP_IO_������(pirp);
			//	return STATUS_SUCCESS;
			//}
			//else if (������ == д����)
			//{
			//	char* ������ = (char*)pirp->AssociatedIrp.SystemBuffer;
			//	������;
			//	//KdPrint(("R0 yjx:�û�������� DeviceIoControl ������=%X", ������));
			//	KdPrint(("R0 lkzs:�û�������� DeviceIoControl ������=%X ������=%s", ������, ������));
			//}
			//else if (������ == ��д����)
			//{
			//	IRP_IO_��д����(pirp);
			//	return STATUS_SUCCESS;

			//}
			//else if (������ == IO_����ܱ�����PID)
			//{
			//	IRP_IO_����ܱ���PID(pirp);
			//	//���pid
			//	return STATUS_SUCCESS;
			//}
			//else if (������ == IO_ɾ���ܱ�����PID)
			//{
			//	//ɾ��pid
			//	IRP_IO_ɾ���ܱ���PID(pirp);
			//	return STATUS_SUCCESS;
			//}
			//else if (CTL_IO_ͨ��PID��ȡ���� == ������)
			//{
			//	IRP_IO_ͨ��PID��ȡ����(pirp); //IRP_IO_ͨ��PID��ȡ����
			//	return STATUS_SUCCESS;
			//}
			//else if (CTL_IO_ͨ��PID��ȡ����2 == ������)
			//{
			//	return IRP_ReadProcessMemory2(pirp);
			//}
			break;
		}
	case IRP_MJ_CREATE: //CreateFile
	{
		KdPrint(("lkzs:�û�������� CreateFile"));
		break;
	}
	case IRP_MJ_CLOSE: //CloseHandle
	{
		KdPrint(("lkzs:�û�������� CloseHandle"));
		break;
	}
	//case IRP_MJ_READ:
	//{
	//	//char* ָ��1 = (char*)pirp->AssociatedIrp.SystemBuffer;
	//	//char* ָ��2 = (char*)pirp->MdlAddress; //
	//	char* ָ��3 = (char*)pirp->UserBuffer;

	//	//KdPrint(("lkzs:SYS IRP_MJ_READ (%p,%p,%p)", ָ��1, ָ��2, ָ��3));
	//	char �����ַ���[] = "SYS IRP_MJ_READ���ص��ַ���";
	//	ULONG ���� = sizeof(�����ַ���);
	//	if (ָ��3 && irpStackL->Parameters.Read.Length >= ����)
	//	{
	//		//irpStackL->Parameters.Read.Length;

	//		RtlCopyMemory(ָ��3, �����ַ���, ����);
	//	}
	//	pirp->IoStatus.Status = STATUS_SUCCESS;
	//	pirp->IoStatus.Information = ����;//strlen // ��Ӧ ReadFile��4������ len
	//	IoCompleteRequest(pirp, IO_NO_INCREMENT);//���÷����������I/O��������� ���Ҳ��������ȼ� 
	//	KdPrint(("yjx IRP_MJ_READ �뿪��ǲ����"));
	//	return STATUS_SUCCESS;  //0 ���سɹ�
	//	break;
	//}
	//case IRP_MJ_WRITE:
	//{
	//	//char* ָ��1 = (char*)pirp->AssociatedIrp.SystemBuffer;
	//	//char* ָ��2 = (char*)pirp->MdlAddress; //
	//	//char* ָ��3 = (char*)pirp->UserBuffer;

	//	//KdPrint(("lkzs:SYS IRP_MJ_WRITE (%p,%p,%p)", ָ��1, ָ��2, ָ��3));
	//	//KdPrint(("lkzs:SYS IRP_MJ_WRITE (%s,%s,%s)", ָ��1, ָ��2, ָ��3));
	//	break;
	//}
	}

	pirp->IoStatus.Status = STATUS_SUCCESS;
	pirp->IoStatus.Information = 4;//���ظ�DeviceIoControl�е� �����ڶ�������lpBytesReturned
	IoCompleteRequest(pirp, IO_NO_INCREMENT);//���÷����������I/O��������� ���Ҳ��������ȼ� 
	KdPrint(("lkzs:�뿪��ǲ����"));
	return STATUS_SUCCESS;  //0 ���سɹ�
}
void UnLoad(PDRIVER_OBJECT Driver)
{
	(Driver);
	KdPrint(("lkzs:����ж������ Driver=%p\n", Driver));
	//ж���ڴ汣��();
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
		//�����������ҵ��Ķ˿��������豸��չ��ÿһ��ָ��  
		for (i = 0; i < 4096; i++, DeviceExt++)
		{
			PVOID tmp;
			if (!MmIsAddressValid(DeviceExt))
			{
				break;
			}
			//�ҵ������д�����ȫ�ֱ����У��������Ƿ��Ѿ������  
			//����Ѿ�����˾Ͳ��ü������ˣ�����ֱ���˳�  
			if (g_KoMCallBack.MouDeviceObject && g_KoMCallBack.MouseClassServiceCallback)
			{
				status = STATUS_SUCCESS;
				break;
			}
			//�ڶ˿��������豸��չ��ҵ����������豸��������������豸��������  
			tmp = *(PVOID*)DeviceExt;
			if (tmp == pTargetDeviceObject)
			{
				g_KoMCallBack.MouDeviceObject = pTargetDeviceObject;
				continue;
			}

			//������豸��չ���ҵ�һ����ַλ��KbdClass��������У��Ϳ�����Ϊ�����������Ҫ�ҵĻص�����  
			if ((tmp > KbdDriverStart) && (tmp < (UCHAR*)KbdDriverStart + KbdDriverSize) &&
				(MmIsAddressValid(tmp)))
			{
				//������ص�������¼����  
				g_KoMCallBack.MouseClassServiceCallback = (MY_MOUSECALLBACK)tmp;
				//g_KoMCallBack.MouSerCallAddr = (PVOID *)DeviceExt;
				status = STATUS_SUCCESS;
			}
		}
		if (status == STATUS_SUCCESS)
		{
			break;
		}
		//������һ���豸����������  
		pTargetDeviceObject = pTargetDeviceObject->NextDevice;
	}
	return status;
}



NTSTATUS  SearchMouServiceCallBack(IN PDRIVER_OBJECT DriverObject)
{
	//�����õ���һ��ȫ�ֱ�������Щ����������ǹ���˼���  
	NTSTATUS status = STATUS_SUCCESS;
	UNICODE_STRING uniNtNameString;
	PDEVICE_OBJECT pTargetDeviceObject = NULL;
	PDRIVER_OBJECT KbdDriverObject = NULL;
	PDRIVER_OBJECT KbdhidDriverObject = NULL;
	PDRIVER_OBJECT Kbd8042DriverObject = NULL;
	PDRIVER_OBJECT UsingDriverObject = NULL;
	PDEVICE_OBJECT UsingDeviceObject = NULL;

	PVOID UsingDeviceExt = NULL;

	//����Ĵ���������USB���̶˿���������������  
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
	//��PS/2���̵���������  
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
	//��������豸��û���ҵ�  
	if (!Kbd8042DriverObject && !KbdhidDriverObject)
	{
		return STATUS_SUCCESS;
	}
	//���USB���̺�PS/2����ͬʱ���ڣ�ʹ��USB���
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
		//���û�гɹ���ֱ�ӷ��ؼ���  
		KdPrint(("MyAttach: Coundn't get the Mouse driver Object\n"));
		return STATUS_UNSUCCESSFUL;
	}
	else
	{
		ObDereferenceObject(KbdDriverObject);
	}
	//����KbdDriverObject�µ��豸���� 
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
	//��װ�ڴ汣��();
	return 0;
}

