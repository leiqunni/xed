/************************************************************************/
/*																		*/
/*	クリッピング				by H.Watanabe							*/
/*	Ver1.00	2009/04/05 13:52:04	作成									*/
/*																		*/
/************************************************************************/

#	ifndef	CLP3_H
#	define	CLP3_H
#	ifdef	__cplusplus
	extern "C"{
#	endif

	int		clpLin( int *x1p, int *y1p, int *x2p, int *y2p, int x1, int y1, int x2, int y2, int wxc, int wyc, int wxd, int wyd );
	int		clp1( int *c1p, int *c2p, int s1, int s2, int w1, int w2 );
	int		clpBox( int *xcp, int *ycp, int *xdp, int *ydp, int xc, int yc, int xd, int yd, int wxc, int wyc, int wxd, int wyd );
	int		clpPos( int *xp, int *yp, int x, int y, int wxc, int wyc, int wxd, int wyd );

#	ifdef	__cplusplus
	}
#	endif
#	endif

