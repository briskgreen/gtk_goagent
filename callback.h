#ifndef _CALLBACK_H
#define _CALLBACK_H

#include "dialog.h"
#include <pthread.h>
#include <string.h>
#include <signal.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>

typedef struct 
{
	GtkWidget *text;
	pthread_t thread;
	int off;
	pid_t pid;
	guint offset;
}DATA;

void connect_goagent(GtkWidget *widget,DATA *data);

void disconnect_goagent(GtkWidget *widget,DATA *data);

void help(GtkWidget *widget,gpointer data);

void about(GtkWidget *widget,gpointer data);

void properties(GtkWidget *widget,gpointer data);

void tray_on_menu(GtkWidget *widget,gpointer data);

void hide_window(GtkWidget *widget,GdkEventWindowState *event,gpointer data);

void tray_on_click(GtkWidget *widget,gpointer data);

#endif
