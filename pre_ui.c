#include "callback.h"

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

	gtk_init(&argc,&argv);

	dialog=gtk_dialog_new();
	gtk_window_set_title(GTK_WINDOW(dialog),_("Preferences"));

	notebook=gtk_notebook_new();
	gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog)->vbox),notebook,
			FALSE,FALSE,0);
	gtk_widget_show(notebook);
}
