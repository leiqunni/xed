/************************************************************************/
/*																		*/
/*	クリップボード等からの行変換	by H.Watanabe						*/
/*	Ver1.00	2009/04/05 13:52:04	作成									*/
/*																		*/
/************************************************************************/

#	include	<stdlib.h>													/* 標準ライブラリ関数*/
#	include	<string.h>													/* 標準文字列関数*/
#	include	"readdb.h"													/* クリップボード等からの行変換*/

/************************************************************************/
/*	ローカル定義														*/
/************************************************************************/

	struct	tgREADDB{
		stDB				*dbp;										// 行DB
		char				*leftp;										// 残り文字列ポインタ
	};

/************************************************************************/
/*	オープン															*/
/************************************************************************/

stREADDB	*readdbOpen(void){

	stREADDB	*rhp;													// 管理構造体

	rhp=(stREADDB*)malloc( sizeof(stREADDB) );							// 管理構造体メモリ確保
	memset( rhp, 0, sizeof(stREADDB) );									// 管理構造体クリア
	rhp->dbp=dbOpen( 0, DBSIZ_STR, 0, 4096 );							// 行DBオープン
	return( rhp );														// 管理構造体を持ってリターン
}

/************************************************************************/
/*	クローズ															*/
/************************************************************************/

stDB	*readdbClose( stREADDB *rhp ){

	stDB	*dbp=rhp->dbp;												// 行DB

	if( rhp->leftp ){													// 残り文字列がある
		dbAlc( dbp, rhp->leftp );										// 行DBに登録
		dbMemFre( rhp->leftp );											// 残り文字列開放
	}
	free( rhp );														// 管理構造体メモリ開放
	return( dbp );														// 行DBをもってリターン
}

/************************************************************************/
/*	登録																*/
/************************************************************************/

void	readdbAlc( stREADDB *rhp, const void *bufadr, int bufsiz ){

	int			leftsiz=0;												// 残りサイズ
	char		*txtalc, *txtp, *nxtp;									// テキストポインタ
	char		chr;													// バックアップ文字
	int			i;														// ループカウンタ

	if( bufadr && bufsiz ){
		if( rhp->leftp ){												// 前の残りが存在する
			leftsiz=strlen( rhp->leftp );								// 残りサイズ
		}
		txtalc=(char*)malloc( leftsiz+bufsiz+1 );						// テキストメモリ確保
		memcpy( txtalc, rhp->leftp, leftsiz );							// 残り文字列をコピー
		if( rhp->leftp ){
			dbMemFre( rhp->leftp );										// 残り文字列開放
			rhp->leftp=0;												// 残り文字列ポインタクリア
		}
		txtp=txtalc+leftsiz;											// テキストポインタセット
		for( i=0; i<bufsiz; i++ ){										// バッファサイズ分ループ
			chr=((char*)bufadr)[i];										// 一文字を得る
			if( chr!='\r' && chr!='\x1a' ){								// DOSの余計なコードは無視
				*txtp++=chr;											// 一文字セット
			}
		}
		*txtp=0;														// ０ターミネート
		txtp=txtalc;													// テキストポインタセット
		while( *txtp ){
			nxtp=strchr( txtp, '\n' );									// 文字列を走査し、指定された文字セットに含まれる文字を見つけます
			if( !nxtp ){												// 見つからない
				rhp->leftp=(char*)dbStrAlc( rhp->dbp, txtp );			// 残り文字列登録
				break;													// ループ終了
			}
			*nxtp=0;													// ０ターミネート
			dbAlc( rhp->dbp, txtp );									// 行登録
			txtp=nxtp+1;												// テキストポインタ更新
		}
		free( txtalc );													// テキストメモリ開放
	}
}

