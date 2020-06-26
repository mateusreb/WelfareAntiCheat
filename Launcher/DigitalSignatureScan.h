#pragma once
#include "stdafx.h"

class DigitalSignatureScan {
	static BOOL PrintCertificateInfo(PCCERT_CONTEXT pCertContext);
	static BOOL CertScan(const char*FileName);
	static BOOL IsFileDigitalSignedRoutine(const char *FileName);
};