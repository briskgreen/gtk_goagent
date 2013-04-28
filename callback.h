#ifndef _CALLBACK_H
#define _CALLBACK_H

#include "config.h"
#include <string.h>
#include <signal.h>
#include <unistd.h>

void connect_goagent(GtkWidget *widget,DATA *data);

void disconnect_goagent(GtkWidget *widget,DATA *data);

void clean_buffer(GtkWidget *widget,DATA *data);

void help(GtkWidget *widget,gpointer data);

void about(GtkWidget *widget,gpointer data);

void update(GtkWidget *widget,gpointer data);

void preferences(GtkWidget *widget,gpointer data);

//void change_language(GtkWidget *widget,gpointer data);

//void tray_on_menu(GtkWidget *widget,gpointer data);
void tray_on_menu(GtkWidget *widget,guint button,
		guint32 activate_time,gpointer data);

void hide_win(GtkWidget *widget,gpointer data);

void hide_window(GtkWidget *widget,GdkEventWindowState *event,gpointer data);

void tray_on_click(GtkWidget *widget,gpointer data);

void kill_pthread(int signum);

void select_python_path(GtkWidget *widget,gpointer data);

void select_goagent_path(GtkWidget *widget,gpointer data);

void select_gtk_goagent_path(GtkWidget *widget,gpointer data);

void select_language(GtkWidget *widget,gpointer data);

#endif
