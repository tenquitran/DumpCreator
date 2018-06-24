#pragma once


namespace DumpCreatorApp
{
	// Options to pass to the dump creator.
	struct DumpOptions
	{
	public:
		DumpOptions()
			: m_processId{}
		{
		}

	public:
		// Full path to the executable.
		std::wstring m_exePath;

		// Process arguments.
		std::wstring m_arguments;

		// ID of the process to attach to.
		DWORD m_processId;
	};
}
