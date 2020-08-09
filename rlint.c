#include <stdio.h>
//#include <err.h>
#include <stdlib.h>
//#include <unistd.h>
#include <string.h>

#include <gsos.h>
#include <resources.h>
#include <memory.h>

#include "rlint.h"


struct {
  ResType type;
  ResID id;
} history[10];
unsigned level;
unsigned error_count;
unsigned flag_v = 0;

struct node {
	struct node *next;
	unsigned status;
	ResType type;
	ResID id;
};

struct node *processed_map[256];


void free_processed_map(void) {
	struct node *e;
	unsigned i;

	for (i = 0; i < 256; ++i) {
		e = processed_map[i];
		while (e) {
			struct node *next = e->next;
			free(e);
			e = next;
		}
		processed_map[i] = 0;
	}

}
unsigned *processed(ResType type, ResID id) {
	unsigned hash = 0xaaaa;
	struct node *e;

	hash ^= type;
	hash ^= (unsigned)id;
	hash ^= id >> 16;

	hash ^= hash >> 8;
	hash &= 0xff;

	e = processed_map[hash];
	while(e) {
		if (e->type == type && e->id == id) return &e->status;
		e = e->next;
	}

	e = malloc(sizeof(struct node));
	e->next = processed_map[hash];
	e->status = 0;
	e->type = type;
	e->id = id;
	processed_map[hash] = e;
	return &e->status;
}

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
		if (i > 0) fputs(" > ", stderr);
		fprintf(stderr, "%s:%08lx", ResName(history[i].type), history[i].id);
	}
	fputs("\n  ", stderr);
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
		if (ref) check(rPString, ref);
	}

}


void check(ResType type, ResID id) {

	Handle h;
	void (*callback)(Handle);
	unsigned *status;

	if (flag_v) {
		fprintf(stdout," %s:%08lx\n", ResName(type), id);
	}

	history[level].type = type;
	history[level].id = id;
	++level;


	if (id == 0) {
		error("Invalid resource ID");
		--level;
		return;
	}

	status = processed(type, id);
	if (*status == 2) {
		/* missing */
		error("Unable to load Resource");
		--level;
		return;
	}
	if (*status == 1) {
		--level;
		return;
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
		case rBundle: callback = check_rBundle; break;
		case rAlertString:
		case rErrorString:
			callback = check_rAlertString;
			break;
		default: callback = 0;
	}


	h = LoadResource(type, id);
	if (_toolErr) {
		*status = 2;
		error("Unable to load Resource");
		--level;
		return;
	}
	*status = 1;

	if (callback) {
		HLock(h);
		callback(h);
		HUnlock(h);
	}
	ReleaseResource(-1, type, id);
	--level;
}




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


	if (flag_v) fprintf(stdout, "Checking %s\n", name);
	gname = c2gs(name);

	rfd = OpenResourceFile(0x8000 | readEnable, NULL, (Pointer)gname);
	if (_toolErr) {
		++error_count;
		fprintf(stderr, "%s: OpenResourceFile: $%04x\n", name, _toolErr);
		free(gname);
		return;
	}

	depth = SetResourceFileDepth(1);

	/* now iterate through all resource types... */
	for (ti = 1; ; ++ti) {
		ResType type = GetIndType(ti);
		if (_toolErr == resIndexRange) break;
		if (_toolErr) {
			fprintf(stderr, "%s: GetIndType: $%04x\n", name, _toolErr);
			continue;
		}

		switch(type) {
			case rMenu:
			case rMenuItem:
			case rMenuBar:
			case rItemStruct:
			case rControlList:
			case rControlTemplate:
			case rWindParam1:
			case rStringList:
			case rBundle:
			case rAlertString:
			case rErrorString:
				break;
			default:
				continue;
		}

		for (ri = 1; ; ++ri) {
			ResID id = GetIndResource(type, ri);
			if (_toolErr == resIndexRange) break;
			if (_toolErr) {
				fprintf(stderr, "%s: GetIndResource: $%04x\n", name, _toolErr);
				continue;
			}
			check(type, id);
		}
	}



	SetResourceFileDepth(depth);
	CloseResourceFile(rfd);

	free_processed_map();
	free(gname);
}

int main(int argc, char **argv) {

	int c;
	unsigned i;

	flag_v = 0;
	error_count = 0;
	level = 0;
	memset(processed_map, 0, sizeof(processed_map));
	memset(history, 0, sizeof(history));

	for (i = 1; i < argc; ++i) {
		char *cp = argv[i];
		if (cp[0] != '-') break;
		if (cp[1] == '-' && cp[2] == 0) {++i; break; }
		if (cp[1] == 'v') flag_v = 1;
		else usage(1);
	}
	argv += i;
	argc -= i;
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
	if (argc == 0) usage(0);


	ResourceStartUp(MMStartUp());
	for (i = 0; i < argc; ++i) {
		one_file(argv[i]);
	}
	ResourceShutDown();

	if (error_count) return 1;
	return 0;
}