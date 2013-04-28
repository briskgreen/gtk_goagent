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

int main(int argc,char **argv)
{
	GtkWidget *dialog;
	GtkWidget *notebook;
	GtkWidget *page;
	GtkWidget *hbox;
	GtkWidget *vbox;
	GtkWidget *button;
	GtkWidget *save;
	GtkWidget *cancel;
	GtkWidget *open;
	GtkWidget *label;
	GtkWidget *entry;
	CONFDATA conf;
	ENV_DATA env;

	init_with_conf(&conf);

	bindtextdomain("pre_ui","locale");
	textdomain("pre_ui");

	gtk_init(&argc,&argv);

	dialog=gtk_dialog_new();
	gtk_window_set_title(GTK_WINDOW(dialog),_("Preferences"));
	g_signal_connect(G_OBJECT(dialog),"delete_event",G_CALLBACK(gtk_main_quit),NULL);

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

	create_combo_box(page,&conf);

	gtk_widget_show_all(dialog);

	gtk_main();

	return 0;
}
