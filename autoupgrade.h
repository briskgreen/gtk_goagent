#ifndef _AUTOUPGRADE_H
#define _AUTOUPGRADE_H

#include <curl/curl.h>
#include <zlib.h>
#include "config.h"

#define GOAGENT_URL "https://code.google.com/p/goagent/"
//#define GTK_GOAGENT_URL "https://briskgreen.github.io/Download/Gtk GoAgent"
#define PROXY "127.0.0.1:8087"
#define UPDATE_TIME 60*10

struct zip_head
{
	unsigned long head;
	unsigned short pk;
	unsigned short q;
	unsigned short type;
	unsigned short time;
	unsigned short date;
	unsigned long crc;
	unsigned long d_len;
	unsigned long len;
	unsigned short f_len;
	unsigned short e_len;
}__attribute__((packed));

typedef struct
{
	char *url;
	gboolean d_ok;
	GtkWidget *progress_bar;
}CURL_DATA;

void auto_upgrade_goagent(char *url,CONFDATA *conf);

//void auto_upgrade_gtk_goagent(char *url);

void unzip(char *zip_file,char *goagent_path);

int get_zip_file_num(char *zip_file);

void quit_no_download(GtkWidget *widget,gpointer data);

void copy_file(char *old_path,char *new_path);

void rm_dir(char *path);

#endif
