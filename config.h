#ifndef _CONFIG_H
#define _CONFIG_H

#include <stdio.h>
#include "dialog.h"
#include <errno.h>

/*得到家目录绝对路径*/
#define HOME getenv("HOME")
/*配置文件名*/
#define CONFFILE "/.gtk_goagentrc"
/*proxy.py相对路径*/
#define PYPATH "/local/proxy.py"
/*uploader.zip相对路径*/
#define UPPATH "/server/uploader.zip"

/*配置文件数据结构
 * 是否已经保存
 * python路径
 * proxy.py路径
 * goagent路径
 * 语言环境
 * Gtk GoAgent路径
 * 字体名称
 * 是否自动更新GoAgent
 */
typedef struct
{
	gboolean save;
	char *python_path;
	char *proxy_py_path;
	char *goagent_path;
	char *language_env;
	char *gtk_goagent_path;
	char *font;
	char *goagent_auto_upgrade;
	//char *gtk_goagent_auto_upgrade;
}CONFDATA;

/*判断配置文件是否存在*/
gboolean is_config_file_ok(void);

/*打开并初始化配置数据结构*/
FILE *open_config(CONFDATA *data);

/*关闭配置*/
void close_config(FILE *fp,CONFDATA *data);

/*保存配置*/
void save_config(FILE **fp,CONFDATA *data);

/*得到python路径*/
char *get_python_path(FILE *fp);

/*得到proxy.py路径*/
char *get_proxy_py_path(FILE *fp);

/*得到语言环境*/
char *get_language_env(FILE *fp);

/*得到Gtk GoAgent路径*/
char *get_gtk_goagent_path(FILE *fp);

/*得到是否自动更新GoAgent*/
char *get_goagent_auto_upgrade(FILE *fp);

/*得到proxy.ini路径*/
char *get_proxy_ini_path(char *goagent_path);

//char *get_gtk_goagent_auto_upgrade(FILE *fp);

/*得到字体名称*/
char *get_font_name(FILE *fp);

/*得到GoAgent路径*/
char *get_goagent_path(FILE *fp);

/*以下以set开关的为设置相应与环境*/
void set_python_path(CONFDATA *data,char *arg);

void set_goagent_path(CONFDATA *data,char *arg);

void set_language_env(CONFDATA *data,char *arg);

void set_gtk_goagent_path(CONFDATA *data,char *arg);

void set_font_by_name(CONFDATA *data,char *arg);

void set_goagent_auto_upgrade(CONFDATA *data,char *arg);

//void set_gtk_goagent_auto_upgrade(CONFDATA *data,char *arg);

/*得到配置文件绝对路径*/
char *get_conf_file_path(void);

#endif
