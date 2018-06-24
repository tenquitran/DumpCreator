#include "stdafx.h"

//////////////////////////////////////////////////////////////////////////

using namespace DumpCreatorApp;

//////////////////////////////////////////////////////////////////////////


int _tmain(int argc, _TCHAR* argv[])
{
	// TODO: get arguments to launch the process or to attach to it.
	std::wstring exePath = L"C:\\Windows\\system32\\calc.exe";
	std::wstring args = L"-arg1 SomeValue -arg2 OtherValue";

	DumpCreator dumpCreator;

	if (!dumpCreator.launchProcess(exePath, args))
	{
		std::wcerr << L"Failed to launch \"" << exePath << L"\" as a debuggee\n";
		return 1;
	}

	return 0;
}
