#include "MeshtasticOSEnvironment-win.h"

#include <winrt/windows.foundation.h>
#include <Windows.h>

/**
 * @see https://stackoverflow.com/questions/45575863/how-to-print-utf-8-strings-to-stdcout-on-windows
 */
MeshtasticOSEnvironment::MeshtasticOSEnvironment()
    : MeshtasticEnvironment()
{
    winrt::init_apartment();

    // Set console code page to UTF-8 so console known how to interpret string data
    SetConsoleOutputCP(CP_UTF8);

    // Enable buffering to prevent VS from chopping up UTF-8 byte sequences
    setvbuf(stdout, nullptr, _IOFBF, 1000);
}

MeshtasticOSEnvironment::~MeshtasticOSEnvironment()
{
    winrt::uninit_apartment();
}
