#include "callback.h"

void create_tray(GtkWidget *win)
{
	GtkStatusIcon *tray;
	GtkWidget *menu;
	GtkWidget *item;

	tray=gtk_status_icon_new_from_file("img/gtk_goagent.png");
	gtk_status_icon_set_tooltip_text(tray,"Gtk GoAgent");
	gtk_status_icon_set_visible(tray,TRUE);
	g_signal_connect(G_OBJECT(tray),"activate",G_CALLBACK(tray_on_click),win);

	menu=gtk_menu_new();
	item=gtk_menu_item_new_with_mnemonic(_("_Show"));
	gtk_menu_shell_append(GTK_MENU_SHELL(menu),item);
	g_signal_connect(G_OBJECT(item),"activate",G_CALLBACK(tray_on_click),win);
	item=gtk_menu_item_new_with_mnemonic(_("H_ide"));
	gtk_menu_shell_append(GTK_MENU_SHELL(menu),item);
	g_signal_connect(G_OBJECT(item),"activate",G_CALLBACK(hide_win),win);
	item=gtk_image_menu_item_new_from_stock(GTK_STOCK_QUIT,NULL);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu),item);
	g_signal_connect(G_OBJECT(item),"activate",G_CALLBACK(really_quit),NULL);
	gtk_widget_show_all(menu);

	g_signal_connect(G_OBJECT(tray),"popup_menu",G_CALLBACK(tray_on_menu),menu);
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

void init_with_conf(CONFDATA *data)
{
	FILE *fp;

	if((fp=open_config(data))==NULL)
		return;

	fclose(fp);
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
	GtkWidget *clean;
	GtkTextBuffer *buffer;
	GtkAccelGroup *accel_group;
	PangoFontDescription *font_name;
	DATA data;
	CONFDATA conf;
	struct sigaction act,old;

	setpgrp();

	init_with_conf(&conf);
	data.python_path=conf.python_path;
	data.proxy_py_path=conf.proxy_py_path;
	setlocale(LC_ALL,"");

	if(conf.language_env == NULL)
	{
		setlocale(LC_CTYPE,"zh_CN.UTF-8");
		setenv("LANG","zh_CN.UTF-8",1);
	}
	else
	{
		setlocale(LC_CTYPE,conf.language_env);
		setenv("LANG",conf.language_env,1);
	}
	
	if(conf.gtk_goagent_path!=NULL)
		chdir(conf.gtk_goagent_path);

	bindtextdomain("gtk_goagent","./locale/");
	textdomain("gtk_goagent");

	act.sa_flags=0;
	act.sa_handler=clean_data;
	sigaction(SIGUSR1,&act,&old);

	gtk_init(&argc,&argv);

	win=gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_position(GTK_WINDOW(win),GTK_WIN_POS_CENTER);
	gtk_window_set_title(GTK_WINDOW(win),"Gtk GoAgent");
	gtk_window_set_icon_from_file(GTK_WINDOW(win),"img/gtk_goagent.png",NULL);

	create_tray(win);
	//create_pre_ui(&pre,&conf);

	vbox=gtk_vbox_new(FALSE,0);
	accel_group=gtk_accel_group_new();
	gtk_window_add_accel_group(GTK_WINDOW(win),accel_group);
	gtk_container_add(GTK_CONTAINER(win),vbox);

	text=gtk_text_view_new();

	if(conf.font!=NULL)
	{
		font_name=pango_font_description_from_string(conf.font);
		gtk_widget_modify_font(text,font_name);
	}

	buffer=gtk_text_view_get_buffer(GTK_TEXT_VIEW(text));
	gtk_text_buffer_create_tag(buffer,"green_fg",
			"foreground","green",NULL);
	gtk_text_buffer_create_tag(buffer,"red_fg","foreground",
			"red",NULL);
	gtk_text_buffer_create_tag(buffer,"black_fg","foreground",
			"black",NULL);

	data.text=text;
	data.off=0;

	menu_bar=gtk_menu_bar_new();
	gtk_box_pack_start(GTK_BOX(vbox),menu_bar,FALSE,FALSE,0);
	menu=create_menu(menu_bar,_("_File"));
	create_menu_with_image(menu,GTK_STOCK_OPEN,accel_group,connect_goagent,&data);
	create_menu_with_image(menu,GTK_STOCK_CLOSE,accel_group,disconnect_goagent,&data);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu),gtk_separator_menu_item_new());
	create_menu_with_image(menu,GTK_STOCK_QUIT,accel_group,really_quit,&data);

	menu=create_menu(menu_bar,_("_Edit"));
	create_menu_with_image(menu,GTK_STOCK_PREFERENCES,accel_group,preferences,NULL);
	create_menu_with_image(menu,_("Up_load"),accel_group,upload,&conf);

	menu=create_menu(menu_bar,_("_Help"));
	create_menu_with_image(menu,GTK_STOCK_HELP,accel_group,help,NULL);
	create_menu_with_image(menu,GTK_STOCK_ABOUT,accel_group,about,NULL);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu),gtk_separator_menu_item_new());
	create_menu_with_image(menu,_("Upgrade GoAg_ent"),accel_group,upgrade_goagent,NULL);
	//create_menu_with_image(menu,_("Upgrade _Gtk GoAGent"),accel_group,upgrade_gtk_goagent,NULL);

	gtk_widget_set_size_request(text,0x300,0x180);
	gtk_text_view_set_editable(GTK_TEXT_VIEW(text),FALSE);
	gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(text),GTK_WRAP_CHAR);
	scrolled=gtk_scrolled_window_new(NULL,NULL);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled),GTK_POLICY_AUTOMATIC,GTK_POLICY_AUTOMATIC);
	gtk_container_add(GTK_CONTAINER(scrolled),text);
	gtk_box_pack_start(GTK_BOX(vbox),scrolled,FALSE,FALSE,0);

	gtk_box_pack_start(GTK_BOX(vbox),gtk_separator_menu_item_new(),FALSE,FALSE,5);
	show_time(vbox);
	gtk_box_pack_start(GTK_BOX(vbox),gtk_separator_menu_item_new(),FALSE,FALSE,5);

	hbox=gtk_hbox_new(FALSE,0);
	gtk_box_pack_start(GTK_BOX(vbox),hbox,FALSE,FALSE,5);
	
	open=gtk_button_new_with_label(_("Connect"));
	gtk_box_pack_start(GTK_BOX(hbox),open,FALSE,FALSE,30);
	g_signal_connect(G_OBJECT(open),"clicked",G_CALLBACK(connect_goagent),&data);
	clean=gtk_button_new_with_label(_("Clean"));
	gtk_box_pack_start(GTK_BOX(hbox),clean,TRUE,TRUE,100);
	g_signal_connect(G_OBJECT(clean),"clicked",G_CALLBACK(clean_buffer),&data);
	close=gtk_button_new_with_label(_("Disconnect"));
	gtk_box_pack_end(GTK_BOX(hbox),close,FALSE,FALSE,30);
	g_signal_connect(G_OBJECT(close),"clicked",G_CALLBACK(disconnect_goagent),&data);

	g_signal_connect(G_OBJECT(win),"delete_event",G_CALLBACK(really_quit),NULL);

	gtk_widget_show_all(win);

	gtk_main();

	//setpgrp();
	g_idle_remove_by_data(&data);

	kill(0,SIGKILL);
	//killpg(getpgrp(),SIGKILL);
	while(waitpid(-1,NULL,WNOHANG)!=-1);
	//g_idle_remove_by_data(&data);

	return 0;
}
