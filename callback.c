#include "callback.h"
#include <locale.h>
#include <stdlib.h>

void get_connect(DATA *data);

void connect_goagent(GtkWidget *widget,DATA *data)
{
	static pthread_t thread;
	GtkTextBuffer *buffer=gtk_text_view_get_buffer(GTK_TEXT_VIEW(data->text));

	if(data->off)
	{
		//gtk_text_buffer_insert_at_cursor(buffer,"\nConnected Now\n",15);
		message_box(widget,_("Connected Now\n"));
		return;
	}

	pthread_create(&thread,NULL,(void *)get_connect,data);
	data->thread=thread;
	data->off=1;
}

void disconnect_goagent(GtkWidget *widget,DATA *data)
{
	GtkTextBuffer *buffer=gtk_text_view_get_buffer(GTK_TEXT_VIEW(data->text));

	data->off=0;
	kill(data->pid,SIGKILL);
	while(waitpid(-1,NULL,WNOHANG)!=-1);
	pthread_cancel(data->thread);
	gtk_text_buffer_set_text(buffer,"",0);
}

void help(GtkWidget *widget,gpointer data)
{}

void about(GtkWidget *widget,gpointer data)
{}

void properties(GtkWidget *widget,gpointer data)
{
	setlocale(LC_ALL,"");
	setlocale(LC_CTYPE,"zh_CN.UTF-8");
	setenv("LANG","zh_CN.UTF-8",1);
}

/*void change_language(GtkWidget *widget,gpointer data)
{
	setlocale(LC_ALL,"");
	setlocale(LC_CTYPE,"zh_CN.UTF-8");
	setenv("LANG","zh_CN.UTF-8",1);
}*/

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
	gtk_widget_show_all(GTK_WIDGET(data));
	gtk_window_present(GTK_WINDOW(data));
	//gtk_status_icon_set_visible(GTK_STATUS_ICON(widget),FALSE);
}

void get_connect(DATA *data)
{
	GtkTextBuffer *buffer=gtk_text_view_get_buffer(GTK_TEXT_VIEW(data->text));
	GtkTextIter end;
	int pipefd[2];
	char buf[1024];
	int len;

	//gtk_text_buffer_get_iter_at_mark(buffer,&iter,mark);
	pipe(pipefd);

	if((data->pid=fork())==0)
	{
		close(pipefd[0]);
		dup2(pipefd[1],STDERR_FILENO);
		dup2(pipefd[1],STDOUT_FILENO);

		execl("/usr/bin/python","python","/home/brisk/vbox-share/goagent/local/proxy.py",NULL);
	}

	while(len=read(pipefd[0],buf,sizeof(buf))-1)
	{
		gtk_text_buffer_get_end_iter(buffer,&end);

		if(gtk_text_iter_get_line_offset(&end)>5120)
			gtk_text_buffer_set_text(buffer,"",0);

		if(strstr(buf,"ERROR") || strstr(buf,"WARNING"))
		{
			gtk_text_buffer_insert(buffer,&end,buf,len);
			gtk_text_buffer_insert(buffer,&end,"\n",1);
		}
		bzero(buf,sizeof(buf));
	}
}
