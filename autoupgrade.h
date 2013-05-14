#ifndef _AUTOUPGRADE_H
#define _AUTOUPGRADE_H

#include <curl/curl.h>
#include <zlib.h>
#include "config.h"

#define GOAGENT_URL "https://code.google.com/p/goagent/"
//#define GTK_GOAGENT_URL "https://briskgreen.github.io/Download/Gtk GoAgent"
#define PROXY "127.0.0.1:8087"

void auto_upgrade_goagent(char *url);

//void auto_upgrade_gtk_goagent(char *url);

void unzip(char *path);

#endif
