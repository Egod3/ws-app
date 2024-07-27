
#include <stdio.h>
#include <string.h>
#include <zephyr/shell/shell.h>
#include "app_version.h"

static int version(const struct shell* sh)
{
    shell_print(sh, "App version: v%d.%d.%d.%d", APP_VERSION_MAJOR, APP_VERSION_MINOR, APP_PATCHLEVEL, APP_TWEAK);
    return 0;
}

/* Creating root (level 0) command "version" */
SHELL_CMD_REGISTER(version, NULL, "the application version", version);
