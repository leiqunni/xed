/************************************************************************/
/*																		*/
/*	パス名関連					by H.Watanabe							*/
/*	Ver1.00	2009/03/29 10:03:05	作成									*/
/*																		*/
/************************************************************************/

#	ifndef	PATH2_H
#	define	PATH2_H
#	ifdef	__cplusplus
	extern "C"{
#	endif

	char	*pathSeekNam( char *pathp, const char *txtadr );			// ノード名までシークする
	char	*pathSeekExt( char *pathp, char *namp, const char *txtadr );// 拡張子名までシークする
	void	pathCat( char *dstp, const char *pathp, const char *namp );	// パス名とファイル名を連結する
	void	pathRepExt( char *dstp, const char *soup, const char *extp );		// 拡張子名変更

#	ifdef	__cplusplus
	}
#	endif
#	endif

