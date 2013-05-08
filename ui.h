#ifndef _UI_H
#define _UI_H

#include "config.h"
#include <string.h>

typedef struct
{
	GtkWidget *entry;
	char *title;
	char **env;
	gboolean *save;
}ENV_DATA;

void create_env_interface(char *title,GtkWidget *hbox,callback func,char **conf,gboolean *save);

void select_python_path(GtkWidget *widget,gpointer data);

void select_goagent_path(GtkWidget *widget,gpointer data);

void select_gtk_goagent_path(GtkWidget *widget,gpointer data);

void select_language(GtkWidget *widget,gpointer data);

void select_font(GtkWidget *widget,gpointer data);

void select_auto_upgrade_goagent(GtkWidget *widget,gpointer data);

void select_auto_upgrade_gtk_goagent(GtkWidget *widget,gpointer data);

void exit_pre(GtkWidget *widget,CONFDATA *data);

void save_conf_with_exit(GtkWidget *widget,gpointer data);

#endif
