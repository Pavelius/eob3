#pragma once

extern char console_text[512];

void console(const char* format, ...);
void console_clear();
void console_delete_line();
void console_scroll(unsigned long seconds);
void consolen(const char* format, ...);
void consolens(char sym = '\n');
void consolev(const char* format, const char* format_param);
