//////////////////////////////////////////////////////////////////////////
//                                                                      //
//      NES APU core                                                    //
//                                                           Norix      //
//                                               written     2002/06/27 //
//                                               last modify ----/--/-- //
//////////////////////////////////////////////////////////////////////////
#include "DebugOut.h"
#include "App.h"
#include "Config.h"

#include "nes.h"
#include "mmu.h"
#include "cpu.h"
#include "ppu.h"
#include "rom.h"
#include "apu.h"

// Volume adjust
// Internal sounds
#define	RECTANGLE_VOL	(0x0F0)
#define	TRIANGLE_VOL	(0x130)
#define	NOISE_VOL	(0x0C0)
#define	DPCM_VOL	(0x0F0)
// Extra sounds
#define	VRC6_VOL	(0x0F0)
#define	VRC7_VOL	(0x130)
#define	FDS_VOL		(0x0F0)
#define	MMC5_VOL	(0x0F0)
#define	N106_VOL	(0x088)
#define	FME7_VOL	(0x130)

APU::APU( NES* parent )
{
	exsound_select = 0;

	nes = parent;
	internal.SetParent( parent );

	last_data = last_diff = 0;

	ZEROMEMORY( m_SoundBuffer, sizeof(m_SoundBuffer) );

	ZEROMEMORY( lowpass_filter, sizeof(lowpass_filter) );
	ZEROMEMORY( &queue, sizeof(queue) );
	ZEROMEMORY( &exqueue, sizeof(exqueue) );
	ZEROMEMORY( &queueDAC, sizeof(queueDAC) );

	for( INT i = 0; i < 16; i++ ) {
		m_bMute[i] = TRUE;
	}
}

APU::~APU()
{
}

void	APU::SetQueue( u64 writetime, WORD addr, BYTE data )
{
	/*if( queue.wrptr == queue.rdptr )
	{
		if (writetime - queue.data[queue.wrptr].time >= 0x1ff && new_elapsed_time == 0)
			new_elapsed_time = writetime;
	}*/
	
	queue.data[queue.wrptr].time = writetime;
	queue.data[queue.wrptr].addr = addr;
	queue.data[queue.wrptr].data = data;
	
	// Write to wrptr after completing both increment and masking.
	// (helps prevent race conditions?)
	//queue.wrptr++;
	//queue.wrptr&=QUEUE_LENGTH-1;
	queue.wrptr = (queue.wrptr + 1) & (QUEUE_LENGTH - 1);

	if( queue.wrptr == queue.rdptr ) {
		DEBUGOUT( "queue overflow.\n" );
	}
}

// Fixed: Writes to the DAC register is now synced to the emulation
// cycles so that the samples play correctly.
//
void	APU::SetQueueDAC( u64 writetime, WORD addr, BYTE data )
{
	if( queueDAC.wrptr == queueDAC.rdptr )
	{
		if (writetime - queueDAC.data[queueDAC.wrptr].time >= 0x1ff && new_elapsed_time == 0)
			new_elapsed_time = writetime;
	}

	queueDAC.data[queueDAC.wrptr].time = writetime;
	queueDAC.data[queueDAC.wrptr].addr = addr;
	queueDAC.data[queueDAC.wrptr].data = data;

	// Write to wrptr after completing both increment and masking.
	// (helps prevent race conditions?)
	//queueDAC.wrptr++;
	//queueDAC.wrptr&=QUEUE_LENGTH-1;
	queueDAC.wrptr = (queueDAC.wrptr + 1) & (QUEUE_LENGTH - 1);

	if( queueDAC.wrptr == queueDAC.rdptr ) {
		DEBUGOUT( "queue overflow.\n" );
	}
}

BOOL	APU::GetQueue( u64 writetime, QUEUEDATA& ret )
{
	if( queue.wrptr == queue.rdptr ) {
		return	FALSE;
	}
	if( queue.data[queue.rdptr].time <= writetime ) {
		ret = queue.data[queue.rdptr];
		queue.rdptr++;
		queue.rdptr&=QUEUE_LENGTH-1;
		return	TRUE;
	}
	return	FALSE;
}

// Fixed: Reads from the DAC register is now synced to the emulation
// cycles so that the samples play correctly.
//
BOOL	APU::GetQueueDAC( u64 writetime, QUEUEDATA& ret )
{
	if( queueDAC.wrptr == queueDAC.rdptr ) {
		return	FALSE;
	}
	if( queueDAC.data[queueDAC.rdptr].time <= writetime ) {
		ret = queueDAC.data[queueDAC.rdptr];
		queueDAC.rdptr++;
		queueDAC.rdptr&=QUEUE_LENGTH-1;
		return	TRUE;
	}
	return	FALSE;
}

void	APU::SetExQueue( u64 writetime, WORD addr, BYTE data )
{
	/*if( exqueue.wrptr == exqueue.rdptr )
	{
		if (writetime - exqueue.data[exqueue.wrptr].time >= 0x1ff && new_elapsed_time == 0)
			new_elapsed_time = writetime;
	}*/
		
	exqueue.data[exqueue.wrptr].time = writetime;
	exqueue.data[exqueue.wrptr].addr = addr;
	exqueue.data[exqueue.wrptr].data = data;

	// Write to wrptr after completing both increment and masking.
	// (helps prevent race conditions?)
	//exqueue.wrptr++;
	//exqueue.wrptr&=QUEUE_LENGTH-1;
	exqueue.wrptr = (exqueue.wrptr + 1) & (QUEUE_LENGTH - 1);

	if( exqueue.wrptr == exqueue.rdptr ) {
		DEBUGOUT( "exqueue overflow.\n" );
	}
}


BOOL	APU::GetExQueue( u64 writetime, QUEUEDATA& ret )
{
	if( exqueue.wrptr == exqueue.rdptr ) {
		return	FALSE;
	}
	if( exqueue.data[exqueue.rdptr].time <= writetime ) {
		ret = exqueue.data[exqueue.rdptr];
		exqueue.rdptr++;
		exqueue.rdptr&=QUEUE_LENGTH-1;
		return	TRUE;
	}
	return	FALSE;
}


void	APU::QueueClear()
{
	ZEROMEMORY( &queue, sizeof(queue) );
	ZEROMEMORY( &exqueue, sizeof(exqueue) );
	ZEROMEMORY( &queueDAC, sizeof(queueDAC) );
}

void	APU::QueueFlush()
{
	int wrptr = queue.wrptr;
	while( wrptr != queue.rdptr ) {
		WriteProcess( queue.data[queue.rdptr].addr, queue.data[queue.rdptr].data );
		queue.rdptr++;
		queue.rdptr&=QUEUE_LENGTH-1;
	}

	wrptr = exqueue.wrptr;
	while( exqueue.wrptr != exqueue.rdptr ) {
		WriteExProcess( exqueue.data[exqueue.rdptr].addr, exqueue.data[exqueue.rdptr].data );
		exqueue.rdptr++;
		exqueue.rdptr&=QUEUE_LENGTH-1;
	}
}

void	APU::SoundSetup()
{
	FLOAT	fClock = nes->nescfg->CpuClock;
	INT	nRate = (INT)Config.sound.nRate;
	internal.Setup( fClock, nRate );
	vrc6.Setup( fClock, nRate );
	vrc7.Setup( fClock, nRate );
	mmc5.Setup( fClock, nRate );
	fds.Setup ( fClock, nRate );
	n106.Setup( fClock, nRate );
	fme7.Setup( fClock, nRate );
}

void	APU::Reset()
{
	ZEROMEMORY( &queue, sizeof(queue) );
	ZEROMEMORY( &exqueue, sizeof(exqueue) );
	ZEROMEMORY( &queueDAC, sizeof(queueDAC) );

	elapsed_time = 0;
	new_elapsed_time = 0;

	FLOAT	fClock = nes->nescfg->CpuClock;
	INT	nRate = (INT)Config.sound.nRate;
	internal.Reset( fClock, nRate );
	vrc6.Reset( fClock, nRate );
	vrc7.Reset( fClock, nRate );
	mmc5.Reset( fClock, nRate );
	fds.Reset ( fClock, nRate );
	n106.Reset( fClock, nRate );
	fme7.Reset( fClock, nRate );

	SoundSetup();
}

void	APU::SelectExSound( BYTE data )
{
	exsound_select = data;
}

BYTE	APU::Read( WORD addr )
{
	return	internal.SyncRead( addr );
}

void	APU::Write( WORD addr, BYTE data )
{
	// $4018��VirtuaNES�ŗL�|�[�g
	if( addr >= 0x4000 && addr <= 0x401F ) {
		internal.SyncWrite( addr, data );

		// Fix: If we are writing to 4011, put this in the synchronized DAC queue.
		if (addr == 0x4011)
			SetQueueDAC( nes->cpu->GetTotalCycles(), addr, data );
		else
			SetQueue( nes->cpu->GetTotalCycles(), addr, data );
	}
}

BYTE	APU::ExRead( WORD addr )
{
BYTE	data = 0;

	if( exsound_select & 0x10 ) {
		if( addr == 0x4800 ) {
			SetExQueue( nes->cpu->GetTotalCycles(), 0, 0 );
		}
	}
	if( exsound_select & 0x04 ) {
		if( addr >= 0x4040 && addr < 0x4100 ) {
			data = fds.SyncRead( addr );
		}
	}
	if( exsound_select & 0x08 ) {
		if( addr >= 0x5000 && addr <= 0x5015 ) {
			data = mmc5.SyncRead( addr );
		}
	}

	return	data;
}

void	APU::ExWrite( WORD addr, BYTE data )
{
	// Fix: If we are writing to 4011, put this in the synchronized DAC queue.
	if (addr == 0x5011)
		SetQueueDAC( nes->cpu->GetTotalCycles(), addr, data );
	else
		SetExQueue( nes->cpu->GetTotalCycles(), addr, data );

	if( exsound_select & 0x04 ) {
		if( addr >= 0x4040 && addr < 0x4100 ) {
			fds.SyncWrite( addr, data );
		}
	}

	if( exsound_select & 0x08 ) {
		if( addr >= 0x5000 && addr <= 0x5015 ) {
			mmc5.SyncWrite( addr, data );
		}
	}
}

void	APU::Sync()
{
}

void	APU::SyncDPCM( INT cycles )
{
	internal.Sync( cycles );

	if( exsound_select & 0x04 ) {
		fds.Sync( cycles );
	}
	if( exsound_select & 0x08 ) {
		mmc5.Sync( cycles );
	}
}

void	APU::WriteProcess( WORD addr, BYTE data )
{
	// $4018��VirtuaNES�ŗL�|�[�g
	if( addr >= 0x4000 && addr <= 0x401F ) {
		internal.Write( addr, data );
	}
}

void	APU::WriteExProcess( WORD addr, BYTE data )
{
	if( exsound_select & 0x01 ) {
		vrc6.Write( addr, data );
	}
	if( exsound_select & 0x02 ) {
		vrc7.Write( addr, data );
	}
	if( exsound_select & 0x04 ) {
		fds.Write( addr, data );
	}
	if( exsound_select & 0x08 ) {
		mmc5.Write( addr, data );
	}
	if( exsound_select & 0x10 ) {
		if( addr == 0x0000 ) {
			BYTE	dummy = n106.Read( addr );
		} else {
			n106.Write( addr, data );
		}
	}
	if( exsound_select & 0x20 ) {
		fme7.Write( addr, data );
	}
}

void	APU::Process( LPBYTE lpBuffer, DWORD dwSize )
{
INT	nBits = Config.sound.nBits;

DWORD	dwLength = dwSize / (nBits/8);

INT	output;
QUEUEDATA q;
double	writetime;

LPSHORT	pSoundBuf = m_SoundBuffer;
INT	nCcount = 0;

INT	nFilterType = Config.sound.nFilterType;

	if( !Config.sound.bEnable ) {
		::FillMemory( lpBuffer, dwSize, (BYTE)(Config.sound.nRate==8?128:0) );
		return;
	}

	// Volume setup
	//  0:Master
	//  1:Rectangle 1
	//  2:Rectangle 2
	//  3:Triangle
	//  4:Noise
	//  5:DPCM
	//  6:VRC6
	//  7:VRC7
	//  8:FDS
	//  9:MMC5
	// 10:N106
	// 11:FME7
	INT	vol[24];
	BOOL*	bMute = m_bMute;
	SHORT*	nVolume = Config.sound.nVolume;

	INT	nMasterVolume = bMute[0]?nVolume[0]:0;

	// Internal
	vol[ 0] = bMute[1]?(RECTANGLE_VOL*nVolume[1]*nMasterVolume)/(100*100):0;
	vol[ 1] = bMute[2]?(RECTANGLE_VOL*nVolume[2]*nMasterVolume)/(100*100):0;
	vol[ 2] = bMute[3]?(TRIANGLE_VOL *nVolume[3]*nMasterVolume)/(100*100):0;
	vol[ 3] = bMute[4]?(NOISE_VOL    *nVolume[4]*nMasterVolume)/(100*100):0;
	vol[ 4] = bMute[5]?(DPCM_VOL     *nVolume[5]*nMasterVolume)/(100*100):0;

	// VRC6
	vol[ 5] = bMute[6]?(VRC6_VOL*nVolume[6]*nMasterVolume)/(100*100):0;
	vol[ 6] = bMute[7]?(VRC6_VOL*nVolume[6]*nMasterVolume)/(100*100):0;
	vol[ 7] = bMute[8]?(VRC6_VOL*nVolume[6]*nMasterVolume)/(100*100):0;

	// VRC7
	vol[ 8] = bMute[6]?(VRC7_VOL*nVolume[7]*nMasterVolume)/(100*100):0;

	// FDS
	vol[ 9] = bMute[6]?(FDS_VOL*nVolume[8]*nMasterVolume)/(100*100):0;

	// MMC5
	vol[10] = bMute[6]?(MMC5_VOL*nVolume[9]*nMasterVolume)/(100*100):0;
	vol[11] = bMute[7]?(MMC5_VOL*nVolume[9]*nMasterVolume)/(100*100):0;
	vol[12] = bMute[8]?(MMC5_VOL*nVolume[9]*nMasterVolume)/(100*100):0;

	// N106
	vol[13] = bMute[ 6]?(N106_VOL*nVolume[10]*nMasterVolume)/(100*100):0;
	vol[14] = bMute[ 7]?(N106_VOL*nVolume[10]*nMasterVolume)/(100*100):0;
	vol[15] = bMute[ 8]?(N106_VOL*nVolume[10]*nMasterVolume)/(100*100):0;
	vol[16] = bMute[ 9]?(N106_VOL*nVolume[10]*nMasterVolume)/(100*100):0;
	vol[17] = bMute[10]?(N106_VOL*nVolume[10]*nMasterVolume)/(100*100):0;
	vol[18] = bMute[11]?(N106_VOL*nVolume[10]*nMasterVolume)/(100*100):0;
	vol[19] = bMute[12]?(N106_VOL*nVolume[10]*nMasterVolume)/(100*100):0;
	vol[20] = bMute[13]?(N106_VOL*nVolume[10]*nMasterVolume)/(100*100):0;

	// FME7
	vol[21] = bMute[6]?(FME7_VOL*nVolume[11]*nMasterVolume)/(100*100):0;
	vol[22] = bMute[7]?(FME7_VOL*nVolume[11]*nMasterVolume)/(100*100):0;
	vol[23] = bMute[8]?(FME7_VOL*nVolume[11]*nMasterVolume)/(100*100):0;

//	double	cycle_rate = ((double)FRAME_CYCLES*60.0/12.0)/(double)Config.sound.nRate;
	//double	cycle_rate = ((double)nes->nescfg->FrameCycles*60.0/12.0)/(double)Config.sound.nRate;
	cycle_rate = ((double)nes->nescfg->FrameCycles*60.0/12.0)/(double)Config.sound.nRate;

	// CPU�T�C�N���������[�v���Ă��܂������̑΍􏈗�
	//if( elapsed_time > nes->cpu->GetTotalCycles() ) {
		// This flushes writes to the non-DAC registers
		QueueFlush();
	//}

	if (new_elapsed_time != 0)
	{
		elapsed_time = new_elapsed_time;
		new_elapsed_time = 0;
	}

	while( dwLength-- ) 
	{
		writetime = elapsed_time;
		
		/*
		while( GetQueue( writetime, q ) ) {
			WriteProcess( q.addr, q.data );
		}

		while( GetExQueue( writetime, q ) ) {
			WriteExProcess( q.addr, q.data );
		}
		*/

		// We handle writes to the DAC here
		//
		while( GetQueueDAC( writetime, q ) ) {
			if (q.addr == 0x4011)
				WriteProcess( q.addr, q.data );
			if (q.addr == 0x5011)
				WriteExProcess( q.addr, q.data );
		}
		
		// 0-4:internal 5-7:VRC6 8:VRC7 9:FDS 10-12:MMC5 13-20:N106 21-23:FME7
		output = 0;
		output += internal.Process( 0 )*vol[0];
		output += internal.Process( 1 )*vol[1];
		output += internal.Process( 2 )*vol[2];
		output += internal.Process( 3 )*vol[3];
		output += internal.Process( 4 )*vol[4];

		if( exsound_select & 0x01 ) {
			output += vrc6.Process( 0 )*vol[5];
			output += vrc6.Process( 1 )*vol[6];
			output += vrc6.Process( 2 )*vol[7];
		}
		if( exsound_select & 0x02 ) {
			output += vrc7.Process( 0 )*vol[8];
		}
		if( exsound_select & 0x04 ) {
			output += fds.Process( 0 )*vol[9];
		}
		if( exsound_select & 0x08 ) {
			output += mmc5.Process( 0 )*vol[10];
			output += mmc5.Process( 1 )*vol[11];
			output += mmc5.Process( 2 )*vol[12];
		}
		if( exsound_select & 0x10 ) {
			output += n106.Process( 0 )*vol[13];
			output += n106.Process( 1 )*vol[14];
			output += n106.Process( 2 )*vol[15];
			output += n106.Process( 3 )*vol[16];
			output += n106.Process( 4 )*vol[17];
			output += n106.Process( 5 )*vol[18];
			output += n106.Process( 6 )*vol[19];
			output += n106.Process( 7 )*vol[20];
		}
		if( exsound_select & 0x20 ) {
			fme7.Process( 3 );	// Envelope & Noise
			output += fme7.Process( 0 )*vol[21];
			output += fme7.Process( 1 )*vol[22];
			output += fme7.Process( 2 )*vol[23];
		}

		output >>= 8;

		if( nFilterType == 1 ) {
			//���[�p�X�t�B���^�[TYPE 1(Simple)
			output = (lowpass_filter[0]+output)/2;
			lowpass_filter[0] = output;
		} else if( nFilterType == 2 ) {
			//���[�p�X�t�B���^�[TYPE 2(Weighted type 1)
			output = (lowpass_filter[1]+lowpass_filter[0]+output)/3;
			lowpass_filter[1] = lowpass_filter[0];
			lowpass_filter[0] = output;
		} else if( nFilterType == 3 ) {
			//���[�p�X�t�B���^�[TYPE 3(Weighted type 2)
			output = (lowpass_filter[2]+lowpass_filter[1]+lowpass_filter[0]+output)/4;
			lowpass_filter[2] = lowpass_filter[1];
			lowpass_filter[1] = lowpass_filter[0];
			lowpass_filter[0] = output;
		} else if( nFilterType == 4 ) {
			//���[�p�X�t�B���^�[TYPE 4(Weighted type 3)
			output = (lowpass_filter[1]+lowpass_filter[0]*2+output)/4;
			lowpass_filter[1] = lowpass_filter[0];
			lowpass_filter[0] = output;
		}

#if	0
		// DC�����̃J�b�g
		{
		static double ave = 0.0, max=0.0, min=0.0;
		double delta;
		delta = (max-min)/32768.0;
		max -= delta;
		min += delta;
		if( output > max ) max = output;
		if( output < min ) min = output;
		ave -= ave/1024.0;
		ave += (max+min)/2048.0;
		output -= (INT)ave;
		}
#endif
#if	1
		// DC�����̃J�b�g(HPF TEST)
		{
//		static	double	cutoff = (2.0*3.141592653579*40.0/44100.0);
		static	double	cutofftemp = (2.0*3.141592653579*40.0);
		double	cutoff = cutofftemp/(double)Config.sound.nRate;
		static	double	tmp = 0.0;
		double	in, out;

		in = (double)output;
		out = (in - tmp);
		tmp = tmp + cutoff * out;

		output = (INT)out;
		}
#endif
#if	0
		// �X�p�C�N�m�C�Y�̏���(AGC TEST)
		{
		INT	diff = abs(output-last_data);
		if( diff > 0x4000 ) {
			output /= 4;
		} else 
		if( diff > 0x3000 ) {
			output /= 3;
		} else
		if( diff > 0x2000 ) {
			output /= 2;
		}
		last_data = output;
		}
#endif
		// Limit
		if( output > 0x7FFF ) {
			output = 0x7FFF;
		} else if( output < -0x8000 ) {
			output = -0x8000;
		}

//printf ("nBits = %d\n", nBits);
		if( nBits != 8 ) {
			*(SHORT*)lpBuffer = (SHORT)output;
			lpBuffer += sizeof(SHORT);

		} else {
			*lpBuffer++ = (output>>8)^0x80;
		}

		//if( nCcount < 0x0100 )
		//	pSoundBuf[nCcount++] = (SHORT)output;

//		elapsedtime += cycle_rate;
		if (new_elapsed_time != 0)
		{
			elapsed_time = new_elapsed_time;
			new_elapsed_time = 0;
		}
		else
			elapsed_time += cycle_rate;
	}

/*
#if	1
	u64 total_cycles = nes->cpu->GetTotalCycles();
	if( elapsed_time > total_cycles + ((double)(nes->nescfg->FrameCycles/24)) ) {
		elapsed_time = total_cycles;
	}
	if( elapsed_time < total_cycles - ((double)(nes->nescfg->FrameCycles/6)) ) {
		elapsed_time = total_cycles;
	}
#else
	elapsed_time = nes->cpu->GetTotalCycles();
#endif
*/
}

// �`�����l���̎��g���擾�T�u���[�`��(NSF�p)
INT	APU::GetChannelFrequency( INT no )
{
	if( !m_bMute[0] )
		return	0;

	// Internal
	if( no < 5 ) {
		return	m_bMute[no+1]?internal.GetFreq( no ):0;
	}
	// VRC6
	if( (exsound_select & 0x01) && no >= 0x0100 && no < 0x0103 ) {
		return	m_bMute[6+(no&0x03)]?vrc6.GetFreq( no & 0x03 ):0;
	}
	// FDS
	if( (exsound_select & 0x04) && no == 0x300 ) {
		return	m_bMute[6]?fds.GetFreq( 0 ):0;
	}
	// MMC5
	if( (exsound_select & 0x08) && no >= 0x0400 && no < 0x0402 ) {
		return	m_bMute[6+(no&0x03)]?mmc5.GetFreq( no & 0x03 ):0;
	}
	// N106
	if( (exsound_select & 0x10) && no >= 0x0500 && no < 0x0508 ) {
		return	m_bMute[6+(no&0x07)]?n106.GetFreq( no & 0x07 ):0;
	}
	// FME7
	if( (exsound_select & 0x20) && no >= 0x0600 && no < 0x0603 ) {
		return	m_bMute[6+(no&0x03)]?fme7.GetFreq( no & 0x03 ):0;
	}
	// VRC7
	if( (exsound_select & 0x02) && no >= 0x0700 && no < 0x0709 ) {
		return	m_bMute[6]?vrc7.GetFreq(no&0x0F):0;
	}
	return	0;
}

// State Save/Load
void	APU::SaveState( LPBYTE p )
{
#ifdef	_DEBUG
LPBYTE	pold = p;
#endif

	// ���Ԏ��𓯊���������Flush����
	QueueFlush();

	internal.SaveState( p );
	p += (internal.GetStateSize()+15)&(~0x0F);	// Padding

	// VRC6
	if( exsound_select & 0x01 ) {
		vrc6.SaveState( p );
		p += (vrc6.GetStateSize()+15)&(~0x0F);	// Padding
	}
	// VRC7 (not support)
	if( exsound_select & 0x02 ) {
		vrc7.SaveState( p );
		p += (vrc7.GetStateSize()+15)&(~0x0F);	// Padding
	}
	// FDS
	if( exsound_select & 0x04 ) {
		fds.SaveState( p );
		p += (fds.GetStateSize()+15)&(~0x0F);	// Padding
	}
	// MMC5
	if( exsound_select & 0x08 ) {
		mmc5.SaveState( p );
		p += (mmc5.GetStateSize()+15)&(~0x0F);	// Padding
	}
	// N106
	if( exsound_select & 0x10 ) {
		n106.SaveState( p );
		p += (n106.GetStateSize()+15)&(~0x0F);	// Padding
	}
	// FME7
	if( exsound_select & 0x20 ) {
		fme7.SaveState( p );
		p += (fme7.GetStateSize()+15)&(~0x0F);	// Padding
	}

#ifdef	_DEBUG
DEBUGOUT( "SAVE APU SIZE:%d\n", p-pold );
#endif
}

void	APU::LoadState( LPBYTE p )
{
	// ���Ԏ��𓯊��������ׂɏ���
	QueueClear();

	internal.LoadState( p );
	p += (internal.GetStateSize()+15)&(~0x0F);	// Padding

	// VRC6
	if( exsound_select & 0x01 ) {
		vrc6.LoadState( p );
		p += (vrc6.GetStateSize()+15)&(~0x0F);	// Padding
	}
	// VRC7 (not support)
	if( exsound_select & 0x02 ) {
		vrc7.LoadState( p );
		p += (vrc7.GetStateSize()+15)&(~0x0F);	// Padding
	}
	// FDS
	if( exsound_select & 0x04 ) {
		fds.LoadState( p );
		p += (fds.GetStateSize()+15)&(~0x0F);	// Padding
	}
	// MMC5
	if( exsound_select & 0x08 ) {
		mmc5.LoadState( p );
		p += (mmc5.GetStateSize()+15)&(~0x0F);	// Padding
	}
	// N106
	if( exsound_select & 0x10 ) {
		n106.LoadState( p );
		p += (n106.GetStateSize()+15)&(~0x0F);	// Padding
	}
	// FME7
	if( exsound_select & 0x20 ) {
		fme7.LoadState( p );
		p += (fme7.GetStateSize()+15)&(~0x0F);	// Padding
	}
}
