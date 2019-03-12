#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define STR_LENGTH		256
#define DAT_LENGTH		100

#define ERR_STRING		"エラー：iniファイルがありません。\n"
#define USG_STRING		"USAGE:timechk.exe <IP Address>\n"

#define MSG_STRING		"現在時刻 [%s] IP:%s の指定時間帯(%s-%s)です。\n"

#define INI_TEMPLATE1	"#検索文字列     ,開始 ,終了 \n"
#define INI_TEMPLATE2	"#xxx.xxx.xxx.xxx,HH:MM,HH:MM\n"

// グローバル変数定義
char Data[DAT_LENGTH][STR_LENGTH];

// INIファイル名を作成する
void get_ini_name(char *path, const char *src)
{

	strcpy_s(path, STR_LENGTH, src);
	*strchr(path, '.') = '\0';
	strcat_s(path, STR_LENGTH, ".ini");
}

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

// 空のiniファイルを作成する
void make_ini_file(char *path)
{
	FILE *fp;
	errno_t err;

	err = fopen_s(&fp, path, "w");
	if (err != 0) return;
	fputs(INI_TEMPLATE1, fp);
	fputs(INI_TEMPLATE2, fp);
	fclose(fp);
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
void min2str(char *str, int min)
{
	char tmp[STR_LENGTH];

	sprintf_s(str, STR_LENGTH, "%02d", min / 60);
	sprintf_s(tmp, STR_LENGTH, "%02d", min % 60);
	strcat_s(str, STR_LENGTH, ":");
	strcat_s(str, STR_LENGTH, tmp);
}

// データ文字列から検索キー文字列を取得
void get_key_str(char *str, char *data)
{
	char *p = NULL;

	strcpy_s(str, STR_LENGTH, data);
	strcpy_s(str, STR_LENGTH, strtok_s(str, ",", &p));
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

void print_message(int now_min, char *key, int start_min, int end_min)
{
	char snow[STR_LENGTH],  tgt_st[STR_LENGTH], tgt_et[STR_LENGTH];

	min2str(snow, now_min);
	min2str(tgt_st, start_min);
	min2str(tgt_et, end_min);
	printf(MSG_STRING, snow, key, tgt_st, tgt_et);
}

// IPをもとに時間のチェックを行う
int chktime(char *key)
{
	char tgt_key[STR_LENGTH];
	int i = 0,now_min;

	now_min = now();
	while (strlen(Data[i]) > 0) {
		get_key_str(tgt_key, Data[i]);
		if (strcmp(tgt_key, key) == 0) {
			int start_min = get_start_time(Data[i]);
			int end_min = get_end_time(Data[i]);
			if (now_min >= start_min && now_min <= end_min) {
				print_message(now_min, tgt_key, start_min, end_min);
				return 1;
			}
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
		get_key_str(tmp, Data[i]);
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
	char path[STR_LENGTH];

	get_ini_name(path, argv[0]);
	if (load_data(path)) {
		printf(ERR_STRING);
		make_ini_file(path);
	}
	else {
		if (argc == 2) {
			// debug_print();
			ret = chktime(argv[1]);
		}
		else {
			printf(USG_STRING);
		}
	}
	return ret;
}
