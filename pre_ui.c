#include "ui.h"

void init_with_conf(CONFDATA *conf)
{
	FILE *fp;

	if((fp=open_config(conf))==NULL)
		return;

	fclose(fp);
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

	/*label=gtk_label_new(_("Python Path:"));
	gtk_box_pack_start(GTK_BOX(hbox),label,FALSE,FALSE,5);
	entry=gtk_entry_new();
	gtk_box_pack_start(GTK_BOX(hbox),entry,TRUE,TRUE,5);
	button=gtk_button_new_with_label(". . .");
	gtk_box_pack_start(GTK_BOX(hbox),button,FALSE,FALSE,0);
	gtk_entry_set_text(GTK_ENTRY(entry),conf.python_path);
	env.entry=entry;
	env.env=conf.python_path;
	g_signal_connect(G_OBJECT(button),"clicked",G_CALLBACK(select_python_path),&env);*/
	create_env_interface(_("Python Path     : "),hbox,select_python_path,conf.python_path,conf.save);

	hbox=gtk_hbox_new(FALSE,10);
	gtk_box_pack_start(GTK_BOX(page),hbox,FALSE,FALSE,10);
	create_env_interface(_("GoAgent Path    :"),hbox,select_goagent_path,conf.goagent_path,conf.save);

	hbox=gtk_hbox_new(FALSE,10);
	gtk_box_pack_start(GTK_BOX(page),hbox,FALSE,FALSE,10);
	create_env_interface(_("Gtk GoAgent Path:"),hbox,select_gtk_goagent_path,conf.gtk_goagent_path,conf.save);

	gtk_widget_show_all(dialog);

	gtk_main();

	return 0;
}
