#include <windows.h>
#include <stdio.h>
#include <strsafe.h>

#define PIPE_TIMEOUT 5000
#define BUFSIZE 4096

typedef struct
{
	OVERLAPPED overlap;
	HANDLE pipe;
	char request[BUFSIZE];
	DWORD read;
	char reply[BUFSIZE];
	DWORD toWrite;
} PIPEINST;

void DisconnectAndClose(PIPEINST*);
BOOL CreateAndConnectInstance(OVERLAPPED*);
BOOL ConnectToNewClient(HANDLE, OVERLAPPED*);
void GetAnswerToRequest(PIPEINST*);

void WINAPI CompletedWriteRoutine(DWORD, DWORD, OVERLAPPED*);
void WINAPI CompletedReadRoutine(DWORD, DWORD, OVERLAPPED*);

HANDLE hPipe;

int main()
{
	HANDLE hConnectEvent;
	OVERLAPPED oConnect;
	PIPEINST *pPipeInst;
	DWORD wait, ret;
	BOOL success, pendingIO;

	// create one event object for the connect operation.
	hConnectEvent = CreateEvent(
		NULL,  // default security attribute
		TRUE,  // manual reset event
		TRUE,  // initial state = signaled
		NULL   // unnamed event object
	);

	if (hConnectEvent == NULL)
	{
		printf("CreateEvent failed with %d.\n", GetLastError());
		return 0;
	}

	oConnect.hEvent = hConnectEvent;
	pendingIO = CreateAndConnectInstance(&oConnect);

	while (1)
	{
		// Wait for a client to connect, or for a read or write
		// operation to be completed, which causes a completion
		// routine to be queued for execution.
		wait = WaitForSingleObjectEx(
			hConnectEvent, // event object to wait for
			INFINITE,      // waits indefinitely
			TRUE           // alertable wait enabled
		);

		switch (wait)
		{
		// The wait conditions are satisfied by a completed connect operation.
		case 0:
			// If an operation is pending, get the result of the
			// connect operations.
			if (pendingIO)
			{
				success = GetOverlappedResult(
					hPipe,       // pipe handle
					&oConnect,   // OVERLAPPED structure
					&ret,        // bytes transferred
					FALSE        // does not wait
				);
				if (!success)
				{
					printf("ConnectNamedPipe (%d)\n", GetLastError());
					return 0;
				}
			}

			// Allocate storage for this instance
			pPipeInst = (PIPEINST*)GlobalAlloc(GPTR, sizeof(PIPEINST));
			if (pPipeInst == NULL)
			{
				printf("GlobalAlloc failed (%d)\n", GetLastError());
				return 0;
			}

			pPipeInst->pipe = hPipe;

			// Start the read operation for this client.
			// Note that this same routine is later used as a
			// completion routine after a write operation.
			pPipeInst->toWrite = 0;
			CompletedWriteRoutine(0, 0, (OVERLAPPED*)pPipeInst);

			// Create new pipe instance for the next client.
			pendingIO = CreateAndConnectInstance(&oConnect);
			break;

		// The wait is satisfied by completed read or write
		// operation. This allows the system to execute the
		// completion routine.
		case WAIT_IO_COMPLETION:
			break;
	
		// An error occurred in the wait function.
		default:
			printf("WaitForSingleObjectEx (%d)\n", GetLastError());
			return 0;
		}
	}

	return 0;
}

void WINAPI CompletedWriteRoutine(DWORD err, DWORD written, OVERLAPPED* overlap)
{
	PIPEINST *pPipeInst;
	BOOL read = FALSE;

	pPipeInst = (PIPEINST*) overlap;

	// The write operation has finished, so read the next request (if
	// there is no error).
	if ((err == 0) && (written == pPipeInst->toWrite))
	{
		read = ReadFileEx(
			pPipeInst->pipe,
			pPipeInst->request,
			BUFSIZE*sizeof(char),
			(LPOVERLAPPED)pPipeInst,
			(LPOVERLAPPED_COMPLETION_ROUTINE)CompletedReadRoutine
		);

		if (!read)
			DisconnectAndClose(pPipeInst);
	}
}

void WINAPI CompletedReadRoutine(DWORD err, DWORD bytesRead, OVERLAPPED* overlap)
{
	PIPEINST *pPipeInst;
	BOOL write = FALSE;

	pPipeInst = (PIPEINST*) overlap;

	// The read operation has finsihed, so write a response (if no
	// error occurred).
	if ((err == 0) && (bytesRead != 0))
	{
		GetAnswerToRequest(pPipeInst);

		write = WriteFileEx(
			pPipeInst->pipe,
			pPipeInst->reply,
			pPipeInst->toWrite,
			(LPOVERLAPPED) pPipeInst,
			(LPOVERLAPPED_COMPLETION_ROUTINE) CompletedWriteRoutine
		);

		if (!write)
			DisconnectAndClose(pPipeInst);
	}
}

void DisconnectAndClose(PIPEINST* pPipeInst)
{
	if (!DisconnectNamedPipe(pPipeInst->pipe))
	{
		printf("DisconnectNamedPipe failed with %d.\n", GetLastError());
	}

	CloseHandle(pPipeInst->pipe);
	GlobalFree(pPipeInst);
}

BOOL CreateAndConnectInstance(OVERLAPPED *oConnect)
{
	char pipeName[] = "\\\\.\\pipe\\mynamedpipe";
	hPipe = CreateNamedPipe(
		pipeName,
		PIPE_ACCESS_DUPLEX | FILE_FLAG_OVERLAPPED,
		PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT,
		PIPE_UNLIMITED_INSTANCES,
		BUFSIZE*sizeof(char),
		BUFSIZE*sizeof(char),
		PIPE_TIMEOUT,
		NULL
	);

	if (hPipe == INVALID_HANDLE_VALUE)
	{
		printf("CreateNamedPipe failed with %d.\n", GetLastError());
		return FALSE;
	}

	return ConnectToNewClient(hPipe, oConnect);
}

BOOL ConnectToNewClient(HANDLE hPipe, OVERLAPPED* oConnect)
{
	BOOL connected, pendingIO = FALSE;
	connected = ConnectNamedPipe(hPipe, oConnect);

	// Overlapped ConnectNamedPipe should return zero.
	if (connected)
	{
		printf("ConnectNamedPipe failed with %d.\n", GetLastError());
		return FALSE;
	}

	switch (GetLastError())
	{
	// The overlapped connection in progress.
	case ERROR_IO_PENDING:
		pendingIO = TRUE;
		break;

	// Client is already connected, so signal an event.
	case ERROR_PIPE_CONNECTED:
		if (SetEvent(oConnect->hEvent))
			break;

	// If an error occurs during the connect operation...
	default:
		printf("ConnectNamedPipe failed with %d.\n", GetLastError());
		return FALSE;
	}

	return pendingIO;
}

void GetAnswerToRequest(PIPEINST* pPipeInst)
{
	printf("[%d] %s\n", pPipeInst->pipe, pPipeInst->request);
	StringCchCopy(pPipeInst->reply, BUFSIZE, "Default answer from server");
	pPipeInst->toWrite = (strlen(pPipeInst->reply) + 1) * sizeof(char);
}

