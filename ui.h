#ifndef _UI_H
#define _UI_H

#include "config.h"
#include <string.h>

/*配置环境数据结构
 * 读数据构件
 * 标题
 * 环境
 * 是否保存
 */
typedef struct
{
	GtkWidget *entry;
	char *title;
	char **env;
	gboolean *save;
}ENV_DATA;

/*创建环境配置界面*/
void create_env_interface(char *title,GtkWidget *hbox,callback func,char **conf,gboolean *save);

/*以下以select开头函数为选择相应配置数据*/
void select_python_path(GtkWidget *widget,gpointer data);

void select_goagent_path(GtkWidget *widget,gpointer data);

void select_gtk_goagent_path(GtkWidget *widget,gpointer data);

void select_language(GtkWidget *widget,gpointer data);

void select_font(GtkWidget *widget,gpointer data);

void select_auto_upgrade_goagent(GtkWidget *widget,gpointer data);

void select_auto_upgrade_gtk_goagent(GtkWidget *widget,gpointer data);

void exit_pre(GtkWidget *widget,CONFDATA *data);

void save_conf_now(GtkWidget *widget,gpointer data);

#endif
