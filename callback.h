/*这个头文件主要是架设函数的函数声明
 * 以及一些数据的定义
 */

#ifndef _CALLBACK_H
#define _CALLBACK_H

#include "autoupgrade.h"
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <pty.h>

/*定义上传数据结构
 * 更新文本构件
 * 开关
 * 进程ID
 * 虚拟终端
 * python路径
 * goagent路径
 */

typedef struct
{
	GtkWidget *text;
	int off;
	pid_t pid;
	int pty;
	char *python_path;
	char *goagent_path;
}UP_DATA;

/*连接回调函数*/
void connect_goagent(GtkWidget *widget,DATA *data);

/*断开回调函数*/
void disconnect_goagent(GtkWidget *widget,DATA *data);

/*清屏回调函数*/
void clean_buffer(GtkWidget *widget,DATA *data);

/*帮助回调函数*/
void help(GtkWidget *widget,gpointer data);


/*关于回调函数*/
void about(GtkWidget *widget,gpointer data);

/*更新GoAgent回调函数*/
void upgrade_goagent(GtkWidget *widget,gpointer data);

//void upgrade_gtk_goagent(GtkWidget *widget,gpointer data);

/*上传回调函数*/
void upload(GtkWidget *widget,gpointer data);

/*设置回调函数*/
void preferences(GtkWidget *widget,gpointer data);

/*系统托盘右击事件回调函数*/
void tray_on_menu(GtkWidget *widget,guint button,
		guint32 activate_time,gpointer data);

/*隐藏主界面*/
void hide_win(GtkWidget *widget,gpointer data);

/*隐藏回调函数*/
void hide_window(GtkWidget *widget,GdkEventWindowState *event,gpointer data);

/*单击系统托盘回调函数*/
void tray_on_click(GtkWidget *widget,gpointer data);

/*信号处理函数*/
void clean_data(int signum);

/*上传回调函数*/
void upload_goagent(GtkWidget *widget,UP_DATA *data);

#endif
