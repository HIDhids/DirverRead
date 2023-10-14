#include <ntifs.h>

NTSTATUS CreateDevice(PDRIVER_OBJECT driver)
{
	NTSTATUS status;
	UNICODE_STRING MyDriver;
	PDEVICE_OBJECT device;//���ڴ���豸����
	RtlInitUnicodeString(&MyDriver, L"\\DEVICE\\MyDriver");//�����豸����

	status = IoCreateDevice(driver, sizeof(driver->DriverExtension), &MyDriver, FILE_DEVICE_UNKNOWN, FILE_DEVICE_SECURE_OPEN, FALSE, &device);


	if (status == STATUS_SUCCESS)//STATUS_SUCCESS)
	{
		KdPrint(("lkzs:�����豸���󴴽��ɹ���OK \n"));
		//������������
		UNICODE_STRING uzSymbolName; //������������		 
		RtlInitUnicodeString(&uzSymbolName, L"\\??\\MyDriver"); //CreateFile
		status = IoCreateSymbolicLink(&uzSymbolName, &MyDriver);
		if (status == STATUS_SUCCESS)
		{
			KdPrint(("lkzs:������������ %wZ �ɹ� ", &uzSymbolName));
		}
		else
		{
			KdPrint(("lkzs:������������ %wZ ʧ�� status=%X", &uzSymbolName, status));
		}
	}
	else
	{

		KdPrint(("lkzs:�����豸���󴴽�ʧ�ܣ�ɾ���豸\n"));
		IoDeleteDevice(device);
	}
	return status;
}

void DriverUnLoad(PDRIVER_OBJECT pDriver)
{
	KdPrint(("lkzs:������ DriverUnLoad����"));
	if (pDriver->DeviceObject)
	{

		//ɾ����������

		UNICODE_STRING uzSymbolName; //������������		 
		RtlInitUnicodeString(&uzSymbolName, L"\\??\\MyDriver"); //CreateFile
		KdPrint(("lkzs:ɾ����������=%wZ", &uzSymbolName));
		IoDeleteSymbolicLink(&uzSymbolName);
		//
		KdPrint(("lkzs:ɾ�������豸"));
		IoDeleteDevice(pDriver->DeviceObject);//ɾ���豸����

	}
	KdPrint(("lkzs:�˳� DriverUnLoad����"));
}