#ifndef _MENU_H
#define _MENU_H

#include <gtk/gtk.h>

typedef struct
{
	gchar *name;
	gchar *version;
	gchar *copyright;
	gchar *comments;
	gchar *license;
	gchar *website;
	gchar **authors;
}ABOUT;

typedef void (*callback)(GtkWidget *,gpointer);

void init_about_data(ABOUT *about);

void create_menu_with_image(GtkWidget *menu,gchar *item,GtkAccelGroup *accel_group,callback func,gpointer data);

GtkWidget *create_menu(GtkWidget *menu_bar,gchar *name);

#endif
