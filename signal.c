#include "signal.h"

void get_connect(DATA *data);

void connect_goagent(GtkWidget *widget,DATA *data)
{
	static pthread_t thread;
	GtkTextBuffer *buffer=gtk_text_view_get_buffer(GTK_TEXT_VIEW(data->text));

	if(data->off)
	{
		//gtk_text_buffer_insert_at_cursor(buffer,"\nConnected Now\n",15);
		message_box(widget,"Connected Now\n");
		return;
	}

	pthread_create(&thread,NULL,(void *)get_connect,data);
	data->thread=thread;
	data->off=1;
}

void disconnect_goagent(GtkWidget *widget,DATA *data)
{
	GtkTextBuffer *buffer=gtk_text_view_get_buffer(GTK_TEXT_VIEW(data->text));

	data->off=0;
	pthread_cancel(data->thread);
	gtk_text_buffer_set_text(buffer," ",1);
}

void help(GtkWidget *widget,gpointer data)
{}

void about(GtkWidget *widget,gpointer data)
{}

void properties(GtkWidget *widget,gpointer data)
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

void get_connect(DATA *data)
{
	GtkTextBuffer *buffer=gtk_text_view_get_buffer(GTK_TEXT_VIEW(data->text));
	GtkTextMark *mark=gtk_text_buffer_get_insert(buffer);
	GtkTextIter iter;

	gtk_text_buffer_get_iter_at_mark(buffer,&iter,mark);

	while(1)
	{
		//gtk_text_buffer_insert_at_cursor(buffer,"Hello World\n",12);
		//gtk_text_buffer_get_iter_at_mark(buffer,&iter,mark);
		gtk_text_buffer_insert(buffer,&iter,"Hello World\n",12);
		sleep(1);
	}
}
