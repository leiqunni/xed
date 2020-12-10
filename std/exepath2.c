/************************************************************************/
/*																		*/
/*	実行ファイルパスを得る		by H.Watanabe							*/
/*	Ver1.00	2009/04/05 13:52:04	作成									*/
/*																		*/
/************************************************************************/

#	include	<stdio.h>													/* 標準入出力関数*/
#	include	<stdlib.h>													/* 標準ライブラリ関数*/
#	include	<string.h>													/* 標準文字列関数*/
#	include	<sys/stat.h>												/* ファイルステータス*/
#	include	"exepath2.h"												/* 実行ファイルパス得る*/

/************************************************************************/
/*	ローカル定義														*/
/************************************************************************/

#	define	RNGTSTB(C,L,H)	((unsigned char)((C)-(L))<=(unsigned char)((H)-(L)))				/* 符号無し８ビットでレンジチェックをする*/
#	if	defined(_WIN32) || defined(__CYGWIN__)							/* SJIS*/
#		define	KANJI(C)	(RNGTSTB((C),0x81,0x9f)||RNGTSTB((C),0xe0,0xfc))
#		define	ENVSEP		';'
#	else																/* EUC*/
#		define	KANJI(C)	(((C)&0x80)>>7)
#		define	ENVSEP		':'
#	endif

/************************************************************************/
/*	実行ファイル存在チェック											*/
/************************************************************************/

static int	exechk( char *pacnamp, const char *pathp, const char *exenamp, const char **extpp ){

	int			res=0;
	struct stat	st;														// stat 構造体
	const char	*extp;
	const char	*sepp="";												// ディレクトリ名セパレータ
	int			len;
	int			i;

	len=strlen( pathp );												// パス名文字列長
	if( len ){															// パス名が存在する
		if( pathp[len-1]!='\\' && pathp[len-1]!='/' ){					// パス名の終端がパス名セパレータで終わっていない
			sepp="/";													// パス名セパレータ文字列セット
		}
	}
	for( i=0; ( extp=extpp[i] )!=0; i++ ){
		sprintf( pacnamp, "%s%s%s%s", pathp, sepp, exenamp, extp );
		res=!stat( pacnamp, &st );										// ファイル存在チェック
#	ifdef	unix
		if( res ){
			res=( ((S_IXUSR|S_IXGRP|S_IXOTH)&( st.st_mode ))!=0 );		// 実行アトリビュートが立っている
		}
#	endif
		if( res ){														// ファイルが存在する
			break;														// ループ終了
		}
	}
	return( res );
}

/************************************************************************/
/*	実行ファイルパスを得る												*/
/************************************************************************/

int		exepath2( char *pacnamp, const char *exenamp, const char **extpp ){

	static const char	*extpa[]={
		"",
		0
	};
	int				res;												// リザルトステータス
	const char		*txtp;												// テキストポインタ
	const char		*nxtp;												// 検索ポインタ
	const char		*extp=0;											// 拡張子名ポインタ
	unsigned char	chr;
	char			path[4096];											// パス名

	txtp=exenamp;
	while( ( chr=*txtp++ )!=0 ){
		if( chr==':' || chr=='/' || chr=='\\' ){						// パスセパレータを見つけた
			extp=0;														// 拡張子名ポインタリセット
		}else if( chr=='.' ){											// カンマを見つけた
			extp="";													// 拡張子名ポインタセット
		}
		txtp+=KANJI( chr );												// 漢字ならば+1
	}
	if( extp ){															// 拡張子の指定があった
		extpp=extpa;
	}else{																// 拡張子の指定は無い
		if( !extpp ){
#	ifdef	unix
			extpp=extpa;
#	else
			static const char	*winextpa[]={
				".exe",
				".com",
				".bat",
				0
			};
			extpp=winextpa;
#	endif
		}
	}
	res=exechk( pacnamp, "", exenamp, extpp );							// 実行ファイル存在チェック
	if( !res ){															// 見つからない
		txtp=getenv( "PATH" );											// 環境変数を得る
		if( txtp ){
			while( *txtp ){												// テキストポインタ終端までループ
				nxtp=strchr( txtp, ENVSEP );							// パスセパレータ検索
				if( nxtp ){												// パスセパレータが見つかった
					memcpy( path, txtp, nxtp-txtp );					// パスセパレータまでの文字列をパス名にコピー
					path[nxtp-txtp]=0;									// ０ターミネート
					txtp=nxtp+1;										// テキストポインタをパスセパレータの次にシーク
				}else{													// パスセパレータは見つからない
					strcpy( path, txtp );								// 終端までコピー
					txtp+=strlen( txtp );								// テキストポインタを終端にシーク
				}
				res=exechk( pacnamp, path, exenamp, extpp );			// 実行ファイル存在チェック
				if( res ){												// ファイルが存在する
					break;												// ループ終了
				}
			}
		}
	}
	return( res );														// 帰り値: 0=見つからない, 1:見つかった
}

