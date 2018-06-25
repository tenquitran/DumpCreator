#pragma once


namespace DumpCreatorApp
{
	// Method of the process debugging.
	enum class EDebugMethod
	{
		Undefined,
		Launch,    // launch a new process
		Attach     // attach to the existing process
	};
}
