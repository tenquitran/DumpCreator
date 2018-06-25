DumpCreator is a Windows API console application to debug a target program (by launching it or by attaching to its process). 
On an exception, creates a minidump. Displays the process exit event.

Notes:
	1) Should run as an Administrator.
	2) To debug a 64-bit process, DumpCreator should be built as a 64-bit process, too.

Date: June 24, 2018

////////////////////////////////////////////////////////////////////////

TODO:
- implement the exception handler for the debuggee
- test both launching and attaching options
