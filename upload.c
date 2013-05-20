#include "callback.h"

void init_with_conf(CONFDATA *conf)
{
	FILE *fp;

	if((fp=open_config(conf))==NULL)
		return;

	fclose(fp);
}

void clean_upload_data(GtkWidget *widget,UP_DATA *data)
{
	if(data->off==0)
	{
		message_box(NULL,_("No Upload Now!"));
		return;
	}

	kill(data->pid,SIGKILL);
	while(waitpid(-1,NULL,WNOHANG));
	g_idle_remove_by_data(data);
	data->off=0;
}

int main(int argc,char **argv)
{
	GtkWidget *dialog;
	GtkWidget *text;
	GtkWidget *upload;
	GtkWidget *cancel;
	GtkWidget *scrolled;
	GtkWidget *hbox;
	GtkTextBuffer *buffer;
	PangoFontDescription *font_name;
	CONFDATA conf;
	UP_DATA data;

	init_with_conf(&conf);

	data.off=0;
	data.python_path=conf.python_path;
	data.goagent_path=conf.goagent_path;
	data.pid=0;

	bindtextdomain("upload","locale");
	textdomain("upload");

	gtk_init(&argc,&argv);

	dialog=gtk_dialog_new();
	gtk_window_set_icon_from_file(GTK_WINDOW(dialog),"img/upload.png",NULL);
	g_signal_connect(G_OBJECT(dialog),"delete_event",G_CALLBACK(gtk_main_quit),NULL);
	text=gtk_text_view_new();
	gtk_text_view_set_editable(GTK_TEXT_VIEW(text),FALSE);
	gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(text),GTK_WRAP_CHAR);
	data.text=text;
	if(conf.font != NULL)
	{
		font_name=pango_font_description_from_string(conf.font);
		gtk_widget_modify_font(text,font_name);
	}

	gtk_widget_set_size_request(text,0x200,0x100);
	gtk_text_view_set_editable(GTK_TEXT_VIEW(text),FALSE);
	scrolled=gtk_scrolled_window_new(NULL,NULL);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled),
			GTK_POLICY_AUTOMATIC,GTK_POLICY_AUTOMATIC);
	gtk_container_add(GTK_CONTAINER(scrolled),text);
	gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog)->vbox),scrolled,TRUE,TRUE,0);

	gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog)->vbox),gtk_hseparator_new(),TRUE,TRUE,10);

	hbox=gtk_hbox_new(FALSE,0);

	gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog)->vbox),hbox,TRUE,TRUE,10);

	buffer=gtk_text_view_get_buffer(GTK_TEXT_VIEW(text));
	gtk_text_buffer_create_tag(buffer,"green_fg",
			"foreground","green",NULL);
	gtk_text_buffer_create_tag(buffer,"red_fg","foreground",
			"red",NULL);
	gtk_text_buffer_create_tag(buffer,"black_fg","foreground",
			"black",NULL);

	upload=gtk_button_new_with_label(_("Upload"));
	gtk_box_pack_end(GTK_BOX(hbox),upload,FALSE,FALSE,10);
	g_signal_connect(G_OBJECT(upload),"clicked",G_CALLBACK(upload_goagent),&data);

	cancel=gtk_button_new_with_label(_("Cancel"));
	gtk_box_pack_end(GTK_BOX(hbox),cancel,FALSE,FALSE,10);
	g_signal_connect(G_OBJECT(cancel),"clicked",G_CALLBACK(clean_upload_data),&data);

	gtk_widget_show_all(dialog);

	gtk_main();

	g_idle_remove_by_data(&data);
	if(data.pid)
		kill(data.pid,SIGKILL);
	//while(waitpid(-1,NULL,WNOHANG)!=-1);

	return 0;
}
