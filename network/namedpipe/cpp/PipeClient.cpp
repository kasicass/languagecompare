#include <windows.h>
#include <stdio.h>
#include <conio.h>

#define BUFSIZE 512

int main(int argc, char *argv[])
{
	HANDLE hPipe;
	const char* msg = "Default message from client.";
	char buf[BUFSIZE];
	BOOL success = FALSE;
	DWORD read, toWrite, written, mode;
	char pipeName[] = "\\\\.\\pipe\\mynamedpipe";

	if (argc > 1)
		msg = argv[1];

	while (1)
	{
		hPipe = CreateFile(
			pipeName,                       // pipe name
			GENERIC_READ | GENERIC_WRITE,   // read and write access
			0,                              // no sharing
			NULL,                           // default security attributes
			OPEN_EXISTING,                  // opens existing pipe
			0,                              // default attributes
			NULL                            // no template file
		);

		if (hPipe != INVALID_HANDLE_VALUE)
			break;

		if (GetLastError() != ERROR_PIPE_BUSY)
		{
			printf("Could not open pipe. GLE=%d\n", GetLastError());
			return -1;
		}

		// All pipe instances are busy, so wait for 20 seconds.
		if (!WaitNamedPipe(pipeName, 20 * 1000))
		{
			printf("Could not open pipe: 20 seconds wait timed out.\n");
			return -1;
		}
	}

	mode = PIPE_READMODE_MESSAGE;
	success = SetNamedPipeHandleState(
		hPipe,    // pipe handle
		&mode,    // new pipe mode
		NULL,     // don't set maximum bytes
		NULL      // don't set maximum time
	);
	if (!success)
	{
		printf("SetNamedPipeHandleState failed. GLE=%d\n", GetLastError());
		return -1;
	}

	toWrite = (strlen(msg) + 1) * sizeof(char);
	printf("Sending %d byte message: \"%s\"\n", toWrite, msg);

	success = WriteFile(
		hPipe,        // pipe handle
		msg,          // message
		toWrite,      // message length
		&written,     // bytes written
		NULL          // not overlapped
	);
	if (!success)
	{
		printf("WriteFile to pipe failed. GLE=%d\n", GetLastError());
		return -1;
	}

	printf("\nMessage sent to server, receiving reply as follows:\n");

	do
	{
		success = ReadFile(
			hPipe,                // pipe handle
			buf,                  // buffer to receive reply
			BUFSIZE*sizeof(char), // size of buffer
			&read,                // umber of bytes read
			NULL                  // not overlapped
		);

		if (!success && GetLastError() != ERROR_MORE_DATA)
			break;

		printf("\"%s\"\n", buf);
	} while (!success);

	if (!success)
	{
		printf("ReadFile from pipe failed. GLE=%d\n", GetLastError());
		return -1;
	}

	printf("\n<End of message, press ENTER to terminate connection and exit>");
	_getch();

	CloseHandle(hPipe);
	return 0;
}

