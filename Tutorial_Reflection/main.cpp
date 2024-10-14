#include "Systemclass.h"

#pragma comment(linker , "/entry:WinMainCRTStartup /subsystem:console")

#include <Windows.h>

#include "Global.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pScmdline, int iCmdshow)
{
	bool result;

	SystemClass* systemClass;

	//SystemClass ����
	systemClass = new SystemClass;
	if (!systemClass)
	{
		return 0;
	}

	//SystemClass �ʱ�ȭ
	result = systemClass->Initialize();
	if (result)
	{
		systemClass->Run();
	}

	//System Class�� ��� �ڿ� ��ȯ
	systemClass->Shutdown();
	delete systemClass;
	systemClass = 0;

	return 0;
}