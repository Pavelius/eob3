#pragma once

const long empty_focus = -1;

extern long current_focus, pressed_focus;
extern bool disable_input;

struct pushfocus {
	long focus;
	pushfocus() : focus(current_focus) { current_focus = empty_focus; }
	~pushfocus() { current_focus = focus; }
};

void apply_focus(int key);
void clear_focus_data();
void focusing(long focus_data);
bool focus_input();
long focus_next(long focus, int key);

inline bool focus_valid(long v) { return v != empty_focus; }