#include "ui.h"

void _save_config(CONFDATA *data);
void file_select(ENV_DATA *data);

void create_env_interface(char *title,GtkWidget *hbox,callback func,char *info,gboolean save)
{
	GtkWidget *entry;
	GtkWidget *label;
	GtkWidget *button;
	ENV_DATA env;

	label=gtk_label_new(title);
	env.title=title;
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

void select_python_path(GtkWidget *widget,gpointer data)
{
        file_select((ENV_DATA *)data);
}

void select_goagent_path(GtkWidget *widget,gpointer data)
{
	file_select((ENV_DATA *)data);
}

void select_gtk_goagent_path(GtkWidget *widget,gpointer data)
{
	file_select((ENV_DATA *)data);
}

void select_language(GtkWidget *widget,gpointer data)
{
        g_printf("%d:%s\n",gtk_combo_box_get_active(GTK_COMBO_BOX(widget)),
                        gtk_combo_box_get_active_text(GTK_COMBO_BOX(widget)));
}

void select_font(GtkWidget *widget,gpointer data)
{}

void select_auto_upgrade_goagent(GtkWidget *widget,gpointer data)
{}

void select_auto_upgrade_gtk_goagent(GtkWidget *widget,gpointer data)
{}

void exit_pre(GtkWidget *widget,CONFDATA *data)
{
        if(data->save)
                if(message_box_ok(_("No Saved Now\n,Do You Want To Save?")))
                        _save_config(data);

        gtk_main_quit();
}

void save_conf_with_exit(GtkWidget *widget,gpointer data)
{}

void _save_config(CONFDATA *data)
{
	FILE *fp;

	if((fp=fopen(get_conf_file_path(),"w"))==NULL)
	{
		message_box(NULL,strerror(errno));
		return;
	}

	save_config(&fp,data);
}


void file_select(ENV_DATA *data)
{
	GtkWidget *file;
	guint status;

	g_printf("%s\n",data->title);

	file=gtk_file_chooser_dialog_new(data->title,NULL,
			GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER,
			GTK_STOCK_CANCEL,GTK_RESPONSE_CANCEL,
			GTK_STOCK_OK,GTK_RESPONSE_OK,NULL);

	status=gtk_dialog_run(GTK_DIALOG(file));

	switch(status)
	{
		case GTK_RESPONSE_OK:
			data->env=gtk_file_chooser_get_filename(
					GTK_FILE_CHOOSER(file));
			gtk_entry_set_text(GTK_ENTRY(data->entry),
					data->env);
			data->save=FALSE;
			break;
		case GTK_RESPONSE_CANCEL:
		default:
			break;
	}

	gtk_widget_destroy(file);
}
