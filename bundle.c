#include <resources.h>
#include "rlint.h"

typedef struct BundleHeader {
	Word version;
	Word offset;
	Ref iconID;
	Ref bundleID;
	LongWord reserved;
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



void check_rBundle(Handle h){
	
	BundleHeader *ptr;
	OneDocHeader *doc;
	Ref ref;

	ptr = *(BundleHeader **)h;
	ref = h->iconID;
	if (ref) check(rIcon, ref);

	/* bundleID should match the current id, but that would be an uneneding loop */

}