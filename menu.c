#include "menu.h"
//#include <string.h>

gchar *set_string(gchar *source);

void init_about_data(ABOUT *about)
{
	/*const gchar *authors=malloc(strlen("Brisk")+1);

	bzero(authors,strlen(authors)+1);
	strncpy(authors,"Brisk",strlen("Brisk"));*/

	about->name=set_string("Gtk GoAgent");
	about->version=set_string("1.0");
	about->copyright=set_string("Copyright © 2013-05 By 炕头哥");
	about->comments=set_string("一个简单的GoAgent前端");
	about->license=set_string("GPL\nsee <http://www.gnu.org/licenses/>.");
	about->website=set_string("briskgreen@163.com");
	//*about->authors=set_string("Brisk");
	//about->authors=&authors;
}

void create_menu_with_image(GtkWidget *menu,gchar *item,\
		GtkAccelGroup *accel_group,callback func,gpointer data)
{
	GtkWidget *menu_item;

	//if(strstr(item,"gtk"))
		menu_item=gtk_image_menu_item_new_from_stock(item,accel_group);
	//else
	//	menu_item=gtk_menu_item_new_with_mnemonic(item);

	g_signal_connect(G_OBJECT(menu_item),"activate",G_CALLBACK(func),data);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu),menu_item);
	gtk_widget_show(menu_item);
}

GtkWidget *create_menu(GtkWidget *menu_bar,gchar *name)
{
	GtkWidget *item;
	GtkWidget *menu;

	menu=gtk_menu_new();
	item=gtk_menu_item_new_with_mnemonic(name);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu_bar),item);
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(item),menu);

	gtk_widget_show(menu);
	return menu;
}

gchar *set_string(gchar *source/*,int size*/)
{
	/*sink=malloc(sizeof(gchar)*size+1);
	memset(sink,0,sizeof(gchar)*size+1);
	strncpy(sink,source,size);*/
	return source;
}
