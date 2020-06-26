#include "DigitalSignatureScan.h"

BOOL DigitalSignatureScan::PrintCertificateInfo(PCCERT_CONTEXT pCertContext)
{
	BOOL fReturn = FALSE;
	LPTSTR szName = NULL;
	DWORD dwData;

		// Print Serial Number.
		_tprintf(_T("Serial Number: "));
		dwData = pCertContext->pCertInfo->SerialNumber.cbData;
		for (DWORD n = 0; n < dwData; n++)
		{
			_tprintf(_T("%02x"),
				pCertContext->pCertInfo->SerialNumber.pbData[dwData - (n + 1)]);
		}
		_tprintf(_T("\n"));

		// Get Issuer name size.
		if (!(dwData = CertGetNameString(pCertContext,	CERT_NAME_SIMPLE_DISPLAY_TYPE,	CERT_NAME_ISSUER_FLAG,	NULL,NULL, 0)))
		{
			WelfareLog("CertGetNameString ERROR(%i).\n", GetLastError());
			return FALSE;
		}

		// Allocate memory for Issuer name.
		szName = (LPTSTR)LocalAlloc(LPTR, dwData * sizeof(TCHAR));
		if (!szName)
		{
			WelfareLog("LocalAlloc ERROR(%i).\n", GetLastError());
			return FALSE;
		}

		// Get Issuer name.
		if (!CertGetNameString(pCertContext, CERT_NAME_SIMPLE_DISPLAY_TYPE, CERT_NAME_ISSUER_FLAG, NULL, szName, dwData))
		{
			WelfareLog("CertGetNameString ERROR(%i).\n", GetLastError());
			return FALSE;
		}

		// print Issuer name.
		_tprintf(_T("Issuer Name: %s\n"), szName);
		LocalFree(szName);
		szName = NULL;

		// Get Subject name size.
		if (!(dwData = CertGetNameString(pCertContext,	CERT_NAME_SIMPLE_DISPLAY_TYPE,0,NULL,NULL,0)))
		{
			WelfareLog("CertGetNameString ERROR(%i).\n", GetLastError());
			return FALSE;
		}

		// Allocate memory for subject name.
		szName = (LPTSTR)LocalAlloc(LPTR, dwData * sizeof(TCHAR));
		if (!szName)
		{
			WelfareLog("LocalAlloc ERROR(%i).\n", GetLastError());
			return FALSE;
		}

		// Get subject name.
		if (!(CertGetNameString(pCertContext, CERT_NAME_SIMPLE_DISPLAY_TYPE, 0,	NULL,szName,	dwData)))
		{
			WelfareLog("LocalAlloc ERROR(%i).\n", GetLastError());
			return FALSE;
		}
		// Print Subject Name.
		_tprintf(_T("Subject Name: %s\n"), szName);
		fReturn = TRUE;

	if (szName != NULL) 
		LocalFree(szName);

	return fReturn;
}

BOOL DigitalSignatureScan::CertScan(const char*FileName)
{
	HCERTSTORE hStore = NULL;
	HCRYPTMSG hMsg = NULL;
	PCCERT_CONTEXT pCertContext = NULL;
	BOOL fResult = TRUE;
	DWORD dwEncoding = 0, dwContentType = 0, dwFormatType = 0;
	PCMSG_SIGNER_INFO pSignerInfo = NULL;
	DWORD dwSignerInfo = 0;
	CERT_INFO CertInfo;
	bool bRet = false;
	WCHAR szFileName[MAX_PATH];

	if (mbstowcs(szFileName, FileName, MAX_PATH) == -1)
	{
		WelfareLog("mbstowcs ERROR(%i).\n", GetLastError());
		return FALSE;
	}

	if (!CryptQueryObject(CERT_QUERY_OBJECT_FILE, szFileName, CERT_QUERY_CONTENT_FLAG_PKCS7_SIGNED_EMBED, CERT_QUERY_FORMAT_FLAG_BINARY, 0, &dwEncoding, &dwContentType, &dwFormatType, &hStore, &hMsg, NULL))
	{
		WelfareLog("CryptQueryObject ERROR(%i).\n", GetLastError());
		return FALSE;
	}

	if (!CryptMsgGetParam(hMsg, CMSG_SIGNER_INFO_PARAM, 0, NULL, &dwSignerInfo))
	{
		WelfareLog("CryptMsgGetParam ERROR(%i).\n", GetLastError());
		return FALSE;
	}

	pSignerInfo = (PCMSG_SIGNER_INFO)LocalAlloc(LPTR, dwSignerInfo);
	if (!pSignerInfo)
	{
		WelfareLog("pSignerInfo ERROR(%i).\n", GetLastError());
		return FALSE;
	}

	if (!CryptMsgGetParam(hMsg, CMSG_SIGNER_INFO_PARAM, 0, (PVOID)pSignerInfo, &dwSignerInfo))
	{
		WelfareLog("CryptMsgGetParam ERROR(%i).\n", GetLastError());
		return FALSE;
	}

	CertInfo.Issuer = pSignerInfo->Issuer;
	CertInfo.SerialNumber = pSignerInfo->SerialNumber;

	pCertContext = CertFindCertificateInStore(hStore, (X509_ASN_ENCODING | PKCS_7_ASN_ENCODING), 0, CERT_FIND_SUBJECT_CERT, (PVOID)&CertInfo, NULL);
	if (!pCertContext)
	{
		WelfareLog("CertFindCertificateInStore ERROR(%i).\n", GetLastError());
		return FALSE;
	}
	
	bRet = PrintCertificateInfo(pCertContext);

	if (pSignerInfo != NULL)
		LocalFree(pSignerInfo);
	if (pCertContext != NULL)
		CertFreeCertificateContext(pCertContext);
	if (hStore != NULL)
		CertCloseStore(hStore, 0);
	if (hMsg != NULL)
		CryptMsgClose(hMsg);

	return bRet;
}

/*bool DigitalSignatureScan::ManageFsRedirection(bool bDisable, PVOID pCookie, PVOID * ppCookie)
{
	BOOL bIsWow64 = FALSE;
	if (IsWow64Process(GetCurrentProcess(), &bIsWow64))
	{
		PVOID OldValue = NULL;
		if (bDisable && !Wow64DisableWow64FsRedirection(&OldValue))
		{
			if (ppCookie) *ppCookie = OldValue;
			return false;
		}

		if (!bDisable && !Wow64RevertWow64FsRedirection(pCookie))
			return false;
	}
	return true;
}*/

VOID ErrorExit(PTSTR lpszFunction, DWORD dw)
{
	LPVOID lpMsgBuf;
	LPVOID lpDisplayBuf;
	

	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM |
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		dw,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf,
		0, NULL);

	lpDisplayBuf = (LPVOID)LocalAlloc(LMEM_ZEROINIT,
		(lstrlen((LPCTSTR)lpMsgBuf) + lstrlen((LPCTSTR)lpszFunction) + 40) * sizeof(TCHAR));
	StringCchPrintf((LPTSTR)lpDisplayBuf,
		LocalSize(lpDisplayBuf) / sizeof(TCHAR),
		TEXT("%s failed with error 0x%X: %s"),
		lpszFunction, dw, lpMsgBuf);
	MessageBox(NULL, (LPCTSTR)lpDisplayBuf, TEXT("WelFareAntiCheat"), MB_ICONERROR | MB_SYSTEMMODAL | MB_HELP);

	LocalFree(lpMsgBuf);
	LocalFree(lpDisplayBuf);
	ExitProcess(1);
}

BOOL DigitalSignatureScan::IsFileDigitalSignedRoutine(const char *FileName)
{
	WelfareLog("Digital sign check started! Target file: %ls", FileName);
	WCHAR szFileName[MAX_PATH];
	bool bResult = false;
	LONG lStatus = ERROR_SUCCESS;
	DWORD dwLastError = ERROR_SUCCESS;
	GUID WVTPolicyGUID = WINTRUST_ACTION_GENERIC_VERIFY_V2;


	if (mbstowcs(szFileName, FileName, MAX_PATH) == -1)
	{
		WelfareLog("mbstowcs ERROR(%i).\n", GetLastError());
		return FALSE;
	}

	// Initialize the WINTRUST_FILE_INFO structure.
	WINTRUST_FILE_INFO FileData = { 0 };
	FileData.cbStruct = sizeof(WINTRUST_FILE_INFO);
	FileData.pcwszFilePath = szFileName;
	FileData.hFile = NULL;
	FileData.pgKnownSubject = NULL;

	// Initialize the WinVerifyTrust input data structure.
	WINTRUST_DATA WinTrustData = { 0 };
	WinTrustData.cbStruct = sizeof(WinTrustData);
	WinTrustData.pPolicyCallbackData = NULL; // Use default code signing EKU.	
	WinTrustData.pSIPClientData = NULL; // No data to pass to SIP.
	WinTrustData.dwUIChoice = WTD_UI_NONE; // Disable WVT UI.	
	WinTrustData.fdwRevocationChecks = WTD_REVOKE_NONE; // No revocation checking.	
	WinTrustData.dwUnionChoice = WTD_CHOICE_FILE; // Verify an embedded signature on a file.	
	WinTrustData.dwStateAction = WTD_STATEACTION_VERIFY; // Verify action.
	WinTrustData.hWVTStateData = NULL; // Verification sets this value.
	WinTrustData.pwszURLReference = NULL; // Not used.
	WinTrustData.dwUIContext = 0; // This is not applicable if there is no UI because it changes, the UI to accommodate running applications instead of installing applications.
	WinTrustData.pFile = &FileData; // Set pFile.

	// Disables file system redirection for the calling thread. Redirection cause to 0x80092003 - CRYPT_E_FILE_ERROR
	//PVOID OldValue = NULL;
	//if (!ManageFsRedirection(true, nullptr, &OldValue))
	//	return bResult;

	// WinVerifyTrust verifies signatures as specified by the GUID and Wintrust_Data.
	lStatus = WinVerifyTrust(NULL, &WVTPolicyGUID, &WinTrustData);

	switch (lStatus)
	{
	case ERROR_SUCCESS:
	{
		//WelfareLog("The file \"%ls\" is signed and the signature was verified.", pwszSourceFile);
		bResult = true;
	} break;

	case TRUST_E_NOSIGNATURE:
	{
		// The file was not signed or had a signature 
		// that was not valid.

		// Get the reason for no signature.
		dwLastError = GetLastError();
		if (TRUST_E_NOSIGNATURE == dwLastError || TRUST_E_SUBJECT_FORM_UNKNOWN == dwLastError || TRUST_E_PROVIDER_UNKNOWN == dwLastError)
		{
			// The file was not signed.
		//	DEBUG_LOG(LL_ERR, "The file \"%ls\" is not signed.", pwszSourceFile);
		}
		else
		{
			// The signature was not valid or there was an error 
			// opening the file.
			WelfareLog("An unknown error occurred trying to verify the signature of the \"%ls\" file.", FileName);
		}
	} break;

	case TRUST_E_EXPLICIT_DISTRUST:
	{
		// The hash that represents the subject or the publisher 
		// is not allowed by the admin or user.
		WelfareLog("The signature is present, but specifically disallowed.\n");
	} break;

	case TRUST_E_SUBJECT_NOT_TRUSTED:
	{
		// The user clicked "No" when asked to install and run.
		WelfareLog("The signature is present, but not trusted.\n");
	} break;

	case CRYPT_E_SECURITY_SETTINGS:
	{
		/*
		The hash that represents the subject or the publisher
		was not explicitly trusted by the admin and the
		admin policy has disabled user trust. No signature,
		publisher or time stamp errors.
		*/
		WelfareLog("CRYPT_E_SECURITY_SETTINGS - The hash representing the subject or the publisher wasn't explicitly trusted by the admin and admin policy has disabled user trust. No signature, publisher or timestamp errors.\n");
	} break;

	default:
	{
		// The UI was disabled in dwUIChoice or the admin policy 
		// has disabled user trust. lStatus contains the 
		// publisher or time stamp chain error.
		WelfareLog("WinVerifyTrust fail! Error code: 0x%x.\n", lStatus);
		ErrorExit((PTSTR)"WinVerifyTrust", lStatus);
	} break;
	}

	// Any hWVTStateData must be released by a call with close.
	WinTrustData.dwStateAction = WTD_STATEACTION_CLOSE;

	// Set created structs
	lStatus = WinVerifyTrust(NULL, &WVTPolicyGUID, &WinTrustData);

	// Restores file system redirection for the calling thread.
	//if (!ManageFsRedirection(false, OldValue, nullptr))
	//	return bResult;

	SetLastError(lStatus);
	return bResult;
}

