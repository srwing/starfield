#include "MyRecord.h"	
//#include "../two2/wavrec/pipe.h"
//#include "../two2/wavrec/wavrecdef.h"
//#pragma comment (lib, "wavrec.lib")



CMyRecord::CMyRecord()
{
	// returns a handle to the DLL, otherwise NULL
}

CMyRecord::~CMyRecord()
{
    mciSendCommand(wDeviceID, MCI_CLOSE, 0, NULL);
//        return (0L);
//__declspec( dllimport ) void 
//	StopRecord();

}
void CMyRecord::StopRecord()
{
//	StopRecord();
}

DWORD CMyRecord::RecordWAVEFile(DWORD dwMilliSeconds)
{
    DWORD dwReturn;

//__declspec( dllimport ) void 
//	Record();


    MCI_OPEN_PARMS mciOpenParms;
    MCI_RECORD_PARMS mciRecordParms;
    MCI_SAVE_PARMS mciSaveParms;
    MCI_PLAY_PARMS mciPlayParms;

    // Open a waveform-audio device with a new file for recording.
    mciOpenParms.lpstrDeviceType = "waveaudio";
    mciOpenParms.lpstrElementName = "";
    if (dwReturn = mciSendCommand(0, MCI_OPEN,
        MCI_OPEN_ELEMENT | MCI_OPEN_TYPE, 
        (DWORD)(LPVOID) &mciOpenParms))
    {
        // Failed to open device; don't close it, just return error.
        return (dwReturn);
    }

    // The device opened successfully; get the device ID.
    wDeviceID = mciOpenParms.wDeviceID;

    // Begin recording and record for the specified number of 
    // milliseconds. Wait for recording to complete before continuing. 
    // Assume the default time format for the waveform-audio device 
    // (milliseconds).
    mciRecordParms.dwTo = dwMilliSeconds;
    if (dwReturn = mciSendCommand(wDeviceID, MCI_RECORD, 
        MCI_TO | MCI_WAIT, (DWORD)(LPVOID) &mciRecordParms))
    {
        mciSendCommand(wDeviceID, MCI_CLOSE, 0, NULL);
        return (dwReturn);
    }

    // Play the recording and query user to save the file.
    mciPlayParms.dwFrom = 0L;
    if (dwReturn = mciSendCommand(wDeviceID, MCI_PLAY,
        MCI_FROM | MCI_WAIT, (DWORD)(LPVOID) &mciPlayParms))
    {
        mciSendCommand(wDeviceID, MCI_CLOSE, 0, NULL);
        return (dwReturn);
    }
//    if (MessageBox("Do you want to save this recording?",
//        "", MB_YESNO) == IDNO)
//    {
//        mciSendCommand(wDeviceID, MCI_CLOSE, 0, NULL);
//        return (0L);
//    }

    // Save the recording to a file named TEMPFILE.WAV. Wait for
    // the operation to complete before continuing.
    mciSaveParms.lpfilename = "tempfile.wav";
    if (dwReturn = mciSendCommand(wDeviceID, MCI_SAVE,
        MCI_SAVE_FILE | MCI_WAIT, (DWORD)(LPVOID) &mciSaveParms))
    {
        mciSendCommand(wDeviceID, MCI_CLOSE, 0, NULL);
        return (dwReturn);
    }

    return (0L);

}
