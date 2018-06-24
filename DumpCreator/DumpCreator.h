#pragma once


namespace DumpCreatorApp
{
	// Creates a dump of the user-mode application specified.
	// Each application requires a separate instance of this class.
	class DumpCreator
	{
	public:
		DumpCreator();

		virtual ~DumpCreator();

		// Launch to the process as a debuggee.
		// Parameters: path - full path to the executable;
		//             args - process arguments (can be empty).
		bool launchProcess(const std::wstring& path, const std::wstring& args);

	private:
		// The debugger loop.
		void debugLoop(const LPDEBUG_EVENT pDbgEvent) const;

		DWORD OnException(const LPDEBUG_EVENT pDbgEvent) const;

		DWORD OnExitProcess(const LPDEBUG_EVENT pDbgEvent) const;

		DWORD OnRip(const LPDEBUG_EVENT pDbgEvent) const;

	private:
		// Full path to the executable.
		std::wstring m_exePath;

		// Process arguments.
		std::wstring m_arguments;
	};
}
