#ifdef WIN32

#include <Windows.h>
#include <stdio.h>

void bootstrap_win32_unicode() {
    SetConsoleOutputCP(CP_UTF8);
    setvbuf(stdout, nullptr, _IOFBF, 1000);
}

#else

void bootstrap_win32_unicode() {}

#endif
