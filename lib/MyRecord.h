
#include <windows.h>
#include <stdio.h>
//#include "../two2/two2/pipe.h"

class CMyRecord //: public CPipe
{
private:
	UINT wDeviceID;
	int m_nCount;
HMODULE hMod;        // handle to loaded library module

public:
	CMyRecord();
	~CMyRecord();
	DWORD CMyRecord::RecordWAVEFile(DWORD dwMilliSeconds);
	void StopRecord();
};