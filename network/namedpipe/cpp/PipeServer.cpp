#include <windows.h>
#include <stdio.h>
#include <strsafe.h>

#define BUFSIZE 512

DWORD WINAPI InstanceThread(void*);
void GetAnswerToRequest(char*, char*, DWORD*);

int main()
{
	BOOL   fConnected = FALSE;
	DWORD  dwThreadId = 0;
	HANDLE hPipe = INVALID_HANDLE_VALUE, hThread = NULL;
	char pipeName[] = "\\\\.\\pipe\\mynamedpipe";

	for (;;)
	{
		printf("\nPipe Server: Main thread awaiting client connection on %s\n", pipeName);
		hPipe = CreateNamedPipe(
			pipeName,                        // pipe name
			PIPE_ACCESS_DUPLEX,              // read/write access
			PIPE_TYPE_MESSAGE |              // message type pipe
			PIPE_READMODE_MESSAGE |          // message-read mode
			PIPE_WAIT,                       // blocking mode
			PIPE_UNLIMITED_INSTANCES,        // max. instances
			BUFSIZE,                         // output buffer size
			BUFSIZE,                         // input buffer size
			0,                               // client time-out
			NULL                             // default security attribute
		);

		if (hPipe == INVALID_HANDLE_VALUE)
		{
			printf("CreateNamedPipe failed, GLE=%d.\n", GetLastError());
			return -1;
		}

		// Wait for the client to connect; if it succeeds,
		// the function returns a nonzero value. If the function
		// returns zero, GetLastError returns ERROR_PIPE_CONNECTED.
		fConnected = ConnectNamedPipe(hPipe, NULL) ? TRUE : (GetLastError() == ERROR_PIPE_CONNECTED);
		if (fConnected)
		{
			printf("Client connected, creating a processing thread.\n");

			hThread = CreateThread(
				NULL,             // no security attribute
				0,                // default stack size
				InstanceThread,   // thread proc
				(void*)hPipe,     // thread parameter
				0,                // not suspended
				&dwThreadId       // returns thread ID
			);

			if (hThread == NULL)
			{
				printf("CreateThread failed, GLE=%d.\n", GetLastError());
				return -1;
			}
			else
			{
				CloseHandle(hThread);
			}
		}
		else
		{
			// The client could not connect, so close the pipe.
			CloseHandle(hPipe);
		}
	}

	return 0;
}

DWORD WINAPI InstanceThread(void* param)
{
	HANDLE hHeap  = GetProcessHeap();
	char* request = (char*) HeapAlloc(hHeap, 0, BUFSIZE*sizeof(char));
	char* reply   = (char*) HeapAlloc(hHeap, 0, BUFSIZE*sizeof(char));

	DWORD bytesRead = 0, replyBytes = 0, written = 0;
	BOOL success = FALSE;
	HANDLE hPipe = NULL;

	if (param == NULL)
	{
		printf("\nERROR - Pipe Server Failure:\n");
		printf("  InstanceThread got an unexpected NULL value in param.\n");
		printf("  InstanceThread exiting.\n");
		if (request != NULL) HeapFree(hHeap, 0, request);
		if (reply != NULL) HeapFree(hHeap, 0, reply);
		return (DWORD)-1;
	}

	if (request == NULL)
	{
		printf("\nERROR - Pipe Server Failure:\n");
		printf("  InstanceThread got an unexptected NULL heap allocation.\n");
		printf("  InstanceThread exiting.\n");
		if (reply != NULL) HeapFree(hHeap, 0, reply);
		return (DWORD)-1;
	}

	if (reply == NULL)
	{
		printf("\nERROR - Pipe Server Failure:\n");
		printf("  InstanceThread got an unexptected NULL heap allocation.\n");
		printf("  InstanceThread exiting.\n");
		if (request != NULL) HeapFree(hHeap, 0, request);
		return (DWORD)-1;
	}

	printf("InstanceThread created, receiving and processing messages.\n");

	hPipe = (HANDLE)param;
	while (1)
	{
		success = ReadFile(
			hPipe,                          // handle to pipe
			request,                        // buffer to receive data
			BUFSIZE*sizeof(char),           // size of buffer
			&bytesRead,                     // number of bytes read
			NULL
		);

		if (!success || bytesRead == 0)
		{
			if (GetLastError() == ERROR_BROKEN_PIPE)
			{
				printf("InstanceThread: client disconnected.\n", GetLastError());
			}
			else
			{
				printf("InstanceThread ReadFile failed, GLE=%d.\n", GetLastError());
			}
			break;
		}

		GetAnswerToRequest(request, reply, &replyBytes);

		success = WriteFile(
			hPipe,                 // handle to pipe
			reply,                 // buffer to write from
			replyBytes,            // number of bytes to write
			&written,              // number of bytes written
			NULL                   // not overlapped I/O
		);

		if (!success || replyBytes != written)
		{
			printf("InstanceThread WriteFile failed, GLE=%d.\n", GetLastError());
			break;
		}
	}

	// Flush the pipe to allow the client to read the pipe's contents
	// before disconnecting. Then disconnect the pipe, and close the
	// handle to this pipe instance.
	FlushFileBuffers(hPipe);
	DisconnectNamedPipe(hPipe);
	CloseHandle(hPipe);

	HeapFree(hHeap, 0, request);
	HeapFree(hHeap, 0, reply);

	printf("InstanceThread exitting.\n");
	return 1;
}

void GetAnswerToRequest(char *request, char *reply, DWORD* bytes)
{
	printf("Client Request String: \"%s\"\n", request);

	if (FAILED(StringCchCopyA(reply, BUFSIZE, "default answer from server")))
	{
		*bytes = 0;
		reply[0] = 0;
		printf("StringCchCopy failed, no outgoing message.\n");
	}
	else
	{
		*bytes = (strlen(reply) + 1) * sizeof(char);
	}
}

