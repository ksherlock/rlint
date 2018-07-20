#include <menu.h>
#include <resources.h>

#include "rlint.h"




void check_rMenuItem(Handle h) {

	MenuItemTemplate *mi;
	Ref ref;

	mi = *(MenuItemTemplate **)h;
	ref = mi->itemTitleRef;

	if (mi->itemFlag & 0x0400) {
		if (mi->itemFlag & 0x200) {
			if (ref) check(rItemStruct, ref);
		}
	}
	else if (mi->itemFlag & mRefResource) {
		if (ref) check(rPString, ref);
	}
}

void check_rMenu(Handle h) {

	MenuTemplate *m;
	unsigned i;
	Ref ref;

	m = *(MenuTemplate **)h;
	if (m->menuFlag & mRefResource) {
		ref = m->menuTitleRef;
		if (ref) check(rPString, ref);
	}
	if (m->menuFlag & 0x2000) {
		for (i = 0; ; ++i) {
			ref = m->itemRefArray[i];
			if (!ref) break;
			check(rMenuItem, ref);
		}
	}
}


void check_rMenuBar(Handle h) {
	
	MenuBarTemplate *mb;
	unsigned i;

	mb = *(MenuBarTemplate **)h;
	if (mb->menuFlag & mRefResource) {
		for (i = 0; ; ++i) {
			Ref ref = mb->menuRefArray[i];
			if (!ref) break;
			check(rMenu, ref);
		}
	}
}


void check_rItemStruct(Handle h) {

	itemStruct *ptr;
	Ref ref;

	ptr = *(itemStruct **)h;
	if (ptr->itemFlag2 & mRefResource) {
		ref = ptr->itemTitleRef;
		if (ref) check(rPString, ref);
		ref = ptr->itemIconRef;
		if (ref) check(rIcon, ref);

	}
}

