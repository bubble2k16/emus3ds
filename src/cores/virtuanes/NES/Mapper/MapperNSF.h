//////////////////////////////////////////////////////////////////////////
// MapperNSF  NSF:NES Sound Format                                      //
//////////////////////////////////////////////////////////////////////////
class	MapperNSF : public Mapper
{
public:
	MapperNSF( NES* parent ) : Mapper(parent) {}

	void	Reset();

	BYTE	ExRead ( WORD addr );
	void	ExWrite( WORD addr, BYTE data );

	BYTE	ReadLow( WORD addr );
	void	WriteLow( WORD addr, BYTE data );
	void	Write(WORD addr, BYTE data);

	void	VSync();

protected:
	NSFHEADER	nsfheader;
	BYTE		exchip;
	BYTE		bankswitch;
	INT		banksize;
	BYTE		multipul[2];

	BYTE	exaddr;
	BYTE	exram[128];

	void	BankSwitch( INT no, BYTE bank );

	// Song number
	INT	songno;

	// Key buffer
	BYTE	pad, padold;
	BYTE	repcnt;

	// For keyboard
	INT	GetKeyTable( INT freq );
	void	DrawKey( INT key );

	// For Drawing
	void	DrawRect( INT x, INT y, INT w, INT h, BYTE col );
	void	DrawFont( INT x, INT y, BYTE chr, BYTE col );
	void	DrawString( INT x, INT y, LPSTR str, BYTE col );
	void	DrawWave( INT x, INT y, INT col );

	void	DrawBitmap( INT x, INT y, LPBYTE lpBitmap );

	static	BYTE	NsfPlayerCode[];

	static	BYTE	Font6x8[];

	static	BYTE	KeyBoardBitmap[];
	static	BYTE	KeyBitmapA[];
	static	BYTE	KeyBitmapB[];
	static	BYTE	KeyBitmapC[];
	static	BYTE	KeyBitmapD[];

	static	BYTE	NeonBitmapA[];
	static	BYTE	NeonBitmapB[];
	static	BYTE	NeonBitmapC[];

	static	INT	KeyBitmapOfs[];
	static	LPBYTE	KeyBitmapTbl[];

	static	BYTE	StarBitmapA[];
	static	BYTE	StarBitmapB[];
	static	BYTE	StarBitmapC[];
	static	BYTE	StarBitmapD[];
	static	LPBYTE	StarBitmapTbl[];

	// ‚¨‚Ü‚¯
	enum	{ STAR_MAX = 128 };

	struct	STAR	{
		INT	x, y, z;
	};
	struct	STAR	StarBuf[STAR_MAX];

	void	StarInitial();
	void	Star();

	INT	Freq2KeyTbl[96+1];
private:
};
