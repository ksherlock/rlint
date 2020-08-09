#include "rlint.h"

void check_rAlertString(Handle h) {
	/* if there's a custom icon, assume it's a resource ref */

	unsigned char *ptr = *(unsigned char **)h;
	unsigned x;
	unsigned offset = 0;

	x = ptr[offset++];
	if (x == '0') offset += 8; /* skip past custom size rect */
	x = ptr[offset++];
	if (x == '1') {
		/* custom icon */
		unsigned long ref = *(unsigned long *)(ptr + offset);
		if (ref) check(rIcon, ref); 
	}
}
