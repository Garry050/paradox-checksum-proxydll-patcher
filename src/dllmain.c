#include "log.h"
#include "patcher.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>

static HINSTANCE s_hinstDLL = NULL;

static DWORD WINAPI patcher_thread(LPVOID param) {
    (void)param;

    log_init(s_hinstDLL);
    LOG_INFO("version.dll proxy DLL loaded");

    run_patcher();

    LOG_INFO("patcher thread exiting");
    log_close();
    return 0;
}

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved) {
    (void)lpvReserved;

    if (fdwReason == DLL_PROCESS_ATTACH) {
        s_hinstDLL = hinstDLL;
        DisableThreadLibraryCalls(hinstDLL);

        /* Open a console window for debug output */
        AllocConsole();
        freopen("CONOUT$", "w", stdout);
        freopen("CONOUT$", "w", stderr);

        CreateThread(NULL, 0, patcher_thread, NULL, 0, NULL);
    }

    return TRUE;
}
