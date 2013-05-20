#ifndef _MENU_H
#define _MENU_H

#include <gtk/gtk.h>
#include <locale.h>
#include <libintl.h>
#include <stdlib.h>

#define _(string) gettext(string)
#define N_(string) string

typedef struct
{
	gchar *name;
	gchar *version;
	gchar *copyright;
	gchar *comments;
	gchar *license;
	gchar *website;
	//const gchar **authors;
}ABOUT;

typedef void (*callback)(GtkWidget *,gpointer);

void init_about_data(ABOUT *about);

void create_menu_with_image(GtkWidget *menu,gchar *item,GtkAccelGroup *accel_group,callback func,gpointer data);

GtkWidget *create_menu(GtkWidget *menu_bar,gchar *name);

#endif
