#include "callback.h"
#include "autoupgrade.h"
#include <locale.h>
#include <stdlib.h>
#include <limits.h>
#include <fcntl.h>

/*初始化数据并启动GoAgent*/
void get_connect(DATA *data);
/*判断python和goagent路径是否已经设置，如果为NULL，则未设置*/
gboolean is_python_and_goagent_path(char *python_path,char *goagent_path);
/*重定向标准输出与日志输出到主界面日志框*/
gboolean _get_connect(DATA *data);
/*上传到服务器*/
gboolean _upload_goagent(UP_DATA *data);
/*返回重写向交互输入*/
char *get_input_string(const char *msg);
/*返回上传文件所在绝对路径*/
char *get_uploader_path(const char *goagent_path);
//void _upgrade_goagent(char *proxy_py_path);

/*信号处理函数所处理的数据*/
DATA *sig_data;

void connect_goagent(GtkWidget *widget,DATA *data)
{
	/*如果off为开，则当前已经启动*/
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

	/*断开连接设置off开关并杀死所调用的进程*/
	data->off=0;
	kill(data->pid,SIGKILL);
	//while(waitpid(-1,NULL,WNOHANG)!=-1);
	g_idle_remove_by_data(data);

	/*while(gtk_events_pending())
		gtk_main_iteration();*/

	/*清除主界面日志框内容*/
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
{
	help_dialog(widget,data);
}

void about(GtkWidget *widget,gpointer data)
{
	about_dialog(widget,data);
}

void upgrade_goagent(GtkWidget *widget,gpointer data)
{
	if(fork()==0)
		execl("./upgrade","upgrade",GOAGENT_URL,(char *)data,NULL);
}

/*void upgrade_gtk_goagent(GtkWidget *widget,gpointer data)
{}*/

void upload(GtkWidget *widget,gpointer data)
{
	if(fork()==0)
		if(execl("./upload","upload",NULL)==-1)
		{
			perror("Start Upload");
			_exit(-1);
		}
}

void preferences(GtkWidget *widget,gpointer data)
{
	/*setlocale(LC_ALL,"");
	setlocale(LC_CTYPE,"zh_CN.UTF-8");
	setenv("LANG","zh_CN.UTF-8",1);*/
	if(fork()==0)
		if(execl("./pre_ui","pre_ui",NULL)==-1)
		{
			perror("Start pre_ui");
			_exit(-1);
		}

	//wait(NULL);
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
	/*如果当前窗口可见，刚隐藏，否则显示*/
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
	GtkTextMark *mark;
	//char buf[2048];
	char buf;
	int ret;
	//int len;
	fd_set reads;
	struct timeval timeout;
	char *error_color="\033[31m";
	char *warning_color="\033[33m";
	char *debug_color="\033[32m";
	char *reset_color="\033[0m";

	/*len=read(data->pipefd[0],buf,sizeof(buf)-1);

	if(len<=0)
	{
		usleep(300);
		return TRUE;
	}*/
	/*设置超时*/
	/*timeout.tv_sec=0;
	timeout.tv_usec=300;

	FD_ZERO(&reads);
	FD_SET(data->pty,&reads);*/

	/*侦听读，直到有可读数据，或者超时
	 * 如果超时则返回TRUE，断续运行
	 * 否则将显示读到的数据
	 */
	/*ret=select(data->pty+1,&reads,NULL,NULL,&timeout);

	if(ret==-1 || ret==0)
		return TRUE;*/

	/*len=read(data->pty,buf,sizeof(buf)-1);

	if(len<=0)
		return TRUE;

	buf[len]='\0';*/
	while(1)
	{
		FD_ZERO(&reads);
		FD_SET(data->pty,&reads);
		timeout.tv_sec=0;
		timeout.tv_usec=300;

		ret=select(data->pty+1,&reads,NULL,NULL,&timeout);
		if(ret == -1 || ret == 0)
			return TRUE;

		ret=read(data->pty,&buf,sizeof(char));
		if(ret > 0)
		{
			if(buf == '\n')
				break;

			g_string_append_c(data->buf,buf);
		}
		else
			return TRUE;
	}

	gtk_text_buffer_get_end_iter(buffer,&end);
	if(gtk_text_iter_get_offset(&end)>INT_MAX)
	{
		gtk_text_buffer_set_text(buffer,"",0);

		return TRUE;
	}

	if(strncmp(data->buf->str,reset_color,strlen(reset_color)) ==0)
		g_string_erase(data->buf,0,strlen(reset_color));

	if(strncmp(data->buf->str,error_color,strlen(error_color)) == 0)
	{
		gtk_text_buffer_insert_with_tags_by_name(buffer,&end,
				data->buf->str+strlen(error_color),
				-1,"red_fg",NULL);
	}
	else if(strncmp(data->buf->str,warning_color,strlen(warning_color)) == 0)
	{
		gtk_text_buffer_insert_with_tags_by_name(buffer,&end,
				data->buf->str+strlen(warning_color),
				-1,"green_fg",NULL);
	}
	else if(strncmp(data->buf->str,debug_color,strlen(debug_color)) == 0)
	{
		gtk_text_buffer_insert_with_tags_by_name(buffer,&end,
				data->buf->str+strlen(debug_color),
				-1,"yellow_fg",NULL);
	}
	/*else if(strncmp(data->buf->str,reset_color,strlen(reset_color)) == 0)
	{
		gtk_text_buffer_insert_with_tags_by_name(buffer,&end,
				data->buf->str+strlen(reset_color),
				-1,"red_fg",NULL);
	}*/
	else
		gtk_text_buffer_insert_with_tags_by_name(buffer,&end,
				data->buf->str,-1,"black_fg",NULL);
	/*对WARNING与ERROR关键字设置颜色*/
/*	if(strstr(data->buf->str,"WARNING"))
		gtk_text_buffer_insert_with_tags_by_name(buffer,&end,
				data->buf->str,
				-1,"green_fg",NULL);
	else if(strstr(data->buf->str,"ERROR"))
		gtk_text_buffer_insert_with_tags_by_name(buffer,&end,
				data->buf->str,
				-1,"red_fg",NULL);
	else
		gtk_text_buffer_insert_with_tags_by_name(buffer,&end,
				data->buf->str,
				-1,"black_fg",NULL);*/

	/*gtk_text_view_scroll_to_iter(GTK_TEXT_VIEW(data->text),&end,
			0,FALSE,FALSE,FALSE);*/

	/*创建一个mark标记
	 * 并移动到文本最后
	 * 然后将滚动条移动到最后面
	 */
	mark=gtk_text_buffer_create_mark(buffer,NULL,&end,TRUE);
	gtk_text_buffer_move_mark(buffer,mark,&end);
	gtk_text_view_scroll_to_mark(GTK_TEXT_VIEW(data->text),mark,
			0,TRUE,TRUE,TRUE);

	g_string_erase(data->buf,0,-1);

	return TRUE;
}

void get_connect(DATA *data)
{

	//pipe(data->pipefd);
	sig_data=data;

	//RD_DATA *put_data=malloc(sizeof(RD_DATA));
	//put_data->text=data->text;

	if((data->pid=forkpty(&data->pty,NULL,NULL,NULL))==0)
	{
		if(execl(data->python_path,"python",data->proxy_py_path,NULL)==-1)
		{
			write(STDERR_FILENO,_("Error Python Path!"),
					strlen(_("Error Python Path!")));
			kill(getppid(),SIGUSR1);
			_exit(-1);
		}
	}

	/*if((data->pid=fork())==0)
	{
		close(data->pipefd[0]);
		dup2(data->pipefd[1],STDERR_FILENO);
		dup2(data->pipefd[1],STDOUT_FILENO);

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
	fcntl(data->pipefd[0],F_SETFL,flags);*/

	g_idle_add((GSourceFunc)_get_connect,data);
}

void clean_data(int signum)
{
	//message_box(NULL,strerror(errno));
	/*信息处理函数*/
	sig_data->off=0;
	while(waitpid(-1,NULL,WNOHANG)!=-1);
	//pthread_cancel(sig_data->thread);
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

gboolean _upload_goagent(UP_DATA *data)
{
	GtkTextBuffer *buffer=gtk_text_view_get_buffer(GTK_TEXT_VIEW(data->text));
	GtkTextIter end;
	GtkTextMark *mark;
	char buf[512];
	int ret;
	char *result;
	int len;
	fd_set reads;
	struct timeval timeout;

	timeout.tv_sec=0;
	timeout.tv_usec=300;

	FD_ZERO(&reads);
	FD_SET(data->pty,&reads);

	/*侦听读，直到有数据可读
	 * 或者超时
	 * 如果超时则返回TRUE
	 * 否则显示内容
	 */
	ret=select(data->pty+1,&reads,NULL,NULL,&timeout);

	if(ret==-1 || ret==0)
		return TRUE;

	len=read(data->pty,buf,sizeof(buf)-1);

	if(len<=0)
		return TRUE;

	buf[len]='\0';

	gtk_text_buffer_get_end_iter(buffer,&end);

	if(strstr(buf,"WARNING"))
		gtk_text_buffer_insert_with_tags_by_name(buffer,&end,buf,
				len,"green_fg",NULL);
	else if(strstr(buf,"ERROR"))
		gtk_text_buffer_insert_with_tags_by_name(buffer,&end,buf,
				len,"red_fg",NULL);
	else
		gtk_text_buffer_insert_with_tags_by_name(buffer,&end,buf,
				len,"black_fg",NULL);

	/*交互输入APPID Email与Password*/
	if(strstr(buf,"APPID:"))
	{
		result=get_input_string("APPID");
		write(data->pty,result,strlen(result));
		write(data->pty,"\n",1);
	}

	if(strstr(buf,"Email:"))
	{
		result=get_input_string("Email");
		write(data->pty,result,strlen(result));
		write(data->pty,"\n",1);
	}

	if(strstr(buf,"Password for"))
	{
		result=get_input_string("Password");
		write(data->pty,result,strlen(result));
		write(data->pty,"\n",1);
	}

	if(strstr(buf,"proxy.ini") || strstr(buf,"python_upload:"))
	{
		kill(data->pid,SIGKILL);
		while(waitpid(-1,NULL,WNOHANG)!=-1);
		data->off=0;
		return FALSE;
	}

	/*gtk_text_view_scroll_to_iter(GTK_TEXT_VIEW(data->text),&end,
			0,FALSE,FALSE,FALSE);*/

	/*移动滚动条到最后*/
	mark=gtk_text_buffer_create_mark(buffer,NULL,&end,TRUE);
	gtk_text_buffer_move_mark(buffer,mark,&end);
	gtk_text_view_scroll_to_mark(GTK_TEXT_VIEW(data->text),mark,
			0,TRUE,TRUE,TRUE);

	return TRUE;
}

void upload_goagent(GtkWidget *widget,UP_DATA *data)
{
	if(data->off)
	{
		message_box(widget,_("Upload Now!"));
		return;
	}

	if(!is_python_and_goagent_path(data->python_path,data->goagent_path))
		return;

	//chdir(data->goagent_path);

	if((data->pid=forkpty(&data->pty,NULL,NULL,NULL))==0)
	{
		if(execl(data->python_path,"python_upload",
			get_uploader_path(data->goagent_path),NULL)==-1)
		{
			write(STDERR_FILENO,_("Error Python Path!"),
					strlen(_("Error Python Path!")));
			_exit(-1);
		}
	}

	g_idle_add((GSourceFunc)_upload_goagent,data);
	data->off=1;
}

char *get_input_string(const char *msg)
{
	GtkWidget *entry;
	GtkWidget *dialog;

	dialog=gtk_dialog_new();
	gtk_window_set_title(GTK_WINDOW(dialog),msg);
	entry=gtk_entry_new();
	/*如果传过来的字符为Password则设置密码输入不可见*/
	if(strcmp(msg,"Password")==0)
		gtk_entry_set_visibility(GTK_ENTRY(entry),FALSE);

	gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog)->vbox),entry,FALSE,FALSE,10);
	gtk_widget_show(entry);
	gtk_dialog_add_button(GTK_DIALOG(dialog),GTK_STOCK_OK,GTK_RESPONSE_OK);

	gtk_dialog_run(GTK_DIALOG(dialog));

	gtk_widget_hide(dialog);

	return gtk_entry_get_text(GTK_ENTRY(entry));
}

char *get_uploader_path(const char *goagent_path)
{
	char *upload_path;
	char *upload_name="/server/uploader.zip";

	upload_path=malloc(strlen(goagent_path)+strlen(upload_name)+1);
	bzero(upload_path,strlen(goagent_path)+strlen(upload_name)+1);
	strncpy(upload_path,goagent_path,strlen(goagent_path));
	strncat(upload_path,upload_name,strlen(upload_name));

	return upload_path;
}
