#include "stdafx.h"

//////////////////////////////////////////////////////////////////////////

using namespace DumpCreatorApp;

//////////////////////////////////////////////////////////////////////////


DumpCreator::DumpCreator()
{
}

DumpCreator::~DumpCreator()
{
}

bool DumpCreator::launchProcess(const DumpOptions& options)
{
	m_options = options;

	STARTUPINFO si = {};
	si.cb = sizeof(STARTUPINFO);
	// ... you can specify standard input, output and error handles, etc.

	PROCESS_INFORMATION pi = {};

	WCHAR *pArgs = nullptr;
	std::unique_ptr<WCHAR[]> spArgs;

	// Copy the process arguments, if any, to the modifiable buffer.
	if (!m_options.m_arguments.empty())
	{
		try
		{
			spArgs = std::make_unique<WCHAR[]>(m_options.m_arguments.length() + 1);

			errno_t err = wmemcpy_s(spArgs.get(), m_options.m_arguments.length() + 1,
									m_options.m_arguments.c_str(), m_options.m_arguments.length());
			if (0 != err)
			{
				std::wcerr << L"Failed to copy process arguments: " << err << '\n';
				assert(false); return false;
			}
			spArgs[m_options.m_arguments.length()] = '\0';

			pArgs = spArgs.get();
		}
		catch (const std::bad_alloc&)
		{
			std::wcerr << L"Memalloc failure on copying process arguments\n";
			assert(false); return false;
		}
	}

	if (!CreateProcess(m_options.m_exePath.c_str(), pArgs, nullptr, nullptr, FALSE, DEBUG_PROCESS, nullptr, nullptr, &si, &pi))
	{
		std::wcerr << L"CreateProcess() failed (" << m_options.m_exePath << L"): " << GetLastError() << '\n';
		return false;
	}
	CHandle hProcess(pi.hProcess);
	CHandle hThread(pi.hThread);

	std::wcout << L"Process \"" << m_options.m_exePath << L"\" launched as a debuggee" << std::endl;

	DEBUG_EVENT dbgEvent = {};

	debugLoop(&dbgEvent);

	return true;
}

bool DumpCreator::attachToProcess(const DumpOptions& options)
{
	m_options = options;

	if (!DebugActiveProcess(options.m_processId))
	{
		DWORD err = GetLastError();

		if (ERROR_NOT_SUPPORTED == err)
		{
			std::wcerr << L"DebugActiveProcess() failed: " << err << L" (PID = " << options.m_processId << L"). "
						  L"Please check the bitness of DumpCreator\n";
		} 
		else
		{
			std::wcerr << L"DebugActiveProcess() failed: " << err << L" (PID = " << options.m_processId << L")\n";
		}
		
		return false;
	}

	std::wcout << L"Attached for the process (PID = " << options.m_processId << L") for debugging" << std::endl;

	DEBUG_EVENT dbgEvent = {};

	debugLoop(&dbgEvent);

	return true;
}

void DumpCreator::debugLoop(const LPDEBUG_EVENT pDbgEvent) const
{
	DWORD continueStatus = DBG_CONTINUE;    // exception continuation

	for (;;)
	{
		if (!WaitForDebugEvent(pDbgEvent, INFINITE))
		{
			std::wcerr << L"WaitForDebugEvent() failed: " << GetLastError() << '\n';
			return;
		}

		// Process the debugging event.
		switch (pDbgEvent->dwDebugEventCode)
		{
		case EXCEPTION_DEBUG_EVENT:
			OnException(pDbgEvent);
			break;
		case EXIT_PROCESS_DEBUG_EVENT:
			continueStatus = OnExitProcess(pDbgEvent);

			// Stop debugging.
			if (!DebugActiveProcessStop(pDbgEvent->dwProcessId))
			{
				std::wcerr << L"Failed to stop debugging the process (process exit): " << GetLastError() << '\n';
			}
			return;
		case RIP_EVENT:
			continueStatus = OnRip(pDbgEvent);

			// Stop debugging, just in case.
			if (!DebugActiveProcessStop(pDbgEvent->dwProcessId))
			{
				std::wcerr << L"Failed to stop debugging the process (RIP event): " << GetLastError() << '\n';
			}
			return;
		default:    // ignore other events
			break;
		}

		// Resume executing the thread that reported the debugging event.
		ContinueDebugEvent(pDbgEvent->dwProcessId,
			pDbgEvent->dwThreadId,
			continueStatus);
	}
}

DWORD DumpCreator::OnException(const LPDEBUG_EVENT pDbgEvent) const
{
	// TODO: implement. In some or all cases, we need to create a minidump.

	// Process the exception code. When handling 
	// exceptions, remember to set the continuation 
	// status parameter (continueStatus). This value is used by ContinueDebugEvent.
	switch (pDbgEvent->u.Exception.ExceptionRecord.ExceptionCode)
	{
	case EXCEPTION_ACCESS_VIOLATION:
		// First chance: Pass this on to the system.
		// Last chance: Display an appropriate error.
		break;
	case EXCEPTION_BREAKPOINT:
		// First chance: Display the current instruction and register values.
		break;
	case EXCEPTION_DATATYPE_MISALIGNMENT:
		// First chance: Pass this on to the system.
		// Last chance: Display an appropriate error.
		break;
	case EXCEPTION_SINGLE_STEP:
		// First chance: Update the display of the current instruction and register values.
		break;
	case DBG_CONTROL_C:
		// First chance: Pass this on to the system.
		// Last chance: Display an appropriate error.

		// TODO: stop debugging?
		break;
	default:
		// Handle other exceptions. 
		break;
	}

	return DBG_CONTINUE;
}

DWORD DumpCreator::OnExitProcess(const LPDEBUG_EVENT pDbgEvent) const
{
	// Displaying the EXE path is not required: this has been done on launch.
	std::wcout << L"Debuggee process exit: " << pDbgEvent->u.ExitProcess.dwExitCode << std::endl;

	return DBG_CONTINUE;
}

DWORD DumpCreator::OnRip(const LPDEBUG_EVENT pDbgEvent) const
{
	// Displaying the EXE path is not required: this has been done on launch.
	std::wcerr << L"RIP event (system debugging error), code " << pDbgEvent->u.RipInfo.dwError;

	switch (pDbgEvent->u.RipInfo.dwType)
	{
	case SLE_ERROR:
		std::wcout << L". Invalid data was passed to the function causing the app failure";
		break;
	case SLE_MINORERROR:
		std::wcout << L". Invalid data was passed to the function but the app probably didn\'t fail";
		break;
	case SLE_WARNING:
		std::wcout << L". Potentially invalid data was passed to the function but the function completed processing";
		break;
	case 0:    // only the error code is set
		break;
	default:
		break;
	}

	std::wcerr << '\n';

	return DBG_CONTINUE;
}
