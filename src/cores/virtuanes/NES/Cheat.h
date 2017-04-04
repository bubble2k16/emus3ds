#ifndef	__CEHAT_INCLUDED__
#define	__CEHAT_INCLUDED__

#include "typedef.h"

#include <string>
using namespace std;

// �ȉ��̂Q��OR�}�X�N
#define	CHEAT_ENABLE		(1<<0)
#define	CHEAT_KEYDISABLE	(1<<1)

// �������ݎ���
#define	CHEAT_TYPE_ALWAYS	0	// ���ɏ�������
#define	CHEAT_TYPE_ONCE		1	// �P�񂾂���������
#define	CHEAT_TYPE_GREATER	2	// �f�[�^�����傫����
#define	CHEAT_TYPE_LESS		3	// �f�[�^���菬������

// �f�[�^��
#define	CHEAT_LENGTH_1BYTE	0
#define	CHEAT_LENGTH_2BYTE	1
#define	CHEAT_LENGTH_3BYTE	2
#define	CHEAT_LENGTH_4BYTE	3

class	CHEATCODE {
public:
	BYTE	enable;
	BYTE	type;
	BYTE	length;
	WORD	address;
	DWORD	data;

	string	comment;
};

class	GENIECODE {
public:
	bool	enabled;
	BYTE	original;
	
	WORD	address;
	BYTE	data;
	BYTE	cmp;
};

#endif	// !__CEHAT_INCLUDED__

