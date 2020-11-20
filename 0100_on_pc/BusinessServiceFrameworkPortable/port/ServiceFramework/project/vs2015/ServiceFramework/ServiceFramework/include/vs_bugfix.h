#ifndef PORTABLE_H
#define PORTABLE_H
/*
#ifndef NULL
#define NULL ((void *)0x0000)
#endif

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif
*/
//typedef signed char          BOOLEAN;
typedef unsigned char        UINT8;
typedef signed char          SINT8;
typedef unsigned short int   UINT16;
typedef signed short int     SINT16;
//typedef unsigned long int    UINT32;
typedef signed long int      SINT32;
typedef float                REAL32;
typedef double               REAL64;


typedef unsigned char        boolean;
typedef unsigned char        uint8;
typedef signed char          int8;
typedef unsigned short int   uint16;
typedef signed short int     int16;
typedef unsigned long int    uint32;
typedef signed long int      sint32;
typedef float                real32;
typedef double               real64;


#endif /* End of PORTABLE_H */
