#include <gtk/gtk.h>

int main(int argc,char **argv)
{
	GtkWidget *win;
	GtkWidget *text;
	GtkWidget *scroll;
	GtkTextBuffer *text_buffer;

	gtk_init(&argc,&argv);

	win=gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_position(GTK_WINDOW(win),GTK_WIN_POS_CENTER);
	g_signal_connect(G_OBJECT(win),"destroy",G_CALLBACK(gtk_main_quit),NULL);

	text=gtk_text_view_new();
	gtk_widget_set_size_request(text,200,100);
	scroll=gtk_scrolled_window_new(NULL,NULL);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scroll),GTK_POLICY_AUTOMATIC,GTK_POLICY_AUTOMATIC);
	gtk_container_add(GTK_CONTAINER(scroll),text);
	gtk_container_add(GTK_CONTAINER(win),scroll);

	text_buffer=gtk_text_buffer_new(gtk_text_tag_table_new());
	gtk_text_buffer_set_text(text_buffer,"Hello World!",11);
	gtk_text_view_set_buffer(GTK_TEXT_VIEW(text),text_buffer);

	gtk_widget_show_all(win);
	gtk_main();

	return 0;
}
