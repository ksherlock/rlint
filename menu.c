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
			if (ref) check(rItemStruct, ref, check_rItemStruct);
		}
	}
	else if (mi->itemFlag & mRefResource) {
		if (ref) check(rPString, ref, check_rPString);
	}
}

void check_rMenu(Handle h) {

	MenuTemplate *m;
	unsigned i;
	Ref ref;

	m = *(MenuTemplate **)h;
	if (m->menuFlag & mRefResource) {
		ref = m->menuTitleRef;
		if (ref) check(rPString, ref, check_rPString);
	}
	if (m->menuFlag & 0x2000) {
		for (i = 0; ; ++i) {
			ref = m->itemRefArray[i];
			if (!ref) break;
			check(rMenuItem, ref, check_rMenuItem);
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
			check(rMenu, ref, check_rMenu);
		}
	}
}


void check_rItemStruct(Handle h) {

	itemStruct *ptr;
	Ref ref;

	ptr = *(itemStruct **)h;
	if (ptr->itemFlag2 & mRefResource) {
		ref = ptr->itemTitleRef;
		if (ref) check(rPString, ref, check_rPString);
		ref = ptr->itemIconRef;
		if (ref) check(rIcon, ref, check_rIcon);

	}
}



