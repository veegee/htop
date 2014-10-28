/* Do not edit this file. It was automatically generated. */

#ifndef HEADER_OpenFilesScreen
#define HEADER_OpenFilesScreen
/*
htop - OpenFilesScreen.h
(C) 2005-2006 Hisham H. Muhammad
Released under the GNU GPL, see the COPYING file
in the source distribution for its full text.
*/

#include "Process.h"
#include "Panel.h"
#include "FunctionBar.h"

typedef struct OpenFiles_Data_ {
    char *data[256];
} OpenFiles_Data;

typedef struct OpenFiles_ProcessData_ {
    OpenFiles_Data data;
    int error;
    struct OpenFiles_FileData_ *files;
} OpenFiles_ProcessData;

typedef struct OpenFiles_FileData_ {
    OpenFiles_Data data;
    struct OpenFiles_FileData_ *next;
} OpenFiles_FileData;

typedef struct OpenFilesScreen_ {
    Process *process;
    pid_t pid;
    Panel *display;
    FunctionBar *bar;
} OpenFilesScreen;


OpenFilesScreen *OpenFilesScreen_new(Process *process);

void OpenFilesScreen_delete(OpenFilesScreen *this);

void OpenFilesScreen_run(OpenFilesScreen *this);

#endif
