#ifndef __AM2RLAUNCHER_MINUS_H
#define __AM2RLAUNCHER_MINUS_H

#include <stdbool.h>

// util.c
int CheckFile(char* path);
char* PathCat(const char* s1, const char* s2);
int LowercaseFile(const char* name, const char* path);
void GetInput(char* buf, int max);
bool GetAndUnzipFile(char* outputPath);

// setup.c
extern char* repo;
extern char* patchDataPath;
extern char* pathTo11;

bool CloneAutopatcher();
bool CheckPatchData(bool installIfMissing);
bool CheckForAM2R11();
void BegForAM2R11();

// patcher.c
bool InstallMod(char* modPath, char* profileName);

// profiles.c
extern char* profileDir;
extern char* cuProfile;

typedef struct {
    char** ptr;
    int length;
} ProfileList;

ProfileList GetProfiles();
void FreeProfiles(ProfileList list);
bool IsCUInstalled();
int GetProfileCount();

// tui.c
bool MainMenu();

#endif
