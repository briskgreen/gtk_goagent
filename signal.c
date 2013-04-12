#include "signal.h"

void connect_goagent(GtkWidget *widget,gpointer data)
{}

void disconnect_goagent(GtkWidget *widget,gpointer data)
{}

void help(GtkWidget *widget,gpointer data)
{}

void about(GtkWidget *widget,gpointer data)
{}

void tray_on_menu(GtkWidget *widget,gpointer data)
{}

void hide_window(GtkWidget *widget,GdkEventWindowState *event,gpointer data)
{
	if(event->changed_mask == GDK_WINDOW_STATE_ICONIFIED && event->new_window_state == GDK_WINDOW_STATE_ICONIFIED)
	{
		gtk_widget_hide_all(widget);
		gtk_status_icon_set_visible(GTK_STATUS_ICON(data),TRUE);
	}
}

void tray_on_click(GtkWidget *widget,gpointer data)
{
	gtk_widget_show_all(GTK_WIDGET(data));
	gtk_window_present(GTK_WINDOW(data));
	gtk_status_icon_set_visible(GTK_STATUS_ICON(widget),FALSE);
}
