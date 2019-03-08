#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define STR_LENGTH 256
#define DATA_LENGTH 100

#define ERR_STRING "�G���[�F%s ������܂���B"

// ini�t�@�C����ǂݍ���
void load_data(char data[DATA_LENGTH][STR_LENGTH],char *path)
{
	FILE *fp;
	errno_t err;
	int i = 0;

	err = fopen_s(&fp, path, "r");
	if (err != 0) return;
	while (fgets(data[i], STR_LENGTH, fp) != NULL) {
		int j = 0;
		while (data[i][j] != '\0' && j < STR_LENGTH) {
			if (data[i][j] == '\n') data[i][j] = '\0';
			j++;
		}
		if (data[i][0] != '#' && strlen(data[i]) > 0) i++;
		if (i >= DATA_LENGTH) break;
	}
	fclose(fp);
	while (i < DATA_LENGTH) data[i++][0] = '\0';
}

// ������������쐬
void time_str(char *str,tm lt)
{
	char num[STR_LENGTH];

	sprintf_s(num, STR_LENGTH, "%02d", lt.tm_hour);
	strcpy_s(str, STR_LENGTH, num);
	strcat_s(str, STR_LENGTH, ":");
	sprintf_s(num, STR_LENGTH, "%02d", lt.tm_min);
	strcat_s(str, STR_LENGTH, num);
}

// ���ݎ�����������擾
void now_str(char *str)
{
	time_t t;
	tm *lt = new tm;
	
	t = time(NULL);
	localtime_s(lt, &t);
	time_str(str, *lt);
	delete(lt);
}

// �t�@�C�����i�g���q�����j���쐬����
void get_basename(char *path,const char *src) {
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

// �f�[�^�����񂩂�IP��������擾
void get_ip_address(char *str, char *data)
{
	char *p = NULL;

	strcpy_s(str, STR_LENGTH, data);
	strcpy_s(str, STR_LENGTH, strtok_s(str, ",", &p));
}

// �f�[�^�����񂩂�J�n�������擾
void get_start_time(char *str, char *data)
{
	char *p = NULL;

	strcpy_s(str, STR_LENGTH, data);
	strtok_s(str, ",", &p);
	strcpy_s(str, STR_LENGTH, strtok_s(NULL, ",", &p));
}

// �f�[�^�����񂩂�I���������擾
void get_end_time(char *str, char *data)
{
	char *p = NULL;

	strcpy_s(str, STR_LENGTH, data);
	strtok_s(str, ",", &p);
	strtok_s(NULL, ",", &p);
	strcpy_s(str, STR_LENGTH, strtok_s(NULL, ",", &p));
}

// IP�����ƂɎ��Ԃ̃`�F�b�N���s��
int chktime(char data[DATA_LENGTH][STR_LENGTH],char *ip)
{
	char now[STR_LENGTH],tgt[STR_LENGTH];
	int i;

	
	return 0;
}

// �f�o�b�O�p
void debug_print(char data[DATA_LENGTH][STR_LENGTH])
{
	int i = 0;
	char tmp[STR_LENGTH];

	now_str(tmp);
	printf("���ݎ����@�F%s\n", tmp);
	printf("/////////////////////////////////\n\n");
	while (strlen(data[i]) > 0 && i < DATA_LENGTH) {
		printf("Index:%03d-------------------\n",i);
		get_ip_address(tmp, data[i]);
		printf("IP�A�h���X�F%s\n", tmp);
		get_start_time(tmp, data[i]);
		printf("�J�n�����@�F%s\n", tmp);
		get_end_time(tmp, data[i]);
		printf("�I�������@�F%s\n", tmp);
		printf("\n");
		i++;
	}
}

// ���C�����[�`��
int main(int argc, const char** argv)
{
	char path[STR_LENGTH];
	char data[DATA_LENGTH][STR_LENGTH];
	int ret = 0;

	get_basename(path,argv[0]);
	strcat_s(path,STR_LENGTH,".ini");
	load_data(data,path);
	if (data == NULL) {
		fprintf(stderr, ERR_STRING, path);
		ret = 1;
	}
	else {
		debug_print(data);
	}
	return ret;
}
