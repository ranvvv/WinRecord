#include"common.h"

PDRIVER_OBJECT gDriverObject;			// 驱动对象指针
PUNICODE_STRING gRegistryPath;			// 注册表路径指针

NTSTATUS run();
NTSTATUS cleanup();

// 卸载钩子
VOID DriverUnload(PDRIVER_OBJECT  DriverObject)
{
	UNREFERENCED_PARAMETER(DriverObject);	// 忽略参数警告

	cleanup();

	DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "<<<<<< DriverUnload\n");
}

// 驱动入口函数
NTSTATUS DriverEntry(IN PDRIVER_OBJECT DriverObject, IN PUNICODE_STRING RegistryPath)
{
	/**
	 * @brief				: 驱动入口函数,在system进程环境中调用.
	 * @param DriverObject  : 驱动对象。
	 * @param RegistryPath	: 注册表路径 (一般为驱动所在的路径),在注册表 HKEY_LOCAL_MACHINE\SYSTEM\CurrentControlSet\Services\驱动名
	 * @return				: 如果初始化成功，则返回STATUS_SUCCESS. 如果返回STATUS_UNSUCCESSFUL，则初始化失败,模块会自动卸载.
	*/

	DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, ">>>>>> DriverRun\n");

	gDriverObject = DriverObject;
	gRegistryPath = RegistryPath;
	gDriverObject->DriverUnload = DriverUnload;

	return run();
}
