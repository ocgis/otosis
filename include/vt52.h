#ifndef _vt52_h
#define _vt52_h

void vt52_put_char( int ch, int fd );
void vt52_put_string( char *str, int fd );
void setup_vt52( void );

#endif  /* _vt52_h */

