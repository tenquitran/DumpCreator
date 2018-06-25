#include "stdafx.h"

//////////////////////////////////////////////////////////////////////////

using namespace DumpCreatorApp;

//////////////////////////////////////////////////////////////////////////

// Parse command-line arguments.
// Parameters: argc - number of command-line arguments;
//             argv - command-line arguments;
//             options - dump creation options.
// Returns: true if we can continue execution, false on an error.
bool parseArguments(int argc, _TCHAR* argv[], DumpOptions& options);

// Display usage information.
void displayUsage();

//////////////////////////////////////////////////////////////////////////


int _tmain(int argc, _TCHAR* argv[])
{
	DumpOptions options;

	if (!parseArguments(argc, argv, options))
	{
		// Error message has been already displayed.
		std::wcout << L"\nPress any key to exit...\n";
		getchar();
		return 1;
	}

	DumpCreator dumpCreator;

	switch (options.m_debugMethod)
	{
	case EDebugMethod::Launch:
		if (!dumpCreator.launchProcess(options))
		{
			return 2;
		}
		break;
	case EDebugMethod::Attach:
		if (!dumpCreator.attachToProcess(options))
		{
			return 3;
		}
		break;
	default:
		std::wcerr << L"Unexpected debug method\n";
		assert(false); break;
	}

	return 0;
}

bool parseArguments(int argc, _TCHAR* argv[], DumpOptions& options)
{
	if (argc < 2)
	{
		displayUsage();
		return false;
	}

	std::wstring dbgMethod = argv[1];

	if (0 == _wcsicmp(dbgMethod.c_str(), L"-L"))
	{
		// Launch process as a debuggee.
		//
		// Format:
		// > dumpcreator -L -p full_path_to_EXE [-args process_arguments]
		//
		// Example:
		// > -L - p "C:\Windows\system32\notepad.exe" - args "/W d:\file1.txt"

		if (argc < 4)
		{
			std::wcout << L"Error: not enough arguments for the launch mode\n\n";
			displayUsage();
			return false;
		}

		options.m_debugMethod = EDebugMethod::Launch;

		for (int i = 2; i < argc; i += 2)
		{
			std::wstring arg = argv[i];

			if (0 == _wcsicmp(arg.c_str(), L"-p"))
			{
				if (argc >= i + 1)
				{
					options.m_exePath = argv[i + 1];
				}
			} 
			else if (0 == _wcsicmp(arg.c_str(), L"-args"))
			{
				if (argc >= i + 1)
				{
					options.m_arguments = argv[i + 1];
				}
			}
		}

		if (options.m_exePath.empty())
		{
			std::wcout << L"Error: no EXE path\n\n";
			displayUsage();
			return false;
		}
	} 
	else if (0 == _wcsicmp(dbgMethod.c_str(), L"-A"))
	{
		// Attach to existing process.
		//
		// Format:
		// > dumpcreator -A -pid process_ID
		//
		// Example:
		// > dumpcreator -A -pid 1234

		if (argc < 4)
		{
			std::wcout << L"Error: not enough arguments for the attach mode\n\n";
			displayUsage();
			return false;
		}

		options.m_debugMethod = EDebugMethod::Attach;

		std::wstring arg = argv[2];

		// Convert PID to a number.
		if (0 == _wcsicmp(arg.c_str(), L"-pid"))
		{
			errno_t err = {};
			int processId = _ttoi(argv[3]);
			_get_errno(&err);

			// Error checks. Zero is not a valid number.
			if (   !processId
				|| INT_MAX == processId
				|| INT_MIN == processId
				|| ERANGE == err)
			{
				std::wcout << L"Error: failed to convert \"" << argv[3] << L"\" to process ID\n\n";
				displayUsage();
				return false;
			}
			else
			{
				options.m_processId = processId;
			}
		}

		if (0 == options.m_processId)
		{
			std::wcout << L"Error: no process ID\n\n";
			displayUsage();
			return false;
		}
	}
	else
	{
		std::wcout << L"Error: invalid method of debugging\n\n";
		displayUsage();
		return false;
	}

	return true;
}

void displayUsage()
{
	std::wcout << L"Usage:\n"
				  L"\tOption 1 - launch process as a debuggee:\n"
				  L"> dumpcreator -L -p full_path_to_EXE [-args process_arguments]\n\n"
				  L"\tOption 2 - attach to process:\n"
				  L"> dumpcreator -A -pid process_ID" << std::endl;
}
