#include "stdafx.h"
#include "DumpOptions.h"

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
#if 1
	DumpOptions options;

	// TODO: test

	if (!parseArguments(argc, argv, options))
	{
		// Error message has been already displayed.
		std::wcout << L"\nPress any key to exit...\n";
		getchar();
		return 1;
	}
#else
	// TODO: get arguments to launch the process or to attach to it.
	std::wstring exePath = L"C:\\Windows\\system32\\calc.exe";
	std::wstring args = L"-arg1 SomeValue -arg2 OtherValue";

	DumpCreator dumpCreator;

	if (!dumpCreator.launchProcess(exePath, args))
	{
		std::wcerr << L"Failed to launch \"" << exePath << L"\" as a debuggee\n";
		return 2;
	}

	// TODO: add attachment functionality
#endif

	return 0;
}

bool parseArguments(int argc, _TCHAR* argv[], DumpOptions& options)
{
	if (1 == argc)
	{
		displayUsage();
		return false;
	}

	std::wstring dbgMethod = argv[1];

	if (0 == _wcsicmp(dbgMethod.c_str(), L"-L"))
	{
		// Launch as a debuggee.

		// > dumpcreator -L -p full_path_to_EXE [-args process_arguments]

		for (int i = 2; i < argc; ++i)
		{
			std::wstring arg = argv[i];

			if (0 == _wcsicmp(dbgMethod.c_str(), L"-p"))
			{
				if (argc >= i + 1)
				{
					options.m_exePath = argv[i + 1];
				}
			} 
			else if (0 == _wcsicmp(dbgMethod.c_str(), L"-args"))
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

		// > dumpcreator -A -pid process_ID

		if (0 == _wcsicmp(dbgMethod.c_str(), L"-pid"))
		{
			if (argc >= 4)
			{
				errno_t err = 0;
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
		std::wcout << L"Error: invalid option\n\n";
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
