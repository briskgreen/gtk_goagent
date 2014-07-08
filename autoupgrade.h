#ifndef _AUTOUPGRADE_H
#define _AUTOUPGRADE_H

#include <curl/curl.h>
#include <zlib.h>
#include <dirent.h>
#include <sys/stat.h>
#include <pcre.h>
#include "config.h"
//#include "mysock/mysock.h"

//GoAgent项目主页地址
#define GOAGENT_URL "https://code.google.com/p/goagent/"
#define GOAGENT_HIS_URL "https://code.google.com/p/goagent/wiki/History"
//#define GTK_GOAGENT_URL "https://briskgreen.github.io/Download/Gtk GoAgent"
//代理地址与端口
#define PROXY "127.0.0.1:8087"
//更新周期
#define UPDATE_TIME 60*60*2
#define error_quit(s)\
{\
	perror(s);\
	return;\
}

/*
         组成    　                长度
      文件头标记                  4 bytes  (0x04034b50)
      解压文件所需 pkware 版本    2 bytes
      全局方式位标记              2 bytes
  　　压缩方式                    2 bytes
  　　最后修改文件时间             2 bytes
 　　 最后修改文件日期             2 bytes
 　　 CRC-32校验                  4 bytes
 　 　压缩后尺寸                  4 bytes
 　 　未压缩尺寸                  4 bytes
 　　 文件名长度                  2 bytes

      扩展记录长度                2 bytes
 　　 文件名                     （不定长度）
 　　 扩展字段                   （不定长度）
*/
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
}__attribute__((packed)); //通知编译器不要对该结构体进行内存对齐

/*CURL相关数据
 * 目标地址
 * 数据下载是否完成
 * 进度条构件
 */

typedef struct
{
	char *url;
	gboolean d_ok;
	GtkWidget *progress_bar;
}CURL_DATA;

/* 获取HTTP请求字符串
 * data为字符串
 * len为当前获取的长度
 */
typedef struct
{
	int len;
	char *data;
}RET_DATA;

//char *goagent_version; //goagent的当前版本号

/*得到当前goagent版本号*/
char *get_version(char *path);

/*检查是否需要更新*/
char *is_upgrade_goagent(RET_DATA *data,char *goagent_version);

/*检查是否需要上传*/
int goagent_is_upload(RET_DATA *data);

/*下载主界面*/
void download_file(char *path,int is_upload);

/*下载文件线程*/
void _download_file(CURL_DATA *data);

/*更新进度条函数*/
int update_progress(void *data,double dltotal,double dlnow,
		double ultotal,double ulnow);

void memcat(char *tmp,char *buf,unsigned long len);

char *get_zip_first_file_name(char *zip_file);

char *get_zip_first_file_name(char *zip_file);

void auto_upgrade_goagent(char *url,CONFDATA *conf);

/*由于目测Gtk GoAgent不会有什么发动的样子
 * 所以取消了Gtk GoAgent的自动更新*/

//void auto_upgrade_gtk_goagent(char *url);

/*解压并复制文件*/
void unzip(char *zip_file,char *goagent_path);

/*由于最新goagent最新的zip文件采用了非标准的文件压缩方式
 * 所以需要使用到unzip程序*/
void _unzip(char *zip_file,char *goagent_path);

/*检查该使用何种方式解压文件*/
void unzip_t(char *zip_file,char *goagent_path);

int get_zip_file_num(char *zip_file);

/*在没有完成下载的情况下退出设置d_ok为FALSE*/
void quit_no_download(GtkWidget *widget,gpointer data);

void copy_file(const char *old_path,const char *new_path);

void rm_dir(const char *path);

void copy_dir(const char *old_path,const char *new_path);

size_t get_data(char *ptr,size_t size,size_t nmemb,RET_DATA *data);

#endif
