#ifndef _MENU_H
#define _MENU_H

#include <gtk/gtk.h>
#include <locale.h>
#include <libintl.h>
#include <stdlib.h>

/*国际化支持，字符替换宏*/
#define _(string) gettext(string)
#define N_(string) string

/*关于数据结构
 * 作者
 * 版本
 * 版权
 * 注解
 * 许可
 * 网址
 */
typedef struct
{
	gchar *name;
	gchar *version;
	gchar *copyright;
	gchar *comments;
	gchar *license;
	gchar *website;
	//const gchar **authors;
}ABOUT;

/*回调函数*/
typedef void (*callback)(GtkWidget *,gpointer);

/*初始化关于数据结构*/
void init_about_data(ABOUT *about);

/*创建菜单项*/
void create_menu_with_image(GtkWidget *menu,gchar *item,GtkAccelGroup *accel_group,callback func,gpointer data);

/*创建菜单*/
GtkWidget *create_menu(GtkWidget *menu_bar,gchar *name);

#endif
