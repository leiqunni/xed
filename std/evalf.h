/************************************************************************/
/*																		*/
/*	数式評価					by H.Watanabe							*/
/*	Ver1.00	2009/04/04 13:44:21	作成									*/
/*																		*/
/************************************************************************/

#	ifndef	EVALF_H
#	define	EVALF_H
#	ifdef	__cplusplus
	extern "C"{
#	endif

#	ifndef	FPTYP														/* FPTYP が定義されていない*/
#		ifdef	_WIN32													/* Windows の場合*/
#			define	FPTYP	double										/* 単精度を使用すると不思議なことがいろいろ起きるので倍精度を使用する*/
#			define	FPSIZ	8
#		else															/* Windows 以外の場合*/
#			define	FPTYP	float										/* WIN32 以外ではメモリ効率を重視して単精度を使用する*/
#			define	FPSIZ	4
#		endif
#	endif

	typedef char *tpEVALDVAL( FPTYP *, const char *, void * );			// 値の評価をするコールバック関数

	char	*evaldUni( FPTYP *dstp, const char *txtp, tpEVALDVAL *valprcp, void *usrp );
	char	*evalf( FPTYP *dstp, const char *txtp, tpEVALDVAL *valprcp, void *usrp );

#	ifdef	__cplusplus
	}
#	endif
#	endif

