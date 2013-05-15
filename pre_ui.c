#include "ui.h"
#include <unistd.h>

typedef struct
{
	GtkTextBuffer *buffer;
	char *goagent_path;
}PROXY_INI;

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

	hbox=gtk_hbox_new(FALSE,20);
	gtk_box_pack_start(GTK_BOX(page),hbox,TRUE,TRUE,20);
	label=gtk_label_new(_("Language:"));
	gtk_box_pack_end(GTK_BOX(hbox),combo_box,FALSE,FALSE,20);
	gtk_box_pack_end(GTK_BOX(hbox),label,FALSE,FALSE,20);
	g_signal_connect(G_OBJECT(combo_box),"changed",G_CALLBACK(select_language),conf);
}

char *get_proxy_ini_path(char *goagent_path)
{
	char *proxy_ini="/local/proxy.ini";
	char *proxy_ini_path;
	int goagent_path_len=strlen(goagent_path);
	int proxy_ini_len=strlen(proxy_ini);
	int len=goagent_path_len+proxy_ini_len+1;

	proxy_ini_path=malloc(len);
	bzero(proxy_ini_path,len);

	strncpy(proxy_ini_path,goagent_path,goagent_path_len);
	strncat(proxy_ini_path,proxy_ini,proxy_ini_len);

	return proxy_ini_path;
}

void change_save(GtkWidget *widget,gpointer data)
{
	GtkTextIter start;
	GtkTextIter end;
	PROXY_INI *ini=(PROXY_INI *)data;
	FILE *fp;
	char *buf;

	if((fp=fopen(get_proxy_ini_path(ini->goagent_path),"w"))==NULL)
	{
		message_box(NULL,strerror(errno));
		return;
	}

	gtk_text_buffer_get_start_iter(ini->buffer,&start);
	gtk_text_buffer_get_end_iter(ini->buffer,&end);

	buf=gtk_text_buffer_get_text(ini->buffer,&start,&end,FALSE);

	fwrite(buf,strlen(buf),1,fp);

	fclose(fp);
	g_free(buf);
}

GtkWidget *read_and_edit_proxy_ini(GtkWidget *page,char *path)
{
	GtkWidget *text;
	GtkWidget *scrolled;
	GtkWidget *save;
	FILE *fp;
	char *buf;
	PROXY_INI *ini=malloc(sizeof(PROXY_INI));
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

	gtk_box_pack_start(GTK_BOX(page),scrolled,TRUE,TRUE,0);
	save=gtk_button_new_with_label(_("Save"));
	gtk_box_pack_start(GTK_BOX(page),save,FALSE,FALSE,0);

	ini->buffer=gtk_text_view_get_buffer(GTK_TEXT_VIEW(text));
	ini->goagent_path=path;
	g_signal_connect(G_OBJECT(save),"clicked",G_CALLBACK(change_save),
			ini);

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
	GtkWidget *button;
	CONFDATA conf;

	init_with_conf(&conf);
	conf.save=TRUE;

	bindtextdomain("pre_ui","locale");
	textdomain("pre_ui");

	gtk_init(&argc,&argv);

	dialog=gtk_dialog_new();
	gtk_window_set_title(GTK_WINDOW(dialog),_("Preferences"));
	g_signal_connect(G_OBJECT(dialog),"destroy",G_CALLBACK(exit_pre),&conf);

	notebook=gtk_notebook_new();
	gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog)->vbox),notebook,
			FALSE,FALSE,0);

	page=gtk_vbox_new(FALSE,0);
	label=gtk_label_new(_("Environment"));
	gtk_notebook_append_page(GTK_NOTEBOOK(notebook),page,label);

	hbox=gtk_hbox_new(FALSE,0);
	gtk_box_pack_start(GTK_BOX(page),hbox,FALSE,FALSE,0);

	create_env_interface(_("Python Path     : "),hbox,select_python_path,&conf.python_path,&conf.save);

	hbox=gtk_hbox_new(FALSE,20);
	gtk_box_pack_start(GTK_BOX(page),hbox,FALSE,FALSE,20);
	create_env_interface(_("GoAgent Path    :"),hbox,select_goagent_path,&conf.goagent_path,&conf.save);

	hbox=gtk_hbox_new(FALSE,20);
	gtk_box_pack_start(GTK_BOX(page),hbox,FALSE,FALSE,20);
	create_env_interface(_("Gtk GoAgent Path:"),hbox,select_gtk_goagent_path,&conf.gtk_goagent_path,&conf.save);

	gtk_box_pack_start(GTK_BOX(page),gtk_hseparator_new(),FALSE,FALSE,20);

	create_combo_box(page,&conf);

	font=gtk_button_new_with_label(conf.font);
	gtk_box_pack_start(GTK_BOX(page),font,TRUE,TRUE,20);
	g_signal_connect(G_OBJECT(font),"clicked",G_CALLBACK(select_font),&conf);

	gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog)->vbox),gtk_hseparator_new(),FALSE,FALSE,5);
	label=gtk_label_new(_("All Changes Will Take Effect After Restart"));
	gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog)->vbox),label,FALSE,FALSE,5);

	page=gtk_vbox_new(FALSE,0);
	label=gtk_label_new("proxy.ini");
	gtk_notebook_append_page(GTK_NOTEBOOK(notebook),page,label);
	read_and_edit_proxy_ini(page,conf.goagent_path);

	page=gtk_vbox_new(FALSE,0);
	label=gtk_label_new(_("Upgrade"));
	gtk_notebook_append_page(GTK_NOTEBOOK(notebook),page,label);

	button=gtk_check_button_new_with_label(_("Auto Upgrade GoAgent"));
	if(strcmp(conf.goagent_auto_upgrade,"true")==0)
		gtk_button_clicked(GTK_BUTTON(button));
	gtk_box_pack_start(GTK_BOX(page),button,TRUE,TRUE,20);
	g_signal_connect(G_OBJECT(button),"clicked",G_CALLBACK(select_auto_upgrade_goagent),&conf);

	/*button=gtk_check_button_new_with_label(_("Auto Upgrade Gtk GoaGent"));
	if(strcmp(conf.gtk_goagent_auto_upgrade,"true")==0)
		gtk_button_clicked(GTK_BUTTON(button));
	gtk_box_pack_start(GTK_BOX(page),button,TRUE,TRUE,20);
	g_signal_connect(G_OBJECT(button),"clicked",G_CALLBACK(select_auto_upgrade_gtk_goagent),&conf);*/

	hbox=gtk_hbox_new(FALSE,0);
	gtk_box_pack_end(GTK_BOX(GTK_DIALOG(dialog)->vbox),hbox,FALSE,FALSE,0);

	save=gtk_button_new_with_label(_("Save"));
	gtk_box_pack_end(GTK_BOX(hbox),save,FALSE,FALSE,5);
	g_signal_connect(G_OBJECT(save),"clicked",G_CALLBACK(save_conf_now),&conf);

	cancel=gtk_button_new_with_label(_("Cancel"));
	gtk_box_pack_end(GTK_BOX(hbox),cancel,FALSE,FALSE,5);
	g_signal_connect(G_OBJECT(cancel),"clicked",G_CALLBACK(exit_pre),&conf);

	gtk_widget_show_all(dialog);

	gtk_main();

	return 0;
}
