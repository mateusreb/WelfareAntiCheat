#include <ntifs.h>
#include <ntddk.h>
#include <windef.h>
#include <wdf.h>
#include <ntdef.h>

UNICODE_STRING DeviceName, DosName;
PVOID ObHandle = NULL;

#define PROCESS_VM_READ			(0x0010)
#define PROCESS_VM_WRITE		(0x0020)
#define PROCESS_SUSPEND_RESUME	(0x0800)
#define PROCESS_VM_OPERATION	(0x0008)

typedef struct _OB_REG_CONTEXT {
	USHORT Version;
	UNICODE_STRING Altitude;
	USHORT ulIndex;
	OB_OPERATION_REGISTRATION *OperationRegistration;
} REG_CONTEXT, *PREG_CONTEXT;

NTSTATUS CreateCall(PDEVICE_OBJECT pDeviceObject, PIRP irp)
{
	UNREFERENCED_PARAMETER(pDeviceObject);
	irp->IoStatus.Status = STATUS_SUCCESS;
	irp->IoStatus.Information = 0;
	IoCompleteRequest(irp, IO_NO_INCREMENT);
	return STATUS_SUCCESS;
}
  
NTSTATUS CloseCall(PDEVICE_OBJECT pDeviceObject, PIRP irp)
{
	UNREFERENCED_PARAMETER(pDeviceObject);
	irp->IoStatus.Status = STATUS_SUCCESS;
	irp->IoStatus.Information = 0;
	IoCompleteRequest(irp, IO_NO_INCREMENT);
	return STATUS_SUCCESS;
} 
 
OB_PREOP_CALLBACK_STATUS objPreCB(PVOID RegistrationContext, POB_PRE_OPERATION_INFORMATION OperationInformation)
{
	UNREFERENCED_PARAMETER(RegistrationContext);

	if(PsGetProcessId((PEPROCESS)OperationInformation->Object) == (PVOID)13268)
	{
		if (OperationInformation->Operation == OB_OPERATION_HANDLE_CREATE)
		{
			if ((OperationInformation->Parameters->CreateHandleInformation.OriginalDesiredAccess & PROCESS_VM_OPERATION) == PROCESS_VM_OPERATION)
			{
				DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_WARNING_LEVEL, "[WAC-DRV] - Requested PROCESS_VM_OPERATION - GAME.\n");
				OperationInformation->Parameters->CreateHandleInformation.DesiredAccess &= ~PROCESS_VM_OPERATION;
			}
			if ((OperationInformation->Parameters->CreateHandleInformation.OriginalDesiredAccess & PROCESS_VM_READ) == PROCESS_VM_READ)
			{
				DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_WARNING_LEVEL, "[WAC-DRV] - Requested PROCESS_VM_READ - GAME.\n");
				OperationInformation->Parameters->CreateHandleInformation.DesiredAccess &= ~PROCESS_VM_READ;
			}
			if ((OperationInformation->Parameters->CreateHandleInformation.OriginalDesiredAccess & PROCESS_VM_WRITE) == PROCESS_VM_WRITE)
			{
				DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_WARNING_LEVEL, "[WAC-DRV] - Requested PROCESS_VM_WRITE - GAME.\n");
				OperationInformation->Parameters->CreateHandleInformation.DesiredAccess &= ~PROCESS_VM_WRITE;
			}
			if ((OperationInformation->Parameters->CreateHandleInformation.OriginalDesiredAccess & PROCESS_SUSPEND_RESUME) == PROCESS_SUSPEND_RESUME)
			{
				DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_WARNING_LEVEL, "[WAC-DRV] - Requested PROCESS_SUSPEND_RESUME - GAME.\n");
				OperationInformation->Parameters->CreateHandleInformation.DesiredAccess &= ~PROCESS_SUSPEND_RESUME;
			}
		}
	}
	return OB_PREOP_SUCCESS;
}

VOID objPostCB(PVOID RegistrationContext, POB_POST_OPERATION_INFORMATION OperationInformation)
{
	UNREFERENCED_PARAMETER(RegistrationContext);
	UNREFERENCED_PARAMETER(OperationInformation);
}

VOID mod_processprotection(BOOLEAN bEnable)
{
	if (bEnable)
	{
		NTSTATUS NtRet = STATUS_SUCCESS;
		OB_OPERATION_REGISTRATION obOpReg;
		OB_CALLBACK_REGISTRATION obCbReg;
		REG_CONTEXT regContext;
		UNICODE_STRING usAltitude;
		memset(&obOpReg, 0, sizeof(OB_OPERATION_REGISTRATION));
		memset(&obCbReg, 0, sizeof(OB_CALLBACK_REGISTRATION));
		memset(&regContext, 0, sizeof(REG_CONTEXT));
		regContext.ulIndex = 1;
		regContext.Version = 120;
		RtlInitUnicodeString(&usAltitude, L"XXXXXXX");
		if ((USHORT)ObGetFilterVersion() == OB_FLT_REGISTRATION_VERSION)
		{
			obOpReg.ObjectType = PsProcessType;
			obOpReg.Operations = OB_OPERATION_HANDLE_CREATE;
			obOpReg.PostOperation = objPostCB;
			obOpReg.PreOperation = objPreCB;
			obCbReg.Altitude = usAltitude;
			obCbReg.OperationRegistration = &obOpReg;
			obCbReg.RegistrationContext = &regContext;
			obCbReg.Version = OB_FLT_REGISTRATION_VERSION;
			obCbReg.OperationRegistrationCount = (USHORT)1;
			NtRet = ObRegisterCallbacks(&obCbReg, &ObHandle);
			DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_WARNING_LEVEL, "[WAC-DRV] - CallBack\n");
		}
	}
	else
	{
		if (ObHandle != NULL)
		{
			ObUnRegisterCallbacks(ObHandle);
			ObHandle = NULL;
		}
	}
}

NTSTATUS UnloadDriver(PDRIVER_OBJECT pDriverObject)
{
	DbgPrintEx(0, 0, "[WAC-DRV] UnloadDriver.\n");
	if (ObHandle != NULL)
	{
		mod_processprotection(FALSE);
	}
	IoDeleteSymbolicLink(&DosName);
	IoDeleteDevice(pDriverObject->DeviceObject);
	return STATUS_SUCCESS;
}

NTSTATUS DriverEntry(PDRIVER_OBJECT pDriverObject, PUNICODE_STRING pRegistryPath)
{
	UNREFERENCED_PARAMETER(pRegistryPath);
	PDEVICE_OBJECT pDeviceObject;
	DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_WARNING_LEVEL, "[WAC-DRV] - DriverEntry.\n");
	RtlInitUnicodeString(&DeviceName, L"\\Device\\WelfireSystem");
	RtlInitUnicodeString(&DosName, L"\\DosDevices\\WelfireSystem");
	
	IoCreateDevice(pDriverObject, 0, &DeviceName, FILE_DEVICE_UNKNOWN, FILE_DEVICE_SECURE_OPEN, FALSE, &pDeviceObject);
	IoCreateSymbolicLink(&DosName, &DeviceName);

	pDriverObject->MajorFunction[IRP_MJ_CREATE] = CreateCall;
	pDriverObject->MajorFunction[IRP_MJ_CLOSE] = CloseCall;
	//pDriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = IoControl;
	pDriverObject->DriverUnload = UnloadDriver;

	

	pDeviceObject->Flags |= DO_DIRECT_IO;
	pDeviceObject->Flags &= ~DO_DEVICE_INITIALIZING;
	mod_processprotection(TRUE);
	return STATUS_SUCCESS;
}
