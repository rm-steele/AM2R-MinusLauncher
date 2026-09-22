#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "launcher.h"

int main()
{
    bool git = system("git --version &> /dev/null") == 0;
    bool xdelta = system("xdelta3 -V &> /dev/null") == 0;
    bool unzip = system("unzip &> /dev/null") == 0;
    if (!(git && xdelta && unzip))
    {
        if (!git) puts("FATAL: You must install git and have it on your PATH to use this program");
        if (!xdelta) puts("FATAL: You must install xdelta3 and have it on your PATH to use this program");
        if (!unzip) puts("FATAL: You must install unzip and have it on your PATH to use this program");
        exit(1);
    }

    if (!CheckForAM2R11())
    {
        BegForAM2R11();
        if (!CheckForAM2R11())
        {
            puts("AM2R 1.1's data.win could not be found! Something must have gone wrong extracting it. Please report this.");
            exit(1);
        }
        else puts("1.1 successfully copied for patching.");
    }

    while (MainMenu()) {}

    return 0;
}
