#ifndef _UI_H
#define _UI_H

#include "callback.h"

typedef struct
{
	GtkWidget *entry;
	char *env;
	gboolean save;
}ENV_DATA;

void create_env_interface(char *title,GtkWidget *hbox,callback func,char *conf,gboolean save);

#endif
