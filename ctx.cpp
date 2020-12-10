/************************************************************************/
/*																		*/
/*	コンテキスト管理			by H.Watanabe							*/
/*	Ver1.00	2009/04/04 13:49:05	作成開始								*/
/*																		*/
/************************************************************************/

#	include	<stdio.h>													/* 標準入出力関数*/
#	include	<string.h>													/* 標準文字列関数*/
#	include	"main.h"													/* メインインクルードファイル*/

/************************************************************************/
/*	改行コードを調べる													*/
/************************************************************************/

static int	crtypchk( const char *fnamp ){

	int		crtyp=CRTYP_DEFAULT;										// 改行タイプ
	FILE	*fp;														// ファイルポインタ
	int		iosiz;														// 入出力サイズ
	char	txta[4096];													// バッファ
	void	*txtp;														// テキストポインタ

	fp=fopen( fnamp, "rb" );											// ファイルオープン
	if( fp ){															// ファイルオープン成功
		while( !feof( fp ) ){											// ファイルが終端でない間ループ
			iosiz=fread( txta, 1, sizeof(txta), fp );					// ファイルリード
			if( !iosiz ){												// ファイル終了
				break;
			}
			txtp=memchr( txta, 0x0d, iosiz );							// 0x0dを検索
			if( txtp ){													// 0x0dが見つかった
				crtyp=CRTYP_WIN;										// Windows
				break;
			}
			txtp=memchr( txta, 0x0a, iosiz );							// 0x0aを検索
			if( txtp ){													// 0x0aが見つかった
				crtyp=CRTYP_UNIX;										// Linux
				break;
			}
		}
		fclose( fp );													// ファイルクローズ
	}
	return( crtyp );													// 改行タイプを持ってリターン
}

/************************************************************************/
/*	コンテキストオープン												*/
/************************************************************************/

stCTX	*ctxOpen( const char *fnamp ){

	stCTX	*ctxp;														// 管理構造体
	FILE	*fp;														// ファイルポインタ
	char	txta[0x4000], *txtp;										// テキストバッファ、ポインタ
	int		crflg=1;													// 改行コードフラグ

	ctxp=(stCTX*)dbAlc( g_ctxdbp, fnamp, 0 );							// コンテキスト登録
	ctxp->lindbp=dbOpen( DBSIZ_VAR, sizeof(stLIN), 0, 0x10000 );		// 行DBオープン
	fp=fopen( fnamp, "r" );												// ファイルオープン
	if( fp ){															// ファイルオープン成功
		while( !feof( fp ) ){											// ファイル終端に達しない間ループ
			if( !fgets( txta, sizeof(txta), fp ) ){						// バッファは無効
				continue;
			}
			txtp=strpbrk( txta, "\x1a" );								// [EOF]検索
			if( txtp ){													// 見つかった
				*txtp=0;												// [EOF]クリア
			}
			txtp=strpbrk( txta, "\r\n" );								// 改行コード検索
			if( txtp ){													// 見つかった
				*txtp=0;												// 改行コードクリア
			}
			crflg=( txtp!=0 );											// 改行コードが無ければ１になる
			linAlcStr( ctxp->lindbp, txta );							// 行登録
		}
		fclose( fp );													// ファイルクローズ

#	ifdef	unix
		if( !stat( fnamp , &ctxp->st ) ){								// ファイルは存在している
			ctxp->statflg=1;											// ファイル存在フラグセット
		}
#	endif

	}
	if( crflg ){														// 最終行に改行コードが存在した
		linAlcStr( ctxp->lindbp, "" );									// [EOF]行登録
	}
	ctxp->crtyp=crtypchk( fnamp );										// 改行コードを調べる
	ctxp->pgp=ctxp->curp=(stLIN*)dbTop( ctxp->lindbp );
	return( ctxp );														// 管理構造体を持ってリターン
}

/************************************************************************/
/*	コンテキストクローズ												*/
/************************************************************************/

void	ctxClose( stCTX *ctxp ){

	stCTX	*p, *nxtp;													// コンテキストポインタ

	if( ctxp ){															// 一つのコンテキスト削除
		if( ctxp==g_ctxp ){												// クローズしようとしているのはカレント
			g_ctxp=(stCTX*)dbNxt( g_ctxp );								// カレントは次
			if( !g_ctxp ){												// 次は無かった
				g_ctxp=(stCTX*)dbTop( g_ctxdbp );						// カレントは先頭( NULL の場合もあるので注意 )
			}
		}
		dbClose( ctxp->lindbp );										// コンテキストの行DB破棄
		dbFre( g_ctxdbp, ctxp );										// 管理構造体メモリ開放
	}else{																// 全て削除
		dbFOR2( g_ctxdbp, p, nxtp, stCTX* ){							// コンテキスト総当りループ
			ctxClose( p );												// コンテキストクローズ
		}
	}
}

/************************************************************************/
/*	コンテキストセーブ													*/
/************************************************************************/

int		ctxSave( const stCTX *ctxp ){

	int		errn=0;														// エラー数
	char	*fnamp;														// ファイル名ポインタ
	char	baknam[256];												// バックアップファイル名
	FILE	*fp;														// ファイルポインタ
	stLIN	*linp;														// 行ポインタ
	char	*txtp;														// テキストポインタ

	fnamp=(char*)dbKEYADR( ctxp );										// ファイル名
	if( !g_optn ){														// *.bak ファイルを作成する
		sprintf( baknam, "%s.bak", fnamp );								// バックアップファイル名作成
		remove( baknam );												// 旧ファイル削除
		rename( fnamp, baknam );										// リネーム
	}
	fp=fopen( fnamp, "wb" );											// ファイル出力オープン
	if( fp ){															// ファイル出力オープン成功
		dbFOR( ctxp->lindbp, linp, stLIN* ){							// 形総当りループ
			txtp=(char*)dbKEYADR( linp );
			if( dbNxt( linp ) ){										// 終端行ではない
				if( ctxp->crtyp==CRTYP_UNIX ){							// UNIX
					fprintf( fp, "%s\n", txtp );						// 0a
				}else{													// WINDOWS
					fprintf( fp, "%s\r\n", txtp );						// 0d0a
				}
			}else{														// 終端行
				fprintf( fp, "%s", (char*)dbKEYADR( linp ) );
			}
		}
		fclose( fp );													// ファイル出力クローズ
#	ifdef	unix
		if( ctxp->statflg ){											// 前のファイルが存在している
			chmod( fnamp, ctxp->st.st_mode );							// ファイルモード変更
		}
#	endif

	}else{
		errn++;															// エラー数加算
	}
	return( !errn );													// 0:エラー, 1:正常終了
}

/************************************************************************/
/*	コンテキストリネーム												*/
/************************************************************************/

stCTX	*ctxRename( stCTX *ctxp, const char *fnamp ){

	stCTX	*newp;														// 新しいコンテキストポインタ

	newp=(stCTX*)dbAlc( g_ctxdbp, fnamp, 0 );							// コンテキスト登録
	memcpy( newp, ctxp, sizeof(stCTX) );								// 旧構造体を新構造体にコピー
	dbFre( g_ctxdbp, ctxp );											// 旧コンテキスト破棄
	return( newp );														// 新しいコンテキストポインタを持ってリターン
}

