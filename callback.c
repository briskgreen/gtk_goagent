#include "callback.h"
#include <locale.h>
#include <stdlib.h>
#include <limits.h>
#include <fcntl.h>

void get_connect(DATA *data);
gboolean is_python_and_goagent_path(char *python_path,char *goagent_path);
gboolean _get_connect(DATA *data);

DATA *sig_data;

void connect_goagent(GtkWidget *widget,DATA *data)
{
	if(data->off)
	{
		message_box(widget,_("Connected Now\n"));
		return;
	}

	if(!is_python_and_goagent_path(data->python_path,data->proxy_py_path))
		return;

	get_connect(data);
	data->off=1;
}

void disconnect_goagent(GtkWidget *widget,DATA *data)
{
	GtkTextBuffer *buffer=gtk_text_view_get_buffer(GTK_TEXT_VIEW(data->text));

	if(!data->off)
	{
		message_box(widget,_("No Connected Now!"));
		return;
	}

	data->off=0;
	kill(data->pid,SIGKILL);
	while(waitpid(-1,NULL,WNOHANG)!=-1);
	close(data->pipefd[0]);
	g_idle_remove_by_data(data);

	/*while(gtk_events_pending())
		gtk_main_iteration();*/

	gtk_text_buffer_set_text(buffer,"",0);
}

void clean_buffer(GtkWidget *widget,DATA *data)
{
	/*while(gtk_events_pending())
		gtk_main_iteration();*/

	gtk_text_buffer_set_text(gtk_text_view_get_buffer(
				GTK_TEXT_VIEW(data->text)),"",0);
}

void help(GtkWidget *widget,gpointer data)
{}

void about(GtkWidget *widget,gpointer data)
{}

void update(GtkWidget *widget,gpointer data)
{}

void preferences(GtkWidget *widget,gpointer data)
{
	/*setlocale(LC_ALL,"");
	setlocale(LC_CTYPE,"zh_CN.UTF-8");
	setenv("LANG","zh_CN.UTF-8",1);*/
	if(fork()==0)
		execl("pre_ui","./pre_ui",NULL);

	wait(NULL);
}

void tray_on_menu(GtkWidget *widget,guint button,
		guint32 activate_time,gpointer data)
{
	gtk_menu_popup(GTK_MENU(data),NULL,NULL,gtk_status_icon_position_menu,GTK_STATUS_ICON(widget),button,activate_time);
}

void hide_win(GtkWidget *widget,gpointer data)
{
	gtk_widget_hide_all(GTK_WIDGET(data));
}

void hide_window(GtkWidget *widget,GdkEventWindowState *event,gpointer data)
{
	if(event->changed_mask == GDK_WINDOW_STATE_ICONIFIED && event->new_window_state == GDK_WINDOW_STATE_ICONIFIED)
		gtk_widget_hide_all(widget);
}

void tray_on_click(GtkWidget *widget,gpointer data)
{
	if(!gtk_widget_get_visible(GTK_WIDGET(data)))
	{
		gtk_widget_show_all(GTK_WIDGET(data));
		gtk_window_present(GTK_WINDOW(data));
	}
	else
		gtk_widget_hide_all(GTK_WIDGET(data));
	//gtk_status_icon_set_visible(GTK_STATUS_ICON(widget),FALSE);
}

gboolean _get_connect(DATA *data)
{
	GtkTextBuffer *buffer=gtk_text_view_get_buffer(GTK_TEXT_VIEW(data->text));
	GtkTextIter end;
	char buf[2048];
	int len;

	len=read(data->pipefd[0],buf,sizeof(buf)-1);

	if(len<=0)
	{
		usleep(300);
		return TRUE;
	}

	gtk_text_buffer_get_end_iter(buffer,&end);
	if(gtk_text_iter_get_offset(&end)>INT_MAX)
	{
		gtk_text_buffer_set_text(buffer,"",0);

		return TRUE;
	}

	if(strstr(buf,"WARNING"))
		gtk_text_buffer_insert_with_tags_by_name(buffer,&end,buf,
				len,"green_fg",NULL);
	else if(strstr(buf,"ERROR"))
		gtk_text_buffer_insert_with_tags_by_name(buffer,&end,buf,
				len,"red_fg",NULL);
	else
		gtk_text_buffer_insert_with_tags_by_name(buffer,&end,buf,
				len,"black_fg",NULL);

	return TRUE;
}

void get_connect(DATA *data)
{
	/*GtkTextBuffer *buffer=gtk_text_view_get_buffer(GTK_TEXT_VIEW(data->text));
	GtkTextIter end;
	int pipefd[2];
	char buf[1024];
	int len;
	guint offset;*/
	int flags;

	//gtk_text_buffer_get_iter_at_mark(buffer,&iter,mark);
	pipe(data->pipefd);
	sig_data=data;

	if((data->pid=fork())==0)
	{
		close(data->pipefd[0]);
		dup2(data->pipefd[1],STDERR_FILENO);
		dup2(data->pipefd[1],STDOUT_FILENO);

		//execl("/usr/bin/python","python","/home/brisk/vbox-share/goagent/local/proxy.py",NULL);
		if(execl(data->python_path,"python",data->proxy_py_path,NULL)==-1)
		{
			write(STDERR_FILENO,_("Error Python Path!"),
					strlen(_("Error Python Path!")));
			kill(getppid(),SIGUSR1);
			_exit(-1);
		}
	}

	close(data->pipefd[1]);
	flags=fcntl(data->pipefd[0],F_GETFL,0);
	flags|=O_NONBLOCK;
	fcntl(data->pipefd[0],F_SETFL,flags);

	g_idle_add((GSourceFunc)_get_connect,data);

	/*while(len=read(pipefd[0],buf,sizeof(buf))-1)
	{
		gtk_text_buffer_get_end_iter(buffer,&end);

		if(len<=0)
			continue;

		if(gtk_text_iter_get_offset(&end)>INT_MAX)
		{
			gtk_text_buffer_set_text(buffer,"",0);
			bzero(buf,sizeof(buf));
			continue;
		}

		if(strstr(buf,"ERROR") || strstr(buf,"WARNING"))
		{
			gtk_text_buffer_insert(buffer,&end,buf,len);
			gtk_text_buffer_insert(buffer,&end,"\n",1);
		}
		bzero(buf,sizeof(buf));
	}*/
	/*while(1)
	{
		len=read(pipefd[0],buf,sizeof(buf)-1);
		if(len<=0)
			continue;

		while(gtk_events_pending())
			gtk_main_iteration();

		pthread_mutex_lock(&data->mutex);
		gtk_text_buffer_get_end_iter(buffer,&end);
		gtk_text_iter_get_offset(&end);
		pthread_mutex_unlock(&data->mutex);

		if(offset>INT_MAX)
		{
			while(gtk_events_pending())
				gtk_main_iteration();

			pthread_mutex_lock(&data->mutex);
			gtk_text_buffer_set_text(buffer,"",0);
			pthread_mutex_unlock(&data->mutex);

			bzero(buf,sizeof(buf));
			continue;
		}

		while(gtk_events_pending())
			gtk_main_iteration();

		pthread_mutex_lock(&data->mutex);
		gtk_text_buffer_insert(buffer,&end,buf,len);
		pthread_mutex_unlock(&data->mutex);
		//gtk_text_buffer_insert(buffer,&end,"\n",1);
		bzero(buf,sizeof(buf));
	}*/
}

void kill_pthread(int signum)
{
	//message_box(NULL,strerror(errno));
	sig_data->off=0;
	while(waitpid(-1,NULL,WNOHANG)!=-1);
	//pthread_cancel(sig_data->thread);
	close(sig_data->pipefd[0]);
	g_idle_remove_by_data(sig_data);
}

gboolean is_python_and_goagent_path(char *python_path,char *goagent_path)
{
	if(python_path==NULL)
	{
		message_box(NULL,_("You Should Set Python Path!"));
		return FALSE;
	}

	if(goagent_path==NULL)
	{
		message_box(NULL,_("You Should Set GoAgent Path!"));
		return FALSE;
	}

	return TRUE;
}

void select_python_path(GtkWidget *widget,gpointer data)
{}

void select_goagent_path(GtkWidget *widget,gpointer data)
{}

void select_gtk_goagent_path(GtkWidget *widget,gpointer data)
{}

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

void exit_pre(GtkWidget *widget,gpointer data)
{
	gtk_main_quit();
}

void save_conf_with_exit(GtkWidget *widget,gpointer data)
{}
