#include <ntifs.h>
#include "���̱���.h"


static UINT32 �ܱ����Ľ���PID[256] = { 0 };//���汻����PID������

void ����ܱ�������()
{
	memset(�ܱ����Ľ���PID, 0, sizeof(�ܱ����Ľ���PID));
}

void ����ܱ�����PID(UINT32 pid)
{
	for (size_t i = 0; i < 256; i++)
	{
		if (�ܱ����Ľ���PID[i] == 0 || �ܱ����Ľ���PID[i] == pid)
		{
			//�ǿ�λ��
			�ܱ����Ľ���PID[i] = pid;
			KdPrint(("yjx:SYS ����ܱ�����PID =%d ", pid));
			break;
		}
	}
}

void ɾ���ܱ�����PID(UINT32 pid)
{
	for (size_t i = 0; i < 256; i++)
	{
		if (�ܱ����Ľ���PID[i] == pid) //��ȱ�ʾ�ҵ���
		{

			�ܱ����Ľ���PID[i] = 0;
			KdPrint(("yjx:SYS �Ƴ��ܱ�����PID =%d ", pid));
			//break;
		}
	}
	return;
}

BOOLEAN �ж��ܱ�����PID(UINT64 pid)
{
	if (pid == 0) return 0;

	for (size_t i = 0; i < 256; i++)
	{
		if (�ܱ����Ľ���PID[i] == pid) //��ȱ�ʾ�ҵ���
		{

			return TRUE;
		}
	}
	return FALSE;
}

OB_PREOP_CALLBACK_STATUS
my_pre_callback(
	PVOID RegistrationContext,
	POB_PRE_OPERATION_INFORMATION OperationInformation
)
{
	(RegistrationContext);
	//DbgPrint("lkzs:sys pEPROCESS=%p ", OperationInformation->Object);
	if (OperationInformation->KernelHandle)
	{
		//�ں˴���
	}
	else
	{
		//UINT32 curPID = PsGetCurrentProcessId(); //��ȡ��ǰ����PID
		//const char* ������ = PsGetProcessImageFileName(PsGetCurrentProcess()); //11����Ч���ַ�

		HANDLE dwProcessId = PsGetProcessId((PEPROCESS)OperationInformation->Object);; ///Ŀ�����PID ��Ҫ������PID
		UINT64 PID = (UINT64)dwProcessId;
		if (�ж��ܱ�����PID(PID))
		{
			//�û���
			ACCESS_MASK oldȨ�� = OperationInformation->Parameters->CreateHandleInformation.OriginalDesiredAccess;
			//ACCESS_MASK old1 = oldȨ��;
			ACCESS_MASK new1 = OperationInformation->Parameters->CreateHandleInformation.DesiredAccess;
			//�ų� PROCESS_VM_READ Ȩ��
			//oldȨ�� &= ~PROCESS_VM_READ;
			//�ų��� PROCESS_VM_WRITE
			//oldȨ�� &= ~PROCESS_VM_WRITE;
			oldȨ�� &= ~PROCESS_TERMINATE;
			//���������޸Ĺ���Ȩ�� OpenProcess
			OperationInformation->Parameters->CreateHandleInformation.DesiredAccess = 0;
			DbgPrint("lkzs:oldȨ��=%x ��Ȩ��=%X", oldȨ��, new1);
		}
		

	}

	return OB_PREOP_SUCCESS;
};

HANDLE gs_HandleCallback = NULL;//������ŷ��صľ��  �Է���ж�ض�Ӧ����
void ��װ�ڴ汣��()
{
	OB_CALLBACK_REGISTRATION ob1_callback_reg = { 0 };
	OB_OPERATION_REGISTRATION ob2_operation = { 0 };
	//ob1 5��
	RtlInitUnicodeString(&ob1_callback_reg.Altitude, L"321000");
	ob1_callback_reg.RegistrationContext = NULL;
	ob1_callback_reg.Version = OB_FLT_REGISTRATION_VERSION;// ObGetFilterVersion(); //OB_FLT_REGISTRATION_VERSION
	ob1_callback_reg.OperationRegistrationCount = 1;//ֻע����һ���ص�pre
	ob1_callback_reg.OperationRegistration = &ob2_operation;
	//������ ��ʼ�� ob2_operation
	ob2_operation.ObjectType = PsProcessType; //OpenProcess OpenThread PsThreadType
	ob2_operation.Operations = OB_OPERATION_HANDLE_CREATE;
	ob2_operation.PostOperation = NULL;
	ob2_operation.PreOperation = my_pre_callback;
	ObRegisterCallbacks(&ob1_callback_reg, &gs_HandleCallback); // /INTEGRITYCHECK 
	DbgPrint("lkzs:sys ��װ�ڴ汣�� gs_HandleCallback=%p", gs_HandleCallback);
}

void ж���ڴ汣��()
{
	ObUnRegisterCallbacks(gs_HandleCallback);
	DbgPrint("lkzs:sys ж���ڴ汣��");
}
