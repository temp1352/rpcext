#pragma once

class TokenInformation
{
public:
	TokenInformation()
	{
		m_pBuffer = nullptr;
	}

	~TokenInformation()
	{
		if (m_pBuffer)
		{
			free(m_pBuffer);
		}
	}

	HRESULT Init(HANDLE hToken, TOKEN_INFORMATION_CLASS tiClass)
	{
		DWORD dwLen = 0;
		if (!GetTokenInformation(hToken, tiClass, nullptr, 0, &dwLen))
		{
			DWORD dwError = GetLastError();
			if (dwError != ERROR_INSUFFICIENT_BUFFER)
				return HRESULT_FROM_WIN32(dwError);
		}

		void* pBuffer = malloc(dwLen);
		if (!pBuffer)
			return E_OUTOFMEMORY;

		if (!GetTokenInformation(hToken, tiClass, pBuffer, dwLen, &dwLen))
		{
			free(pBuffer);

			return AtlHresultFromLastError();
		}

		if (m_pBuffer)
			free(m_pBuffer);

		m_pBuffer = pBuffer;

		return S_OK;
	}

	void* GetBuffer()
	{
		return m_pBuffer;
	}

protected:
	void* m_pBuffer;
};
