
#include "stdio.h"
#include "3dsfilereadahead.h"

extern void debugWait();

void read_ahead_init(cd_read_ahead_struct *cd_read_ahead)
{
  cd_read_ahead->fptr = NULL;
}

long read_ahead_ftell(cd_read_ahead_struct *cd_read_ahead, FILE *fp) 
{
  if (fp == NULL)
    return 0;
  
  if (cd_read_ahead->fptr != fp)
  {
    return ftell(fp);
  }  
  else
  {
    return cd_read_ahead->seek_pos + cd_read_ahead->buffer_pos;
  }
}

void read_ahead_fseek(cd_read_ahead_struct *cd_read_ahead, FILE *fp, int pos, int origin) 
{
  if (fp == NULL)
    return;

  fseek(fp, pos, origin);

  if (cd_read_ahead->fptr != fp)
  {
    cd_read_ahead->fptr = fp;

    if (origin == SEEK_SET)
      cd_read_ahead->seek_pos = pos;
    else
      cd_read_ahead->seek_pos = 0x7fffffff;

    cd_read_ahead->buffer_pos = 0;
    cd_read_ahead->buffer_length = 0;
  }
  else
  {
    if (origin == SEEK_SET)
    {
      if (cd_read_ahead->seek_pos != 0x7fffffff &&
        cd_read_ahead->seek_pos <= pos && 
        pos < (cd_read_ahead->seek_pos + cd_read_ahead->buffer_length))
      {
        cd_read_ahead->buffer_pos = pos - cd_read_ahead->seek_pos;
      }
      else
      {
        cd_read_ahead->seek_pos = pos;
        cd_read_ahead->buffer_pos = 0;
        cd_read_ahead->buffer_length = 0;
      }
    }
    else
    {
      cd_read_ahead->seek_pos = -1;
      cd_read_ahead->buffer_pos = 0;
      cd_read_ahead->buffer_length = 0;
    }
  }

}

int read_ahead_fread(cd_read_ahead_struct *cd_read_ahead, void *dest_buffer, int size, FILE *fp) 
{
  if (fp == NULL)
    return;
  
  int total_size = size;  
  if (cd_read_ahead->fptr != fp || 
      (cd_read_ahead->buffer_pos + total_size) > cd_read_ahead->buffer_length) 
  { 
    fseek(fp, cd_read_ahead->buffer_pos + cd_read_ahead->seek_pos, SEEK_SET);
    cd_read_ahead->seek_pos = cd_read_ahead->buffer_pos + cd_read_ahead->seek_pos;
    cd_read_ahead->buffer_length = fread(cd_read_ahead->buffer, 1, CD_READ_AHEAD_BUFFER_SIZE, fp); 
    cd_read_ahead->buffer_pos = 0; 
    cd_read_ahead->fptr = fp; 
  } 
  if (cd_read_ahead->buffer_pos + total_size > cd_read_ahead->buffer_length) 
  {
    total_size = cd_read_ahead->buffer_length - cd_read_ahead->buffer_pos; 
  }
  memcpy(dest_buffer, &cd_read_ahead->buffer[cd_read_ahead->buffer_pos], total_size); 
  cd_read_ahead->buffer_pos += total_size; 

  return total_size;
}
