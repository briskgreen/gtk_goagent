#ifndef _DIALOG_H
#define _DIALOG_H

#include "menu.h"
#include <sys/types.h>
#include <sys/wait.h>

/*typedef struct
{
	gchar *name;
	gchar *email;
	gchar *readme;
}HELP;*/

/*主数据结构
 * 用来在主界面中传递相关调用
 * 主界面显示构件
 * 当前是否已经连接
 * 启动进程pid
 * 虚拟终端
 * proxy.py路径
 * python路径
 */
typedef struct
{
	GtkWidget *text;
	int off;
	pid_t pid;
	int pty;
	char *proxy_py_path;
	char *python_path;
}DATA;

//void init_help_data(HELP *help);

/*关于对话框，也是关于架设函数*/
void about_dialog(GtkWidget *about,gpointer data);

/*是否确定退出*/
void really_quit(GtkWidget *widget,gpointer data);

/*错误弹出对话框*/
void error_message(GtkWidget *widget,gchar *data);

/*消息弹出框*/
void message_box(GtkWidget *widget,gchar *data);

/*判断对话框*/
gboolean message_box_ok(gchar *data);

/*帮助对话框，也是帮助回调函数*/
void help_dialog(GtkWidget *help,gpointer data);

#endif
