/************************************************************************/
/*																		*/
/*	置換・検索					by H.Watanabe							*/
/*	Ver1.00	2009/04/04 13:49:05	作成開始								*/
/*																		*/
/************************************************************************/

#	include	<stdio.h>													/* 標準入出力関数*/
#	include	<stdlib.h>													/* 標準ライブラリ関数*/
#	include	<string.h>													/* 標準文字列関数*/
#	include	"std/search2.h"												/* 検索*/
#	include	"std/lm.h"													/* ラインマップ*/
#	include	"main.h"													/* メインインクルードファイル*/

/************************************************************************/
/*	文字列置換															*/
/************************************************************************/

static void	repstr( char *dstp, const char *txtadr, int txtofs, int txtsiz, const char *newp ){
	memcpy( dstp, txtadr, txtofs );
	dstp+=txtofs;
	strcpy( dstp, newp );
	dstp+=strlen( dstp );
	strcpy( dstp, txtadr+txtofs+txtsiz );
}

/************************************************************************/
/*	置換・検索															*/
/************************************************************************/

void	replace( const char *orgp, const char *newp, int bakflg, int chkflg, int notdsp ){

	stLIN		*linp;													// 行ポインタ
	char		*txtadr;												// テキスト先頭アドレス
	char		*txtp;													// テキストポインタ
	stLM		lm;														// 行マップ
	char		tmpa[0x10000];											// テンポラリバッファ
	int			curxx;													// カーソル横座標
	int			lno;													// 行番号
	int			yn;														// イエス、ノーリザルト
	int			txtofs, newofs;											// テキストオフセット
	int			searchlen;												// 検索文字列長
	int			curbak;													// カーソル表示情報バックアップ

	linp	=CURP;														// 行ポインタ
	txtadr	=CURTXTP;													// テキストアドレス
	lmMak( &lm, txtadr, g_tabstp, 0 );									// 行マップ作成
	curxx	=lmPosAdj( &lm, CURXX, 0 );									// カーソル位置正規化
	lno=PGYY+CURYY;														// 行番号
	txtp=txtadr+lm.txtofsa[curxx];										// テキストポインタセット
	if( !newp ){														// 検索の場合
		if( !bakflg ){													// 前方
			if( *txtp ){												// 文字がある
				txtp++;													// テキストポインタインクリメント
			}
		}else{															// 後方
			if( curxx ){												// 行先頭ではない
				txtp--;													// 左へ
			}else{														// 行先頭
				txtadr=txtp="";											// ヌル行をポイント
			}
		}
	}
	for(;;){
		txtp=searchLin( txtadr, txtp, orgp, bakflg, g_flglbl, g_flgwld, g_flgc2l );
		if( txtp ){														// 見つかった
			if( g_flgwld ){												// ワイルドカードフラグが立っている
				searchlen=searchWld( txtp, orgp, g_flgc2l )-txtp;		// 検索文字列長
			}else{														// ワイルドカード無効
				searchlen=strlen( orgp );								// 検索文字列長
			}
			g_scdbiz=searchlen;											// グローバルワークセット
			lnoset( lno );												// 行番号セット
			if( !notdsp ){												// 表示有り
				lmMak( &lm, txtadr, g_tabstp, 0 );						// 行マップ作成
				CURXX=lmOfs2Pos( &lm, txtp-txtadr );					// カーソル座標を得る
				linAdjScr();											// 横スクロール正規化
				invalidateAll();										// 全て再表示
			}
			if( !newp ){												// 検索の場合
				break;													// ループ終了
			}
			yn=YN_YES;
			if( chkflg ){												// 確認有り
				curbak=curSw( 1 );										// カーソルオン
				yn=YorN( "Replace ? (Y/N)" );							// 確認
				curSw( curbak );										// カーソルオフ
				if( yn==YN_CANCEL ){									// キャンセルの場合
					break;												// ループ終了
				}
			}
			if( yn==YN_YES ){											// 変換実行
				txtofs=txtp-txtadr;										// テキストオフセット
				newofs=txtofs+strlen( newp );							// 新しいオフセット
				repstr( tmpa, txtadr, txtp-txtadr, searchlen, newp );	// 文字列置換
				linp=linflush( linp, tmpa );							// テキスト更新
				linchg( linp );											// 行変更セット
				txtadr=(char*)dbKEYADR( linp );							// テキストアドレス更新
				if( !bakflg ){											// 前方
					txtp=txtadr+newofs;
				}else{													// 後方
					if( txtofs ){										// 行先頭ではない
						txtp=txtadr+txtofs-1;							// テキストポインタセット
					}else{												// 行先頭の場合
						txtadr=txtp="";									// ヌル行をポイント
					}
				}
				if( !notdsp ){											// 表示有り
					invalidateAll();									// 全て再表示
				}
			}else{
				if( !bakflg ){											// 前方
					txtp+=searchlen;									// テキストポインタ加算
				}else{													// 後方
					txtp-=searchlen;									// テキストポインタ減算
				}
			}
		}else{															// 見つからなかった
			if( !bakflg ){												// 前方
				linp=(stLIN*)dbNxt( linp );								// フォワード
				if( !linp ){											// ファイル終端
					break;												// 終了
				}
				txtadr=(char*)dbKEYADR( linp );							// テキスト先頭アドレスセット
				txtp=txtadr;											// テキストポインタを行先頭にセット
				lno++;													// 行番号インクリメント
			}else{														// 後方
				linp=(stLIN*)dbBak( linp );								// バック
				if( !linp ){											// ファイル先頭
					break;												// 終了
				}
				txtadr=(char*)dbKEYADR( linp );							// テキスト先頭アドレスセット
				txtp=txtadr+strlen( txtadr );							// テキストポインタを行終端にポイント
				lno--;													// 行番号デクリメント
			}
		}
	}
	if( notdsp ){
		invalidateAll();												// 全て再表示
	}
}

