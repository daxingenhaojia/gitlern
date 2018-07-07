// File: ansi_color.h
// Author: �쾸�� Xu Jinghang
// comment: ANSI��׼��ɫ���� ��Ҫ�ն˳���֧��,����debug
// date: 2017-04-21
// demo:
//		printf(ANSI_RED "This text is red color\n" ANSI_OFF); // ��δ��뽫�Ժ�ɫ��ʾ
#ifndef ANSI_COLOR_H
#define ANSI_COLOR_H

#define ANSI_OFF 		"\033[0m"	// �ر���ɫ����
#define ANSI_BOLD 		"\033[1m"	// �Ӵ�
#define ANSI_UNDERLINE	"\033[4m"	// �»���
#define ANSI_BLINK		"\033[5m"	// ��˸
#define ANSI_NEGATIVE	"\033[7m"	// ����

#define ANSI_BLACK		"\033[30m"	// ��
#define ANSI_RED		"\033[31m"	// ��
#define ANSI_GREEN		"\033[32m"	// ��
#define ANSI_YELLOW		"\033[33m"	// ��
#define ANSI_BLUE		"\033[34m"	// ��
#define ANSI_MAGENTA	"\033[35m"	// ��
#define ANSI_CYAN		"\033[36m"	// ��
#define ANSI_GRAY		"\033[37m"	// ��

#endif
