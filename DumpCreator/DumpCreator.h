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

		// Launch process as a debuggee.
		bool launchProcess(const DumpOptions& options);

		// Attach to the existing process.
		bool attachToProcess(const DumpOptions& options);

	private:
		// The debugger loop.
		void debugLoop(const LPDEBUG_EVENT pDbgEvent) const;

		DWORD OnException(const LPDEBUG_EVENT pDbgEvent) const;

		DWORD OnExitProcess(const LPDEBUG_EVENT pDbgEvent) const;

		DWORD OnRip(const LPDEBUG_EVENT pDbgEvent) const;

	private:
#if 1
		// Debugging options for the process.
		DumpOptions m_options;
#else
		// Full path to the executable.
		std::wstring m_exePath;

		// Process arguments.
		std::wstring m_arguments;
#endif
	};
}
