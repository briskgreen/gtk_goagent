#include "ui.h"

void init_with_conf(CONFDATA *conf)
{
	FILE *fp;

	if((fp=open_config(conf))==NULL)
		return;

	fclose(fp);
}

void create_combo_box(GtkWidget *page,CONFDATA *conf)
{
	GtkWidget *combo_box;
	GtkWidget *hbox;
	GtkWidget *label;
	char *language[]={_("Chinese"),_("English")};
	int index;

	if(strstr(conf->language_env,"zh"))
		index=0;
	else if(strstr(conf->language_env,"en"))
		index=1;

	combo_box=gtk_combo_box_new_text();
	gtk_combo_box_append_text(GTK_COMBO_BOX(combo_box),language[0]);
	gtk_combo_box_append_text(GTK_COMBO_BOX(combo_box),language[1]);
	gtk_combo_box_set_active(GTK_COMBO_BOX(combo_box),index);

	hbox=gtk_hbox_new(FALSE,10);
	gtk_box_pack_start(GTK_BOX(page),hbox,TRUE,TRUE,10);
	label=gtk_label_new(_("Language:"));
	gtk_box_pack_end(GTK_BOX(hbox),combo_box,FALSE,FALSE,10);
	gtk_box_pack_end(GTK_BOX(hbox),label,FALSE,FALSE,10);
	g_signal_connect(G_OBJECT(combo_box),"changed",G_CALLBACK(select_language),conf);
}

void change_save(GtkWidget *widget,gpointer data)
{
	gboolean save=*(gboolean *)data;

	if(save)
		save=FALSE;
}

GtkWidget *read_and_edit_proxy_ini(GtkWidget *page,char *path,gboolean save)
{
	GtkWidget *text;
	GtkWidget *scrolled;
	FILE *fp;
	char *buf;
	unsigned long len;
	char *p=getcwd(NULL,0);

	chdir(path);

	if((fp=fopen("local/proxy.ini","r"))==NULL)
	{
		message_box(NULL,strerror(errno));
		chdir(p);
		return NULL;
	}

	chdir(p);

	text=gtk_text_view_new();
	gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(text),GTK_WRAP_CHAR);
	scrolled=gtk_scrolled_window_new(NULL,NULL);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled),
			GTK_POLICY_AUTOMATIC,GTK_POLICY_AUTOMATIC);
	gtk_container_add(GTK_CONTAINER(scrolled),text);

	gtk_container_add(GTK_CONTAINER(page),scrolled);
	g_signal_connect(G_OBJECT(gtk_text_view_get_buffer(GTK_TEXT_VIEW(text))),"changed",G_CALLBACK(change_save),&save);

	fseek(fp,0L,SEEK_END);
	len=ftell(fp);
	rewind(fp);

	buf=malloc(len);
	fread(buf,len,1,fp);
	gtk_text_buffer_set_text(gtk_text_view_get_buffer(GTK_TEXT_VIEW(text)),buf,-1);
	free(buf);
	
	return text;
}

int main(int argc,char **argv)
{
	GtkWidget *dialog;
	GtkWidget *notebook;
	GtkWidget *page;
	GtkWidget *hbox;
	GtkWidget *vbox;
	GtkWidget *font;
	GtkWidget *save;
	GtkWidget *cancel;
	GtkWidget *open;
	GtkWidget *label;
	GtkWidget *entry;
	GtkWidget *button;
	GtkWidget *text;
	CONFDATA conf;
	ENV_DATA env;

	init_with_conf(&conf);

	bindtextdomain("pre_ui","locale");
	textdomain("pre_ui");

	gtk_init(&argc,&argv);

	dialog=gtk_dialog_new();
	gtk_window_set_title(GTK_WINDOW(dialog),_("Preferences"));
	g_signal_connect(G_OBJECT(dialog),"delete_event",G_CALLBACK(exit_pre),&conf);

	notebook=gtk_notebook_new();
	gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog)->vbox),notebook,
			FALSE,FALSE,0);

	page=gtk_vbox_new(FALSE,0);
	label=gtk_label_new(_("Environment"));
	gtk_notebook_append_page(GTK_NOTEBOOK(notebook),page,label);

	hbox=gtk_hbox_new(FALSE,0);
	gtk_box_pack_start(GTK_BOX(page),hbox,FALSE,FALSE,0);

	create_env_interface(_("Python Path     : "),hbox,select_python_path,conf.python_path,conf.save);

	hbox=gtk_hbox_new(FALSE,10);
	gtk_box_pack_start(GTK_BOX(page),hbox,FALSE,FALSE,10);
	create_env_interface(_("GoAgent Path    :"),hbox,select_goagent_path,conf.goagent_path,conf.save);

	hbox=gtk_hbox_new(FALSE,10);
	gtk_box_pack_start(GTK_BOX(page),hbox,FALSE,FALSE,10);
	create_env_interface(_("Gtk GoAgent Path:"),hbox,select_gtk_goagent_path,conf.gtk_goagent_path,conf.save);

	gtk_box_pack_start(GTK_BOX(page),gtk_hseparator_new(),FALSE,FALSE,10);

	create_combo_box(page,&conf);

	font=gtk_button_new_with_label(conf.font);
	gtk_box_pack_start(GTK_BOX(page),font,TRUE,TRUE,10);
	g_signal_connect(G_OBJECT(font),"clicked",G_CALLBACK(select_font),&conf);

	gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog)->vbox),gtk_hseparator_new(),FALSE,FALSE,10);
	label=gtk_label_new(_("All Changes Will Take Effect After Restart"));
	gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog)->vbox),label,FALSE,FALSE,10);

	page=gtk_vbox_new(FALSE,0);
	label=gtk_label_new("proxy.ini");
	gtk_notebook_append_page(GTK_NOTEBOOK(notebook),page,label);
	text=read_and_edit_proxy_ini(page,conf.goagent_path,conf.save);

	page=gtk_vbox_new(FALSE,0);
	label=gtk_label_new(_("Upgrade"));
	gtk_notebook_append_page(GTK_NOTEBOOK(notebook),page,label);

	button=gtk_check_button_new_with_label(_("Auto Upgrade GoAgent"));
	if(strcmp(conf.goagent_auto_upgrade,"true")==0)
		gtk_button_clicked(GTK_BUTTON(button));
	gtk_box_pack_start(GTK_BOX(page),button,TRUE,TRUE,20);
	g_signal_connect(G_OBJECT(button),"clicked",G_CALLBACK(select_auto_upgrade_goagent),&conf);

	button=gtk_check_button_new_with_label(_("Auto Upgrade Gtk GoaGent"));
	if(strcmp(conf.gtk_goagent_auto_upgrade,"true")==0)
		gtk_button_clicked(GTK_BUTTON(button));
	gtk_box_pack_start(GTK_BOX(page),button,TRUE,TRUE,20);
	g_signal_connect(G_OBJECT(button),"clicked",G_CALLBACK(select_auto_upgrade_gtk_goagent),&conf);

	hbox=gtk_hbox_new(FALSE,0);
	gtk_box_pack_end(GTK_BOX(GTK_DIALOG(dialog)->vbox),hbox,FALSE,FALSE,0);

	save=gtk_button_new_with_label(_("Save"));
	gtk_box_pack_end(GTK_BOX(hbox),save,FALSE,FALSE,10);
	g_signal_connect(G_OBJECT(save),"clicked",G_CALLBACK(save_conf_with_exit),&conf);

	cancel=gtk_button_new_with_label(_("Cancel"));
	gtk_box_pack_end(GTK_BOX(hbox),cancel,FALSE,FALSE,10);
	g_signal_connect(G_OBJECT(cancel),"clicked",G_CALLBACK(exit_pre),&conf);

	gtk_widget_show_all(dialog);

	gtk_main();

	return 0;
}
