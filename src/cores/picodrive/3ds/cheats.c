/*
 *  cheats.c
 * 
 *  Genesis Plus GX Cheats menu
 *
 *  Copyright Eke-Eke (2010-2014)
 *
 *  Redistribution and use of this code or any derivative works are permitted
 *  provided that the following conditions are met:
 *
 *   - Redistributions may not be sold, nor may they be used in a commercial
 *     product or activity.
 *
 *   - Redistributions that are modified from the original source must include the
 *     complete source code, including the source code for all components used by a
 *     binary built from the modified sources. However, as a special exception, the
 *     source code distributed need not include anything that is normally distributed
 *     (in either source or binary form) with the major components (compiler, kernel,
 *     and so on) of the operating system on which the executable runs, unless that
 *     component itself accompanies the executable.
 *
 *   - Redistributions must reproduce the above copyright notice, this list of
 *     conditions and the following disclaimer in the documentation and/or other
 *     materials provided with the distribution.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 *  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 *  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 *  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 *  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 *  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 *  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 *
 ****************************************************************************************/

#include "3ds.h"
#include "pico/pico.h"
#include "pico/pico_int.h"

#define MAX_CHEATS (200)
#define MAX_DESC_LENGTH (63)

typedef struct 
{
  char code[12];
  char text[MAX_DESC_LENGTH];
  u8 enable;
  u16 data;
  u16 old;
  u32 address;
  u8 *prev;
} CHEATENTRY;

static int string_offset = 0;
static int selection = 0;
static int offset = 0;
static int type = 0;
static int maxcheats = 0;
static int maxROMcheats = 0;
static int maxRAMcheats = 0;

static CHEATENTRY cheatlist[MAX_CHEATS];
static u8 cheatIndexes[MAX_CHEATS];

static char ggvalidchars[] = "ABCDEFGHJKLMNPRSTVWXYZ0123456789";

static char arvalidchars[] = "0123456789ABCDEF";

u32 decode_cheat(char *string, int index)
{
  char *p;
  int i,n;
  u32 len = 0;
  u32 address = 0;
  u16 data = 0;
  u8 ref = 0;

  /* 16-bit Game Genie code (ABCD-EFGH) */
  if ((strlen(string) >= 9) && (string[4] == '-'))
  {
    /* 16-bit system only */
    if ((PicoIn.AHW & PAHW_SMS))
    {
      return 0;
    }

    for (i = 0; i < 8; i++)
    {
      if (i == 4) string++;
      p = strchr (ggvalidchars, *string++);
      if (p == NULL) return 0;
      n = p - ggvalidchars;

      switch (i)
      {
        case 0:
        data |= n << 3;
        break;

        case 1:
        data |= n >> 2;
        address |= (n & 3) << 14;
        break;

        case 2:
        address |= n << 9;
        break;

        case 3:
        address |= (n & 0xF) << 20 | (n >> 4) << 8;
        break;
    
        case 4:
        data |= (n & 1) << 12;
        address |= (n >> 1) << 16;
        break;

        case 5:
        data |= (n & 1) << 15 | (n >> 1) << 8;
        break;

        case 6:
        data |= (n >> 3) << 13;
        address |= (n & 7) << 5;
        break;

        case 7:
        address |= n;
        break;
      }
    }

    /* code length */
    len = 9;
  }

  /* 8-bit Game Genie code (DDA-AAA-XXX) */
  else if ((strlen(string) >= 11) && (string[3] == '-') && (string[7] == '-'))
  {
    /* 8-bit system only */
    if (!(PicoIn.AHW & PAHW_SMS))
    {
      return 0;
    }

    /* decode 8-bit data */
    for (i=0; i<2; i++)
    {
      p = strchr (arvalidchars, *string++);
      if (p == NULL) return 0;
      n = (p - arvalidchars) & 0xF;
      data |= (n  << ((1 - i) * 4));
    }

    /* decode 16-bit address (low 12-bits) */
    for (i=0; i<3; i++)
    {
      if (i==1) string++; /* skip separator */
      p = strchr (arvalidchars, *string++);
      if (p == NULL) return 0;
      n = (p - arvalidchars) & 0xF;
      address |= (n  << ((2 - i) * 4));
    }

    /* decode 16-bit address (high 4-bits) */
    p = strchr (arvalidchars, *string++);
    if (p == NULL) return 0;
    n = (p - arvalidchars) & 0xF;
    n ^= 0xF; /* bits inversion */
    address |= (n  << 12);

    /* RAM address are also supported */
    if (address >= 0xC000)
    {
      /* convert to 24-bit Work RAM address */
      address = 0xFF0000 | (address & 0x1FFF);
    }

    /* decode reference 8-bit data */
    for (i=0; i<2; i++)
    {
      string++; /* skip separator and 2nd digit */
      p = strchr (arvalidchars, *string++);
      if (p == NULL) return 0;
      n = (p - arvalidchars) & 0xF;
      ref |= (n  << ((1 - i) * 4));
    }
    ref = (ref >> 2) | ((ref & 0x03) << 6);  /* 2-bit right rotation */
    ref ^= 0xBA;  /* XOR */

    /* update old data value */
    cheatlist[index].old = ref;

    /* code length */
    len = 11;
  }

  /* Action Replay code */
  else if (string[6] == ':')
  {
    if (!(PicoIn.AHW & PAHW_SMS))
    {
      /* 16-bit code (AAAAAA:DDDD) */
      if (strlen(string) < 11) return 0;

      /* decode 24-bit address */
      for (i=0; i<6; i++)
      {
        p = strchr (arvalidchars, *string++);
        if (p == NULL) return 0;
        n = (p - arvalidchars) & 0xF;
        address |= (n << ((5 - i) * 4));
      }

      /* decode 16-bit data */
      string++;
      for (i=0; i<4; i++)
      {
        p = strchr (arvalidchars, *string++);
        if (p == NULL) return 0;
        n = (p - arvalidchars) & 0xF;
        data |= (n << ((3 - i) * 4));
      }

      /* code length */
      len = 11;
    }
    else
    {
      /* 8-bit code (xxAAAA:DD) */
      if (strlen(string) < 9) return 0;

      /* decode 16-bit address */
      string+=2;
      for (i=0; i<4; i++)
      {
        p = strchr (arvalidchars, *string++);
        if (p == NULL) return 0;
        n = (p - arvalidchars) & 0xF;
        address |= (n << ((3 - i) * 4));
      }

      /* ROM addresses are not supported */
      if (address < 0xC000) return 0;

      /* convert to 24-bit Work RAM address */
      address = 0xFF0000 | (address & 0x1FFF);

      /* decode 8-bit data */
      string++;
      for (i=0; i<2; i++)
      {
        p = strchr (arvalidchars, *string++);
        if (p == NULL) return 0;
        n = (p - arvalidchars) & 0xF;
        data |= (n  << ((1 - i) * 4));
      }

      /* code length */
      len = 9;
    }
  }

  /* Valid code found ? */
  if (len)
  {
    /* update cheat address & data values */
    cheatlist[index].address = address;
    cheatlist[index].data = data;

    if (!(PicoIn.AHW & PAHW_SMS))
      cheatlist[index].old = *(u16 *)(Pico.rom + (cheatlist[index].address & 0xFFFFFE));

    maxcheats++;
  }

  /* return code length (0 = invalid) */
  return len;
}

u32 enable_cheat(int index, u8 enable)
{
    cheatlist[index].enable = enable;

    if (!enable)
    {
      if (!(PicoIn.AHW & PAHW_SMS))
      {
        *(u16 *)(Pico.rom + (cheatlist[index].address & 0xFFFFFE)) = cheatlist[index].old;
      }
    }
}


void apply_cheats(void)
{
  u8 *ptr;
  
  /* clear ROM&RAM patches counter */
  maxROMcheats = maxRAMcheats = 0;

  int i;
  for (i = 0; i < maxcheats; i++)
  {
    if (cheatlist[i].enable)
    {
      if (cheatlist[i].address < Pico.romsize)
      {
        if (!(PicoIn.AHW & PAHW_SMS))
        {
          /* patch ROM data */
          //cheatlist[i].old = *(u16 *)(Pico.rom + (cheatlist[i].address & 0xFFFFFE));
          *(u16 *)(Pico.rom + (cheatlist[i].address & 0xFFFFFE)) = cheatlist[i].data;
        }
        else
        {
          // LATER
          /* add ROM patch */
          //maxROMcheats++;
          //cheatIndexes[MAX_CHEATS - maxROMcheats] = i;

          /* get current banked ROM address */
          //ptr = &z80_readmap[(cheatlist[i].address) >> 10][cheatlist[i].address & 0x03FF];

          /* check if reference matches original ROM data */
          //if (((u8)cheatlist[i].old) == *ptr)
          //{
            /* patch data */
            //*ptr = cheatlist[i].data;

            /* save patched ROM address */
            //cheatlist[i].prev = ptr;
          //}
          //else
          //{
            /* no patched ROM address yet */
            //cheatlist[i].prev = NULL;
          //}
        }
      }
      else if (cheatlist[i].address >= 0xFF0000)
      {
        /* add RAM patch */
        cheatIndexes[maxRAMcheats++] = i;
      }
    }
  }
}

void clear_cheats(void)
{
  int i = maxcheats;

  /* disable cheats in reversed order in case the same address is used by multiple patches */
  while (i > 0)
  {
    if (cheatlist[i-1].enable)
    {
      if (cheatlist[i-1].address < Pico.romsize)
      {
        if (!(PicoIn.AHW & PAHW_SMS))
        {
          /* restore original ROM data */
          *(u16 *)(Pico.rom + (cheatlist[i-1].address & 0xFFFFFE)) = cheatlist[i-1].old;
        }
        else
        {
          // LATER
          /* check if previous banked ROM address has been patched */
          if (cheatlist[i-1].prev != NULL)
          {
            /* restore original data */
            *cheatlist[i-1].prev = cheatlist[i-1].old;

            /* no more patched ROM address */
            cheatlist[i-1].prev = NULL;
          }
        }
      }
    }

    i--;
  }

  maxcheats = 0;
}


/****************************************************************************
 * RAMCheatUpdate
 *
 * Apply RAM patches (this should be called once per frame)
 *
 ****************************************************************************/ 
void RAMCheatUpdate(void)
{
  int index, cnt = maxRAMcheats;
  
  while (cnt)
  {
    /* get cheat index */
    index = cheatIndexes[--cnt];

    /* apply RAM patch */
    if (cheatlist[index].data & 0xFF00)
    {
      /* word patch */
      *(u16 *)(PicoMem.ram + (cheatlist[index].address & 0xFFFE)) = cheatlist[index].data;
    }
    else
    {
      /* byte patch */
      PicoMem.ram[cheatlist[index].address & 0xFFFF] = cheatlist[index].data;
    }
  }
}

