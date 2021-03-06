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

void check_rBundle(Handle h);

void check_rAlertString(Handle h);


void error(const char *msg);
void check(ResType type, ResID id);

#pragma lint -1
#pragma optimize -1
