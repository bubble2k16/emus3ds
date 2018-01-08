//////////////////////////////////////////////////////////////////////////
//                                                                      //
//      Konami VRC7                                                     //
//                                                           Norix      //
//                                               written     2001/09/18 //
//                                               last modify ----/--/-- //
//////////////////////////////////////////////////////////////////////////
#include "APU_VRC7.h"

APU_VRC7::APU_VRC7()
{
    u8 new3DS = false;
    APT_CheckNew3DS(&new3DS);
	int sampleRate = 32000;
	if (!new3DS)
		sampleRate = 20000;
	
	OPLL_init( 3579545, (uint32)sampleRate );	// ���̃T���v�����O���[�g
	VRC7_OPLL = OPLL_new();

	if( VRC7_OPLL ) {
		OPLL_reset( VRC7_OPLL );
		OPLL_reset_patch( VRC7_OPLL, OPLL_VRC7_TONE );
		VRC7_OPLL->masterVolume = 128;
	}

	// ���ݒ�
	Reset( APU_CLOCK, sampleRate );
}

APU_VRC7::~APU_VRC7()
{
	if( VRC7_OPLL ) {
		OPLL_delete( VRC7_OPLL );
		VRC7_OPLL = NULL;
//		OPLL_close();	// �����Ă��ǂ�(���g����)
	}
}

void	APU_VRC7::Reset( FLOAT fClock, INT nRate )
{
	if( VRC7_OPLL ) {
		OPLL_reset( VRC7_OPLL );
		OPLL_reset_patch( VRC7_OPLL, OPLL_VRC7_TONE );
		VRC7_OPLL->masterVolume = 128;
	}

	address = 0;

	Setup( fClock, nRate );
}

void	APU_VRC7::Setup( FLOAT fClock, INT nRate )
{
	OPLL_setClock( (uint32)(fClock*2.0f), (uint32)nRate );
}

void	APU_VRC7::Write( WORD addr, BYTE data )
{
	if( VRC7_OPLL ) {
		if( addr == 0x9010 ) {
			address = data;
		} else if( addr == 0x9030 ) {
			OPLL_writeReg( VRC7_OPLL, address, data );
		}
	}
}

INT	APU_VRC7::Process( INT channel )
{
	if( VRC7_OPLL )
		return	OPLL_calc( VRC7_OPLL );

	return	0;
}

INT	APU_VRC7::GetFreq( INT channel )
{
	if( VRC7_OPLL && channel < 8 ) {
		INT	fno = (((INT)VRC7_OPLL->reg[0x20+channel]&0x01)<<8)
			    + (INT)VRC7_OPLL->reg[0x10+channel];
		INT	blk = (VRC7_OPLL->reg[0x20+channel]>>1) & 0x07;
		float	blkmul[] = { 0.5f, 1.0f, 2.0f, 4.0f, 8.0f, 16.0f, 32.0f, 64.0f };

		if( VRC7_OPLL->reg[0x20+channel] & 0x10 ) {
			return	(INT)((256.0*(double)fno*blkmul[blk]) / ((double)(1<<18)/(3579545.0/72.0)));
		}
	}

	return	0;
}
