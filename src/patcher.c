#include "patcher.h"
#include "log.h"
#include <string.h>
#include <stddef.h>

/* ------------------------------------------------------------------ */
/* Byte patterns — direct port from hex.go                             */
/* ------------------------------------------------------------------ */

#define START_LEN  3
#define END_LEN    6
#define SCAN_LIMIT 14

static const BYTE start1[] = { 0x48, 0x8B, 0x12 };
static const BYTE start2[] = { 0x48, 0x8D, 0x0D };
static const BYTE start3[] = { 0x48, 0x8B, 0xD0 };
/* start4 in Go is identical to start2, so no separate array needed */

/* EU4 / HOI4 */
static const BYTE end_default[]  = { 0x85, 0xC0, 0x0F, 0x94, 0xC3, 0xE8 };
static const BYTE repl_default[] = { 0x31, 0xC0, 0x0F, 0x94, 0xC3, 0xE8 };

/* EU5 */
static const BYTE end_eu5[]      = { 0x85, 0xC0, 0x0F, 0x94, 0xC1, 0x88 };
static const BYTE repl_eu5[]     = { 0x31, 0xC0, 0x0F, 0x94, 0xC1, 0x88 };

/* ------------------------------------------------------------------ */

static int is_start_candidate(const BYTE *p) {
    return memcmp(p, start1, START_LEN) == 0 ||
           memcmp(p, start2, START_LEN) == 0 ||
           memcmp(p, start3, START_LEN) == 0;
}

/* Returns pointer to the correct replacement bytes if p matches an end
   pattern, NULL otherwise. */
static const BYTE *match_end(const BYTE *p) {
    if (memcmp(p, end_default, END_LEN) == 0) return repl_default;
    if (memcmp(p, end_eu5,     END_LEN) == 0) return repl_eu5;
    return NULL;
}

/* ------------------------------------------------------------------ */

GameType detect_game(void) {
    char path[MAX_PATH];
    if (!GetModuleFileNameA(NULL, path, MAX_PATH)) return GAME_UNKNOWN;

    /* Find the last backslash to isolate the filename */
    char *name = path;
    for (char *p = path; *p; p++) {
        if (*p == '\\' || *p == '/') name = p + 1;
    }

    if (_stricmp(name, "eu4.exe")  == 0) return GAME_EU4;
    if (_stricmp(name, "eu5.exe")  == 0) return GAME_EU5;
    if (_stricmp(name, "hoi4.exe") == 0) return GAME_HOI4;
    return GAME_UNKNOWN;
}

/* ------------------------------------------------------------------ */

static int find_text_section(BYTE **out_start, SIZE_T *out_size) {
    HMODULE base = GetModuleHandle(NULL);
    if (!base) return 0;

    IMAGE_DOS_HEADER *dos = (IMAGE_DOS_HEADER *)base;
    if (dos->e_magic != IMAGE_DOS_SIGNATURE) return 0;

    IMAGE_NT_HEADERS *nt = (IMAGE_NT_HEADERS *)((BYTE *)base + dos->e_lfanew);
    if (nt->Signature != IMAGE_NT_SIGNATURE) return 0;

    WORD num_sections = nt->FileHeader.NumberOfSections;
    IMAGE_SECTION_HEADER *sec = (IMAGE_SECTION_HEADER *)
        ((BYTE *)&nt->OptionalHeader + nt->FileHeader.SizeOfOptionalHeader);

    for (WORD i = 0; i < num_sections; i++, sec++) {
        if (memcmp(sec->Name, ".text", 5) == 0) {
            *out_start = (BYTE *)base + sec->VirtualAddress;
            *out_size  = sec->Misc.VirtualSize;
            return 1;
        }
    }

    /* Fallback: scan the whole module image */
    LOG_INFO("'.text' section not found, falling back to full image scan");
    *out_start = (BYTE *)base;
    *out_size  = nt->OptionalHeader.SizeOfImage;
    return 1;
}

/* ------------------------------------------------------------------ */

/* Direct port of modifyBytes() from patch.go.
   Works on live memory instead of a file buffer. */
static int scan_and_patch(BYTE *mem, SIZE_T mem_size) {
    int matches = 0;

    for (SIZE_T i = 0; i + START_LEN <= mem_size; i++) {
        if (!is_start_candidate(mem + i)) continue;

        SIZE_T j_max = i + START_LEN + SCAN_LIMIT;
        for (SIZE_T j = i + START_LEN;
             j + END_LEN <= mem_size && j <= j_max;
             j++) {

            const BYTE *repl = match_end(mem + j);
            if (!repl) continue;

            LOG_TRACE("found match at offset 0x%llx", (unsigned long long)j);

            /* Temporarily make the page writable */
            DWORD old_protect = 0;
            if (!VirtualProtect(mem + j, END_LEN, PAGE_EXECUTE_READWRITE, &old_protect)) {
                LOG_ERROR("VirtualProtect (rw) failed at 0x%llx: %lu",
                          (unsigned long long)(mem + j), GetLastError());
                break;
            }

            memcpy(mem + j, repl, END_LEN);

            DWORD dummy = 0;
            VirtualProtect(mem + j, END_LEN, old_protect, &dummy);

            matches++;
            break;
        }
    }

    return matches;
}

/* ------------------------------------------------------------------ */

int run_patcher(void) {
    GameType game = detect_game();
    if (game == GAME_UNKNOWN) {
        LOG_INFO("Unknown game process — patcher will not run");
        return 0;
    }

    const char *names[] = { "", "eu4.exe", "eu5.exe", "hoi4.exe" };
    LOG_INFO("Detected game: %s", names[game]);

    /* Wait for the game's main module to be fully initialised */
    Sleep(1000);

    BYTE   *text_start = NULL;
    SIZE_T  text_size  = 0;
    if (!find_text_section(&text_start, &text_size)) {
        LOG_ERROR("Failed to locate .text section");
        return 0;
    }
    LOG_INFO(".text section: base=0x%p size=0x%llx",
             text_start, (unsigned long long)text_size);

    int count = scan_and_patch(text_start, text_size);
    if (count == 0) {
        LOG_INFO("No matches found — game may have been updated or is already patched");
    } else {
        LOG_INFO("Applied %d patch(es) successfully", count);
    }

    return count;
}
