#include "pch.h"
#include "framework.h"
// SHARED_HANDLERS 可以在实现预览、缩略图和搜索筛选器句柄的
// ATL 项目中进行定义，并允许与该项目共享文档代码。
#ifndef SHARED_HANDLERS
#include "Sdi.h"
#endif

#include "SdiDoc.h"

#include <propkey.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


IMPLEMENT_DYNCREATE(CSdiDoc, CDocument)

BEGIN_MESSAGE_MAP(CSdiDoc, CDocument)
END_MESSAGE_MAP()



CSdiDoc::CSdiDoc() noexcept
{

}

CSdiDoc::~CSdiDoc()
{
}

BOOL CSdiDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;


	return TRUE;
}







