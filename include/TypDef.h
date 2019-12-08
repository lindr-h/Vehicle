

#ifndef __INC_TYPDEF
#define __INC_TYPDEF

#ifdef __cplusplus
extern "C" {
#endif

#if	!defined(NULL)
#if defined __GNUG__
#define NULL (__null)
#else
#if !defined(__cplusplus) && 0
#define NULL ((void*)0)
#endif
#endif
#endif

#if	!defined(EOF) || (EOF!=(-1))
#define EOF		(-1)
#endif

#if	!defined(FALSE) || (FALSE!=0)
#define FALSE		0
#endif

#if	!defined(TRUE) || (TRUE!=1)
#define TRUE		1
#endif


#define NONE		(-1)	/* for times when NULL won't do */
#define EOS		'\0'	/* C string terminator */


/* return status values */

#define OK		0



typedef	char		INT8;
typedef	short		INT16;
typedef	int		INT32;

typedef	unsigned char	UINT8;
typedef	unsigned short	UINT16;
typedef	unsigned int	UINT32;

typedef	unsigned char	UCHAR;
typedef unsigned short	USHORT;
typedef	unsigned int	UINT;
typedef unsigned long	ULONG;

typedef	int		BOOL;
typedef	int		STATUS;
typedef int 		ARGINT;

/* low-level I/O input, output, error fd's */

#define	STD_IN	0
#define	STD_OUT	1
#define	STD_ERR	2

/* BUS types */

#define	BUS_TYPE_NONE	    NONE
#define BUS_TYPE_VME	    1
#define BUS_TYPE_MULTIBUS   2
#define BUS_TYPE_PCI	    3

#define VME_BUS		    BUS_TYPE_VME	/* for backward compat. */
#define MULTI_BUS	    BUS_TYPE_MULTIBUS

/* common macros */

#define MSB(x)	(((x) >> 8) & 0xff)	  /* most signif byte of 2-byte integer */
#define LSB(x)	((x) & 0xff)		  /* least signif byte of 2-byte integer*/
#define MSW(x) (((x) >> 16) & 0xffff) /* most signif word of 2-word integer */
#define LSW(x) ((x) & 0xffff) 		  /* least signif byte of 2-word integer*/

/* swap the MSW with the LSW of a 32 bit integer */
#define WORDSWAP(x) (MSW(x) | (LSW(x) << 16))

#define LLSB(x)	((x) & 0xff)		/* 32bit word byte/word swap macros */
#define LNLSB(x) (((x) >> 8) & 0xff)
#define LNMSB(x) (((x) >> 16) & 0xff)
#define LMSB(x)	 (((x) >> 24) & 0xff)
#define LONGSWAP(x) ((LLSB(x) << 24) | \
		     (LNLSB(x) << 16)| \
		     (LNMSB(x) << 8) | \
		     (LMSB(x)))

#define OFFSET(structure, member)	/* byte offset of member in structure*/\
		((int) &(((structure *) 0) -> member))

#define MEMBER_SIZE(structure, member)	/* size of a member of a structure */\
		(sizeof (((structure *) 0) -> member))

#define NELEMENTS(array)		/* number of elements in an array */ \
		(sizeof (array) / sizeof ((array) [0]))

#define FOREVER	for (;;)

#define isascii(c)	((unsigned) (c) <= 0177)
#define toascii(c)	((c) & 0177)


/* storage class specifier definitions */

#define FAST	register
#define IMPORT	extern
#define LOCAL	static

#define ROUND_UP(x, align)	(((int) (x) + (align - 1)) & ~(align - 1))
#define ROUND_DOWN(x, align)	((int)(x) & ~(align - 1))
#define ALIGNED(x, align)	(((int)(x) & (align - 1)) == 0)

#define MEM_ROUND_UP(x)		ROUND_UP(x, _ALLOC_ALIGN_SIZE)
#define MEM_ROUND_DOWN(x)	ROUND_DOWN(x, _ALLOC_ALIGN_SIZE)
#define STACK_ROUND_UP(x)	ROUND_UP(x, _STACK_ALIGN_SIZE)
#define STACK_ROUND_DOWN(x)	ROUND_DOWN(x, _STACK_ALIGN_SIZE)
#define MEM_ALIGNED(x)		ALIGNED(x, _ALLOC_ALIGN_SIZE)


#ifdef __cplusplus
}
#endif

#endif /* __INC_TYPDEF */
