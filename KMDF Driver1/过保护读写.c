#include<ntifs.h>
#include"过保护读写.h"


//OK 测试通过 遇到2个坑 
//第1个坑 sizeof(PKAPC_STATE)是指针 得改结构大小 sizeof(KAPC_STATE)
//第2个坑 KeStackAttachProcess后 进程空间变化了 得用内核内存 中转 BUF缓冲区
//Address为目标进程的内存地址
//Buffer //当前进程的地址
BOOLEAN KReadProcessMemory(IN PEPROCESS Process, IN PVOID Address, IN UINT32 Length, IN OUT PVOID Buffer)
{

	KAPC_STATE apc_state;
	RtlZeroMemory(&apc_state, sizeof(KAPC_STATE));
	//在所有进程通用
	PVOID tmpBuf_Kernel = ExAllocatePool(NonPagedPool, Length);//内核中从内存池中分配指定大小的连续空间

	//可以改MDL把　进程地址　映射到　内核驱动也可以
	//中转内存地址 因为 KeStackAttachProcess后 原来的进程R3 Buffer 在目标地址里不存在
	//RtlCopyMemory(tmpBuf_Kernel, Buffer, Length);
	//进入到目标进程的 内存空间
	//DbgPrint("yjx:附加到目标进程 Address=%p  Buffer=%p", Address, Buffer);
	//进入目标进程内存空间
	KeStackAttachProcess((PVOID)Process, &apc_state);//将当前线程附加到目标进程的地址空间
	//判断目标地址是否可以访问
	BOOLEAN dwRet = MmIsAddressValid(Address);
	if (dwRet)
	{
		//KdPrint(("yjx[sys64] RtlCopyMemory(Address, Buffer, Length);\r\n", Address, Buffer, Length));

		//RtlCopyMemory(Address, tmpBuf_Kernel, Length); //一定要用内核内存 中转
		RtlCopyMemory(tmpBuf_Kernel, Address, Length); // c/c++ memcpy     将源内存块的内容复制到目标内存块。

	}
	else
	{
		KdPrint(("yjx:sys64:Error Line37"));
	}
	//分离目标进程空间 恢复环境
	KeUnstackDetachProcess(&apc_state);
	//DbgPrint("yjx: sys分离目标进程");
	RtlCopyMemory(Buffer, tmpBuf_Kernel, Length);
	//DbgPrint("yjx:sys: Buffer[0]=%llx", *(UINT64*)Buffer);;
	ExFreePool(tmpBuf_Kernel);
	return dwRet;
}
//dwPid为目标进程id
//lpBaseAddress 目标进程地址

//lpBuffer 当前进程地址 1
//内核内存地址 当前进程地址 2
int ReadProcessMemoryForPid(UINT32 dwPid, PVOID pBase, PVOID lpBuffer, UINT32 nSize)
{
	//根据pid获取PEPROCESS OpenProcess
	PEPROCESS Seleted_pEPROCESS = NULL;
	DbgPrint("yjx:sys64 ReadMemory pid=%d pBase=%p 行号=%d %s", dwPid, pBase, __LINE__, __FUNCDNAME__);
	if (PsLookupProcessByProcessId((PVOID)(UINT_PTR)(dwPid), &Seleted_pEPROCESS) == STATUS_SUCCESS)//PsLookupProcessByProcessId 例程接受进程的进程 ID，并返回指向进程的 EPROCESS 结构的引用指针。
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



//OK 测试通过 遇到2个坑 
//第1个坑 sizeof(PKAPC_STATE)是指针 得改结构大小 sizeof(KAPC_STATE)
//第2个坑 KeStackAttachProcess后 进程空间变化了 得用内核内存 中转 BUF缓冲区
//Address为目标进程的内存地址
//Buffer //当前进程的地址
BOOLEAN KReadProcessMemory2(IN PEPROCESS Process, IN PVOID Address, IN UINT32 Length, IN PVOID UserBuffer)
{

	KAPC_STATE apc_state;
	RtlZeroMemory(&apc_state, sizeof(KAPC_STATE));
	//1为UserBuffer 创建 MDL内存描述
	//创建MDL来读取内存 UserBuffer=0x200000
	//DbgPrint("yjx:sys64 %s 行号=%d(Process=%p,Address=%p,Length=%d,UserBuffer=%p)", __FUNCDNAME__, __LINE__, Process, Address, Length, UserBuffer);

	PMDL g_pmdl = IoAllocateMdl(UserBuffer, Length, 0, 0, NULL); //8 可以修改成 要读取的内存大小
	if (!g_pmdl)
	{
		return FALSE;
	}

	//2标记为非分页内存
	MmBuildMdlForNonPagedPool(g_pmdl);
	//3锁定 映射用户内存 到 内核内存 0x100000
	unsigned char* Mapped = (unsigned char*)MmMapLockedPages(g_pmdl, KernelMode); //UserMode
	if (!Mapped)
	{ //映射失败
		IoFreeMdl(g_pmdl);
		return FALSE;
	}
	//成功 映射了 地址
	//切换到 目标进程 3920  UserBuffer 能被访问   3796 Address 不能访问
	KeStackAttachProcess((PVOID)Process, &apc_state);
	//切换到 目标进程 3920  UserBuffer 不能访问   3796 Address   能访问
	//判断目标地址是否可以访问 UserBuffer不可访问 Mapped可以访问
	BOOLEAN dwRet = MmIsAddressValid(Address);
	if (dwRet)
	{
		KdPrint(("yjx[sys64] RtlCopyMemory(Address, Buffer, Length);\r\n", Address, UserBuffer, Length));

		//如果目标地址 可以访问 直接复制目标地址内容 到映射的内核地址区域
		RtlCopyMemory(Mapped, Address, Length); //memcpy 

	}
	else
	{
		KdPrint(("yjx:sys64:Error Line37"));
	}
	//分离目标进程空间 恢复环境
	KeUnstackDetachProcess(&apc_state);
	//DbgPrint("yjx: sys分离目标进程");

	//MDL清理工作
	////释放MDL相关 资源
	MmUnmapLockedPages((PVOID)Mapped, g_pmdl);
	IoFreeMdl(g_pmdl);

	return dwRet;
}

//dwPid为目标进程id
//lpBaseAddress 目标进程地址

//lpBuffer 当前进程地址 1
//内核内存地址 当前进程地址 2
int ReadProcessMemoryForPid2(UINT32 dwPid, PVOID pBase, PVOID lpBuffer, UINT32 nSize)
{
	//根据pid获取PEPROCESS
	PEPROCESS Seleted_pEPROCESS = NULL;
	//DbgPrint("yjx:sys64 %s 行号=%d %s", __FUNCDNAME__, __LINE__);
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