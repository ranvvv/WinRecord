#include"../common.h"

// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

//				1. 模态对话框

// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

static LRESULT CALLBACK mdlgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	// m_debugPrint(TEXT("hwnd:%x\tmsg:%x\twParam:%x\tlParam:%x\n"),hwnd,msg,wParam,lParam);

	// 消息处理
	switch (msg)
	{
	case WM_INITDIALOG:		// dlg初始化,dlg没有WM_CREATE
		printf("WM_INITDIALOG lParma : %08X\n", (int)lParam);
		break;
	case WM_COMMAND:		// 子窗口消息处理
	{
		switch (LOWORD(wParam))
		{
		case 1001:
			mDbgPrint(TEXT("点击按钮\n"));
			break;
		default:
			break;
		}
		break;
	}
	case WM_CLOSE:						// 点右上角关闭按钮时发送此消息.
		EndDialog(hwnd, 0x1111);		// 退出消息循环,使得DialogBox得以返回.
		break;
	default:
		return FALSE;			// 没处理交给默认,返回false
	}
	return TRUE;					// 处理了返回true
}

static void s1()
{
	// DialogBox(GetModuleHandle(0), (LPCTSTR)MAKEINTRESOURCEA(IDD_DIALOG_TEMPLATE), 0, (DLGPROC)mdlgProc);

	INT_PTR result = DialogBoxParam(GetModuleHandle(0), (LPCTSTR)MAKEINTRESOURCEA(IDD_DIALOG_TEMPLATE), 0, (DLGPROC)mdlgProc, 0x12345678);
	printf("result : %p\n", (PVOID)result);
}


// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

//				2. 非模态对话框

// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$


static LRESULT CALLBACK nmdlgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	// m_debugPrint(TEXT("hwnd:%x\tmsg:%x\twParam:%x\tlParam:%x\n"),hwnd,msg,wParam,lParam);

	// 消息处理
	switch (msg)
	{
	case WM_INITDIALOG:		// dlg初始化

		break;
	case WM_COMMAND:	// 子窗口消息处理
	{

		switch (LOWORD(wParam))
		{
		case 1001:
			mDbgPrint(TEXT("点击按钮\n"));
			break;
		default:
			break;
		}
		break;
	}
	case WM_CLOSE:				// 点右上角关闭按钮时发送此消息.
		DestroyWindow(hwnd);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return FALSE;		// 没处理交给默认,返回false
	}
	return TRUE; // 处理了返回true
}

static void s2()
{
	HWND hDlg = CreateDialog(GetModuleHandle(0), (LPCTSTR)MAKEINTRESOURCEA(IDD_DIALOG_TEMPLATE), 0, (DLGPROC)nmdlgProc);
	ShowWindow(hDlg, SW_SHOW);

	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!IsDialogMessage(hDlg, &msg)) // 只处理这个对话框的消息
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
}


// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

//				3. 打开文件与保存文件

// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

static void s3()
{
	OPENFILENAME ofn;
	TCHAR szFile[260];

	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = NULL;
	ofn.lpstrFile = szFile;
	// Set lpstrFile[0] to '\0' so that GetOpenFileName does not 
	// use the contents of szFile to initialize itself.
	ofn.lpstrFile[0] = '\0';
	ofn.nMaxFile = sizeof(szFile);
	ofn.lpstrFilter = TEXT("All\0*.*\0Text\0*.TXT\0");
	ofn.nFilterIndex = 1;
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = NULL;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

	// Display the Open dialog box. 

	if (GetOpenFileName(&ofn) == TRUE)
	{
		_tprintf(TEXT("file path : %s \n"), ofn.lpstrFile);
	}

	// OPENFILENAME ofn;
	// TCHAR szFile[260];

	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = NULL;
	ofn.lpstrFilter = TEXT("All Files (*.*)\0*.*\0");
	ofn.lpstrCustomFilter = NULL;
	ofn.nMaxCustFilter = 0;
	ofn.nFilterIndex = 0;
	ofn.lpstrFile = szFile;
	ofn.nMaxFile = sizeof(szFile);
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = NULL;
	ofn.lpstrTitle = TEXT("Save As");
	ofn.Flags = OFN_OVERWRITEPROMPT;
	ofn.nFileOffset = 0;
	ofn.nFileExtension = 0;
	ofn.lpstrDefExt = TEXT("txt");

	if (GetSaveFileName(&ofn))
	{
		_tprintf(TEXT("file path : %s \n"), szFile);
	}
}


// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$


// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$



void p000_007()
{

}
