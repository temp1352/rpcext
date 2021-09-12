#pragma once

class MemStream : public IStream
{
public:
	MemStream()
		: m_pStart(nullptr)
		, m_pCurr(nullptr)
		, m_pEnd(nullptr)
	{
	}

	HRESULT __stdcall QueryInterface(REFIID riid, void** ppv)
	{
		if (ppv == nullptr)
		{
			return E_POINTER;
		}

		*ppv = nullptr;

		if (InlineIsEqualGUID(riid, IID_IUnknown) ||
			InlineIsEqualGUID(riid, IID_IStream) ||
			InlineIsEqualGUID(riid, IID_ISequentialStream))
		{
			*ppv = static_cast<IStream*>(this);

			AddRef();

			return S_OK;
		}

		return E_NOINTERFACE;
	}

	ULONG __stdcall AddRef()
	{
		return 1;
	}

	ULONG __stdcall Release()
	{
		return 1;
	}

	HRESULT __stdcall Read(void* /*pDest*/, ULONG /*nMaxLen*/, ULONG* /*pnRead*/)
	{
		return E_NOTIMPL;
	}

	HRESULT __stdcall Write(const void* /*pv*/, ULONG /*cb*/, ULONG* /*pcbWritten*/)
	{
		return E_NOTIMPL;
	}

	HRESULT __stdcall Seek(LARGE_INTEGER dlibMove, DWORD dwOrigin, ULARGE_INTEGER* pLibNewPosition)
	{
		BYTE* pByte = m_pStart;
		if (dwOrigin == STREAM_SEEK_CUR)
			pByte = m_pCurr;
		else if (dwOrigin == STREAM_SEEK_END)
			pByte = m_pEnd;

		pByte += dlibMove.QuadPart;
		if (pByte >= m_pStart && pByte <= m_pEnd)
			m_pCurr = pByte;
		else
			XTL_THROW(E_ABORT);

		if (pLibNewPosition)
		{
			pLibNewPosition->QuadPart = (ULONGLONG)(m_pCurr - m_pStart);
		}

		return S_OK;
	}

	HRESULT __stdcall SetSize(ULARGE_INTEGER /*libNewSize*/)
	{
		return E_NOTIMPL;
	}

	HRESULT __stdcall CopyTo(IStream* /*pStream*/, ULARGE_INTEGER /*cb*/,
		ULARGE_INTEGER* /*pcbRead*/, ULARGE_INTEGER* /*pcbWritten*/)
	{
		return E_NOTIMPL;
	}

	HRESULT __stdcall Commit(DWORD /*grfCommitFlags*/)
	{
		return E_NOTIMPL;
	}

	HRESULT __stdcall Revert()
	{
		return E_NOTIMPL;
	}

	HRESULT __stdcall LockRegion(ULARGE_INTEGER /*libOffset*/, ULARGE_INTEGER /*cb*/, DWORD /*dwLockType*/)
	{
		return E_NOTIMPL;
	}

	HRESULT __stdcall UnlockRegion(ULARGE_INTEGER /*libOffset*/, ULARGE_INTEGER /*cb*/, DWORD /*dwLockType*/)
	{
		return E_NOTIMPL;
	}

	HRESULT __stdcall Stat(STATSTG* pstatstg, DWORD /*grfStatFlag*/)
	{
		pstatstg->cbSize.QuadPart = (ULONGLONG)(m_pEnd - m_pStart);

		return S_OK;
	}

	HRESULT __stdcall Clone(IStream** /*ppstm*/)
	{
		return E_NOTIMPL;
	}

protected:
	BYTE* m_pStart;
	BYTE* m_pCurr;
	BYTE* m_pEnd;
};

class ReadStream : public MemStream
{
public:
	ReadStream(const BYTE* pStart, ULONG nLen)
	{
		ATLASSERT(pStart);

		m_pStart = m_pCurr = (BYTE*)pStart;
		m_pEnd = m_pStart + nLen;
	}

	HRESULT __stdcall Read(void* pv, ULONG cb, ULONG* pcbRead)
	{
		ATLASSERT(pv);

		int dwRead = min(m_pEnd - m_pCurr, cb);
		if (dwRead > 0)
		{
			memcpy(pv, m_pCurr, dwRead);
			m_pCurr += dwRead;

			if (pcbRead)
				*pcbRead = dwRead;
		}
		else if (pcbRead)
		{
			*pcbRead = 0;
		}

		return S_OK;
	}
};

class WriteStream : public MemStream
{
public:
	WriteStream(int size = 4096) throw(...)
	{
		ATLASSERT(size > 0);
		m_pStart = (BYTE*)malloc(size);
		if (!m_pStart)
		{
			XTL_THROW(E_OUTOFMEMORY);
		}

		m_pCurr = m_pStart;
		m_pEnd = m_pStart + size;
	}

	~WriteStream()
	{
		if (m_pStart)
		{
			free(m_pStart);
		}
	}

	const BYTE* GetData()
	{
		return m_pStart;
	}

	int GetSize()
	{
		return (int)(m_pCurr - m_pStart);
	}

	BYTE* Detach()
	{
		BYTE* pData = m_pStart;
		m_pStart = m_pCurr = m_pEnd = nullptr;

		return pData;
	}

	HRESULT __stdcall Write(const void* pv, ULONG cb, ULONG* pcbWritten)
	{
		ATLASSERT(pv);

		if (m_pCurr + cb > m_pEnd)
		{
			int size = (int)(m_pCurr + cb - m_pStart) * 2;
			BYTE* pStart = (BYTE*)realloc(m_pStart, size);
			if (!pStart)
				return E_OUTOFMEMORY;

			m_pCurr = pStart + (m_pCurr - m_pStart);
			m_pStart = pStart;
			m_pEnd = pStart + size;
		}

		memcpy(m_pCurr, pv, cb);

		m_pCurr += cb;

		if (pcbWritten != nullptr)
		{
			*pcbWritten = (DWORD)cb;
		}

		return S_OK;
	}
};