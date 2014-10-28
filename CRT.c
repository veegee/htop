/*
htop - CRT.c
(C) 2004-2011 Hisham H. Muhammad
Released under the GNU GPL, see the COPYING file
in the source distribution for its full text.
*/

#include "CRT.h"

#include "config.h"
#include "String.h"
#include "RichString.h"

#include <stdio.h>
#include <errno.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#ifdef HAVE_EXECINFO_H
#include <execinfo.h>
#endif


// TODO: centralize these in Settings.

static bool CRT_hasColors;
static int CRT_delay = 0;
int CRT_colorScheme = 0;
bool CRT_utf8 = false;
int CRT_colors[LAST_COLORELEMENT] = { 0 };
int CRT_cursorX = 0;
int CRT_scrollHAmount = 5;
char *CRT_termType;
void *backtraceArray[128];

static void CRT_handleSIGSEGV(int sgn) {
    (void) sgn;
    CRT_done();
    #if __linux
    fprintf(stderr, "\n\nhtop " VERSION " aborting. Please report bug at http://hisham.hm/htop\n");
    #ifdef HAVE_EXECINFO_H
    size_t size = backtrace(backtraceArray, sizeof(backtraceArray) / sizeof(void *));
    fprintf(stderr, "\n Please include in your report the following backtrace: \n");
    backtrace_symbols_fd(backtraceArray, size, 2);
    fprintf(stderr, "\nAdditionally, in order to make the above backtrace useful,");
    fprintf(stderr,
            "\nplease also run the following command to generate a disassembly of your binary:");
    fprintf(stderr, "\n\n   objdump -d `which htop` > ~/htop.objdump");
    fprintf(stderr, "\n\nand then attach the file ~/htop.objdump to your bug report.");
    fprintf(stderr, "\n\nThank you for helping to improve htop!\n\n");
    #endif
    #else
    fprintf(stderr, "\n\nhtop " VERSION " aborting. Unsupported platform.\n");
    #endif
    abort();
}

static void CRT_handleSIGTERM(int sgn) {
    (void) sgn;
    CRT_done();
    exit(0);
}

// TODO: pass an instance of Settings instead.

void CRT_init(int delay, int colorScheme) {
    initscr();
    noecho();
    CRT_delay = delay / 2;
    if(CRT_delay == 0) {
        CRT_delay = 1;
    }
    CRT_colorScheme = colorScheme;
    halfdelay(CRT_delay);
    nonl();
    intrflush(stdscr, false);
    keypad(stdscr, true);
    curs_set(0);
    if(has_colors()) {
        start_color();
        CRT_hasColors = true;
    } else {
        CRT_hasColors = false;
    }
    CRT_termType = getenv("TERM");
    if(String_eq(CRT_termType, "linux"))
        CRT_scrollHAmount = 20;
    else
        CRT_scrollHAmount = 5;
    if(String_eq(CRT_termType, "xterm") || String_eq(CRT_termType, "xterm-color")
            || String_eq(CRT_termType, "vt220")) {
        define_key("\033[H", KEY_HOME);
        define_key("\033[F", KEY_END);
        define_key("\033[7~", KEY_HOME);
        define_key("\033[8~", KEY_END);
        define_key("\033OP", KEY_F(1));
        define_key("\033OQ", KEY_F(2));
        define_key("\033OR", KEY_F(3));
        define_key("\033OS", KEY_F(4));
        define_key("\033[11~", KEY_F(1));
        define_key("\033[12~", KEY_F(2));
        define_key("\033[13~", KEY_F(3));
        define_key("\033[14~", KEY_F(4));
        define_key("\033[17;2~", KEY_F(18));
    }
    #ifndef DEBUG
    signal(11, CRT_handleSIGSEGV);
    #endif
    signal(SIGTERM, CRT_handleSIGTERM);
    use_default_colors();
    if(!has_colors())
        CRT_colorScheme = 1;
    CRT_setColors(CRT_colorScheme);

    mousemask(BUTTON1_CLICKED, NULL);
}

void CRT_done() {
    curs_set(1);
    endwin();
}

void CRT_fatalError(const char *note) {
    char *sysMsg = strerror(errno);
    CRT_done();
    fprintf(stderr, "%s: %s\n", note, sysMsg);
    exit(2);
}

int CRT_readKey() {
    nocbreak();
    cbreak();
    nodelay(stdscr, FALSE);
    int ret = getch();
    halfdelay(CRT_delay);
    return ret;
}

void CRT_disableDelay() {
    nocbreak();
    cbreak();
    nodelay(stdscr, TRUE);
}

void CRT_enableDelay() {
    halfdelay(CRT_delay);
}

void CRT_setColors(int colorScheme) {
    CRT_colorScheme = colorScheme;

    for(int i = 0; i <= 8; i++) {
        // foreground colours
        int f = (i == COLOR_RESET ? -1 : i);
        for(int j = 0; j <= 8; j++) {
            // background colours
            int b = (j == COLOR_RESET ? -1 : j);
            int id = j * 10 + i;
            init_pair(id, f, b);
            //fprintf(file, "%d: (%d, %d) {%s, %s}\n", id, f, b, table[i], table[j]);
        }
    }

    // use A_BOLD to make the foreground colour bright
    // use A_BLINK to make the background colour bright

    CRT_colors[RESET_COLOR] = A_NORMAL | ColorPair(Reset, Reset);
    CRT_colors[DEFAULT_COLOR] = A_NORMAL | ColorPair(Reset, Reset);

    CRT_colors[FUNCTION_BAR] = ColorPair(Yellow, Reset);
    CRT_colors[FUNCTION_KEY] = ColorPair(Red, Reset);

    CRT_colors[PANEL_HEADER_FOCUS] = ColorPair(Black, Blue);
    CRT_colors[PANEL_HEADER_UNFOCUS] = A_BLINK | ColorPair(Black, Green);

    CRT_colors[PANEL_HIGHLIGHT_FOCUS] = ColorPair(White, Blue);
    CRT_colors[PANEL_HIGHLIGHT_UNFOCUS] = A_BLINK | ColorPair(Black, Green);

    CRT_colors[FAILED_SEARCH] = ColorPair(Red, Reset);

    CRT_colors[UPTIME] = A_BOLD | ColorPair(Cyan, Reset);
    CRT_colors[BATTERY] = A_BOLD | ColorPair(Cyan, Reset);
    CRT_colors[LARGE_NUMBER] = A_BOLD | ColorPair(Red, Reset);

    CRT_colors[METER_TEXT] = A_BOLD | ColorPair(White, Reset);
    CRT_colors[METER_VALUE] = A_BOLD | ColorPair(White, Reset);

    CRT_colors[LED_COLOR] = ColorPair(Green, Reset);

    CRT_colors[TASKS_RUNNING] = ColorPair(Green, Reset);

    CRT_colors[PROCESS] = A_NORMAL;
    CRT_colors[PROCESS_SHADOW] = A_BOLD | ColorPair(Green, Reset);
    CRT_colors[PROCESS_TAG] = A_BOLD | ColorPair(Yellow, Reset);
    CRT_colors[PROCESS_MEGABYTES] = ColorPair(Cyan, Reset);
    CRT_colors[PROCESS_BASENAME] = ColorPair(Blue, Reset);
    CRT_colors[PROCESS_TREE] = A_BOLD | ColorPair(Green, Reset);
    CRT_colors[PROCESS_R_STATE] = ColorPair(Green, Reset);
    CRT_colors[PROCESS_D_STATE] = A_BOLD | ColorPair(Red, Reset);
    CRT_colors[PROCESS_HIGH_PRIORITY] = ColorPair(Red, Reset);
    CRT_colors[PROCESS_LOW_PRIORITY] = ColorPair(Red, Reset);
    CRT_colors[PROCESS_THREAD] = ColorPair(Green, Reset);
    CRT_colors[PROCESS_THREAD_BASENAME] = A_BOLD | ColorPair(Green, Reset);

    CRT_colors[BAR_BORDER] = A_BOLD;
    CRT_colors[BAR_SHADOW] = A_NORMAL;

    CRT_colors[SWAP] = ColorPair(Red, Reset);

    CRT_colors[GRAPH_1] = A_BOLD | ColorPair(Red, Reset);
    CRT_colors[GRAPH_2] = ColorPair(Red, Reset);
    CRT_colors[GRAPH_3] = A_BOLD | ColorPair(Yellow, Reset);
    CRT_colors[GRAPH_4] = A_BOLD | ColorPair(Green, Reset);
    CRT_colors[GRAPH_5] = ColorPair(Green, Reset);
    CRT_colors[GRAPH_6] = ColorPair(Cyan, Reset);
    CRT_colors[GRAPH_7] = A_BOLD | ColorPair(Blue, Reset);
    CRT_colors[GRAPH_8] = ColorPair(Blue, Reset);
    CRT_colors[GRAPH_9] = A_BOLD | ColorPair(Reset, Reset);

    CRT_colors[MEMORY_USED] = ColorPair(Green, Reset);
    CRT_colors[MEMORY_BUFFERS] = ColorPair(Blue, Reset);
    CRT_colors[MEMORY_BUFFERS_TEXT] = A_BOLD | ColorPair(Blue, Reset);
    CRT_colors[MEMORY_CACHE] = ColorPair(Yellow, Reset);

    CRT_colors[LOAD_AVERAGE_FIFTEEN] = A_BOLD | ColorPair(White, Reset);
    CRT_colors[LOAD_AVERAGE_FIVE] = ColorPair(Reset, Reset);
    CRT_colors[LOAD_AVERAGE_ONE] = A_BOLD | ColorPair(Green, Reset);
    CRT_colors[LOAD] = A_BOLD;

    CRT_colors[HELP_BOLD] = A_BOLD | ColorPair(Cyan, Reset);

    CRT_colors[CLOCK] = A_BOLD;

    CRT_colors[CHECK_BOX] = ColorPair(Cyan, Reset);
    CRT_colors[CHECK_MARK] = A_BOLD;
    CRT_colors[CHECK_TEXT] = A_NORMAL;

    CRT_colors[HOSTNAME] = A_BOLD;

    CRT_colors[CPU_NICE] = ColorPair(Blue, Reset);
    CRT_colors[CPU_NICE_TEXT] = A_BOLD | ColorPair(Blue, Reset);
    CRT_colors[CPU_NORMAL] = ColorPair(Green, Reset);
    CRT_colors[CPU_KERNEL] = ColorPair(Red, Reset);
    CRT_colors[CPU_IOWAIT] = A_BOLD | ColorPair(Reset, Reset);
    CRT_colors[CPU_IRQ] = ColorPair(Yellow, Reset);
    CRT_colors[CPU_SOFTIRQ] = ColorPair(Magenta, Reset);
    CRT_colors[CPU_STEAL] = ColorPair(Cyan, Reset);
    CRT_colors[CPU_GUEST] = ColorPair(Cyan, Reset);
}
