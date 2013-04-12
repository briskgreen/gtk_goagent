#include "dialog.h"

void set_help(GtkWidget *dialog,gchar *help);

void init_help_data(HELP *help)
{
	gchar *readme="一个简单的闹钟程序\n使用方法:\n点击菜单的New或者按钮New进行新的闹钟设置\n点击菜单的Delete或者下面的Delete按钮可删除一个闹钟\n如果日历中有高亮显示的(非当日)表示该日已经设置了闹钟\n可以双击进行查看或者删除,如果没有高亮显示的双击可直接进行闹钟设置!\n";
	extern gchar *set_string(gchar *source);
	help->name=set_string("SimpleAlarm");
	help->email=set_string("briskgreen@163.com");
	help->readme=set_string(readme);
}

void about_dialog(GtkWidget *about,gpointer data)
{
	GtkWidget *dialog;
	ABOUT *about_data=(ABOUT *)data;

	dialog=gtk_about_dialog_new();
	gtk_about_dialog_set_program_name(GTK_ABOUT_DIALOG(dialog),about_data->name);
	gtk_about_dialog_set_version(GTK_ABOUT_DIALOG(dialog),about_data->version);
	gtk_about_dialog_set_copyright(GTK_ABOUT_DIALOG(dialog),about_data->copyright);
	gtk_about_dialog_set_comments(GTK_ABOUT_DIALOG(dialog),about_data->comments);
	gtk_about_dialog_set_license(GTK_ABOUT_DIALOG(dialog),about_data->license);
	gtk_about_dialog_set_website(GTK_ABOUT_DIALOG(dialog),about_data->website);
	//gtk_about_dialog_set_authors(GTK_ABOUT_DIALOG(dialog),authors);
	
	gtk_dialog_run(GTK_DIALOG(dialog));
	gtk_widget_destroy(dialog);
}

void really_quit(GtkWidget *widget,gpointer data)
{
	GtkWidget *dialog;
	GtkWidget *label;
	gint status;

	dialog=gtk_dialog_new();
	gtk_window_set_title(GTK_WINDOW(dialog),"Really Quit?");
	label=gtk_label_new("Are You Really Want to Quit Application Now?\nClick OK To Quit,Click NO To Continue!");
	gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog)->vbox),label,FALSE,FALSE,5);
	gtk_widget_show(label);
	gtk_dialog_add_buttons(GTK_DIALOG(dialog),GTK_STOCK_NO,GTK_RESPONSE_NO,GTK_STOCK_OK,GTK_RESPONSE_OK,NULL);
	status=gtk_dialog_run(GTK_DIALOG(dialog));

	switch(status)
	{
		case GTK_RESPONSE_NO:
			break;
		case GTK_RESPONSE_OK:
			gtk_main_quit();
		default:
			break;
	}

	gtk_widget_destroy(dialog);
}

void error_message(GtkWidget *widget,gchar *error)
{
	GtkWidget *dialog;

	dialog=gtk_message_dialog_new(NULL,GTK_DIALOG_MODAL,GTK_MESSAGE_ERROR,GTK_BUTTONS_OK,"%s\n",error);
	gtk_dialog_run(GTK_DIALOG(dialog));

	gtk_widget_destroy(dialog);
	gtk_main_quit();
}

void help_dialog(GtkWidget *help,gpointer data)
{
	GtkWidget *dialog;
	HELP *help_data=(HELP *)data;

	dialog=gtk_dialog_new();
	gtk_window_set_title(GTK_WINDOW(dialog),"Help");
	set_help(dialog,help_data->name);
	set_help(dialog,help_data->email);
	set_help(dialog,help_data->readme);
	gtk_dialog_add_buttons(GTK_DIALOG(dialog),GTK_STOCK_OK,GTK_RESPONSE_OK,NULL);
	gtk_dialog_run(GTK_DIALOG(dialog));

	gtk_widget_destroy(dialog);
}

void set_help(GtkWidget *dialog,gchar *help)
{
	GtkWidget *label;

	label=gtk_label_new(help);
	gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog)->vbox),label,FALSE,FALSE,0);

	gtk_widget_show(label);
}
