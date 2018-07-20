#include <stdio.h>
//#include <err.h>
#include <stdlib.h>
//#include <unistd.h>
#include <string.h>

#include <gsos.h>
#include <resources.h>


#include "rlint.h"


struct {
  ResType type;
  ResID id;
} history[10];
unsigned level;
unsigned error_count;



char *ResName(ResType type) {
	static char buffer[6] = { '$', 'x', 'x', 'x', 'x', 0 };
	static char hex[] = "0123456789abcdef";

	switch(type) {
	case rIcon: return "rIcon";
	case rPicture: return "rPicture";
	case rControlList: return "rControlList";
	case rControlTemplate: return "rControlTemplate";
	case rC1InputString: return "rC1InputString";
	case rPString: return "rPString";
	case rStringList: return "rStringList";
	case rMenuBar: return "rMenuBar";
	case rMenu: return "rMenu";
	case rMenuItem: return "rMenuItem";
	case rTextForLETextBox2: return "rTextForLETextBox2";
	case rCtlDefProc: return "rCtlDefProc";
	case rCtlColorTbl: return "rCtlColorTbl";
	case rWindParam1: return "rWindParam1";
	case rWindParam2: return "rWindParam2";
	case rWindColor: return "rWindColor";
	case rTextBlock: return "rTextBlock";
	case rStyleBlock: return "rStyleBlock";
	case rToolStartup: return "rToolStartup";
	case rResName: return "rResName";
	case rAlertString: return "rAlertString";
	case rText: return "rText";
	case rCodeResource: return "rCodeResource";
	case rCDEVCode: return "rCDEVCode";
	case rCDEVFlags: return "rCDEVFlags";
	case rTwoRects: return "rTwoRects";
	case rFileType: return "rFileType";
	case rListRef: return "rListRef";
	case rCString: return "rCString";
	case rXCMD: return "rXCMD";
	case rXFCN: return "rXFCN";
	case rErrorString: return "rErrorString";
	case rKTransTable: return "rKTransTable";
	case rWString: return "rWString";
	case rC1OutputString: return "rC1OutputString";
	case rSoundSample: return "rSoundSample";
	case rTERuler: return "rTERuler";
	case rFSequence: return "rFSequence";
	case rCursor: return "rCursor";
	case rItemStruct: return "rItemStruct";
	case rVersion: return "rVersion";
	case rComment: return "rComment";
	case rBundle: return "rBundle";
	case rFinderPath: return "rFinderPath";
	case rPaletteWindow: return "rPaletteWindow";
	case rTaggedStrings: return "rTaggedStrings";
	case rPatternList: return "rPatternList";
	case rRectList: return "rRectList";
	case rPrintRecord: return "rPrintRecord";
	case rFont: return "rFont";
	}
	buffer[1] = hex[(type >> 12) & 0x0f];
	buffer[2] = hex[(type >> 8) & 0x0f];
	buffer[3] = hex[(type >> 4) & 0x0f];
	buffer[4] = hex[(type >> 0) & 0x0f];
	return buffer;
}

void error(const char *msg) {
	unsigned i;
	for (i = 0; i < level; ++i) {
		fprintf(stderr, "%s:%08lx -> ", ResName(history[i].id), history[i].type);
	}
	fputs(msg, stderr);
	fputs("\n", stderr);
	++error_count;
}





void check_rStringList(Handle h){

	typedef struct StringList {
		unsigned count;
		Ref strings[1];
	} StringList;

	StringList *ptr = *(StringList **)h;
	unsigned i;
	for (i = 0; i < ptr->count; ++i) {
		Ref ref = ptr->strings[i];
		if (ref) check(rPString, ref, check_rPString);
	}

}


void check(ResType type, ResID id, void (*callback)(Handle)) {

	Handle h;


	history[level].type = type;
	history[level].id = id;
	++level;

	if (id == 0) {
		error("Invalid resource ID");
		return;
	}

	h = LoadResource(type, id);
	if (_toolErr) {
		error("Unable to load Resource");
		return;
	}

	if (callback) {
		HLock(h);
		callback(h);
		HUnlock(h);
	}
	ReleaseResource(type, id, -1);
	--level;
}



unsigned flag_v = 0;

void usage(int rv) {
	fputs("rlint [-v] file [...]\n", stderr);
	exit(rv);
}

GSString255Ptr c2gs(const char *cp) {
	GSString255Ptr gs;
	int l = strlen(cp);
	gs = malloc(l + 3);
	if (gs) {
		gs->length = l;
		strcpy(gs->text, cp);
	}
	return gs;
}



void one_file(const char *name) {
	GSString255Ptr gname;
	unsigned rfd;
	unsigned depth;
	unsigned ti;
	unsigned long ri;

	gname = c2gs(name);

	rfd = OpenResourceFile(0x8000 | readEnable, NULL, (Pointer)gname);
	if (_toolErr) {
		++error_count;
		fprintf(stderr, "%s: OpenResourceFile: $%04x\n", name);
		free(gname);
		return;
	}

	depth = SetResourceFileDepth(1);

	/* now iterate through all resource types... */
	for (ti = 1; ; ++ti) {
		void (*callback)(Handle) = 0;
		ResType type = GetIndType(ti);
		if (_toolErr == resIndexRange) break;
		if (_toolErr) {
			fprintf(stderr, "%s: GetIndType: $%04x\n", name, _toolErr);
			continue;
		}

		switch(type) {
			case rMenu: callback = check_rMenu; break;
			case rMenuItem: callback = check_rMenuItem; break;
			case rMenuBar: callback = check_rMenuBar; break;
			case rItemStruct: callback = check_rItemStruct; break;
			case rControlList: callback = check_rControlList; break;
			case rControlTemplate: callback = check_rControlTemplate; break;
			case rWindParam1: callback = check_rWindParam1; break;
			case rStringList: callback = check_rStringList; break;
			default: callback = 0;
		}

		for (ri = 1; ; ++ri) {
			ResID id = GetIndResource(type, ri);
			if (_toolErr == resIndexRange) break;
			if (_toolErr) {
				fprintf(stderr, "%s: GetIndResource: $%04x\n", name, _toolErr);
				continue;
			}
			check(type, id, callback);
		}
	}



	SetResourceFileDepth(depth);
	CloseResourceFile(rfd);
	free(gname);
}

int main(int argc, char **argv) {

	int c;
	unsigned i;
/*
	for((c = getopt(argc, argv, "hv")) != -1) {

		switch(c) {
			case 'v': flag_v = 1;
			case 'h':
				usage(0);
			case '?'
			case ':'
				usage(1);
		}
	}
	argv += optind;
	argc -= optind;
*/
	argv++;
	argc--;
	if (argc == 0) usage(0);


	ResourceStartUp(MMStartUp());
	for (i = 0; i < argc; ++i) {
		one_file(argv[i]);
	}
	ResourceShutDown();

	if (error_count) return 1;
	return 0;
}