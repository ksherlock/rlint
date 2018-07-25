#include <resources.h>
#include "rlint.h"

typedef struct BundleHeader {
	Word version;
	Word offset;
	Ref iconID;
	Ref bundleID;
	LongWord reserved;
	Word count;
} BundleHeader;

typedef struct OneDocHeader {
	Word oneDocSize;
	Word offsetToFlags;
	Word numResults;
	/*result 1 */
	Word voting;
	/*result 2 */
	LongWord pathID;
	LongWord reserved1;
	/* result 3 */
	Ref iconID;
	LongWord reserved2;
	/* result 4 */
	Ref smallIconID;
	LongWord reserved3;
	/* result 5 */
	char docString[1]; /* p string */
	/* LongWord match flags, custom bits */

} OneDocHeader;


/* equates for rBundle */

#define LaunchThis          0x1
#define DontLaunch          0x0

#define reads               0x10
#define writes              0x20
#define native              0x40
#define creator             0x80

#define Filetype            0x1
#define AuxType             0x2
#define Filename            0x4
#define CreateDateTime      0x8
#define ModDateTime         0x10
#define LocalAccess         0x20
#define NetworkAccess       0x40
#define Where               0x80
#define HFSFileType         0x100
#define HFSCreator          0x200
#define OptionList          0x400
#define EOF                 0x800

#define CompareAll      0x00FF       // compareSpec operators 
#define CompareSecond   0x0001
#define CompareMinute   0x0002
#define CompareHour     0x0004
#define CompareYear     0x0008
#define CompareDay      0x0010
#define CompareMonth    0x0020
#define CompareWeekday  0x0080
#define lessThan        0x0000
#define equal           0x0100
#define greater         0x0200
#define not             0x0400

void check_rBundle(Handle h){
	
	BundleHeader *ptr;
	OneDocHeader *doc;
	Ref ref;
	unsigned offset = 0;
	unsigned i;
	unsigned count;

	ptr = *(BundleHeader **)h;
	ref = ptr->iconID;
	if (ref) check(rIcon, ref);

	/* bundleID should match the current id, but that would be a recursive loop */

	count = ptr->count;
	offset = sizeof(BundleHeader);
	for(i = 0; i < count; ++i) {
		unsigned numResults;
		doc = (OneDocHeader *)((byte *)ptr + offset);

		switch(doc->numResults) {
			case 5:
			case 4:
				ref = doc->smallIconID;
				if (ref) check(rIcon, ref);
			case 3:
				ref = doc->iconID;
				if (ref) check(rIcon, ref);
			case 2:
				ref = doc->pathID;
				if (ref) check(rFinderPath, ref);
			case 1:
			case 0:
				break;
		}

		offset += doc->oneDocSize;
	}

}