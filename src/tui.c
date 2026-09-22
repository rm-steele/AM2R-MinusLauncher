#include <linux/limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include "launcher.h"

typedef struct
{
    char* opText;
    void (*selected)();
    bool closeAfter;

} MenuOption;

static void Op_Download()
{
    if (!CloneAutopatcher())
        puts("Something went wrong trying to set up the patch data.");
}

static void Op_InstallCU()
{
    char* path = PathCat(patchDataPath, "data");
    InstallMod(path, cuProfile);
    free(path);
}

static void Op_InstallMod()
{
    system("rm -r resources/tempmod &> /dev/null");
    puts("Please provide the path to the mod zip you wish to install.");
    GetAndUnzipFile("resources/tempmod");

    fputs("Name of mod: ", stdout);
    char name[NAME_MAX];
    GetInput(name, NAME_MAX);

    if (InstallMod("resources/tempmod", name))
        puts("Mod installed successfully!");

    system("rm -r resources/tempmod &> /dev/null");
}

static void Op_Play()
{
    ProfileList profileList = GetProfiles();
    int count = profileList.length;
    char** profiles = profileList.ptr;
    int choice;
    while (true)
    {
        puts("\nChoose a profile to play:\n");
        for (int i = 0; i < count; i++)
        {
            printf("[%d] %s\n", i + 1, profiles[i]);
        }

        fputs("\nProfile number: ", stdout);

        char in[32];
        GetInput(in, 32);
        choice = atoi(in) - 1;

        if (choice >= 0 && choice < count) break;

        printf("Invalid choice: %s\n", in);
    }
    char* chosenProfile = profiles[choice];

    printf("Opening profile %s...\n", chosenProfile);
    fflush(stdout);

    // pre-calc these because the child is only allowed to touch the PID and call exec
    char* profilePath = PathCat(profileDir, chosenProfile);
    char* scriptName = "run-with-libs.sh";
    char* scriptPath = PathCat(profilePath, scriptName);
    pid_t pid;

    char popDir[PATH_MAX];
    getcwd(popDir, PATH_MAX);
    chdir(profilePath);

    if ((pid = vfork()) == 0)
    { // child
        execl(scriptName, scriptName, (char*)NULL);
        exit(0);
    }
    else
    { // parent
        if (pid < 0) perror("Failed to launch profile");
        sleep(1); // give the user the illusion of the program doing something
    }

    chdir(popDir);

    free(scriptPath);
    free(profilePath);
    FreeProfiles(profileList);
}

static void Op_Exit() {}

MenuOption op_download = {
    .opText = "Download Patch Data",
    .selected = Op_Download,
    .closeAfter = false
};
MenuOption op_installCU = {
    .opText = "Install Community Updates",
    .selected = Op_InstallCU,
    .closeAfter = false
};
MenuOption op_installMod = {
    .opText = "Install a mod",
    .selected = Op_InstallMod,
    .closeAfter = false
};
MenuOption op_play = {
    .opText = "Launch a profile",
    .selected = Op_Play,
    .closeAfter = false
};
MenuOption op_exit = {
    .opText = "Exit",
    .selected = Op_Exit,
    .closeAfter = true
};

#define ITEMS 5
MenuOption* options[ITEMS];

bool MainMenu()
{
    for (int i = 0; i < ITEMS; i++)
        options[i] = NULL;

    int pos = 0;
    if (GetProfileCount() > 0) // show play option
    {
        options[pos] = &op_play;
        pos++;
    }
    if (!CheckPatchData(false)) // show download option
    {
        options[pos] = &op_download;
        pos++;
    }
    if (!IsCUInstalled() && CheckPatchData(false)) // show install CU option
    {
        options[pos] = &op_installCU;
        pos++;
    }
    if (CheckPatchData(false)) // show install mod option
    {
        options[pos] = &op_installMod;
        pos++;
    }
    if (true) // show exit option
    {
        options[pos] = &op_exit;
        pos++;
    }

    if (pos == 0)
    {
        puts("It seems you have no options. You've met with a terrible fate, haven't you?");
        exit(1);
    }

    puts("\nChoose an option:\n");
    for (int i = 0; i < ITEMS; i++)
    {
        if (options[i] == NULL) continue;
        printf("[%d] ", i + 1);
        puts(options[i]->opText);
    }
    fputs("\nOption number: ", stdout);

    char in[32];
    GetInput(in, 32);
    int choice = atoi(in) - 1;

    if (choice < 0 || choice > ITEMS || options[choice] == NULL)
    {
        printf("Invalid choice: %s\n", in);
        return true;
    }
    options[choice]->selected();
    return !(options[choice]->closeAfter);
}
