# ==== depends on curl-7.25.0 ====
# 1. build static-lib using curl-7.25.0\vs2010curl.sln, [LIB Release]
# 2. put include/lib to curl-7.25.0 directory
# 3. libcurl needs ws2_32.lib, wldap32.lib

# ==== test ====
# wget.exe http://cdn2.01234.com.cn/bd/android/DK_HunterV34_5.0.075_sub10026.apk

SRC = wget.cpp
OBJ = wget.obj
CFLAGS = /I".\curl-7.25.0\include" /c /EHsc /Zc:wchar_t /fp:precise /D_WIN32_WINNT=0x0500 /D "_MBCS" /MT /nologo
LDFLAGS = /NOLOGO /SUBSYSTEM:CONSOLE /LIBPATH:".\curl-7.25.0\lib\Release" "libcurl.lib" "ws2_32.lib" "wldap32.lib"

all:
	cl $(CFLAGS) $(SRC)
	link /OUT:wget.exe $(LDFLAGS) $(OBJ)

clean:
	del *.obj *.exe
