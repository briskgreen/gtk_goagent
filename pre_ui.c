#include "callback.h"

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
	GtkWidget *save;
	GtkWidget *cancel;
	GtkWidget *open;
	GtkWidget *label;
	GtkWidget *entry;
	CONFDATA conf;

	init_with_conf(&conf);

	bindtextdomain("pre_ui","locale");
	textdomain("pre_ui");

	gtk_init(&argc,&argv);

	dialog=gtk_dialog_new();
	gtk_window_set_title(GTK_WINDOW(dialog),_("Preferences"));
	g_signal_connect(G_OBJECT(dialog),"delete_event",G_CALLBACK(gtk_main_quit),NULL);

	gtk_widget_show(dialog);

	notebook=gtk_notebook_new();
	gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog)->vbox),notebook,
			FALSE,FALSE,0);
	gtk_widget_show(notebook);

	gtk_main();

	return 0;
}
