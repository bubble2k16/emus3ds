
#ifndef _3DSFILEREADAHEAD_H_
#define _3DSFILEREADAHEAD_H_

#define CD_READ_AHEAD_BUFFER_SIZE (65536)

typedef struct
{
  FILE*             fptr;
  unsigned int      seek_pos;
  unsigned int      buffer_pos;
  unsigned int      buffer_length;
  char              buffer[CD_READ_AHEAD_BUFFER_SIZE];
} cd_read_ahead_struct;

void read_ahead_init(cd_read_ahead_struct *cd_read_ahead);

void read_ahead_fseek(cd_read_ahead_struct *, FILE *fp, int pos, int origin);

int read_ahead_fread(cd_read_ahead_struct *, void *dest_buffer, int size, FILE *fp);

#endif

