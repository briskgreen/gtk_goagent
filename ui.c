#include "ui.h"

void create_env_interface(char *title,GtkWidget *hbox,callback func,char *info,gboolean save)
{
	GtkWidget *entry;
	GtkWidget *label;
	GtkWidget *button;
	ENV_DATA env;

	label=gtk_label_new(title);
	gtk_box_pack_start(GTK_BOX(hbox),label,FALSE,FALSE,5);
	entry=gtk_entry_new();
	gtk_entry_set_editable(GTK_ENTRY(entry),FALSE);
	gtk_box_pack_start(GTK_BOX(hbox),entry,TRUE,TRUE,10);
	button=gtk_button_new_with_label(". . .");
	gtk_box_pack_start(GTK_BOX(hbox),button,FALSE,FALSE,0);
	gtk_entry_set_text(GTK_ENTRY(entry),info);
	env.entry=entry;
	env.env=info;
	env.save=save;
	g_signal_connect(G_OBJECT(button),"clicked",G_CALLBACK(func),&env);
}
