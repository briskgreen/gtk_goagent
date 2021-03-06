#include "dialog.h"
#include <errno.h>

//void set_help(GtkWidget *dialog,gchar *help);

/*void init_help_data(HELP *help)
{
	gchar *readme="Gtk GoAgent是一个简单的GoAgent前端!\n";
	extern gchar *set_string(gchar *source);
	help->name=set_string("Gtk GoAgent");
	help->email=set_string("briskgreen@163.com");
	help->readme=set_string(readme);
}*/

void about_dialog(GtkWidget *about,gpointer data)
{
	GtkWidget *dialog;
	ABOUT *about_data=(ABOUT *)data;
	GdkPixbuf *logo=gdk_pixbuf_new_from_file("img/64x64/gtk_goagent.png",NULL);

	dialog=gtk_about_dialog_new();

	gtk_window_set_icon_from_file(GTK_WINDOW(dialog),"img/64x64/gtk_goagent.png",NULL);
	gtk_about_dialog_set_logo(GTK_ABOUT_DIALOG(dialog),logo);
	gtk_about_dialog_set_program_name(GTK_ABOUT_DIALOG(dialog),about_data->name);
	gtk_about_dialog_set_version(GTK_ABOUT_DIALOG(dialog),about_data->version);
	gtk_about_dialog_set_copyright(GTK_ABOUT_DIALOG(dialog),about_data->copyright);
	gtk_about_dialog_set_comments(GTK_ABOUT_DIALOG(dialog),about_data->comments);
	gtk_about_dialog_set_license(GTK_ABOUT_DIALOG(dialog),about_data->license);
	gtk_about_dialog_set_website(GTK_ABOUT_DIALOG(dialog),about_data->website);
	//gtk_about_dialog_set_authors(GTK_ABOUT_DIALOG(dialog),about_data->authors);
	
	gtk_dialog_run(GTK_DIALOG(dialog));
	gtk_widget_destroy(dialog);
}

gboolean really_quit(GtkWidget *widget,gpointer data)
{
	GtkWidget *dialog;
	GtkWidget *label;
	gint status;

	dialog=gtk_dialog_new();
	gtk_window_set_title(GTK_WINDOW(dialog),_("Really Quit?"));
	gtk_window_set_icon_from_file(GTK_WINDOW(dialog),"img/64x64/quit.png",NULL);
	label=gtk_label_new(_("Are You Really Want to Quit Application Now?\nClick OK To Quit,Click NO To Continue!"));
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
	return TRUE;
}

void error_message(GtkWidget *widget,gchar *error)
{
	GtkWidget *dialog;

	dialog=gtk_message_dialog_new(NULL,GTK_DIALOG_MODAL,GTK_MESSAGE_ERROR,GTK_BUTTONS_OK,"%s\n",error);
	gtk_window_set_icon_from_file(GTK_WINDOW(dialog),"img/64x64/message.png",NULL);
	gtk_dialog_run(GTK_DIALOG(dialog));

	gtk_widget_destroy(dialog);
	gtk_main_quit();
}

void message_box(GtkWidget *widget,gchar *error)
{
	GtkWidget *dialog;

	dialog=gtk_message_dialog_new(NULL,GTK_DIALOG_MODAL,GTK_MESSAGE_INFO,
			GTK_BUTTONS_OK,"%s",error);
	gtk_window_set_icon_from_file(GTK_WINDOW(dialog),"img/64x64/message.png",NULL);
	gtk_dialog_run(GTK_DIALOG(dialog));

	gtk_widget_hide(dialog);
	gtk_widget_destroy(dialog);
}

gboolean message_box_ok(gchar *data)
{
	GtkWidget *dialog;
	GtkWidget *label;
	guint status;

	dialog=gtk_dialog_new();
	gtk_window_set_title(GTK_WINDOW(dialog),_("Message Box"));
	gtk_window_set_icon_from_file(GTK_WINDOW(dialog),"img/64x64/message.png",NULL);
	label=gtk_label_new(data);
	gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog)->vbox),label,FALSE,FALSE,5);
	gtk_widget_show(label);
	gtk_dialog_add_buttons(GTK_DIALOG(dialog),GTK_STOCK_NO,GTK_RESPONSE_NO,GTK_STOCK_OK,GTK_RESPONSE_OK,NULL);

	status=gtk_dialog_run(GTK_DIALOG(dialog));

	switch(status)
	{
		case GTK_RESPONSE_OK:
			gtk_widget_destroy(dialog);
			return TRUE;
			
		case GTK_RESPONSE_NO:
			gtk_widget_destroy(dialog);
			return FALSE;

		default:
			gtk_widget_destroy(dialog);
			return FALSE;
	}
}

void help_dialog(GtkWidget *help,gpointer data)
{
	GtkWidget *dialog;
	GtkWidget *text;
	GtkWidget *scrolled;
	char *buf;
	FILE *fp;
	unsigned long len;
	//HELP *help_data=(HELP *)data;
	//
	if((fp=fopen("readme","rb"))==NULL)
		error_message(NULL,strerror(errno));
	fseek(fp,0L,SEEK_END);
	len=ftell(fp);
	rewind(fp);

	buf=malloc(len+1);
	fread(buf,len,1,fp);
	fclose(fp);

	dialog=gtk_dialog_new();
	gtk_window_set_title(GTK_WINDOW(dialog),_("Help"));
	gtk_window_set_icon_from_file(GTK_WINDOW(dialog),"img/64x64/help.png",NULL);
	/*set_help(dialog,help_data->name);
	set_help(dialog,help_data->email);
	set_help(dialog,help_data->readme);*/
	text=gtk_text_view_new();
	//gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(text),GTK_WRAP_CHAR);
	gtk_text_view_set_editable(GTK_TEXT_VIEW(text),FALSE);
	gtk_widget_set_size_request(text,0x300,0x200);

	scrolled=gtk_scrolled_window_new(NULL,NULL);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled),
			GTK_POLICY_AUTOMATIC,GTK_POLICY_AUTOMATIC);
	gtk_container_add(GTK_CONTAINER(scrolled),text);
	gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog)->vbox),scrolled,
			FALSE,FALSE,0);
	gtk_text_buffer_set_text(gtk_text_view_get_buffer(GTK_TEXT_VIEW(text)),buf,-1);
	gtk_widget_show_all(scrolled);

	gtk_dialog_add_buttons(GTK_DIALOG(dialog),GTK_STOCK_OK,GTK_RESPONSE_OK,NULL);
	gtk_dialog_run(GTK_DIALOG(dialog));

	gtk_widget_destroy(dialog);

	free(buf);
}

/*void set_help(GtkWidget *dialog,gchar *help)
{
	GtkWidget *label;

	label=gtk_label_new(help);
	gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog)->vbox),label,FALSE,FALSE,0);

	gtk_widget_show(label);
}*/
