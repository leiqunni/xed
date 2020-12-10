/************************************************************************/
/*																		*/
/*	テキストエディタのためのディスプレイリスト変換	by H.Watanabe		*/
/*	Ver1.00	2009/04/05 13:52:04	作成									*/
/*																		*/
/************************************************************************/

#	include	<stdio.h>													/* 標準入出力関数*/
#	include	<stdlib.h>													/* 標準ライブラリ関数*/
#	include	<string.h>													/* 標準文字列関数*/
#	include	"txtdl.h"													/* テキストエディタのためのディスプレイリスト変換*/

/************************************************************************/
/*	ローカル定義														*/
/************************************************************************/

#	ifndef	EUC
#		if	defined(_WIN32) || defined(__CYGWIN__)						/* SJIS*/
#			define	EUC			0
#		else															/* EUC*/
#			define	EUC			1
#		endif
#	endif

#	define	RNGTSTB(C,L,H)		((unsigned char)((C)-(L))<=(unsigned char)((H)-(L)))			/* 符号無し８ビットでレンジチェックをする*/

/************************************************************************/
/*	テキストインフォメーション											*/
/************************************************************************/

static void	txtinf( int *dspsizp, int *txtsizp, int chr, int xx, int tabstp ){

	if( chr&0x80 ){														// 漢字
#	if	!EUC															/* Windows の場合*/
		if( RNGTSTB( chr, 0xa0, 0xdf ) ){								// 半角かな
			*dspsizp=1;													// 表示サイズ
			*txtsizp=1;													// テキストサイズ
		}else{															// 全角漢字
			*dspsizp=2;													// 表示サイズ
			*txtsizp=2;													// テキストサイズ
		}
#	else																/* その他*/
		if( chr==0x8e ){												// 半角かな
			*dspsizp=1;													// 表示サイズ
		}else{															// 全角漢字
			*dspsizp=2;													// 表示サイズ
		}
		*txtsizp=2;														// テキストサイズ
#	endif
	}else{																// ASCII
		if( chr=='\t' ){												// タブ
			*dspsizp=((xx+tabstp)&-tabstp)-xx;							// 表示サイズ
		}else{															// 通常文字
			*dspsizp=1;													// 表示サイズ
		}
		*txtsizp=1;														// テキストサイズ
	}
}

/************************************************************************/
/*	フラッシュ															*/
/************************************************************************/

static void	flush( stDB *dldbp, stDB *tmpdbp ){

	stTXTDL		*soup, *nxtp;											// ソースポインタ, 次ポインタ
	stTXTDL		*tmpp;													// テンポラリポインタ
	stTXTDL		*dlp;													// ディスプレイリストポインタ
	char		*keyp;													// キーポインタ
	int			keysiz;													// キーサイズ

	if( tmpdbp ){														// テンポラリDBは有効
		soup=(stTXTDL*)dbTop( tmpdbp );									// 先頭
		while( soup ){
			keysiz=0;													// キーサイズクリア
			for( tmpp=soup; tmpp; tmpp=(stTXTDL*)dbNxt( tmpp ) ){		// 同じアトリビュートである間ループ
				if( tmpp->rvs!=soup->rvs || tmpp->col!=soup->col || tmpp->cur!=soup->cur ){		// アトリビュートが違う
					break;
				}
				keysiz+=dbKEYSIZ( tmpp );								// キーサイズ加算
			}
			nxtp=tmpp;													// 次を記録
			dlp=(stTXTDL*)dbAlc( dldbp, 0, keysiz, 0 );					// ディスプレイリスト登録
			dlp->rvs=soup->rvs;											// 反転フラグ
			dlp->col=soup->col;											// カラー( 0:テキスト, 1:[EOF]等 )
			dlp->cur=soup->cur;											// カーソルフラグ
			keyp=(char*)dbKEYADR( dlp );								// キーポインタを得る
			for( tmpp=soup; tmpp!=nxtp; tmpp=(stTXTDL*)dbNxt( tmpp ) ){	// 次に等しくない間ループ
				memcpy( keyp, dbKEYADR( tmpp ), dbKEYSIZ( tmpp ) );		// キーをコピー
				keyp+=dbKEYSIZ( tmpp );									// キーポインタシーク
				dlp->dspsiz+=tmpp->dspsiz;								// 表示サイズ加算
			}
			soup=nxtp;													// 次にシーク
		}
		dbClose( tmpdbp );												// テンポラリDB破棄
	}
}

/************************************************************************/
/*	ディスプレイリスト登録												*/
/************************************************************************/

static stTXTDL	*dlalc( stDB *dldbp, const char *keyadr, int keysiz, int dspsiz, int rvs, int cur, int col ){

	stTXTDL		*dlp;													// ディスプレイリストポインタ

	dlp=(stTXTDL*)dbAlc( dldbp, keyadr, keysiz, 0 );					// DB登録
	dlp->dspsiz=dspsiz;													// 表示サイズ
	dlp->rvs=rvs;														// 反転フラグ
	dlp->col=col;														// カラー( 0:テキスト, 1:[EOF]等 )
	dlp->cur=cur;														// カーソルフラグ
	return( dlp );														// ディスプレイリストポインタを持ってリターン
}

/************************************************************************/
/*	テキストエディタのためのディスプレイリスト変換						*/
/************************************************************************/

//	txtp:	テキストアドレス
//	curxx:	カーソル座標
//	tabstp:	タブステップ
//	tabdsp:	タブ表示フラグ( 0:非表示, 1:表示 )
//	eof:	エンドオブファイルフラグ( 0:行端, 1:ファイル終端 )
//	eofdsp:	終端記号表示( 0:非表示, 1:表示 )
//	pgxx:	座標スクロール値
//	findsiz:検索結果のサイズ( 0:無し )
//	xxw:	画面表示幅

stDB	*txtdl( const char *txtp, int curxx, int tabstp, int tabdsp, int eof, int eofdsp, int pgxx, int findsiz, int xxw ){

	static const char	*eofpa[2]={ ".", "[EOF]" };						// 終端文字列
	stDB		*dldbp;													// ディスプレイリストDB
	stDB		*tmpdbp=0;												// テンポラリディスプレイリストDB
	stTXTDL		*tabdlp=0;												// タブディスプレイリストポインタ
	const char	*findp=0;												// 検索ポインタ
	int			chr;													// カレント文字
	int			dspsiz, txtsiz;											// 表示サイズ、テキストサイズ
	int			xx=0;													// 表示座標
	const char	*eofp;													// "." または "[EOF]" へのポインタ
	int			rvs;													// 反転フラグ
	int			cur;													// カーソルフラグ
	int			find;													// 検索フラグ
	int			col;													// カラー( 0:テキスト, 1:[EOF]等 )

	dldbp=dbOpen( DBSIZ_VAR, sizeof(stTXTDL), 0, 4096 );				// ディスプレイリストDBオープン
	for(;;){
		chr=(unsigned char)*txtp;										// 一文字得る
		if( !chr || xx>=pgxx+xxw ){										// テキスト終了か範囲外
			break;
		}
		txtinf( &dspsiz, &txtsiz, chr, xx, tabstp );					// テキストインフォメーション
		cur=( xx==curxx );												// カーソルフラグ
		if( cur ){														// カーソルポジション
			findp=txtp;													// 検索ポインタ先頭セット
		}
		col=find=( txtp>=findp && txtp<findp+findsiz );					// 検索フラグセット
		rvs=( find || cur );											// 反転フラグ
		if( xx==pgxx || ( xx==pgxx-1 && dspsiz==2 ) ){					// 座標は画面表示範囲に入った
			flush( dldbp, tmpdbp );										// フラッシュ
			tmpdbp=0;													// テンポラリDBクリア
			tabdlp=0;													// タブ終了
		}
		if( chr=='\t' ){												// タブ
			flush( dldbp, tmpdbp );										// フラッシュ
			tmpdbp=0;													// テンポラリDBクリア
			if( !tabdsp ){												// タブ非表示
				if( cur ){												// カーソル位置である
					tabdlp=0;											// タブ終了
					dlalc( dldbp, " ", 1, 1, rvs, cur, col );			// カーソル表示
					dlalc( dldbp, 0, 0, dspsiz-1, find, 0, col );		// 残りのスペース表示
				}else{
					if( !tabdlp ){
						tabdlp=dlalc( dldbp, 0, 0, 0, rvs, cur, col );	// ディスプレイリスト登録
					}
					tabdlp->dspsiz+=dspsiz;								// タブ加算
				}
			}else{														// タブ表示
				tabdlp=0;												// タブ終了
				dlalc( dldbp, ">", 1, 1, rvs, cur, 1 );					// ディスプレイリスト登録
				dlalc( dldbp, "---------------", dspsiz-1, dspsiz-1, 0, cur, 1 );				// ディスプレイリスト登録
			}
		}else{															// タブ以外の文字
			tabdlp=0;													// タブ終了
			if( !tmpdbp ){												// テンポラリDBは無効
				tmpdbp=dbOpen( DBSIZ_VAR, sizeof(stTXTDL), 0, 4096 );	// テンポラリDBオープン
			}
			dlalc( tmpdbp, txtp, txtsiz, dspsiz, rvs, cur, col );		// テンポラリDBに通常文字登録
		}
		txtp+=txtsiz;													// テキストポインタシーク
		xx+=dspsiz;														// 座標シーク
	}
	flush( dldbp, tmpdbp );												// フラッシュ
	if( xx<pgxx+xxw ){													// 画面範囲内
		cur=rvs=( xx==curxx );											// カーソルフラグ
		if( eofdsp ){													// [EOF]表示はON
			eofp=eofpa[ eof!=0 ];										// "." または "[EOF]" へのポインタ
			dspsiz=strlen( eofp );
		}else{															// [EOF]表示はOFF
			eofp="";
			dspsiz=1;													// カーソル表示
		}
		dlalc( dldbp, eofp, strlen( eofp ), dspsiz, rvs, cur, 1 );		// ディスプレイリスト登録
		xx+=dspsiz;														// 座標シーク
		if( xx<pgxx && curxx>=0 ){										// ページがスクロールしていて画面未満で終わっている
			dlalc( dldbp, 0, 0, pgxx-xx, 0, 0, 0 );						// 表示位置をシークするためのダミー
			dlalc( dldbp, 0, 0, 1, 1, 1, 1 );							// カーソル表示
		}
	}
	return( dldbp );
}

