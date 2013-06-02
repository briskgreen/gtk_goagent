#include "ui.h"

void _save_config(CONFDATA *data);
/*文件选择对话框*/
void file_select(ENV_DATA *data,gboolean folder);

void create_env_interface(char *title,GtkWidget *hbox,callback func,
		char **info,gboolean *save)
{
	GtkWidget *entry;
	GtkWidget *label;
	GtkWidget *button;
	ENV_DATA *env=malloc(sizeof(ENV_DATA));

	label=gtk_label_new(title);
	env->title=title;
	gtk_box_pack_start(GTK_BOX(hbox),label,FALSE,FALSE,5);
	entry=gtk_entry_new();
	gtk_entry_set_editable(GTK_ENTRY(entry),FALSE);
	gtk_box_pack_start(GTK_BOX(hbox),entry,TRUE,TRUE,10);
	button=gtk_button_new_with_label(". . .");
	gtk_box_pack_start(GTK_BOX(hbox),button,FALSE,FALSE,0);
	gtk_entry_set_text(GTK_ENTRY(entry),*info);
	env->entry=entry;
	env->env=info;
	env->save=save;
	g_signal_connect(G_OBJECT(button),"clicked",G_CALLBACK(func),env);
}

void select_python_path(GtkWidget *widget,gpointer data)
{
        file_select((ENV_DATA *)data,FALSE);
}

void select_goagent_path(GtkWidget *widget,gpointer data)
{
	file_select((ENV_DATA *)data,TRUE);
}

void select_gtk_goagent_path(GtkWidget *widget,gpointer data)
{
	file_select((ENV_DATA *)data,TRUE);
}

void select_language(GtkWidget *widget,gpointer data)
{
	CONFDATA *conf=(CONFDATA *)data;

	gtk_combo_box_get_active(GTK_COMBO_BOX(widget)) == 0 ? set_language_env(conf,"zh_CN.UTF-8") : set_language_env(conf,"en_US.UTF-8");

	conf->save=FALSE;
}

void select_font(GtkWidget *widget,gpointer data)
{
	GtkWidget *font;
	guint status;

	font=gtk_font_selection_dialog_new(_("Select Font"));
	status=gtk_dialog_run(GTK_DIALOG(font));

	switch(status)
	{
		case GTK_RESPONSE_OK:
			set_font_by_name((CONFDATA *)data,
				gtk_font_selection_dialog_get_font_name(
					GTK_FONT_SELECTION_DIALOG(font)));
			gtk_button_set_label(GTK_BUTTON(widget),
				gtk_font_selection_dialog_get_font_name(
					GTK_FONT_SELECTION_DIALOG(font)));
			((CONFDATA *)data)->save=FALSE;
					break;
		case GTK_RESPONSE_CANCEL:
		default:
			break;
	}
	
	gtk_widget_destroy(font);
}

void select_auto_upgrade_goagent(GtkWidget *widget,gpointer data)
{
	CONFDATA *conf=(CONFDATA *)data;

	strcmp(conf->goagent_auto_upgrade,"true") == 0 ? set_goagent_auto_upgrade(conf,"false") : set_goagent_auto_upgrade(conf,"true");
	
	conf->save=FALSE;
}

/*void select_auto_upgrade_gtk_goagent(GtkWidget *widget,gpointer data)
{
	CONFDATA *conf=(CONFDATA *)data;

	strcmp(conf->gtk_goagent_auto_upgrade,"true") ==0 ? set_gtk_goagent_auto_upgrade(conf,"false") : set_gtk_goagent_auto_upgrade(conf,"true");

	conf->save=FALSE;
}*/

void exit_pre(GtkWidget *widget,CONFDATA *data)
{
        if(data->save == FALSE)
                if(message_box_ok(_("No Saved Now\n,Do You Want To Save?")))
                        _save_config(data);

        gtk_main_quit();
}

void save_conf_now(GtkWidget *widget,gpointer data)
{
	CONFDATA *conf=(CONFDATA *)data;

	if(conf->save == TRUE)
	{
		message_box(NULL,_("Saved Now!"));
		return;
	}

	_save_config(conf);
}

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


void file_select(ENV_DATA *data,gboolean folder)
{
	GtkWidget *file;
	guint status;
	GtkFileChooserAction action;

	if(folder)
		action=GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER;
	else
		action=GTK_FILE_CHOOSER_ACTION_OPEN;

	file=gtk_file_chooser_dialog_new(data->title,NULL,
			action,
			GTK_STOCK_CANCEL,GTK_RESPONSE_CANCEL,
			GTK_STOCK_OK,GTK_RESPONSE_OK,NULL);

	status=gtk_dialog_run(GTK_DIALOG(file));

	switch(status)
	{
		case GTK_RESPONSE_OK:
			*data->env=gtk_file_chooser_get_filename(
					GTK_FILE_CHOOSER(file));
			gtk_entry_set_text(GTK_ENTRY(data->entry),
					*data->env);
			*data->save=FALSE;
			break;
		case GTK_RESPONSE_CANCEL:
		default:
			break;
	}

	gtk_widget_destroy(file);
}
