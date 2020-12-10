/************************************************************************/
/*																		*/
/*	パス名関連					by H.Watanabe							*/
/*	Ver1.00	2009/03/29 10:03:05	作成									*/
/*																		*/
/************************************************************************/

#	include	<string.h>													/* 標準文字列関数*/
#	include	"path2.h"													/* パス名関連*/

/************************************************************************/
/*	ローカル定義														*/
/************************************************************************/

#	define	RNGTSTB(C,L,H)	((unsigned char)((C)-(L))<=(unsigned char)((H)-(L)))				/* 符号無し８ビットでレンジチェックをする*/
#	if	defined(_WIN32) || defined(__CYGWIN__)							/* SJIS*/
#		define	KANJI(C)	(RNGTSTB((C),0x81,0x9f)||RNGTSTB((C),0xe0,0xfc))
#	else																/* EUC*/
#		define	KANJI(C)	(((C)&0x80)>>7)
#	endif

/************************************************************************/
/*	ノード名までシークする												*/
/************************************************************************/

char	*pathSeekNam( char *pathp, const char *txtadr ){

	const char		*txtp=txtadr, *sepp=txtadr;
	unsigned char	chr;

	while( (chr=*txtp++)!=0 ){
		if( chr==':' || chr=='/' || chr=='\\' ){						// パスセパレータを見つけた
			sepp=txtp;
		}
		txtp+=KANJI( chr );												// 漢字ならば+1
	}
	if( pathp ){														// ディストネーションが指定されている
		memcpy( pathp, txtadr, sepp-txtadr );							// パス文字列コピー
		pathp[sepp-txtadr]=0;											// ０ターミネート
	}
	return( (char*)sepp );
}

/************************************************************************/
/*	拡張子名までシークする												*/
/************************************************************************/

char	*pathSeekExt( char *pathp, char *namp, const char *txtadr ){

	const char	*txtp, *sepp;

	txtp=pathSeekNam( pathp, txtadr );									// ノード名までシークする
	sepp=txtp+strspn( txtp, "." );										// 先頭に続く '.' をスキップ
	sepp=strrchr( sepp, '.' );											// 最後の'.'検索
	if( !sepp ){														// 見つからなかった
		sepp=txtp+strlen( txtp );										// 終端へシーク
	}
	if( namp ){															// ディストネーションが指定されている
		memcpy( namp, txtp, sepp-txtp );								// ノード文字列コピー
		namp[sepp-txtp]=0;												// ０ターミネート
	}
	return( (char*)sepp );
}

/************************************************************************/
/*	パス名とファイル名を連結する										*/
/************************************************************************/

void	pathCat( char *dstp, const char *pathp, const char *namp ){

	unsigned char	chr='/';											// パス文字列が""の場合のデフォルト

	while( *pathp ){
		chr=( *dstp++=*pathp++ );										// 1バイトコピー
		if( KANJI( chr ) ){												// 2バイト目が '\' の場合があるため
			*dstp++=*pathp++;											// 1バイトコピー
		}
	}
	if( chr!='/' && chr!='\\' && chr!=':' ){							// ディレクトリセパレータではない
		*dstp++='/';													// ディレクトリセパレータ付加
	}
	strcpy( dstp, namp );
}

/************************************************************************/
/*	拡張子名変更														*/
/************************************************************************/

void	pathRepExt( char *dstp, const char *soup, const char *extp ){

	int		len;														// パス＋名前のサイズ

	len=pathSeekExt( 0, 0, soup )-soup;									// パス＋名前のサイズ
	memcpy( dstp, soup, len );											// パス＋名前をコピー
	strcpy( dstp+len, extp );											// 拡張子名をコピー
}

