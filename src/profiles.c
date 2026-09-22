#include <stdlib.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <stdio.h>
#include <errno.h>
#include <dirent.h>
#include <string.h>
#include "launcher.h"

char* profileDir = "profiles"; // TODO: config option
char* cuProfile = "CommunityUpdates";

// count number of profiles. caller must free return value using FreeProfiles
ProfileList GetProfiles()
{
    int length = 0;
    int check = CheckFile(profileDir);
    if (check == ENOENT)
    {
        mkdir(profileDir, 0777);
        ProfileList result = {
            .ptr = NULL,
            .length = 0
        };
        return result;
    }
    else if (check != 0)
    {
        perror("Failed to stat the profiles directory");
        exit(1);
    }

    int size = 8;
    char** dirs = calloc(size, sizeof(char*));
    DIR* dir = opendir(profileDir);
    struct dirent* ent;
    while ((ent = readdir(dir)) != NULL)
    {
        if (ent->d_type == DT_DIR && strcmp(ent->d_name, ".") && strcmp(ent->d_name, ".."))
        {
            dirs[length] = malloc(strlen(ent->d_name) + 1);
            strcpy(dirs[length], ent->d_name);
            length++;
            if (length >= size)
            {
                size *= 2;
                dirs = realloc(dirs, size * sizeof(char*));
            }
        }
    }
    ProfileList result = {
        .ptr = dirs,
        .length = length
    };
    return result;
}

void FreeProfiles(ProfileList list)
{
    for (int i = 0; i < list.length; i++) {
        free(list.ptr[i]);
    }
    free(list.ptr);
}

// check if CU is installed
bool IsCUInstalled()
{
    char* dir = PathCat(profileDir, cuProfile);
    int check = CheckFile(dir);
    free(dir);
    return check == 0;
}

int GetProfileCount()
{
    ProfileList arr = GetProfiles();
    int count = arr.length;
    FreeProfiles(arr);
    return count;
}
