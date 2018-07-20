#include <types.h>
#include <resources.h>

void check_rMenu(Handle h);
void check_rMenuItem(Handle h);
void check_rMenuBar(Handle h);
void check_rItemStruct(Handle h);
void check_rControlList(Handle h);
void check_rControlTemplate(Handle h);
void check_rWindParam1(Handle h);
void check_rStringList(Handle h);

#define check_rPString 0
#define check_rIcon 0
#define check_rWindColor 0
#define check_rCtlColorTbl 0

void error(const char *msg);
void check(ResType type, ResID id, void (*callback)(Handle));