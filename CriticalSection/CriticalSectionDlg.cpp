#include "CriticalSectionDlg.h"

CriticalSectionDlg* CriticalSectionDlg::ptr = NULL;

CRITICAL_SECTION cs;

CriticalSectionDlg::CriticalSectionDlg(void)
{
	ptr = this;
}

CriticalSectionDlg::~CriticalSectionDlg(void)
{
	DeleteCriticalSection(&cs);
}

void CriticalSectionDlg::Cls_OnClose(HWND hwnd)
{
	EndDialog(hwnd, 0);
}

BOOL CriticalSectionDlg::Cls_OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam) 
{
	InitializeCriticalSection(&cs);
	return TRUE;
}

void WriteArrayToFile(wchar_t Path[],int Arr[],const int ArrSize) {
	ofstream out1(Path,ios::app);
	if (!out1.is_open())
	{
		MessageBox(0, TEXT("Error"), TEXT("Критическая секция"), MB_OK);
		return;
	}
	for (int i = 0; i < ArrSize; i++)
	{
		out1 << Arr[i] << ' ';
	}
	out1.close();
}

void ReadArrayFromFile(wchar_t Path[], int Arr[], const int ArrSize) {
	ifstream in(Path);
	if (!in.is_open())
	{
		MessageBox(0, TEXT("Error"), TEXT("Error"), MB_OK);
		return;
	}
	for (int i = 0; i < 5; i++)
	{
		in >> Arr[i];
	}
	in.close();
}

DWORD WINAPI Write(LPVOID lp)
{
	srand(time(0));
	EnterCriticalSection(&cs);
	const int ArrSize = 5;
	int Arr[ArrSize]={ 1,2,3,4,5 };
	WriteArrayToFile(TEXT("array1.txt"),Arr,ArrSize);
	WriteArrayToFile(TEXT("array2.txt"),Arr,ArrSize);
	WriteArrayToFile(TEXT("array3.txt"),Arr,ArrSize);
	LeaveCriticalSection(&cs);
	MessageBox(0, TEXT("Поток записал информацию в файл"), TEXT("Критическая секция"), MB_OK);
	return 0;
}

DWORD WINAPI Read(LPVOID lp)
{
	EnterCriticalSection(&cs);
	const int ArrSize = 5;
	int ArrBuff[ArrSize];
	ReadArrayFromFile(TEXT("array1.txt"), ArrBuff, ArrSize);
	WriteArrayToFile(TEXT("arrayfinal.txt"), ArrBuff, ArrSize);
	ReadArrayFromFile(TEXT("array2.txt"), ArrBuff, ArrSize);
	WriteArrayToFile(TEXT("arrayfinal.txt"), ArrBuff, ArrSize);
	ReadArrayFromFile(TEXT("array2.txt"), ArrBuff, ArrSize);
	WriteArrayToFile(TEXT("arrayfinal.txt"), ArrBuff, ArrSize);
	LeaveCriticalSection(&cs);
	MessageBox(0, TEXT("Поток прочитал информацию из файла"), TEXT("Критическая секция"), MB_OK);
	return 0;
}


void CriticalSectionDlg::Cls_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
{
	if(id == IDC_BUTTON1)
	{
		HANDLE hThread = CreateThread(NULL, 0, Write, 0, 0, NULL); 
		CloseHandle(hThread);
		hThread = CreateThread(NULL, 0, Read, 0, 0, NULL);  
		CloseHandle(hThread);
	}
}

BOOL CALLBACK CriticalSectionDlg::DlgProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message)
	{
		HANDLE_MSG(hwnd, WM_CLOSE, ptr->Cls_OnClose);
		HANDLE_MSG(hwnd, WM_INITDIALOG, ptr->Cls_OnInitDialog);
		HANDLE_MSG(hwnd, WM_COMMAND, ptr->Cls_OnCommand);
	}
	return FALSE;
}