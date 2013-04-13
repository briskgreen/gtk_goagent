#include "signal.h"
#include <string.h>

void create_tray(GtkWidget *win)
{
	GtkStatusIcon *tray;

	tray=gtk_status_icon_new_from_file("goagent.png");
	gtk_status_icon_set_tooltip_text(tray,"GoAgent");
	gtk_status_icon_set_visible(tray,FALSE);
	g_signal_connect(G_OBJECT(tray),"activate",G_CALLBACK(tray_on_click),win);
	g_signal_connect(G_OBJECT(tray),"popup_menu",G_CALLBACK(tray_on_menu),win);
	g_signal_connect(G_OBJECT(win),"window_state_event",G_CALLBACK(hide_window),tray);
}

void change_time(gpointer *label)
{
	static time_t t;
	static char buf[30];

	t=time(NULL);
	snprintf(buf,strlen(ctime(&t)),"%s",ctime(&t));
	gtk_label_set_text(GTK_LABEL(label),buf);
}

void show_time(GtkWidget *widget)
{
	GtkWidget *label;

	label=gtk_label_new("00:00:00");
	gtk_box_pack_start(GTK_BOX(widget),label,FALSE,FALSE,0);
	g_timeout_add(1000,(GSourceFunc)change_time,label);
}

int main(int argc,char **argv)
{
	GtkWidget *win;
	GtkWidget *menu;
	GtkWidget *menu_bar;
	GtkWidget *vbox;
	GtkWidget *hbox;
	GtkWidget *text;
	GtkWidget *scrolled;
	GtkWidget *open;
	GtkWidget *close;
	GtkAccelGroup *accel_group;
	DATA data;

	gtk_init(&argc,&argv);

	win=gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_position(GTK_WINDOW(win),GTK_WIN_POS_CENTER);
	gtk_window_set_title(GTK_WINDOW(win),"Gtk GoAgent");
	gtk_window_set_icon_from_file(GTK_WINDOW(win),"goagent.png",NULL);
	g_signal_connect(G_OBJECT(win),"delete_event",G_CALLBACK(really_quit),NULL);

	create_tray(win);

	vbox=gtk_vbox_new(FALSE,0);
	accel_group=gtk_accel_group_new();
	gtk_window_add_accel_group(GTK_WINDOW(win),accel_group);
	gtk_container_add(GTK_CONTAINER(win),vbox);

	text=gtk_text_view_new();
	data.text=text;
	data.off=0;

	menu_bar=gtk_menu_bar_new();
	gtk_box_pack_start(GTK_BOX(vbox),menu_bar,FALSE,FALSE,0);
	menu=create_menu(menu_bar,"_File");
	create_menu_with_image(menu,GTK_STOCK_OPEN,accel_group,connect_goagent,&data);
	create_menu_with_image(menu,GTK_STOCK_CLOSE,accel_group,disconnect_goagent,&data);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu),gtk_separator_menu_item_new());
	create_menu_with_image(menu,GTK_STOCK_QUIT,accel_group,really_quit,NULL);

	menu=create_menu(menu_bar,"_Edit");
	create_menu_with_image(menu,GTK_STOCK_PROPERTIES,accel_group,properties,NULL);

	menu=create_menu(menu_bar,"_Help");
	create_menu_with_image(menu,GTK_STOCK_HELP,accel_group,help,NULL);
	create_menu_with_image(menu,GTK_STOCK_ABOUT,accel_group,about,NULL);

	//text=gtk_text_view_new();
	gtk_widget_set_size_request(text,0x280,0x150);
	gtk_text_view_set_editable(GTK_TEXT_VIEW(text),FALSE);
	scrolled=gtk_scrolled_window_new(NULL,NULL);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled),GTK_POLICY_AUTOMATIC,GTK_POLICY_AUTOMATIC);
	gtk_container_add(GTK_CONTAINER(scrolled),text);
	gtk_box_pack_start(GTK_BOX(vbox),scrolled,FALSE,FALSE,0);

	gtk_box_pack_start(GTK_BOX(vbox),gtk_separator_menu_item_new(),FALSE,FALSE,5);
	show_time(vbox);
	gtk_box_pack_start(GTK_BOX(vbox),gtk_separator_menu_item_new(),FALSE,FALSE,5);

	hbox=gtk_hbox_new(FALSE,0);
	gtk_box_pack_start(GTK_BOX(vbox),hbox,FALSE,FALSE,5);
	
	open=gtk_button_new_with_label("Connect");
	gtk_box_pack_start(GTK_BOX(hbox),open,FALSE,FALSE,10);
	g_signal_connect(G_OBJECT(open),"clicked",G_CALLBACK(connect_goagent),&data);
	close=gtk_button_new_with_label("Disconnect");
	gtk_box_pack_end(GTK_BOX(hbox),close,FALSE,FALSE,10);
	g_signal_connect(G_OBJECT(close),"clicked",G_CALLBACK(disconnect_goagent),&data);

	gtk_widget_show_all(win);
	gtk_main();

	return 0;
}
