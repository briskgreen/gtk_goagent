#ifndef _CONFIG_H
#define _CONFIG_H

#include <stdio.h>
#include "dialog.h"
#include <errno.h>

#define HOME getenv("HOME")
#define CONFFILE "/.gtk_goagentrc"
#define PYPATH "/local/proxy.py"
#define UPPATH "/server/uploader.zip"

typedef struct
{
	gboolean save;
	char *python_path;
	char *goagent_path;
	char *language_env;
	char *gtk_goagent_path;
}CONFDATA;

gboolean is_config_file_ok(void);

FILE *open_config(CONFDATA *data);

void close_config(FILE *fp,CONFDATA *data);

void save_config(FILE **fp,CONFDATA *data);

char *get_python_path(FILE *fp);

char *get_goagent_path(FILE *fp);

char *get_language_env(FILE *fp);

char *get_gtk_goagent_path(FILE *fp);

void set_python_path(CONFDATA *data,char *arg);

void set_goagent_path(CONFDATA *data,char *arg);

void set_language_env(CONFDATA *data,char *arg);

void set_gtk_goagent_path(CONFDATA *data,char *arg);

#endif
