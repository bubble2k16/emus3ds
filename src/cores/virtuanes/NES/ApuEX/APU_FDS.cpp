//////////////////////////////////////////////////////////////////////////
//                                                                      //
//      FDS sound                                                       //
//                                                           Norix      //
//                                               written     2002/06/30 //
//                                               last modify ----/--/-- //
//////////////////////////////////////////////////////////////////////////
#include "DebugOut.h"

#include "APU_FDS.h"
#include "state.h"

APU_FDS::APU_FDS()
{
	ZEROMEMORY( &fds, sizeof(fds) );
	ZEROMEMORY( &fds_sync, sizeof(fds) );

	ZEROMEMORY( output_buf, sizeof(output_buf) );

	sampling_rate = 22050;
}

APU_FDS::~APU_FDS()
{
}

void	APU_FDS::Reset( FLOAT fClock, INT nRate )
{
	ZEROMEMORY( &fds, sizeof(fds) );
	ZEROMEMORY( &fds_sync, sizeof(fds) );

	sampling_rate = nRate;
}

void	APU_FDS::Setup( FLOAT fClock, INT nRate )
{
	sampling_rate = nRate;
}

void	APU_FDS::WriteSub( WORD addr, BYTE data, FDSSOUND& ch, double rate )
{
	if( addr < 0x4040 || addr > 0x40BF )
		return;

	ch.reg[addr-0x4040] = data;
	if( addr >= 0x4040 && addr <= 0x407F ) {
		if( ch.wave_setup ) {
			ch.main_wavetable[addr-0x4040] = 0x20-((INT)data&0x3F);
		}
	} else {
		switch( addr ) {
			case	0x4080:	// Volume Envelope
				ch.volenv_mode = data>>6;
				if( data&0x80 ) {
					ch.volenv_gain = data&0x3F;

					// ‘¦Žž”½‰f
					if( !ch.main_addr ) {
						ch.now_volume = (ch.volenv_gain<0x21)?ch.volenv_gain:0x20;
					}
				}
				// ƒGƒ“ƒxƒ[ƒv1’iŠK‚Ì‰‰ŽZ
				ch.volenv_decay    = data&0x3F;
				ch.volenv_phaseacc = (double)ch.envelope_speed * (double)(ch.volenv_decay+1) * rate / (232.0*960.0);
				break;

			case	0x4082:	// Main Frequency(Low)
				ch.main_frequency = (ch.main_frequency&~0x00FF)|(INT)data;
				break;
			case	0x4083:	// Main Frequency(High)
				ch.main_enable     = (~data)&(1<<7);
				ch.envelope_enable = (~data)&(1<<6);
				if( !ch.main_enable ) {
					ch.main_addr = 0;
					ch.now_volume = (ch.volenv_gain<0x21)?ch.volenv_gain:0x20;
				}
//				ch.main_frequency  = (ch.main_frequency&0x00FF)|(((INT)data&0x3F)<<8);
				ch.main_frequency  = (ch.main_frequency&0x00FF)|(((INT)data&0x0F)<<8);
				break;

			case	0x4084:	// Sweep Envelope
				ch.swpenv_mode = data>>6;
				if( data&0x80 ) {
					ch.swpenv_gain = data&0x3F;
				}
				// ƒGƒ“ƒxƒ[ƒv1’iŠK‚Ì‰‰ŽZ
				ch.swpenv_decay    = data&0x3F;
				ch.swpenv_phaseacc = (double)ch.envelope_speed * (double)(ch.swpenv_decay+1) * rate / (232.0*960.0);
				break;

			case	0x4085:	// Sweep Bias
				if( data&0x40 ) ch.sweep_bias = (data&0x3f)-0x40;
				else		ch.sweep_bias =  data&0x3f;
				ch.lfo_addr = 0;
				break;

			case	0x4086:	// Effector(LFO) Frequency(Low)
				ch.lfo_frequency = (ch.lfo_frequency&(~0x00FF))|(INT)data;
				break;
			case	0x4087:	// Effector(LFO) Frequency(High)
				ch.lfo_enable    = (~data&0x80);
				ch.lfo_frequency = (ch.lfo_frequency&0x00FF)|(((INT)data&0x0F)<<8);
				break;

			case	0x4088:	// Effector(LFO) wavetable
				if( !ch.lfo_enable ) {
					// FIFO?
					for( INT i = 0; i < 31; i++ ) {
						ch.lfo_wavetable[i*2+0] = ch.lfo_wavetable[(i+1)*2+0];
						ch.lfo_wavetable[i*2+1] = ch.lfo_wavetable[(i+1)*2+1];
					}
					ch.lfo_wavetable[31*2+0] = data&0x07;
					ch.lfo_wavetable[31*2+1] = data&0x07;
				}
				break;

			case	0x4089:	// Sound control
				{
				INT	tbl[] = {30, 20, 15, 12};
				ch.master_volume = tbl[data&3];
				ch.wave_setup    = data&0x80;
				}
				break;

			case	0x408A:	// Sound control 2
				ch.envelope_speed = data;
				break;

			default:
				break;
		}
	}
}

// APUƒŒƒ“ƒ_ƒ‰‘¤‚©‚çŒÄ‚Î‚ê‚é
void	APU_FDS::Write( WORD addr, BYTE data )
{
	// ƒTƒ“ƒvƒŠƒ“ƒOƒŒ[ƒgŠî€
	WriteSub( addr, data, fds, (double)sampling_rate );
}

BYTE	APU_FDS::Read ( WORD addr )
{
BYTE	data = addr>>8;

	if( addr >= 0x4040 && addr <= 0x407F ) {
		data = fds.main_wavetable[addr&0x3F] | 0x40;
	} else
	if( addr == 0x4090 ) {
		data = (fds.volenv_gain&0x3F)|0x40;
	} else
	if( addr == 0x4092 ) {
		data = (fds.swpenv_gain&0x3F)|0x40;
	}

	return	data;
}

INT	APU_FDS::Process( INT channel )
{
	// Envelope unit
	if( fds.envelope_enable && fds.envelope_speed ) {
		// Volume envelope
		if( fds.volenv_mode < 2 ) {
			double	decay = ((double)fds.envelope_speed * (double)(fds.volenv_decay+1) * (double)sampling_rate) / (232.0*960.0);
			fds.volenv_phaseacc -= 1.0;
			while( fds.volenv_phaseacc < 0.0 ) {
				fds.volenv_phaseacc += decay;

				if( fds.volenv_mode == 0 ) {
				// Œ¸­ƒ‚[ƒh
					if( fds.volenv_gain )
						fds.volenv_gain--;
				} else
				if( fds.volenv_mode == 1 ) {
					if( fds.volenv_gain < 0x20 )
						fds.volenv_gain++;
				}
			}
		}

		// Sweep envelope
		if( fds.swpenv_mode < 2 ) {
			double	decay = ((double)fds.envelope_speed * (double)(fds.swpenv_decay+1) * (double)sampling_rate) / (232.0*960.0);
			fds.swpenv_phaseacc -= 1.0;
			while( fds.swpenv_phaseacc < 0.0 ) {
				fds.swpenv_phaseacc += decay;

				if( fds.swpenv_mode == 0 ) {
				// Œ¸­ƒ‚[ƒh
					if( fds.swpenv_gain )
						fds.swpenv_gain--;
				} else
				if( fds.swpenv_mode == 1 ) {
					if( fds.swpenv_gain < 0x20 )
						fds.swpenv_gain++;
				}
			}
		}
	}

	// Effector(LFO) unit
	INT	sub_freq = 0;
//	if( fds.lfo_enable && fds.envelope_speed && fds.lfo_frequency ) {
	if( fds.lfo_enable ) {
		if (fds.lfo_frequency)
		{
			static int tbl[8] = { 0, 1, 2, 4, 0, -4, -2, -1};

			fds.lfo_phaseacc -= (1789772.5*(double)fds.lfo_frequency)/65536.0;
			while( fds.lfo_phaseacc < 0.0 ) {
				fds.lfo_phaseacc += (double)sampling_rate;

				if( fds.lfo_wavetable[fds.lfo_addr] == 4 )
					fds.sweep_bias = 0;
				else
					fds.sweep_bias += tbl[fds.lfo_wavetable[fds.lfo_addr]];

				fds.lfo_addr = (fds.lfo_addr+1)&63;
			}
		}

		if( fds.sweep_bias > 63 )
			fds.sweep_bias -= 128;
		else if( fds.sweep_bias < -64 )
			fds.sweep_bias += 128;

		INT	sub_multi = fds.sweep_bias * fds.swpenv_gain;

		if( sub_multi & 0x0F ) {
			// 16‚ÅŠ„‚èØ‚ê‚È‚¢ê‡
			sub_multi = (sub_multi / 16);
			if( fds.sweep_bias >= 0 )
				sub_multi += 2;    // ³‚Ìê‡
			else
				sub_multi -= 1;    // •‰‚Ìê‡
		} else {
			// 16‚ÅŠ„‚èØ‚ê‚éê‡
			sub_multi = (sub_multi / 16);
		}
		// 193‚ð’´‚¦‚é‚Æ-258‚·‚é(-64‚Öƒ‰ƒbƒv)
		if( sub_multi > 193 )
			sub_multi -= 258;
		// -64‚ð‰º‰ñ‚é‚Æ+256‚·‚é(192‚Öƒ‰ƒbƒv)
	        if( sub_multi < -64 )
			sub_multi += 256;

		sub_freq = (fds.main_frequency) * sub_multi / 64;
	}

	// Main unit
	INT	output = 0;
	if( fds.main_enable && fds.main_frequency && !fds.wave_setup ) {
		INT	freq;
		INT	main_addr_old = fds.main_addr;

		freq = (fds.main_frequency+sub_freq)*1789772.5/65536.0;

		fds.main_addr = (fds.main_addr+freq+64*sampling_rate)%(64*sampling_rate);

		// 1ŽüŠú‚ð’´‚¦‚½‚çƒ{ƒŠƒ…[ƒ€XV
		if( main_addr_old > fds.main_addr )
			fds.now_volume = (fds.volenv_gain<0x21)?fds.volenv_gain:0x20;

		output = fds.main_wavetable[(fds.main_addr / sampling_rate)&0x3f] * 8 * fds.now_volume * fds.master_volume / 30;

		if( fds.now_volume )
			fds.now_freq = freq * 4;
		else
			fds.now_freq = 0;
	} else {
		fds.now_freq = 0;
		output = 0;
	}

	// LPF
#if	1
	output = (output_buf[0] * 2 + output) / 3;
	output_buf[0] = output;
#else
	output = (output_buf[0] + output_buf[1] + output) / 3;
	output_buf[0] = output_buf[1];
	output_buf[1] = output;
#endif

	fds.output = output;
	return	fds.output;
}

// CPU‘¤‚©‚çŒÄ‚Î‚ê‚é
void	APU_FDS::SyncWrite( WORD addr, BYTE data )
{
	// ƒNƒƒbƒNŠî€
	WriteSub( addr, data, fds_sync, 1789772.5 );
}

BYTE	APU_FDS::SyncRead( WORD addr )
{
BYTE	data = addr>>8;

	if( addr >= 0x4040 && addr <= 0x407F ) {
		data = fds_sync.main_wavetable[addr&0x3F] | 0x40;
	} else
	if( addr == 0x4090 ) {
		data = (fds_sync.volenv_gain&0x3F)|0x40;
	} else
	if( addr == 0x4092 ) {
		data = (fds_sync.swpenv_gain&0x3F)|0x40;
	}

	return	data;
}

BOOL	APU_FDS::Sync( INT cycles )
{
	// Envelope unit
	if( fds_sync.envelope_enable && fds_sync.envelope_speed ) {
		// Volume envelope
		double	decay;
		if( fds_sync.volenv_mode < 2 ) {
			decay = ((double)fds_sync.envelope_speed * (double)(fds_sync.volenv_decay+1) * 1789772.5) / (232.0*960.0);
			fds_sync.volenv_phaseacc -= (double)cycles;
			while( fds_sync.volenv_phaseacc < 0.0 ) {
				fds_sync.volenv_phaseacc += decay;

				if( fds_sync.volenv_mode == 0 ) {
				// Œ¸­ƒ‚[ƒh
					if( fds_sync.volenv_gain )
						fds_sync.volenv_gain--;
				} else
				if( fds_sync.volenv_mode == 1 ) {
				// ‘‰Áƒ‚[ƒh
					if( fds_sync.volenv_gain < 0x20 )
						fds_sync.volenv_gain++;
				}
			}
		}

		// Sweep envelope
		if( fds_sync.swpenv_mode < 2 ) {
			decay = ((double)fds_sync.envelope_speed * (double)(fds_sync.swpenv_decay+1) * 1789772.5) / (232.0*960.0);
			fds_sync.swpenv_phaseacc -= (double)cycles;
			while( fds_sync.swpenv_phaseacc < 0.0 ) {
				fds_sync.swpenv_phaseacc += decay;

				if( fds_sync.swpenv_mode == 0 ) {
				// Œ¸­ƒ‚[ƒh
					if( fds_sync.swpenv_gain )
						fds_sync.swpenv_gain--;
				} else
				if( fds_sync.swpenv_mode == 1 ) {
				// ‘‰Áƒ‚[ƒh
					if( fds_sync.swpenv_gain < 0x20 )
						fds_sync.swpenv_gain++;
				}
			}
		}
	}

	return	FALSE;
}

INT	APU_FDS::GetFreq( INT channel )
{
	return	fds.now_freq;
}

INT	APU_FDS::GetStateSize()
{
	return	sizeof(fds) + sizeof(fds_sync);
}

void	APU_FDS::SaveState( LPBYTE p )
{
	SETBLOCK( p, &fds, sizeof(fds) );
	SETBLOCK( p, &fds_sync, sizeof(fds_sync) );
}

void	APU_FDS::LoadState( LPBYTE p )
{
	GETBLOCK( p, &fds, sizeof(fds) );
	GETBLOCK( p, &fds_sync, sizeof(fds_sync) );
}

