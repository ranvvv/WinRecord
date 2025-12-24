#pragma once


class CSdiDoc : public CDocument
{
	DECLARE_DYNCREATE(CSdiDoc)
	DECLARE_MESSAGE_MAP()

protected: // 仅从序列化创建
	CSdiDoc() noexcept;
public:
	virtual ~CSdiDoc();

public:
	virtual BOOL OnNewDocument();


};
