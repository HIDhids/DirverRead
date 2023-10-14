#include <ntifs.h>
#include "进程保护.h"


static UINT32 受保护的进程PID[256] = { 0 };//保存被保护PID的数组

void 清空受保护数组()
{
	memset(受保护的进程PID, 0, sizeof(受保护的进程PID));
}

void 添加受保护的PID(UINT32 pid)
{
	for (size_t i = 0; i < 256; i++)
	{
		if (受保护的进程PID[i] == 0 || 受保护的进程PID[i] == pid)
		{
			//是空位置
			受保护的进程PID[i] = pid;
			KdPrint(("yjx:SYS 添加受保护的PID =%d ", pid));
			break;
		}
	}
}

void 删除受保护的PID(UINT32 pid)
{
	for (size_t i = 0; i < 256; i++)
	{
		if (受保护的进程PID[i] == pid) //相等表示找到了
		{

			受保护的进程PID[i] = 0;
			KdPrint(("yjx:SYS 移出受保护的PID =%d ", pid));
			//break;
		}
	}
	return;
}

BOOLEAN 判断受保护的PID(UINT64 pid)
{
	if (pid == 0) return 0;

	for (size_t i = 0; i < 256; i++)
	{
		if (受保护的进程PID[i] == pid) //相等表示找到了
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
		//内核创建
	}
	else
	{
		//UINT32 curPID = PsGetCurrentProcessId(); //获取当前进程PID
		//const char* 进程名 = PsGetProcessImageFileName(PsGetCurrentProcess()); //11个有效的字符

		HANDLE dwProcessId = PsGetProcessId((PEPROCESS)OperationInformation->Object);; ///目标进程PID 想要保护的PID
		UINT64 PID = (UINT64)dwProcessId;
		if (判断受保护的PID(PID))
		{
			//用户层
			ACCESS_MASK old权限 = OperationInformation->Parameters->CreateHandleInformation.OriginalDesiredAccess;
			//ACCESS_MASK old1 = old权限;
			ACCESS_MASK new1 = OperationInformation->Parameters->CreateHandleInformation.DesiredAccess;
			//排除 PROCESS_VM_READ 权限
			//old权限 &= ~PROCESS_VM_READ;
			//排除掉 PROCESS_VM_WRITE
			//old权限 &= ~PROCESS_VM_WRITE;
			old权限 &= ~PROCESS_TERMINATE;
			//返回我们修改过的权限 OpenProcess
			OperationInformation->Parameters->CreateHandleInformation.DesiredAccess = 0;
			DbgPrint("lkzs:old权限=%x 新权限=%X", old权限, new1);
		}
		

	}

	return OB_PREOP_SUCCESS;
};

HANDLE gs_HandleCallback = NULL;//用来存放返回的句柄  以方便卸载对应功能
void 安装内存保护()
{
	OB_CALLBACK_REGISTRATION ob1_callback_reg = { 0 };
	OB_OPERATION_REGISTRATION ob2_operation = { 0 };
	//ob1 5项
	RtlInitUnicodeString(&ob1_callback_reg.Altitude, L"321000");
	ob1_callback_reg.RegistrationContext = NULL;
	ob1_callback_reg.Version = OB_FLT_REGISTRATION_VERSION;// ObGetFilterVersion(); //OB_FLT_REGISTRATION_VERSION
	ob1_callback_reg.OperationRegistrationCount = 1;//只注册了一个回调pre
	ob1_callback_reg.OperationRegistration = &ob2_operation;
	//接下来 初始化 ob2_operation
	ob2_operation.ObjectType = PsProcessType; //OpenProcess OpenThread PsThreadType
	ob2_operation.Operations = OB_OPERATION_HANDLE_CREATE;
	ob2_operation.PostOperation = NULL;
	ob2_operation.PreOperation = my_pre_callback;
	ObRegisterCallbacks(&ob1_callback_reg, &gs_HandleCallback); // /INTEGRITYCHECK 
	DbgPrint("lkzs:sys 安装内存保护 gs_HandleCallback=%p", gs_HandleCallback);
}

void 卸载内存保护()
{
	ObUnRegisterCallbacks(gs_HandleCallback);
	DbgPrint("lkzs:sys 卸载内存保护");
}
