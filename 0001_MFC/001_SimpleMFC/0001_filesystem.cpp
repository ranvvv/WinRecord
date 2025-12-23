#include<afxwin.h>
#include<afxdlgs.h>

class C0001Obj : public CObject
{
public:
	PCHAR mReadFile(LPCTSTR lpszPathName, DWORD* pLengthOut);
	BOOL mWriteFile(LPCTSTR lpszPathName, PCHAR pBuffer, DWORD length);
	   
	void OnFileSaveAs();
};


// 读取文件
PCHAR C0001Obj::mReadFile(LPCTSTR lpszPathName, DWORD* pLengthOut)
{
	BOOL result = 0;
	TCHAR text[0x200] = { 0 };
	PCHAR pBuffer = NULL;
	DWORD length = 0;
	DWORD readLength = 0;
	CFile file;
	CFileException fileException;

	result = file.Open(lpszPathName, CFile::modeRead | CFile::typeBinary, &fileException);
	if (!result)
	{
		fileException.GetErrorMessage(text, 0x200);
		AfxMessageBox(text);
		return NULL;
	}

	length = (DWORD)file.GetLength();
	if (!length)
	{
		AfxMessageBox(TEXT("空文件"));
		return NULL;
	}

	pBuffer = (PCHAR)malloc((size_t)length);
	if (!pBuffer)
	{
		AfxMessageBox(TEXT("空间申请失败!"));
		return NULL;
	}
	memset(pBuffer, 0, (size_t)length);

	readLength = file.Read(pBuffer, (UINT)length);
	if (readLength != length)
	{
		free(pBuffer);
		AfxMessageBox(TEXT("文件读取不完整"));
		return NULL;
	}

	*pLengthOut = length;

	return pBuffer;
}

// 写出文件
BOOL C0001Obj::mWriteFile(LPCTSTR lpszPathName, PCHAR pBuffer, DWORD length)
{
	TCHAR text[0x200] = { 0 };
	BOOL result = 0;

	CFile file;
	CFileException fileException;

	if (!pBuffer || !length)
	{
		AfxMessageBox(TEXT("文件错误,写入失败!"));
		return FALSE;
	}

	result = file.Open(lpszPathName, CFile::modeCreate | CFile::modeWrite | CFile::typeBinary, &fileException);
	if (!result)
	{
		fileException.GetErrorMessage(text, 0x200);
		AfxMessageBox(text);
		return FALSE;
	}

	file.Write(pBuffer, length);
	file.Flush();
	return TRUE;
}

// 保存文件
void C0001Obj::OnFileSaveAs()
{
	DWORD result = 0;
	CString newFilePath;
	CFileDialog dlgFile(FALSE, NULL, NULL, OFN_HIDEREADONLY, _T("PE文件|*.*||"), NULL);
	if (dlgFile.DoModal() == IDOK)
	{
		newFilePath = dlgFile.GetPathName();
		if (newFilePath.GetLength())
		{
			// if (mWriteFile(newFilePath, m_pBuffer, m_dwLength))
				// setStatusBarText(newFilePath + _T(" :  已保存!"));
		}
	}
}
