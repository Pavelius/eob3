#include "perference.h"

long valuei::get() const {
	switch(size) {
	case 1: return *((char*)source);
	case 2: return *((short*)source);
	case 4: return *((int*)source);
	default: return 0;
	}
}

void valuei::set(long value) const {
	switch(size) {
	case 1: *((char*)source) = (char)value; break;
	case 2: *((short*)source) = (short)value; break;
	case 4: *((int*)source) = (int)value; break;
	default: break;
	}
}