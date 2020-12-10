/************************************************************************/
/*																		*/
/*	ŒŸõ						by H.Watanabe							*/
/*	Ver0.00	2009/04/02 20:47:41	ì¬									*/
/*																		*/
/************************************************************************/

#	ifndef	SEARCH2_H
#	define	SEARCH2_H
#	ifdef	__cplusplus
	extern "C"{
#	endif

	char	*searchWld( const char *linadr, const char *wldadr, int c2lflg );
	char	*searchLin( const char *txtbufadr, const char *txtp, const char *orgp, int bakflg, int lblflg, int wldflg, int c2lflg );

#	ifdef	__cplusplus
	}
#	endif
#	endif

