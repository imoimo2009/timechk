#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define STR_LENGTH 256
#define DAT_LENGTH 100

#define ERR_STRING "エラー：%s がありません。"
#define USG_STRING "USAGE:timechk.exe <IP Address>"

// グローバル変数定義
char Data[DAT_LENGTH][STR_LENGTH];

// iniファイルを読み込む
int load_data(char *path)
{
	FILE *fp;
	errno_t err;
	char tmp[STR_LENGTH];
	int i = 0;

	err = fopen_s(&fp, path, "r");
	if (err != 0) return err;
	while (fgets(tmp, STR_LENGTH, fp) != NULL) {
		int j = 0;
		if (tmp[0] != '#' && strlen(tmp) > 1) {
			while (tmp[j] != '\0' && j < STR_LENGTH) {
				if (tmp[j] == '\n') tmp[j] = '\0';
				j++;
			}
			strcpy_s(Data[i], STR_LENGTH, tmp);
			if (i++ >= DAT_LENGTH) break;
		}
	}
	fclose(fp);
	return 0;
}

// 現在時刻を分に変換して返す
int now(void)
{
	time_t t;
	tm lt;

	t = time(NULL);
	localtime_s(&lt, &t);
	return lt.tm_hour * 60 + lt.tm_min;
}

// ファイル名（拡張子無し）を作成する
void get_basename(char *path,const char *src)
{
	char *p, *s, *r = NULL;

	strcpy_s(path, STR_LENGTH, src);
	s = strtok_s(path, "\\", &p);
	while (s != NULL) {
		r = s;
		s = strtok_s(NULL, "\\", &p);
	}
	strtok_s(r, ".", &p);
	strcpy_s(path, STR_LENGTH,r);
}

// データ文字列からIP文字列を取得
void get_ip_address(char *str, char *data)
{
	char *p = NULL;

	strcpy_s(str, STR_LENGTH, data);
	strcpy_s(str, STR_LENGTH, strtok_s(str, ",", &p));
}

// 時刻文字列から分に変換
int str2min(char *str)
{
	char tmp[STR_LENGTH], *tok, *p = NULL;
	int min;

	strcpy_s(tmp, STR_LENGTH, str);
	tok = strtok_s(tmp, ":", &p);
	min = atoi(tok) * 60;
	tok = strtok_s(NULL, ":", &p);
	min += atoi(tok);
	return min;
}

// 分から時刻文字列に変換
void min2str(char *str,int min)
{
	char tmp[STR_LENGTH];

	sprintf_s(str, STR_LENGTH, "%02d", min / 60);
	sprintf_s(tmp, STR_LENGTH, "%02d", min % 60);
	strcat_s(str, STR_LENGTH, ":");
	strcat_s(str, STR_LENGTH, tmp);
}

// データ文字列から開始時刻(分)を取得
int get_start_time(char *data)
{
	char *p = NULL;
	char str[STR_LENGTH];

	strcpy_s(str, STR_LENGTH, data);
	strtok_s(str, ",", &p);
	strcpy_s(str, STR_LENGTH, strtok_s(NULL, ",", &p));
	return str2min(str);
}

// データ文字列から終了時刻(分)を取得
int get_end_time(char *data)
{
	char *p = NULL;
	char str[STR_LENGTH];

	strcpy_s(str, STR_LENGTH, data);
	strtok_s(str, ",", &p);
	strtok_s(NULL, ",", &p);
	strcpy_s(str, STR_LENGTH, strtok_s(NULL, ",", &p));
	return str2min(str);
}

// IPをもとに時間のチェックを行う
int chktime(char *ip)
{
	char tgt_ip[STR_LENGTH];
	int i = 0,now_min;

	now_min = now();
	while (strlen(Data[i]) > 0) {
		get_ip_address(tgt_ip, Data[i]);
		if (strcmp(tgt_ip, ip) == 0) {
			int start_min = get_start_time(Data[i]);
			int end_min = get_end_time(Data[i]);
			if (now_min >= start_min && now_min <= end_min) return 1;
		}
		i++;
	}
	return 0;
}

// デバッグ用
void debug_print(void)
{
	int i = 0;
	char tmp[STR_LENGTH];

	min2str(tmp,now());
	printf("現在時刻　：%s\n", tmp);
	printf("/////////////////////////////////\n\n");
	while (strlen(Data[i]) > 0 && i < DAT_LENGTH) {
		printf("Index:%03d-------------------\n",i);
		get_ip_address(tmp, Data[i]);
		printf("IPアドレス：%s\n", tmp);
		min2str(tmp, get_start_time(Data[i]));
		printf("開始時刻　：%s\n", tmp);
		min2str(tmp, get_end_time(Data[i]));
		printf("終了時刻　：%s\n", tmp);
		printf("\n");
		i++;
	}
}

// エントリポイント
int main(int argc, char** argv)
{
	int ret = 0;

	if (argc == 2) {
		char path[STR_LENGTH];
		get_basename(path, argv[0]);
		strcat_s(path, STR_LENGTH, ".ini");
		if (load_data(path)) {
			fprintf(stderr, ERR_STRING, path);
		}
		else {
			debug_print();
			ret = chktime(argv[1]);
		}
	}
	else {
		fprintf(stderr, USG_STRING);
	}
	return ret;
}
