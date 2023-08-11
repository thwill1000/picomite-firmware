/*-*****************************************************************************

MMBasic for PicoGAME LCD

pglcd.c

Copyright 2023 Geoff Graham, Peter Mather and Thomas Hugo Williams.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice,
   this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

3. Neither the name of the copyright holders nor the names of its contributors
   may be used to endorse or promote products derived from this software
   without specific prior written permission.

4. The name MMBasic be used when referring to the interpreter in any
   documentation and promotional material and the original copyright message
   be displayed  on the console at startup (additional copyright messages may
   be added).

5. All advertising materials mentioning features or use of this software must
   display the following acknowledgement: This product includes software
   developed by Geoff Graham, Peter Mather and Thomas Hugo Williams.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDERS OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*******************************************************************************/

#include <stdio.h>
#include <stdlib.h>

#include "MMBasic_Includes.h"
#include "Hardware_Includes.h"
#include "pglcd.h"
#include "Draw.h"

#define min(a,b) \
    ({ __typeof__ (a) _a = (a); \
        __typeof__ (b) _b = (b); \
        _a < _b ? _a : _b; })

/**
 * Writes a string to the display wrapping lines and starting and ending each
 * line with a space.
 *
 * @param  s     the string.
 * @param  fill  if true then after displaying the string fill to the end of
 *               the line with spaces and move down to the next line.
 */
static void display_string(const char *s, bool fill) {
    // Indent each line by one space.
    if (CurrentX == 0) DisplayPutC(' ');

    // Display characters one at a time,
    for (const char *p = s; *p; ++p) {
        if (CurrentX + gui_font_width >= HRes) {
            DisplayPutC(' '); // Leave one space at the end of each line and wrap to the next.
            DisplayPutC(' '); // Indent each new line by one space.
            if (*p == ' ') continue; // Skip first space on each new line.
        }
        DisplayPutC(*p);
    }

    // Fill to the end of the line with spaces.
    if (fill) {
        while (CurrentX + gui_font_width <= HRes) DisplayPutC(' ');
        CurrentX = 0;
        CurrentY += gui_font_height;
    }
}

/**
 * Displays an error message with context on the display.
 *
 * @param  line_num   The error line,
 *                    -1 for the LIBRARY,
 *                    -2 when error occurs at the prompt.
 * @param  line_txt   The text of the line that caused the error.
 * @param  error_msg  The error message.
 */
void pglcd_error(int line_num, const char *line_txt, const char* error_msg) {
    if (HRes == 0) return; // No display configured.

    // Always write error to the actual display.
    restoreSPIpanel();

    // Store current property display values.
    const unsigned char old_console = Option.DISPLAY_CONSOLE;
    const int old_font = gui_font;
    const int old_fcolour = gui_fcolour;
    const int old_bcolour = gui_bcolour;

    // Override properties required by DisplayPutC.
    const int font = 1;
    Option.DISPLAY_CONSOLE = 1;
    SetFont(font);
    gui_fcolour = 0xEE4B2B; // Bright Red.
    gui_bcolour = 0x0;

    // Display the error message halfway down the display (approx.)
    const int chars_per_line = (HRes / gui_font_width) - 2;
    int num_lines = 2;
    num_lines += strlen(error_msg) / chars_per_line;
    if (strlen(error_msg) % chars_per_line > 0) num_lines++;
    num_lines += strlen(line_txt) / chars_per_line;
    if (strlen(line_txt) % chars_per_line > 0) num_lines++;
    CurrentX = 0;
    CurrentY = (VRes / 2) - (num_lines * gui_font_height / 2);

    display_string("", true);
    display_string("ERROR: ", false);
    display_string(error_msg, true);
    if (*line_txt) {
        char buf[32];
        if (line_num == -1) {
            sprintf(buf, "[LIBRARY] ");
        } else {
            sprintf(buf, "[%d] ", line_num);
        }
        display_string(buf, false);
        display_string(line_txt, true);
    }
    display_string("", true);

    // Restore display property values.
    SetFont(old_font);
    Option.DISPLAY_CONSOLE = old_console;
    gui_fcolour = gui_fcolour;
    gui_bcolour = gui_bcolour;
}
