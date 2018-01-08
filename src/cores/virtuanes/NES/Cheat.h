#ifndef	__CEHAT_INCLUDED__
#define	__CEHAT_INCLUDED__

#include "typedef.h"

#include <string>
using namespace std;

// ˆÈ‰º‚Ì‚Q‚Â‚ÍORƒ}ƒXƒN
#define	CHEAT_ENABLE		(1<<0)
#define	CHEAT_KEYDISABLE	(1<<1)

// ‘‚«ž‚ÝŽí—Þ
#define	CHEAT_TYPE_ALWAYS	0	// í‚É‘‚«ž‚Ý
#define	CHEAT_TYPE_ONCE		1	// ‚P‰ñ‚¾‚¯‘‚«ž‚Ý
#define	CHEAT_TYPE_GREATER	2	// ƒf[ƒ^‚æ‚è‘å‚«‚¢Žž
#define	CHEAT_TYPE_LESS		3	// ƒf[ƒ^‚æ‚è¬‚³‚¢Žž

// ƒf[ƒ^’·
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

