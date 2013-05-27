#ifndef _AUTOUPGRADE_H
#define _AUTOUPGRADE_H

#include <curl/curl.h>
#include <zlib.h>
#include <dirent.h>
#include <sys/stat.h>
#include "config.h"

#define GOAGENT_URL "https://code.google.com/p/goagent/"
//#define GTK_GOAGENT_URL "https://briskgreen.github.io/Download/Gtk GoAgent"
#define PROXY "127.0.0.1:8087"
#define UPDATE_TIME 60*10
#define error_quit(s)\
{\
	perror(s);\
	return;\
}

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

char *goagent_version;

char *get_version(char *path);

size_t is_upgrade_goagent(char *ptr,size_t size,size_t nmebm,
		void *stream);

void download_file(char *path,char *is_upload);

void _download_file(CURL_DATA *data);

int update_progress(void *data,double dltotal,double dlnow,
		double ultotal,double ulnow);

void memcat(char *tmp,char *buf,unsigned long len);

char *get_zip_first_file_name(char *zip_file);

char *get_zip_first_file_name(char *zip_file);

void auto_upgrade_goagent(char *url,CONFDATA *conf);

//void auto_upgrade_gtk_goagent(char *url);

void unzip(char *zip_file,char *goagent_path);

int get_zip_file_num(char *zip_file);

void quit_no_download(GtkWidget *widget,gpointer data);

void copy_file(const char *old_path,const char *new_path);

void rm_dir(const char *path);

void copy_dir(const char *old_path,const char *new_path);

#endif
