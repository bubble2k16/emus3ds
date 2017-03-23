//////////////////////////////////////////////////////////////////////////
//                                                                      //
//      APU Internal                                                    //
//                                                           Norix      //
//                                               written     2002/06/27 //
//                                               last modify ----/--/-- //
//////////////////////////////////////////////////////////////////////////
#include "DebugOut.h"
#include "Pathlib.h"
#include "Config.h"

#include "APU_INTERNAL.h"

#include "state.h"
#include "rom.h"

// Dummy
#define	APU_CLOCK	1789772.5f

// Volume shift
#define	RECTANGLE_VOL_SHIFT	8
#define	TRIANGLE_VOL_SHIFT	9
#define	NOISE_VOL_SHIFT		8
#define	DPCM_VOL_SHIFT		8

INT	APU_INTERNAL::vbl_length[32] = {
    5, 127,   10,   1,   19,   2,   40,   3,
   80,   4,   30,   5,    7,   6,   13,   7,
    6,   8,   12,   9,   24,  10,   48,  11,
   96,  12,   36,  13,    8,  14,   16,  15
};

INT	APU_INTERNAL::freq_limit[8] = {
	0x03FF, 0x0555, 0x0666, 0x071C, 0x0787, 0x07C1, 0x07E0, 0x07F0
};
INT	APU_INTERNAL::duty_lut[4] = {
	 2,  4,  8, 12
};

INT	APU_INTERNAL::noise_freq[16] = {
	  4,    8,   16,   32,   64,   96,  128,  160,
	202,  254,  380,  508,  762, 1016, 2034, 4068
};

// DMC �]���N���b�N���e�[�u��
INT	APU_INTERNAL::dpcm_cycles[16] = {
	428, 380, 340, 320, 286, 254, 226, 214,
	190, 160, 142, 128, 106,  85,  72,  54
};

INT	APU_INTERNAL::dpcm_cycles_pal[16] = {
	397, 353, 315, 297, 265, 235, 209, 198,
	176, 148, 131, 118,  98,  78,  66,  50
};


//INT	APU_INTERNAL::vol_effect[16] = {
//	100,  94,  88,  83,  78,  74,  71,  67,
//	 64,  61,  59,  56,  54,  52,  50,  48
//};

APU_INTERNAL::APU_INTERNAL()
{
	nes = NULL;

	ZEROMEMORY( &ch0, sizeof(ch0) );
	ZEROMEMORY( &ch1, sizeof(ch1) );
	ZEROMEMORY( &ch2, sizeof(ch2) );
	ZEROMEMORY( &ch3, sizeof(ch3) );
	ZEROMEMORY( &ch4, sizeof(ch4) );

	FrameIRQ = 0xC0;
	FrameCycle = 0;
	FrameIRQoccur = 0;
	FrameCount = 0;
	FrameType  = 0;

	reg4015 = sync_reg4015 = 0;

	cpu_clock = APU_CLOCK;
	sampling_rate = 22050;

	// ���ݒ�
	cycle_rate = (INT)(cpu_clock*65536.0f/22050.0f);

}

APU_INTERNAL::~APU_INTERNAL()
{
}

void	APU_INTERNAL::Reset( FLOAT fClock, INT nRate )
{
	ZEROMEMORY( &ch0, sizeof(ch0) );
	ZEROMEMORY( &ch1, sizeof(ch1) );
	ZEROMEMORY( &ch2, sizeof(ch2) );
	ZEROMEMORY( &ch3, sizeof(ch3) );
//	ZEROMEMORY( &ch4, sizeof(ch4) );

	ZEROMEMORY( bToneTableEnable, sizeof(bToneTableEnable) );
	ZEROMEMORY( ToneTable, sizeof(ToneTable) );
	ZEROMEMORY( ChannelTone, sizeof(ChannelTone) );

	reg4015 = sync_reg4015 = 0;

	// Sweep complement
	ch0.complement = 0x00;
	ch1.complement = 0xFF;

	// Noise shift register
	ch3.shift_reg = 0x4000;

	Setup( fClock, nRate );

	// $4011�͏��������Ȃ�
	WORD	addr;
	for( addr = 0x4000; addr <= 0x4010; addr++ ) {
		Write( addr, 0x00 );
		SyncWrite( addr, 0x00 );
	}
//	Write( 0x4001, 0x08 );	// Reset����inc���[�h�ɂȂ�?
//	Write( 0x4005, 0x08 );	// Reset����inc���[�h�ɂȂ�?
	Write( 0x4012, 0x00 );
	Write( 0x4013, 0x00 );
	Write( 0x4015, 0x00 );
	SyncWrite( 0x4012, 0x00 );
	SyncWrite( 0x4013, 0x00 );
	SyncWrite( 0x4015, 0x00 );

	// $4017�͏������݂ŏ��������Ȃ�(�������[�h��0�ł����̂����҂����\�t�g��������)
	FrameIRQ = 0xC0;
	FrameCycle = 0;
	FrameIRQoccur = 0;
	FrameCount = 0;
	FrameType  = 0;

	// ToneLoad
	ToneTableLoad();
}

void	APU_INTERNAL::Setup( FLOAT fClock, INT nRate )
{
	cpu_clock = fClock;
	sampling_rate = nRate;

	cycle_rate = (INT)(fClock*65536.0f/(float)nRate);
}

//
// Wavetable loader
//
void	APU_INTERNAL::ToneTableLoad()
{
FILE*	fp = NULL;
CHAR	buf[512];

	string	tempstr;
	tempstr = CPathlib::MakePathExt( nes->rom->GetRomPath(), nes->rom->GetRomName(), "vtd" );
	DEBUGOUT( "Path: %s\n", tempstr.c_str() );
	if( !(fp = ::fopen( tempstr.c_str(), "r" )) ) {
		// �f�t�H���g�t�@�C�����œǂ��Ō���
		tempstr = CPathlib::MakePathExt( nes->rom->GetRomPath(), "Default", "vtd" );
		DEBUGOUT( "Path: %s\n", tempstr.c_str() );
		if( !(fp = ::fopen( tempstr.c_str(), "r" )) ) {
			DEBUGOUT( "File not found.\n" );
			return;
		}
	}

	DEBUGOUT( "Find.\n" );

	// ���`�t�@�C�����ǂݍ���
	while( ::fgets( buf, 512, fp ) != NULL ) {
		if( buf[0] == ';' || ::strlen(buf) <= 0 )
			continue;

		CHAR	c = ::toupper( buf[0] );

		if( c == '@' ) {
		// ���F�ǂݍ���
		CHAR*	pbuf = &buf[1];
		CHAR*	p;
		INT	no, val;

			// ���F�i���o�[�擾
			no = ::strtol( pbuf, &p, 10 );
			if( pbuf == p )
				continue;
			if( no < 0 || no > TONEDATA_MAX-1 )
				continue;

			// '='��������
			p = ::strchr( pbuf, '=' );
			if( p == NULL )
				continue;
			pbuf = p+1;	// ��

			// ���F�f�[�^���擾
			int i;
			for( i = 0; i < TONEDATA_LEN; i++ ) {
				val = ::strtol( pbuf, &p, 10 );
				if( pbuf == p )	// �擾���s�H
					break;
				if( *p == ',' )	// �J���}�����΂��c
					pbuf = p+1;
				else
					pbuf = p;

				ToneTable[no][i] = val;
			}
			if( i >= TONEDATA_MAX )
				bToneTableEnable[no] = TRUE;
		} else
		if( c == 'A' || c == 'B' ) {
		// �e�`�����l�����F���`
		CHAR*	pbuf = &buf[1];
		CHAR*	p;
		INT	no, val;

			// �������F�i���o�[�擾
			no = ::strtol( pbuf, &p, 10 );
			if( pbuf == p )
				continue;
			pbuf = p;
			if( no < 0 || no > TONE_MAX-1 )
				continue;

			// '='��������
			p = ::strchr( pbuf, '=' );
			if( p == NULL )
				continue;
			pbuf = p+1;	// ��

			// ���F�i���o�[�擾
			val = ::strtol( pbuf, &p, 10 );
			if( pbuf == p )
				continue;
			pbuf = p;

			if( val > TONEDATA_MAX-1 )
				continue;

			if( val >= 0 && bToneTableEnable[val] ) {
				if( c == 'A' ) {
					ChannelTone[0][no] = val+1;
				} else {
					ChannelTone[1][no] = val+1;
				}
			} else {
				if( c == 'A' ) {
					ChannelTone[0][no] = 0;
				} else {
					ChannelTone[1][no] = 0;
				}
			}
		} else
		if( c == 'C' ) {
		// �e�`�����l�����F���`
		CHAR*	pbuf = &buf[1];
		CHAR*	p;
		INT	val;

			// '='��������
			p = ::strchr( pbuf, '=' );
			if( p == NULL )
				continue;
			pbuf = p+1;	// ��

			// ���F�i���o�[�擾
			val = ::strtol( pbuf, &p, 10 );
			if( pbuf == p )
				continue;
			pbuf = p;

			if( val > TONEDATA_MAX-1 )
				continue;

			if( val >= 0 && bToneTableEnable[val] ) {
				ChannelTone[2][0] = val+1;
			} else {
				ChannelTone[2][0] = 0;
			}
		}
	}

	FCLOSE( fp );
}

INT	APU_INTERNAL::Process( INT channel )
{
	switch( channel ) {
		case	0:
			return	RenderRectangle( ch0 );
		case	1:
			return	RenderRectangle( ch1 );
		case	2:
			return	RenderTriangle();
		case	3:
			return	RenderNoise();
		case	4:
			return	RenderDPCM();
		default:
			return	0;
	}

	return	0;
}

void	APU_INTERNAL::Write( WORD addr, BYTE data )
{
	switch( addr ) {
		// CH0,1 rectangle
		case	0x4000:	case	0x4001:
		case	0x4002:	case	0x4003:
		case	0x4004:	case	0x4005:
		case	0x4006:	case	0x4007:
			WriteRectangle( (addr<0x4004)?0:1, addr, data );
			break;

		// CH2 triangle
		case	0x4008:	case	0x4009:
		case	0x400A:	case	0x400B:
			WriteTriangle( addr, data );
			break;

		// CH3 noise
		case	0x400C:	case	0x400D:
		case	0x400E:	case	0x400F:
			WriteNoise( addr, data );
			break;

		// CH4 DPCM
		case	0x4010:	case	0x4011:
		case	0x4012:	case	0x4013:
			WriteDPCM( addr, data );
			break;

		case	0x4015:
			reg4015 = data;

			if( !(data&(1<<0)) ) {
				ch0.enable    = 0;
				ch0.len_count = 0;
			}
			if( !(data&(1<<1)) ) {
				ch1.enable    = 0;
				ch1.len_count = 0;
			}
			if( !(data&(1<<2)) ) {
				ch2.enable        = 0;
				ch2.len_count     = 0;
				ch2.lin_count     = 0;
				ch2.counter_start = 0;
			}
			if( !(data&(1<<3)) ) {
				ch3.enable    = 0;
				ch3.len_count = 0;
			}
			if( !(data&(1<<4)) ) {
				ch4.enable    = 0;
				ch4.dmalength = 0;
			} else {
				ch4.enable = 0xFF;
				if( !ch4.dmalength ) {
					ch4.address   = ch4.cache_addr;
					ch4.dmalength = ch4.cache_dmalength;
					ch4.phaseacc  = 0;
				}
			}
			break;

		case	0x4017:
			break;

		// VirtuaNES�ŗL�|�[�g
		case	0x4018:
			UpdateRectangle( ch0, (INT)data );
			UpdateRectangle( ch1, (INT)data );
			UpdateTriangle ( (INT)data );
			UpdateNoise    ( (INT)data );
			break;

		default:
			break;
	}
}

BYTE	APU_INTERNAL::Read( WORD addr )
{
BYTE	data = addr>>8;

	if( addr == 0x4015 ) {
		data = 0;
		if( ch0.enable && ch0.len_count > 0 ) data |= (1<<0);
		if( ch1.enable && ch1.len_count > 0 ) data |= (1<<1);
		if( ch2.enable && ch2.len_count > 0 ) data |= (1<<2);
		if( ch3.enable && ch3.len_count > 0 ) data |= (1<<3);
	}
	return	data;
}

void	APU_INTERNAL::SyncWrite( WORD addr, BYTE data )
{
//DEBUGOUT( "$%04X=$%02X\n", addr, data );

	switch( addr ) {
		// CH0,1 rectangle
		case	0x4000:	case	0x4001:
		case	0x4002:	case	0x4003:
		case	0x4004:	case	0x4005:
		case	0x4006:	case	0x4007:
			SyncWriteRectangle( (addr<0x4004)?0:1, addr, data );
			break;

		// CH2 triangle
		case	0x4008:	case	0x4009:
		case	0x400A:	case	0x400B:
			SyncWriteTriangle( addr, data );
			break;

		// CH3 noise
		case	0x400C:	case	0x400D:
		case	0x400E:	case	0x400F:
			SyncWriteNoise( addr, data );
			break;

		// CH4 DPCM
		case	0x4010:	case	0x4011:
		case	0x4012:	case	0x4013:
			SyncWriteDPCM( addr, data );
			break;

		case	0x4015:
			sync_reg4015 = data;

			if( !(data&(1<<0)) ) {
				ch0.sync_enable    = 0;
				ch0.sync_len_count = 0;
			}
			if( !(data&(1<<1)) ) {
				ch1.sync_enable    = 0;
				ch1.sync_len_count = 0;
			}
			if( !(data&(1<<2)) ) {
				ch2.sync_enable        = 0;
				ch2.sync_len_count     = 0;
				ch2.sync_lin_count     = 0;
				ch2.sync_counter_start = 0;
			}
			if( !(data&(1<<3)) ) {
				ch3.sync_enable    = 0;
				ch3.sync_len_count = 0;
			}
			if( !(data&(1<<4)) ) {
				ch4.sync_enable     = 0;
				ch4.sync_dmalength  = 0;
				ch4.sync_irq_enable = 0;

				nes->cpu->ClrIRQ( IRQ_DPCM );
			} else {
				ch4.sync_enable = 0xFF;
				if( !ch4.sync_dmalength ) {
//					ch4.sync_cycles    = ch4.sync_cache_cycles;
					ch4.sync_dmalength = ch4.sync_cache_dmalength;
					ch4.sync_cycles    = 0;
				}
			}
			break;

		case	0x4017:
			SyncWrite4017( data );
			break;

		// VirtuaNES�ŗL�|�[�g
		case	0x4018:
			SyncUpdateRectangle( ch0, (INT)data );
			SyncUpdateRectangle( ch1, (INT)data );
			SyncUpdateTriangle ( (INT)data );
			SyncUpdateNoise    ( (INT)data );
			break;

		default:
			break;
	}
}

// $4017 Write
void	APU_INTERNAL::SyncWrite4017( BYTE data )
{
	FrameCycle = 0;
	FrameIRQ = data;
	FrameIRQoccur = 0;

	nes->cpu->ClrIRQ( IRQ_FRAMEIRQ );

	FrameType = (data & 0x80) ? 1 : 0;
	FrameCount = 0;
	if( data & 0x80 ) {
		UpdateFrame();
	}
	FrameCount = 1;
	FrameCycle = 14915;
}

void	APU_INTERNAL::UpdateFrame()
{
	if( !FrameCount ) {
		if( !(FrameIRQ&0xC0) && nes->GetFrameIRQmode() ) {
			FrameIRQoccur = 0xFF;
			nes->cpu->SetIRQ( IRQ_FRAMEIRQ );
		}
	}

	if( FrameCount == 3 ) {
		if( FrameIRQ & 0x80 ) {
			FrameCycle += 14915;
		}
	}

	// Counters Update
	nes->Write( 0x4018, (BYTE)FrameCount );

	FrameCount = (FrameCount + 1) & 3;
}

BYTE	APU_INTERNAL::SyncRead( WORD addr )
{
BYTE	data = addr>>8;

	if( addr == 0x4015 ) {
		data = 0;
		if( ch0.sync_enable && ch0.sync_len_count > 0 ) data |= (1<<0);
		if( ch1.sync_enable && ch1.sync_len_count > 0 ) data |= (1<<1);
		if( ch2.sync_enable && ch2.sync_len_count > 0 ) data |= (1<<2);
		if( ch3.sync_enable && ch3.sync_len_count > 0 ) data |= (1<<3);
		if( ch4.sync_enable && ch4.sync_dmalength )     data |= (1<<4);
		if( FrameIRQoccur )                             data |= (1<<6);
		if( ch4.sync_irq_enable )                       data |= (1<<7);
		FrameIRQoccur = 0;

		nes->cpu->ClrIRQ( IRQ_FRAMEIRQ );
//DEBUGOUT( "R 4015 %02X\n", data );
	}
	if( addr == 0x4017 ) {
		if( FrameIRQoccur ) {
			data = 0;
		} else {
			data |= (1<<6);
		}
//DEBUGOUT( "R 4017 %02X\n", data );
	}
	return	data;
}

BOOL	APU_INTERNAL::Sync( INT cycles )
{
	FrameCycle -= cycles * 2;
	if( FrameCycle <= 0 ) {
		FrameCycle += 14915;

		UpdateFrame();
	}

	return	FrameIRQoccur | SyncUpdateDPCM( cycles );
}

INT	APU_INTERNAL::GetFreq( INT channel )
{
INT	freq = 0;

	// Rectangle
	if( channel == 0 || channel == 1 ) {
		RECTANGLE* ch;
		if( channel == 0 ) ch = &ch0;
		else		   ch = &ch1;
		if( !ch->enable || ch->len_count <= 0 )
			return	0;
		if( (ch->freq < 8) || (!ch->swp_inc && ch->freq > ch->freqlimit) )
			return	0;

		if( !ch->volume )
			return	0;

//		freq = (((INT)ch->reg[3]&0x07)<<8)+(INT)ch->reg[2]+1;
		freq = (INT)(16.0f*cpu_clock/(FLOAT)(ch->freq+1));
		return	freq;
	}

	// Triangle
	if( channel == 2 ) {
		if( !ch2.enable || ch2.len_count <= 0 )
			return	0;
		if( ch2.lin_count <= 0 || ch2.freq < INT2FIX(8) )
			return	0;
		freq = (((INT)ch2.reg[3]&0x07)<<8)+(INT)ch2.reg[2]+1;
		freq = (INT)(8.0f*cpu_clock/(FLOAT)freq);
		return	freq;
	}

	// Noise
	if( channel == 3 ) {
		if( !ch3.enable || ch3.len_count <= 0 )
			return	0;
		if( ch3.env_fixed ) {
			if( !ch3.volume )
				return	0;
		} else {
			if( !ch3.env_vol )
				return	0;
		}
		return	1;
	}

	// DPCM
	if( channel == 4 ) {
		if( ch4.enable && ch4.dmalength )
			return	1;
	}

	return	0;
}

// Write Rectangle
void	APU_INTERNAL::WriteRectangle( INT no, WORD addr, BYTE data )
{
	RECTANGLE& ch = (no==0)?ch0:ch1;

	ch.reg[addr&3] = data;
	switch( addr&3 ) {
		case	0:
			ch.holdnote  = data&0x20;
			ch.volume    = data&0x0F;
			ch.env_fixed = data&0x10;
			ch.env_decay = (data&0x0F)+1;
			ch.duty      = duty_lut[data>>6];
			break;
		case	1:
			ch.swp_on    = data&0x80;
			ch.swp_inc   = data&0x08;
			ch.swp_shift = data&0x07;
			ch.swp_decay = ((data>>4)&0x07)+1;
			ch.freqlimit = freq_limit[data&0x07];
			break;
		case	2:
			ch.freq = (ch.freq&(~0xFF))+data;
			break;
		case	3: // Master
			ch.freq      = ((data&0x07)<<8)+(ch.freq&0xFF);
			ch.len_count = vbl_length[data>>3]*2;
			ch.env_vol   = 0x0F;
			ch.env_count = ch.env_decay+1;
			ch.adder     = 0;

			if( reg4015&(1<<no) )
				ch.enable    = 0xFF;
			break;
	}
}

// Update Rectangle
void	APU_INTERNAL::UpdateRectangle( RECTANGLE& ch, INT type )
{
	if( !ch.enable || ch.len_count <= 0 )
		return;

	// Update Length/Sweep
	if( !(type & 1) ) {
		// Update Length
		if( ch.len_count && !ch.holdnote ) {
			// Holdnote
			if( ch.len_count ) {
				ch.len_count--;
			}
		}

		// Update Sweep
		if( ch.swp_on && ch.swp_shift ) {
			if( ch.swp_count ) {
				ch.swp_count--;
			}
			if( ch.swp_count == 0 ) {
				ch.swp_count = ch.swp_decay;
				if( ch.swp_inc ) {
				// Sweep increment(to higher frequency)
					if( !ch.complement )
						ch.freq += ~(ch.freq >> ch.swp_shift); // CH 0
					else
						ch.freq -=  (ch.freq >> ch.swp_shift); // CH 1
				} else {
				// Sweep decrement(to lower frequency)
					ch.freq += (ch.freq >> ch.swp_shift);
				}
			}
		}
	}

	// Update Envelope
	if( ch.env_count ) {
		ch.env_count--;
	}
	if( ch.env_count == 0 ) {
		ch.env_count = ch.env_decay;

		// Holdnote
		if( ch.holdnote ) {
			ch.env_vol = (ch.env_vol-1)&0x0F;
		} else if( ch.env_vol ) {
			ch.env_vol--;
		}
	}

	if( !ch.env_fixed ) {
		ch.nowvolume = ch.env_vol<<RECTANGLE_VOL_SHIFT;
	}
}

// Sync Write Rectangle
void	APU_INTERNAL::SyncWriteRectangle( INT no, WORD addr, BYTE data )
{
	RECTANGLE& ch = (no==0)?ch0:ch1;

	ch.sync_reg[addr&3] = data;
	switch( addr&3 ) {
		case	0:
			ch.sync_holdnote = data&0x20;
			break;
		case	1:
		case	2:
			break;
		case	3: // Master
			ch.sync_len_count = vbl_length[data>>3]*2;
			if( sync_reg4015&(1<<no) )
				ch.sync_enable = 0xFF;
			break;
	}
}

// Sync Update Rectangle
void	APU_INTERNAL::SyncUpdateRectangle( RECTANGLE& ch, INT type )
{
	if( !ch.sync_enable || ch.sync_len_count <= 0 )
		return;

	// Update Length
	if( ch.sync_len_count && !ch.sync_holdnote ) {
		if( !(type & 1) && ch.sync_len_count ) {
			ch.sync_len_count--;
		}
	}
}

// Render Rectangle
INT	APU_INTERNAL::RenderRectangle( RECTANGLE& ch )
{
	if( !ch.enable || ch.len_count <= 0 )
		return	0;

	// Channel disable?
	if( (ch.freq < 8) || (!ch.swp_inc && ch.freq > ch.freqlimit) ) {
		return	0;
	}

	if( ch.env_fixed ) {
		ch.nowvolume = ch.volume<<RECTANGLE_VOL_SHIFT;
	}
	INT	volume = ch.nowvolume;

	if( !(Config.sound.bChangeTone && ChannelTone[(!ch.complement)?0:1][ch.reg[0]>>6]) ) {
		// ���ԏ���
		double	total;
		double	sample_weight = ch.phaseacc;
		if( sample_weight > cycle_rate ) {
			sample_weight = cycle_rate;
		}
		total = (ch.adder < ch.duty)?sample_weight:-sample_weight;

		INT	freq = INT2FIX( ch.freq+1 );
		ch.phaseacc -= cycle_rate;
		while( ch.phaseacc < 0 ) {
			ch.phaseacc += freq;
			ch.adder = (ch.adder+1)&0x0F;

			sample_weight = freq;
			if( ch.phaseacc > 0 ) {
				sample_weight -= ch.phaseacc;
			}
			total += (ch.adder < ch.duty)?sample_weight:-sample_weight;
		}
		return	(INT)floor( volume*total/cycle_rate + 0.5 );
	} else {
		INT*	pTone = ToneTable[ChannelTone[(!ch.complement)?0:1][ch.reg[0]>>6]-1];

		// �X�V����
		ch.phaseacc -= cycle_rate*2;
		if( ch.phaseacc >= 0 ) {
			return	pTone[ch.adder&0x1F]*volume/((1<<RECTANGLE_VOL_SHIFT)/2);
		}

		// 1�X�e�b�v�����X�V
		INT	freq = INT2FIX( ch.freq+1 );
		if( freq > cycle_rate*2 ) {
			ch.phaseacc += freq;
			ch.adder = (ch.adder+1)&0x1F;
			return	pTone[ch.adder&0x1F]*volume/((1<<RECTANGLE_VOL_SHIFT)/2);
		}

		// ���d����
		INT	num_times, total;
		num_times = total = 0;
		while( ch.phaseacc < 0 ) {
			ch.phaseacc += freq;
			ch.adder = (ch.adder+1)&0x1F;
			total += pTone[ch.adder&0x1F]*volume/((1<<RECTANGLE_VOL_SHIFT)/2);
			num_times++;
		}
		return	total/num_times;
	}
}

/////////////

// Write Triangle
void	APU_INTERNAL::WriteTriangle( WORD addr, BYTE data )
{
	ch2.reg[addr&3] = data;
	switch( addr&3 ) {
		case	0:
			ch2.holdnote = data&0x80;
			break;
		case	1: // Unused
			break;
		case	2:
			ch2.freq = INT2FIX( ((((INT)ch2.reg[3]&0x07)<<8)+(INT)data+1) );
			break;
		case	3: // Master
			ch2.freq      = INT2FIX( ((((INT)data&0x07)<<8)+(INT)ch2.reg[2]+1) );
			ch2.len_count = vbl_length[data>>3]*2;
			ch2.counter_start = 0x80;

			if( reg4015&(1<<2) )
				ch2.enable = 0xFF;
			break;
	}
}

// Update Triangle
void	APU_INTERNAL::UpdateTriangle( INT type )
{
	if( !ch2.enable )
		return;

	if( !(type & 1) && !ch2.holdnote ) {
		if( ch2.len_count ) {
			ch2.len_count--;
		}
	}

//	if( !ch2.len_count ) {
//		ch2.lin_count = 0;
//	}

	// Update Length/Linear
	if( ch2.counter_start ) {
		ch2.lin_count = ch2.reg[0] & 0x7F;
	} else if( ch2.lin_count ) {
		ch2.lin_count--;
	}
	if( !ch2.holdnote && ch2.lin_count ) {
		ch2.counter_start = 0;
	}
}

// Sync Write Triangle
void	APU_INTERNAL::SyncWriteTriangle( WORD addr, BYTE data )
{
	ch2.sync_reg[addr&3] = data;
	switch( addr&3 ) {
		case	0:
			ch2.sync_holdnote = data&0x80;
			break;
		case	1:
			break;
		case	2:
			break;
		case	3: // Master
			ch2.sync_len_count = vbl_length[ch2.sync_reg[3]>>3]*2;
			ch2.sync_counter_start = 0x80;

			if( sync_reg4015&(1<<2) )
				ch2.sync_enable = 0xFF;
			break;
	}
}

// Sync Update Triangle
void	APU_INTERNAL::SyncUpdateTriangle( INT type )
{
	if( !ch2.sync_enable )
		return;

	if( !(type & 1) && !ch2.sync_holdnote ) {
		if( ch2.sync_len_count ) {
			ch2.sync_len_count--;
		}
	}

//	if( !ch2.sync_len_count ) {
//		ch2.sync_lin_count = 0;
//	}

	// Update Length/Linear
	if( ch2.sync_counter_start ) {
		ch2.sync_lin_count = ch2.sync_reg[0] & 0x7F;
	} else if( ch2.sync_lin_count ) {
		ch2.sync_lin_count--;
	}
	if( !ch2.sync_holdnote && ch2.sync_lin_count ) {
		ch2.sync_counter_start = 0;
	}
}

// Render Triangle
INT	APU_INTERNAL::RenderTriangle()
{
	INT	vol;
	if( Config.sound.bDisableVolumeEffect ) {
		vol = 256;
	} else {
		vol = 256-(INT)((ch4.reg[1]&0x01)+ch4.dpcm_value*2);
	}

	if( !ch2.enable || (ch2.len_count <= 0) || (ch2.lin_count <= 0) ) {
		return	ch2.nowvolume*vol/256;
	}

	if( ch2.freq < INT2FIX(8) ) {
		return	ch2.nowvolume*vol/256;
	}

	if( !(Config.sound.bChangeTone && ChannelTone[2][0]) ) {
		ch2.phaseacc -= cycle_rate;
		if( ch2.phaseacc >= 0 ) {
			return	ch2.nowvolume*vol/256;
		}

		if( ch2.freq > cycle_rate ) {
			ch2.phaseacc += ch2.freq;
			ch2.adder = (ch2.adder+1)&0x1F;

			if( ch2.adder < 0x10 ) {
				ch2.nowvolume = (ch2.adder&0x0F)<<TRIANGLE_VOL_SHIFT;
			} else {
				ch2.nowvolume = (0x0F-(ch2.adder&0x0F))<<TRIANGLE_VOL_SHIFT;
			}

			return	ch2.nowvolume*vol/256;
		}

		// ���d����
		INT	num_times, total;
		num_times = total = 0;
		while( ch2.phaseacc < 0 ) {
			ch2.phaseacc += ch2.freq;
			ch2.adder = (ch2.adder+1)&0x1F;

			if( ch2.adder < 0x10 ) {
				ch2.nowvolume = (ch2.adder&0x0F)<<TRIANGLE_VOL_SHIFT;
			} else {
				ch2.nowvolume = (0x0F-(ch2.adder&0x0F))<<TRIANGLE_VOL_SHIFT;
			}

			total += ch2.nowvolume;
			num_times++;
		}

		return	(total/num_times)*vol/256;
	} else {
		INT*	pTone = ToneTable[ChannelTone[2][0]-1];

		ch2.phaseacc -= cycle_rate;
		if( ch2.phaseacc >= 0 ) {
			return	ch2.nowvolume*vol/256;
		}

		if( ch2.freq > cycle_rate ) {
			ch2.phaseacc += ch2.freq;
			ch2.adder = (ch2.adder+1)&0x1F;
			ch2.nowvolume = pTone[ch2.adder&0x1F]*0x0F;
			return	ch2.nowvolume*vol/256;
		}

		// ���d����
		INT	num_times, total;
		num_times = total = 0;
		while( ch2.phaseacc < 0 ) {
			ch2.phaseacc += ch2.freq;
			ch2.adder = (ch2.adder+1)&0x1F;
			total += pTone[ch2.adder&0x1F]*0x0F;
			num_times++;
		}

		return	(total/num_times)*vol/256;
	}
}

//////////////

// Write Noise
void	APU_INTERNAL::WriteNoise( WORD addr, BYTE data )
{
	ch3.reg[addr&3] = data;
	switch( addr&3 ) {
		case	0:
			ch3.holdnote  = data&0x20;
			ch3.volume    = data&0x0F;
			ch3.env_fixed = data&0x10;
			ch3.env_decay = (data&0x0F)+1;
			break;
		case	1: // Unused
			break;
		case	2:
			ch3.freq    = INT2FIX(noise_freq[data&0x0F]);
			ch3.xor_tap = (data&0x80)?0x40:0x02;
			break;
		case	3: // Master
			ch3.len_count = vbl_length[data>>3]*2;
			ch3.env_vol   = 0x0F;
			ch3.env_count = ch3.env_decay+1;

			if( reg4015&(1<<3) )
				ch3.enable    = 0xFF;
			break;
	}
}

// Update Noise
void	APU_INTERNAL::UpdateNoise( INT type )
{
	if( !ch3.enable || ch3.len_count <= 0 )
		return;

	// Update Length
	if( !ch3.holdnote ) {
		// Holdnote
		if( !(type & 1) && ch3.len_count ) {
			ch3.len_count--;
		}
	}

	// Update Envelope
	if( ch3.env_count ) {
		ch3.env_count--;
	}
	if( ch3.env_count == 0 ) {
		ch3.env_count = ch3.env_decay;

		// Holdnote
		if( ch3.holdnote ) {
			ch3.env_vol = (ch3.env_vol-1)&0x0F;
		} else if( ch3.env_vol ) {
			ch3.env_vol--;
		}
	}

	if( !ch3.env_fixed ) {
		ch3.nowvolume = ch3.env_vol<<RECTANGLE_VOL_SHIFT;
	}
}

// Sync Write Noise
void	APU_INTERNAL::SyncWriteNoise( WORD addr, BYTE data )
{
	ch3.sync_reg[addr&3] = data;
	switch( addr&3 ) {
		case	0:
			ch3.sync_holdnote = data&0x20;
			break;
		case	1:
			break;
		case	2:
			break;
		case	3: // Master
			ch3.sync_len_count = vbl_length[data>>3]*2;
			if( sync_reg4015&(1<<3) )
				ch3.sync_enable = 0xFF;
			break;
	}
}

// Sync Update Noise
void	APU_INTERNAL::SyncUpdateNoise( INT type )
{
	if( !ch3.sync_enable || ch3.sync_len_count <= 0 )
		return;

	// Update Length
	if( ch3.sync_len_count && !ch3.sync_holdnote ) {
		if( !(type & 1) && ch3.sync_len_count ) {
			ch3.sync_len_count--;
		}
	}
}

// Noise ShiftRegister
BYTE	APU_INTERNAL::NoiseShiftreg( BYTE xor_tap )
{
int	bit0, bit14;

	bit0 = ch3.shift_reg & 1;
	if( ch3.shift_reg & xor_tap ) bit14 = bit0^1;
	else			      bit14 = bit0^0;
	ch3.shift_reg >>= 1;
	ch3.shift_reg |= (bit14<<14);
	return	(bit0^1);
}

// Render Noise
INT	APU_INTERNAL::RenderNoise()
{
	if( !ch3.enable || ch3.len_count <= 0 )
		return	0;

	if( ch3.env_fixed ) {
		ch3.nowvolume = ch3.volume<<RECTANGLE_VOL_SHIFT;
	}

	INT	vol = 256-(INT)((ch4.reg[1]&0x01)+ch4.dpcm_value*2);

	ch3.phaseacc -= cycle_rate;
	if( ch3.phaseacc >= 0 )
		return	ch3.output*vol/256;

	if( ch3.freq > cycle_rate ) {
		ch3.phaseacc += ch3.freq;
		if( NoiseShiftreg(ch3.xor_tap) )
			ch3.output = ch3.nowvolume;
		else
			ch3.output = -ch3.nowvolume;

		return	ch3.output*vol/256;
	}

	INT	num_times, total;
	num_times = total = 0;
	while( ch3.phaseacc < 0 ) {
		ch3.phaseacc += ch3.freq;
		if( NoiseShiftreg(ch3.xor_tap) )
			ch3.output = ch3.nowvolume;
		else
			ch3.output = -ch3.nowvolume;

		total += ch3.output;
		num_times++;
	}

	return	(total/num_times)*vol/256;
}

//////////

void	APU_INTERNAL::WriteDPCM( WORD addr, BYTE data )
{
	ch4.reg[addr&3] = data;
	switch( addr&3 ) {
		case	0:
			ch4.freq    = INT2FIX( nes->GetVideoMode()?dpcm_cycles_pal[data&0x0F]:dpcm_cycles[data&0x0F] );
//			ch4.freq    = INT2FIX( dpcm_cycles[data&0x0F] );
////			ch4.freq    = INT2FIX( (dpcm_cycles[data&0x0F]-((data&0x0F)^0x0F)*2-2) );
			ch4.looping = data&0x40;
			break;
		case	1:
			ch4.dpcm_value = (data&0x7F)>>1;
			break;
		case	2:
			ch4.cache_addr = 0xC000+(WORD)(data<<6);
			break;
		case	3:
			ch4.cache_dmalength = ((data<<4)+1)<<3;
			break;
	}
}

INT	APU_INTERNAL::RenderDPCM()
{
	if( ch4.dmalength ) {
		ch4.phaseacc -= cycle_rate;

		while( ch4.phaseacc < 0 ) {
			ch4.phaseacc += ch4.freq;
			if( !(ch4.dmalength&7) ) {
				ch4.cur_byte = nes->Read( ch4.address );
				if( 0xFFFF == ch4.address )
					ch4.address = 0x8000;
				else
					ch4.address++;
			}

			if( !(--ch4.dmalength) ) {
				if( ch4.looping ) {
					ch4.address = ch4.cache_addr;
					ch4.dmalength = ch4.cache_dmalength;
				} else {
					ch4.enable = 0;
					break;
				}
			}
			// positive delta
			if( ch4.cur_byte&(1<<((ch4.dmalength&7)^7)) ) {
				if( ch4.dpcm_value < 0x3F )
					ch4.dpcm_value += 1;
			} else {
			// negative delta
				if( ch4.dpcm_value > 1 )
					ch4.dpcm_value -= 1;
			}
		}
	}

#if	1
	// �C���`�L�L���v�`�m�C�Y�J�b�g(TEST)
	ch4.dpcm_output_real = (INT)((ch4.reg[1]&0x01)+ch4.dpcm_value*2)-0x40;
	if( abs(ch4.dpcm_output_real-ch4.dpcm_output_fake) <= 8 ) {
		ch4.dpcm_output_fake = ch4.dpcm_output_real;
		ch4.output = (INT)ch4.dpcm_output_real<<DPCM_VOL_SHIFT;
	} else {
		if( ch4.dpcm_output_real > ch4.dpcm_output_fake )
			ch4.dpcm_output_fake += 8;
		else
			ch4.dpcm_output_fake -= 8;
		ch4.output = (INT)ch4.dpcm_output_fake<<DPCM_VOL_SHIFT;
	}
#else
	ch4.output = (((INT)ch4.reg[1]&0x01)+(INT)ch4.dpcm_value*2)<<DPCM_VOL_SHIFT;
//	ch4.output = ((((INT)ch4.reg[1]&0x01)+(INT)ch4.dpcm_value*2)-0x40)<<DPCM_VOL_SHIFT;
#endif
	return	ch4.output;
}

void	APU_INTERNAL::SyncWriteDPCM( WORD addr, BYTE data )
{
	ch4.reg[addr&3] = data;
	switch( addr&3 ) {
		case	0:
////			ch4.sync_cache_cycles = dpcm_cycles[data&0x0F] * 8 - ((data&0x0F)^0x0F)*16 - 0x10;
////			ch4.sync_cycles       = 0;
//			ch4.sync_cache_cycles = dpcm_cycles[data&0x0F] * 8;
			ch4.sync_cache_cycles = nes->GetVideoMode()?dpcm_cycles_pal[data&0x0F]*8:dpcm_cycles[data&0x0F]*8;
			ch4.sync_looping      = data&0x40;
			ch4.sync_irq_gen      = data&0x80;
			if( !ch4.sync_irq_gen ) {
				ch4.sync_irq_enable = 0;
				nes->cpu->ClrIRQ( IRQ_DPCM );
			}
			break;
		case	1:
			break;
		case	2:
			break;
		case	3:
			ch4.sync_cache_dmalength = (data<<4)+1;
			break;
	}
}

BOOL	APU_INTERNAL::SyncUpdateDPCM( INT cycles )
{
BOOL	bIRQ = FALSE;

	if( ch4.sync_enable ) {
		ch4.sync_cycles -= cycles;
		while( ch4.sync_cycles < 0 ) {
			ch4.sync_cycles += ch4.sync_cache_cycles;
			if( ch4.sync_dmalength ) {
//				if( !(--ch4.sync_dmalength) ) {
				if( --ch4.sync_dmalength < 2 ) {
					if( ch4.sync_looping ) {
						ch4.sync_dmalength = ch4.sync_cache_dmalength;
					} else {
						ch4.sync_dmalength = 0;

						if( ch4.sync_irq_gen ) {
							ch4.sync_irq_enable = 0xFF;
							nes->cpu->SetIRQ( IRQ_DPCM );
						}
					}
				}
			}
		}
	}
	if( ch4.sync_irq_enable ) {
		bIRQ = TRUE;
	}

	return	bIRQ;
}

INT	APU_INTERNAL::GetStateSize()
{
	return	4*sizeof(BYTE) + 3*sizeof(INT)
		+ sizeof(ch0) + sizeof(ch1)
		+ sizeof(ch2) + sizeof(ch3)
		+ sizeof(ch4);
}

void	APU_INTERNAL::SaveState( LPBYTE p )
{
	SETBYTE( p, reg4015 );
	SETBYTE( p, sync_reg4015 );

	SETINT( p, FrameCycle );
	SETINT( p, FrameCount );
	SETINT( p, FrameType );
	SETBYTE( p, FrameIRQ );
	SETBYTE( p, FrameIRQoccur );

	SETBLOCK( p, &ch0, sizeof(ch0) );
	SETBLOCK( p, &ch1, sizeof(ch1) );
	SETBLOCK( p, &ch2, sizeof(ch2) );
	SETBLOCK( p, &ch3, sizeof(ch3) );
	SETBLOCK( p, &ch4, sizeof(ch4) );
}

void	APU_INTERNAL::LoadState( LPBYTE p )
{
	GETBYTE( p, reg4015 );
	GETBYTE( p, sync_reg4015 );

	GETINT( p, FrameCycle );
	GETINT( p, FrameCount );
	GETINT( p, FrameType );
	GETBYTE( p, FrameIRQ );
	GETBYTE( p, FrameIRQoccur );

	GETBLOCK( p, &ch0, sizeof(ch0) );
	GETBLOCK( p, &ch1, sizeof(ch1) );
	GETBLOCK( p, &ch2, sizeof(ch2) );
	GETBLOCK( p, &ch3, sizeof(ch3) );
//	p += sizeof(ch3);
	GETBLOCK( p, &ch4, sizeof(ch4) );
}

