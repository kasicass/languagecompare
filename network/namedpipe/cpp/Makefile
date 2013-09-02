CFLAGS = /EHsc /Zc:wchar_t /fp:precise /D_WIN32_WINNT=0x0500 /D "_MBCS"

all:
	cl $(CFLAGS) PipeServerCompletionRoutine.cpp
	cl $(CFLAGS) PipeServer.cpp
	cl $(CFLAGS) PipeClient.cpp

clean:
	del *.exe *.obj
