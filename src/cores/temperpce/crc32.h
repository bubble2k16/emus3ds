#ifndef CRC32_H
#define CRC32_H

#ifdef EXTERN_C_START
EXTERN_C_START
#endif 

int Crc32_ComputeFile( FILE *file, unsigned long *outCrc32 );
unsigned long Crc32_ComputeBuf( unsigned long inCrc32, const void *buf, size_t bufLen );

#ifdef EXTERN_C_END
EXTERN_C_END
#endif 


#endif
