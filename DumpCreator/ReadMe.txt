Program name: MinidumpCreator
DumpCreator is a Windows API console application to debug the target program (by launching it or by attaching to it). 
On an exception or a program failure, creates a minidump.
Also, logs process and thread exit events (with exit code and other relevant details).

Notes:
	1) Should run as an Administrator.
	2) To debug a 64-bit process, DumpCreator should be built as a 64-bit process, too.

Date: June 24, 2018

////////////////////////////////////////////////////////////////////////

TODO:
- implement the exception handler for the debuggee
- test both launching and attaching options
