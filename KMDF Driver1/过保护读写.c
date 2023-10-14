#include<ntifs.h>
#include"��������д.h"


//OK ����ͨ�� ����2���� 
//��1���� sizeof(PKAPC_STATE)��ָ�� �øĽṹ��С sizeof(KAPC_STATE)
//��2���� KeStackAttachProcess�� ���̿ռ�仯�� �����ں��ڴ� ��ת BUF������
//AddressΪĿ����̵��ڴ��ַ
//Buffer //��ǰ���̵ĵ�ַ
BOOLEAN KReadProcessMemory(IN PEPROCESS Process, IN PVOID Address, IN UINT32 Length, IN OUT PVOID Buffer)
{

	KAPC_STATE apc_state;
	RtlZeroMemory(&apc_state, sizeof(KAPC_STATE));
	//�����н���ͨ��
	PVOID tmpBuf_Kernel = ExAllocatePool(NonPagedPool, Length);//�ں��д��ڴ���з���ָ����С�������ռ�

	//���Ը�MDL�ѡ����̵�ַ��ӳ�䵽���ں�����Ҳ����
	//��ת�ڴ��ַ ��Ϊ KeStackAttachProcess�� ԭ���Ľ���R3 Buffer ��Ŀ���ַ�ﲻ����
	//RtlCopyMemory(tmpBuf_Kernel, Buffer, Length);
	//���뵽Ŀ����̵� �ڴ�ռ�
	//DbgPrint("yjx:���ӵ�Ŀ����� Address=%p  Buffer=%p", Address, Buffer);
	//����Ŀ������ڴ�ռ�
	KeStackAttachProcess((PVOID)Process, &apc_state);//����ǰ�̸߳��ӵ�Ŀ����̵ĵ�ַ�ռ�
	//�ж�Ŀ���ַ�Ƿ���Է���
	BOOLEAN dwRet = MmIsAddressValid(Address);
	if (dwRet)
	{
		//KdPrint(("yjx[sys64] RtlCopyMemory(Address, Buffer, Length);\r\n", Address, Buffer, Length));

		//RtlCopyMemory(Address, tmpBuf_Kernel, Length); //һ��Ҫ���ں��ڴ� ��ת
		RtlCopyMemory(tmpBuf_Kernel, Address, Length); // c/c++ memcpy     ��Դ�ڴ������ݸ��Ƶ�Ŀ���ڴ�顣

	}
	else
	{
		KdPrint(("yjx:sys64:Error Line37"));
	}
	//����Ŀ����̿ռ� �ָ�����
	KeUnstackDetachProcess(&apc_state);
	//DbgPrint("yjx: sys����Ŀ�����");
	RtlCopyMemory(Buffer, tmpBuf_Kernel, Length);
	//DbgPrint("yjx:sys: Buffer[0]=%llx", *(UINT64*)Buffer);;
	ExFreePool(tmpBuf_Kernel);
	return dwRet;
}
//dwPidΪĿ�����id
//lpBaseAddress Ŀ����̵�ַ

//lpBuffer ��ǰ���̵�ַ 1
//�ں��ڴ��ַ ��ǰ���̵�ַ 2
int ReadProcessMemoryForPid(UINT32 dwPid, PVOID pBase, PVOID lpBuffer, UINT32 nSize)
{
	//����pid��ȡPEPROCESS OpenProcess
	PEPROCESS Seleted_pEPROCESS = NULL;
	DbgPrint("yjx:sys64 ReadMemory pid=%d pBase=%p �к�=%d %s", dwPid, pBase, __LINE__, __FUNCDNAME__);
	if (PsLookupProcessByProcessId((PVOID)(UINT_PTR)(dwPid), &Seleted_pEPROCESS) == STATUS_SUCCESS)//PsLookupProcessByProcessId ���̽��ܽ��̵Ľ��� ID��������ָ����̵� EPROCESS �ṹ������ָ�롣
	{

		BOOLEAN br = KReadProcessMemory(Seleted_pEPROCESS, (PVOID)pBase, nSize, lpBuffer);
		ObDereferenceObject(Seleted_pEPROCESS);
		if (br)
		{
			return nSize;
		}
	}
	else
	{
		KdPrint(("yjx sys64 PsLookupProcessByProcessId Fail..."));
	}

	return 0;// STATUS_UNSUCCESSFUL;

}



//OK ����ͨ�� ����2���� 
//��1���� sizeof(PKAPC_STATE)��ָ�� �øĽṹ��С sizeof(KAPC_STATE)
//��2���� KeStackAttachProcess�� ���̿ռ�仯�� �����ں��ڴ� ��ת BUF������
//AddressΪĿ����̵��ڴ��ַ
//Buffer //��ǰ���̵ĵ�ַ
BOOLEAN KReadProcessMemory2(IN PEPROCESS Process, IN PVOID Address, IN UINT32 Length, IN PVOID UserBuffer)
{

	KAPC_STATE apc_state;
	RtlZeroMemory(&apc_state, sizeof(KAPC_STATE));
	//1ΪUserBuffer ���� MDL�ڴ�����
	//����MDL����ȡ�ڴ� UserBuffer=0x200000
	//DbgPrint("yjx:sys64 %s �к�=%d(Process=%p,Address=%p,Length=%d,UserBuffer=%p)", __FUNCDNAME__, __LINE__, Process, Address, Length, UserBuffer);

	PMDL g_pmdl = IoAllocateMdl(UserBuffer, Length, 0, 0, NULL); //8 �����޸ĳ� Ҫ��ȡ���ڴ��С
	if (!g_pmdl)
	{
		return FALSE;
	}

	//2���Ϊ�Ƿ�ҳ�ڴ�
	MmBuildMdlForNonPagedPool(g_pmdl);
	//3���� ӳ���û��ڴ� �� �ں��ڴ� 0x100000
	unsigned char* Mapped = (unsigned char*)MmMapLockedPages(g_pmdl, KernelMode); //UserMode
	if (!Mapped)
	{ //ӳ��ʧ��
		IoFreeMdl(g_pmdl);
		return FALSE;
	}
	//�ɹ� ӳ���� ��ַ
	//�л��� Ŀ����� 3920  UserBuffer �ܱ�����   3796 Address ���ܷ���
	KeStackAttachProcess((PVOID)Process, &apc_state);
	//�л��� Ŀ����� 3920  UserBuffer ���ܷ���   3796 Address   �ܷ���
	//�ж�Ŀ���ַ�Ƿ���Է��� UserBuffer���ɷ��� Mapped���Է���
	BOOLEAN dwRet = MmIsAddressValid(Address);
	if (dwRet)
	{
		KdPrint(("yjx[sys64] RtlCopyMemory(Address, Buffer, Length);\r\n", Address, UserBuffer, Length));

		//���Ŀ���ַ ���Է��� ֱ�Ӹ���Ŀ���ַ���� ��ӳ����ں˵�ַ����
		RtlCopyMemory(Mapped, Address, Length); //memcpy 

	}
	else
	{
		KdPrint(("yjx:sys64:Error Line37"));
	}
	//����Ŀ����̿ռ� �ָ�����
	KeUnstackDetachProcess(&apc_state);
	//DbgPrint("yjx: sys����Ŀ�����");

	//MDL������
	////�ͷ�MDL��� ��Դ
	MmUnmapLockedPages((PVOID)Mapped, g_pmdl);
	IoFreeMdl(g_pmdl);

	return dwRet;
}

//dwPidΪĿ�����id
//lpBaseAddress Ŀ����̵�ַ

//lpBuffer ��ǰ���̵�ַ 1
//�ں��ڴ��ַ ��ǰ���̵�ַ 2
int ReadProcessMemoryForPid2(UINT32 dwPid, PVOID pBase, PVOID lpBuffer, UINT32 nSize)
{
	//����pid��ȡPEPROCESS
	PEPROCESS Seleted_pEPROCESS = NULL;
	//DbgPrint("yjx:sys64 %s �к�=%d %s", __FUNCDNAME__, __LINE__);
	if (PsLookupProcessByProcessId((PVOID)(UINT_PTR)(dwPid), &Seleted_pEPROCESS) == STATUS_SUCCESS)
	{

		BOOLEAN br = KReadProcessMemory2(Seleted_pEPROCESS, (PVOID)pBase, nSize, lpBuffer);
		ObDereferenceObject(Seleted_pEPROCESS);
		if (br)
		{
			return nSize;
		}
	}
	else
	{
		KdPrint(("yjx sys64 PsLookupProcessByProcessId Fail..."));
	}

	return 0;// STATUS_UNSUCCESSFUL;

}
//struct POINT 
//{
//	int x;
//	int y;
//};
//
//
//int MoveMouse(UINT32 dwPid)
//{
//	if (dwPid)
//	{
//		KAPC_STATE apc = { 0 };
//		KeStackAttachProcess(dwPid, &apc);
//
//		UINT64 gptCursorAsync = (UINT64)(get_system_base_export("win32kbase.sys", "gptCursorAsync"));
//
//		POINT cursor = *(POINT*)(gptCursorAsync);
//
//
//		cursor.x = 1920 / 2;
//		cursor.y = 1080 / 2;
//
//		*(POINT*)(gptCursorAsync) = cursor;
//
//		KeUnstackDetachProcess(&apc);
//	}
//}