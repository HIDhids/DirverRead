#include <ntifs.h>

NTSTATUS CreateDevice(PDRIVER_OBJECT driver)
{
	NTSTATUS status;
	UNICODE_STRING MyDriver;
	PDEVICE_OBJECT device;//用于存放设备对象
	RtlInitUnicodeString(&MyDriver, L"\\DEVICE\\MyDriver");//驱动设备名字

	status = IoCreateDevice(driver, sizeof(driver->DriverExtension), &MyDriver, FILE_DEVICE_UNKNOWN, FILE_DEVICE_SECURE_OPEN, FALSE, &device);


	if (status == STATUS_SUCCESS)//STATUS_SUCCESS)
	{
		KdPrint(("lkzs:驱动设备对象创建成功，OK \n"));
		//创建符号链接
		UNICODE_STRING uzSymbolName; //符号链接名字		 
		RtlInitUnicodeString(&uzSymbolName, L"\\??\\MyDriver"); //CreateFile
		status = IoCreateSymbolicLink(&uzSymbolName, &MyDriver);
		if (status == STATUS_SUCCESS)
		{
			KdPrint(("lkzs:创建符号链接 %wZ 成功 ", &uzSymbolName));
		}
		else
		{
			KdPrint(("lkzs:创建符号链接 %wZ 失败 status=%X", &uzSymbolName, status));
		}
	}
	else
	{

		KdPrint(("lkzs:驱动设备对象创建失败，删除设备\n"));
		IoDeleteDevice(device);
	}
	return status;
}

void DriverUnLoad(PDRIVER_OBJECT pDriver)
{
	KdPrint(("lkzs:进入了 DriverUnLoad例程"));
	if (pDriver->DeviceObject)
	{

		//删除符号链接

		UNICODE_STRING uzSymbolName; //符号链接名字		 
		RtlInitUnicodeString(&uzSymbolName, L"\\??\\MyDriver"); //CreateFile
		KdPrint(("lkzs:删除符号链接=%wZ", &uzSymbolName));
		IoDeleteSymbolicLink(&uzSymbolName);
		//
		KdPrint(("lkzs:删除驱动设备"));
		IoDeleteDevice(pDriver->DeviceObject);//删除设备对象

	}
	KdPrint(("lkzs:退出 DriverUnLoad例程"));
}