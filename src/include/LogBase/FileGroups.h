#pragma once

struct _FileGroup
{
	CComBSTR location;
	CComBSTR filterString;
	LONGLONG expireSpan;

	_FileGroup()
	{
		expireSpan = 3;
	}
};

class XNode;

class FileGroups
{
public:
	FileGroups();
	~FileGroups();

	HRESULT Load(XNode* pXonNode);

	ULONG GetCount() { return m_count; }
	_FileGroup* GetAt(ULONG i) { return m_ppGroups[i]; }
	_FileGroup* operator[](ULONG i) { return m_ppGroups[i]; }

	HRESULT Add(LPCWSTR location, LPCWSTR filterString, LONGLONG expireSpan);
	HRESULT Add(_FileGroup* pDirectory);

	void Lock() { m_cs.Lock(); }
	void Unlock() { m_cs.Unlock(); }

protected:
	CComAutoCriticalSection m_cs;

	ULONG m_count;
	ULONG m_allocSize;
	_FileGroup** m_ppGroups;
};