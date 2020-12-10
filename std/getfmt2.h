/************************************************************************/
/*																		*/
/*	フォーマット付き文字列解析	by H.Watanabe							*/
/*	Ver1.00	2009/04/05 13:52:04	作成									*/
/*																		*/
/************************************************************************/

#	ifndef	GETFMT2_H
#	define	GETFMT2_H
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

	char	*getfmt( const char *txtp, const char *format, ... );

#	ifdef	__cplusplus
	}
#	endif
#	endif

