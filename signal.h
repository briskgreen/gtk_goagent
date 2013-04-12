#ifndef _SIGNAL_H
#define _SIGNAL_H

#include "dialog.h"

void connect_goagent(GtkWidget *widget,gpointer data);

void disconnect_goagent(GtkWidget *widget,gpointer data);

void help(GtkWidget *widget,gpointer data);

void about(GtkWidget *widget,gpointer data);

void tray_on_menu(GtkWidget *widget,gpointer data);

void hide_window(GtkWidget *widget,GdkEventWindowState *event,gpointer data);

void tray_on_click(GtkWidget *widget,gpointer data);

#endif
