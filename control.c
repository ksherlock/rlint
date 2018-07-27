#include <resources.h>
#include <control.h>
#include <window.h>

#include "rlint.h"

void check_rControlList(Handle h) {
	unsigned i;
	Ref *list = *(Ref **)h;

	for(i = 0; ; ++i) {
		Ref ref = list[i];
		if (!ref) break;
		check(rControlTemplate, ref);
	}
}

void check_rControlTemplate(Handle h) {

	ControlTemplate *ptr = *(ControlTemplate **)h;
	Ref ref;
	unsigned moreFlags = ptr->moreFlags;

	if (ptr->procRef == simpleButtonControl) {
		ref = ((SimpleButtonTemplate *)ptr)->titleRef;
		if ((moreFlags & 0x03) == titleIsResource)
			check(rPString, ref);

		ref = ((SimpleButtonTemplate *)ptr)->colorTableRef;
		if ((moreFlags & 0x0c) == colorTableIsResource)
			check(rCtlColorTbl, ref);

		return;
	}

	if (ptr->procRef == checkControl) {
		
		ref = ((CheckBoxTemplate *)ptr)->titleRef;
		if ((moreFlags & 0x03) == titleIsResource)
			check(rPString, ref);

		ref = ((CheckBoxTemplate *)ptr)->colorTableRef;
		if ((moreFlags & 0x0c) == colorTableIsResource)
			check(rCtlColorTbl, ref);

		return;
	}

	if (ptr->procRef == radioControl) {
		
		ref = ((RadioButtonTemplate *)ptr)->titleRef;
		if ((moreFlags & 0x03) == titleIsResource)
			check(rPString, ref);

		ref = ((RadioButtonTemplate *)ptr)->colorTableRef;
		if ((moreFlags & 0x0c) == colorTableIsResource)
			check(rCtlColorTbl, ref);

		return;
	}

	if (ptr->procRef == scrollBarControl) {
		
		ref = ((ScrollBarTemplate *)ptr)->colorTableRef;
		if ((moreFlags & 0x0c) == colorTableIsResource)
			check(rCtlColorTbl, ref);

		return;
	}


	if (ptr->procRef == iconButtonControl) {
		
		ref = ((IconButtonTemplate *)ptr)->titleRef;
		if ((moreFlags & 0x03) == titleIsResource)
			check(rPString, ref);

		ref = ((IconButtonTemplate *)ptr)->colorTableRef;
		if ((moreFlags & 0x0c) == colorTableIsResource)
			check(rCtlColorTbl, ref);

		ref = ((IconButtonTemplate *)ptr)->iconRef;
		if ((moreFlags & 0x30) == 0x20)
			check(rIcon, ref);

		return;
	}

	if (ptr->procRef == scrollBarControl) {
		
		ref = ((LineEditTemplate *)ptr)->defaultRef;
		if ((moreFlags & 0x03) == titleIsResource)
			check(rPString, ref);

		return;
	}

	if (ptr->procRef == listControl) {
		
		ref = ((ListTemplate *)ptr)->listRef;
		if ((moreFlags & 0x03) == titleIsResource)
			check(rListRef, ref);

		ref = ((ListTemplate *)ptr)->colorTableRef;
		if ((moreFlags & 0x0c) == colorTableIsResource)
			check(rCtlColorTbl, ref);

		return;
	}

	if (ptr->procRef == pictureControl) {
		
		ref = ((PictureTemplate *)ptr)->pictureRef;
		if ((moreFlags & 0x03) == titleIsResource)
			check(rPicture, ref);

		return;
	}

	if (ptr->procRef == popUpControl) {
		
		ref = ((PopupTemplate *)ptr)->menuRef;
		if ((moreFlags & 0x07) == titleIsResource)
			check(rMenu, ref);			

		ref = ((PopupTemplate *)ptr)->colorTableRef;
		if ((moreFlags & 0x18) == 0x10)
			check(rCtlColorTbl, ref);

		return;
	}


	if (ptr->procRef == growControl) {
		
		ref = ((SizeBoxTemplate *)ptr)->colorTableRef;
		if ((moreFlags & 0x0c) == colorTableIsResource)
			check(rCtlColorTbl, ref);

		return;
	}


	if (ptr->procRef == statTextControl) {
		
		ref = ((StaticTextTemplate *)ptr)->textRef;
		if ((moreFlags & 0x03) == titleIsResource)
			check(rTextForLETextBox2, ref);

		return;
	}


	if (ptr->procRef == editTextControl) {
		
		ref = ((TextEditTemplate *)ptr)->styleRef;
		if ((moreFlags & 0x03) == titleIsResource)
			check(rStyleBlock, ref);

		ref = ((TextEditTemplate *)ptr)->colorRef;
		if ((moreFlags & 0x0c) == colorTableIsResource)
			check(rCtlColorTbl, ref);

		return;
	}

}


void check_rWindParam1(Handle h) {
	WindParam1 *ptr = *(WindParam1 **)h;
	Ref ref;
	unsigned desc = ptr->p1InDesc;

	ref = (Ref)ptr->p1Title;
	if (ref && (desc & 0x0200)) check(rPString, ref);

	ref = (Ref)ptr->p1ColorTable;
	if (ref && (desc & 0x0800)) check(rWindColor, ref);

	ref = (Ref)ptr->p1ControlList;
	if (ref) {
		switch(desc & 0xff) {
			case singleResource: check(rControlTemplate, ref); break;
			case resourceToResource: check(rControlList, ref); break;
			default: break; /* warn about invalid value ? */
		}
	}

}
