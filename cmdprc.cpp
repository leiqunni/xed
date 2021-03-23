/************************************************************************/
/*																		*/
/*	コマンド処理				by H.Watanabe							*/
/*	Ver1.00	2009/04/04 13:49:05	作成開始								*/
/*																		*/
/************************************************************************/

#	include	<stdio.h>													/* 標準入出力関数*/
#	include	<stdlib.h>													/* 標準ライブラリ関数*/
#	include	<string.h>													/* 標準文字列関数*/
#	include	"std/filedb.h"												/* データベースファイル登録*/
#	include	"std/clpbrd.h"												/* クリップボード管理*/
#	include	"std/filterdb.h"											/* フィルター処理*/
#	include	"std/tagparse.h"											/* タグ解析*/
#	include	"std/envcnv3.h"												/* 環境変数変換*/
#	include	"std/eval.h"												/* 数式評価( Evaluation of Expressions )*/
#	include	"std/evalf.h"												/* 数式評価*/
#	include	"main.h"													/* メインインクルードファイル*/

/************************************************************************/
/*	カーソル座標セット( マウス左ボタン押し下げから呼ばれる )			*/
/************************************************************************/

void	curset( int musx, int musy ){

	int			xx, yy;													// キャラクタ座標
	int			seekn;													// 実際にシークした数
	stLIN		*linp;													// 行ポインタ
	stLM		lm;														// 行マップ

	xx=musx/g_fxw-g_txxc;												// キャラクタX座標
	yy=musy/g_fyw-g_tyyc;												// キャラクタY座標
	linp=(stLIN*)dbSeek( &seekn, PGP, yy );								// 次のページ先頭までシークしてみる
	if( seekn==yy ){													// シークできた
		lmMak( &lm, (char*)dbKEYADR( linp ), g_tabstp, 0 );				// 行マップ作成
		if( xx+PGXX<=lm.siz ){											// 有効範囲内
			CURP=linp;													// カーソルポインタ
			CURYY=yy;													// カーソルY座標
			CURXX=PGXX+xx;												// カーソルX座標
			invalidateAll();											// 全て再描画
		}
	}
}

/************************************************************************/
/*	選択モードで選択先頭より上の行で作業をしたら選択モードを解除する	*/
/************************************************************************/

static void	cutchk(void){
	if( CUTCURP ){
		if( PGYY+CURYY<=CUTPGYY+CUTCURYY ){
			CUTCURP=0;													// 範囲指定終了
			invalidateAll();											// 全て再描画
		}
	}
}

/************************************************************************/
/*	行変更セット														*/
/************************************************************************/

void	linchg( stLIN *linp ){
	linp->chgflg=1;														// 行変更フラグセット
	FILCHG=1;															// ファイル変更フラグセット
}

/************************************************************************/
/*	行変更セットと再表示												*/
/************************************************************************/

static void	linchgdsp( stLIN *linp, int yy ){
	if( !linp->chgflg ){												// 行変更フラグは立っていない
		linp->chgflg=1;													// 行変更フラグセット
		invalidateLno( yy );											// 行番号再表示
	}
	if( !FILCHG ){														// ファイル変更フラグは立っていない
		FILCHG=1;														// ファイル変更フラグセット
		invalidateFil();												// ファイル再表示
	}
}

/************************************************************************/
/*	テキスト更新														*/
/************************************************************************/

stLIN	*linflush( stLIN *orgp, const char *txtp ){

	stLIN		*linp;													// 行ポインタ
	int			orgsiz, newsiz;											// 行サイズ

	orgsiz=dbKEYSIZ( orgp );											// 元の行サイズ
	newsiz=linAlcSizStr( txtp );										// 新しい行サイズ
	if( orgsiz==newsiz ){												// 行サイズは同じ
		strcpy( (char*)dbKEYADR( orgp ), txtp );						// そのままコピー
		linp=orgp;														// 行ポインタは元の行ポインタ
	}else{																// 行サイズは違う
		linp=linAlcStr( LINDBP, txtp );									// 行登録
		dbMov( LINDBP, linp, orgp );									// 下の行の前に移動
		if( CUTPGP==orgp ){												// カットページポインタの場合
			CUTPGP=linp;												// カットページポインタ更新
		}
		if( CUTCURP==orgp ){											// カットカレントポインタの場合
			CUTCURP=linp;												// カットカレントポインタ更新
		}
		if( PGP==orgp ){												// ページポインタの場合
			PGP=linp;													// ページポインタ更新
		}
		if( CURP==orgp ){												// カレントポインタの場合
			CURP=linp;													// カレントポインタ更新
		}
		dbFre( LINDBP, orgp );											// 元の業界方
	}
	return( linp );														// 行ポインタを持ってリターン
}

/************************************************************************/
/*	カレントテキスト更新												*/
/************************************************************************/

stLIN	*renewtxt( stLIN *orgp, int yy, const char *txtp ){

	stLIN	*linp;														// 行ポインタ

	linp=linflush( orgp, txtp );										// テキスト更新
	linchgdsp( linp, yy );												// 行変更セットと再表示
	return( linp );
}

/************************************************************************/
/*	コンテキスト変更													*/
/************************************************************************/

static void	ctxchg( int ofs ){

	stCTX	*ctxp;														// コンテキストポインタ

	if( ofs<0 ){														// 後方
		ctxp=(stCTX*)dbBak( g_ctxp );									// 前
		if( !ctxp ){													// 前はない
			ctxp=(stCTX*)dbEnd( g_ctxdbp );								// 先頭
		}
	}else{																// 前方
		ctxp=(stCTX*)dbNxt( g_ctxp );									// 次
		if( !ctxp ){													// 次はない
			ctxp=(stCTX*)dbTop( g_ctxdbp );								// 終端
		}
	}
	if( g_ctxp != ctxp ){												// 変更があった
		g_ctxp=ctxp;													// カレントコンテキスト変更
		invalidateAll();												// 全て再描画
	}
}

/************************************************************************/
/*	行番号セット														*/
/************************************************************************/

void	lnoset( int lno ){

	int			seekn;													// 実際にシークした数
	int			pgseekn;												// ページシーク数
	int			linn;													// 総行数

	if( lno>=PGYY && lno<PGYY+g_tyyd ){									// 表示範囲内
		CURP=(stLIN*)dbSeek( &seekn, PGP, lno-PGYY );					// 次のページ先頭までシークしてみる
		CURYY=seekn;
	}else{																// 表示範囲外
		pgseekn=(lno/g_tyyd)*g_tyyd;									// ページシーク数計算
		linn=dbAlcN( LINDBP );											// 総行数
		if( pgseekn>linn-g_tyyd ){										// ページシーク数が総行数－画面行数より大きい
			pgseekn=linn-g_tyyd;										// クランプ
		}
		PGP=(stLIN*)dbSeek( &seekn, dbTop( LINDBP ), pgseekn );			// 先頭からページシーク
		PGYY=seekn;														// ページ先頭行番号セット
		CURP=(stLIN*)dbSeek( &seekn, PGP, lno-seekn );					// カレントポインタシーク
		CURYY=seekn;													// カレントＹセット
		invalidateAll();												// 全て再描画
	}
}

/************************************************************************/
/*	ページスクロール													*/
/************************************************************************/

void	pgscroll( int ofs ){

	int			seekn;													// 実際にシークした数
	int			pgseekn;												// ページシーク数
	int			linseekn=ofs;											// ラインシーク数
	int			dspflg=0;												// 表示フラグ

	if( ofs<0 ){														// 後方
		PGP=(stLIN*)dbSeek( &seekn, PGP, linseekn );					// 次のページ先頭までシークしてみる
		PGYY+=seekn;													// ページY更新
		CURP=(stLIN*)dbSeek( 0, CURP, seekn );							// カレントポインタ更新
		linseekn-=seekn;												// ラインシーク数減算
		dspflg=( seekn!=0 );											// 表示フラグセット
		CURP=(stLIN*)dbSeek( &seekn, CURP, linseekn );					// 次のページ先頭までシークしてみる
		CURYY+=seekn;													// カレントY更新
		dspflg|=( seekn!=0 );											// 表示フラグセット
	}else if( ofs ){													// 前方
		dbSeek( &seekn, PGP, ofs+g_tyyd-1 );							// 次のページ先頭までシークしてみる
		pgseekn=seekn-(g_tyyd-1);										// ページシーク数
		if( pgseekn>0 ){												// ページシーク可能.
			PGP=(stLIN*)dbSeek( 0, PGP, pgseekn );						// 次のページ先頭までシークしてみる
			PGYY+=pgseekn;												// ページY更新
			CURP=(stLIN*)dbSeek( 0, CURP, pgseekn );					// カレントポインタ更新
			linseekn-=pgseekn;											// ラインシーク数減算
			dspflg=1;													// 表示フラグセット
		}
		CURP=(stLIN*)dbSeek( &seekn, CURP, linseekn );					// カレントポインタシーク
		CURYY+=seekn;													// カレントY更新
		dspflg|=( seekn!=0 );											// 表示フラグセット
	}
	if( dspflg ){														// 表示フラグが立っている
		invalidateAll();												// 全て再描画
	}
}

/************************************************************************/
/*	ラインスクロール													*/
/************************************************************************/

static void	linscroll( int ofs ){

	int			seekn;													// 実際にシークした数

	if( ofs<0 ){														// 後方
		if( dbBak( PGP ) ){												// 先頭行ではない
			PGP=(stLIN*)dbBak( PGP );									// 前へ
			if( CURYY==g_tyyd-1 ){										// 最下行
				CURP=(stLIN*)dbBak( CURP );								// カレントポインタも前へ
			}else{														// 最下行ではない
				CURYY++;												// カーソルＹ座標インクリメント
			}
			PGYY--;														// ページ先頭行番号デクリメント
			invalidateAll();											// 全て再描画
		}
	}else{																// 前方
		dbSeek( &seekn, PGP, g_tyyd );									// 次のページ先頭までシークしてみる
		if( seekn==g_tyyd ){											// シーク成功
			PGP=(stLIN*)dbNxt( PGP );									// 次へ
			if( !CURYY ){												// ページ先頭行
				CURP=PGP;												// カレントポインタ設定
			}else{														// ページ先頭行ではない
				CURYY--;												// カーソルＹ座標デクリメント
			}
			PGYY++;														// ページ先頭行番号インクリメント
			invalidateAll();											// 全て再描画
		}
	}
}

/************************************************************************/
/*	左端に移動															*/
/************************************************************************/

static void	curtop(void){
	CURXX=0;															// カーソルを左端にセット
	if( linAdjScr() ){													// 横スクロールの正規化
		invalidateAll();												// 全て再描画
	}
}

/************************************************************************/
/*	右端に移動															*/
/************************************************************************/

static void	curend(void){

	stLM		lm;														// 行マップ

	lmMak( &lm, CURTXTP, g_tabstp, 0 );									// 行マップ作成
	CURXX=lm.siz;														// カーソルを右端にセット
	if( linAdjScr() ){													// 横スクロールの正規化
		invalidateAll();												// 全て再描画
	}
}

/************************************************************************/
/*	カーソル↑															*/
/************************************************************************/

static void	curup(void){
	if( dbBak( CURP ) ){												// 先頭行ではない
		if( !CURYY ){													// ページ先頭行
			linscroll( -1 );											// スクロール
		}
		CURP=(stLIN*)dbBak( CURP );										// 編集行変更
		CURYY--;														// ローカル行番号インクリメント
		if( CUTCURP ){													// 範囲指定中
			invalidateLin( CURYY+1 );									// 旧行再表示
		}
		invalidateLin( CURYY+2 );										// 旧行再表示
	}
}

/************************************************************************/
/*	カーソル↓															*/
/************************************************************************/

static void	curdown(void){
	if( dbNxt( CURP ) ){												// 最終行ではない
		if( CURYY==g_tyyd-1 ){											// ページ最終行
			linscroll( 1 );												// スクロール
		}
		CURP=(stLIN*)dbNxt( CURP );										// 編集行変更
		CURYY++;														// ローカル行番号インクリメント
		if( CUTCURP ){													// 範囲指定中
			invalidateLin( CURYY-1 );									// 旧行再表示
		}
	}
}

/************************************************************************/
/*	カーソル←															*/
/************************************************************************/

static int	curleft( int wordflg ){

	int			res=0;													// 0:移動不可, 1:移動可能.
	stLM		lm;														// 行マップ

	lmMak( &lm, CURTXTP, g_tabstp, 0 );									// 行マップ作成
	CURXX=lmPosAdj( &lm, CURXX, 0 );									// カーソル位置正規化
	if( CURXX ){														// 行先頭ではない
		lmMak( &lm, CURTXTP, g_tabstp, wordflg );						// 行マップ作成
		CURXX=lmPosAdj( &lm, CURXX, -1 );								// カーソル左移動
		res=1;															// 移動可能.
	}else{																// 行先頭
		if( dbBak( CURP ) ){											// 先頭行ではない
			curup();													// カーソル↑
			curend();													// 右端に移動
			res=1;														// 移動可能.
		}
	}
	if( linAdjScr() ){													// 横スクロールの正規化
		invalidateAll();												// 全て再描画
	}
	return( res );
}

/************************************************************************/
/*	カーソル→															*/
/************************************************************************/

static int	curright( int wordflg ){

	int			res=0;													// 0:移動不可, 1:移動可能.
	stLM		lm;														// 行マップ

	lmMak( &lm, CURTXTP, g_tabstp, 0 );									// 行マップ作成
	CURXX=lmPosAdj( &lm, CURXX, 0 );									// カーソル位置正規化
	if( CURXX!=lm.siz ){												// 行終端ではない
		lmMak( &lm, CURTXTP, g_tabstp, wordflg );						// 行マップ作成
		CURXX=lmPosAdj( &lm, CURXX, 1 );								// カーソル右移動
		res=1;															// 移動可能.
	}else{																// 行先頭
		if( dbNxt( CURP ) ){											// 終端行ではない
			curdown();													// カーソル↓
			curtop();													// 左端に移動
			res=1;														// 移動可能.
		}
	}
	if( linAdjScr() ){													// 横スクロールの正規化
		invalidateAll();												// 全て再描画
	}
	return( res );
}

/************************************************************************/
/*	行挿入																*/
/************************************************************************/

static void	inslin( const char *txtp ){

	stLIN		*linp;													// カレント行ポインタ

	linp=linAlcStr( LINDBP, txtp );										// バッファを複写
	dbMov( LINDBP, linp, CURP );										// 前半
	if( PGP==CURP ){													// ページ行先頭
		PGP=linp;														// ページ先頭変更
	}
	CURP=linp;															// カレントポインタ変更
	linchg( linp );														// 行変更セット
	invalidateAll();													// 全て再描画
}

/************************************************************************/
/*	改行																*/
/************************************************************************/

static void	cr(void){

	stLM		lm;														// 行マップ
	int			curxx;													// カレントX座標
	int			cutofs;													// カレントオフセット
	stLIN		*curp;													// カレント行ポインタ
	stLIN		*nxtp;													// ネクスト行ポインタ

	cutchk();															// 選択モードで選択先頭より上の行で作業をしたら選択モードを解除する
	lmMak( &lm, CURTXTP, g_tabstp, 0 );									// 行マップ作成
	curxx=lmPosAdj( &lm, CURXX, 0 );									// カーソル位置正規化
	cutofs=lm.txtofsa[curxx];											// カットオフセット
	curp=linAlc( LINDBP, CURTXTP, cutofs );								// 改行以前を登録
	nxtp=linAlcStr( LINDBP, CURTXTP+cutofs );							// 改行以降を登録
	dbMov( LINDBP, curp, CURP );										// 前半
	dbMov( LINDBP, nxtp, CURP );										// 後半
	if( PGP==CURP ){													// ページ行先頭
		PGP=curp;														// ページ先頭変更
	}
	dbFre( LINDBP, CURP );												// 旧カレント行破棄
	CURP=nxtp;															// カレントポインタ変更
	CURXX=0;															// カーソルは左端に
	CURYY++;															// カレントY座標加算
	if( CURYY>=g_tyyd ){												// スクリーン範囲外となった
		linscroll( 1 );													// スクロール
	}
	linchg( curp );														// 行変更セット
	linchg( CURP );														// 行変更セット
	invalidateAll();													// 全て再描画
}

/************************************************************************/
/*	削除																*/
/************************************************************************/

static void	del( int wordflg ){

	stLM		lm;														// 行マップ
	int			souxx;													// ソース座標
	stLIN		*nxtp;													// 次の行ポインタ
	int			cursiz, nxtsiz;											// カレントサイズ、ネクストサイズ
	char		tmpa[0x10000];											// テンポラリバッファ
	stLIN		*linp;													// 行ポインタ
	char		*dstp;													// ディストネーションポインタ

	cutchk();															// 選択モードで選択先頭より上の行で作業をしたら選択モードを解除する
	lmMak( &lm, CURTXTP, g_tabstp, 0 );									// 行マップ作成
	CURXX=lmPosAdj( &lm, CURXX, 0 );									// カーソル位置正規化
	if( CURXX!=lm.siz ){												// 行終端ではない
		dstp=CURTXTP+lm.txtofsa[CURXX];									// ディストネーションポインタセット
		lmMak( &lm, CURTXTP, g_tabstp, wordflg );						// 行マップ作成
		souxx=lmPosAdj( &lm, CURXX, 1 );								// ソース座標
		strcpy( dstp, CURTXTP+lm.txtofsa[souxx] );						// ディストネーションポインタにコピー
		renewtxt( CURP, CURYY, CURTXTP );								// カレントテキスト更新
	}else{																// 行先頭
		if( !wordflg ){													// ワードモードではない
			nxtp=(stLIN*)dbNxt( CURP );									// 次のポインタを得る
			if( nxtp ){													// 終端行ではない
				cursiz=strlen( CURTXTP );								// カレントサイズ
				nxtsiz=strlen( (char*)dbKEYADR( nxtp ) );				// ネクストサイズ
				if( cursiz+nxtsiz<(int)sizeof(tmpa)-1 ){				// 合計サイズOK
					strcpy( tmpa, CURTXTP );							// カレントテキストコピー
					strcat( tmpa, (char*)dbKEYADR( nxtp ) );			// 次の行をジョイント
					linp=linAlcStr( LINDBP, tmpa );						// 行登録
					dbMov( LINDBP, linp, CURP );						// カレントポインタの前に移動
					dbFre( LINDBP, nxtp );								// 次の行を削除
					dbFre( LINDBP, CURP );								// カレント行削除
					CURP=linp;											// カレントポインタ更新
					if( !CURYY ){										// ページ先頭行
						PGP=linp;										// ページポインタ更新
					}
					linchg( CURP );										// 行変更セット
					invalidateAll();									// 全て再描画
				}else{													// 文字列が長すぎる
					msgSet( "Line Too Long" );							// 文字列が長すぎる
				}
			}
		}
	}
}

/************************************************************************/
/*	一文字セット														*/
/************************************************************************/

void	setchr( int xedk ){

	char		txta[0x10000];											// テキストバッファ
	stLIN		*linp;													// 行ポインタ
	int			toplno, curlno;											// 先頭行番号, カレント行番号
	int			lno;													// 行番号
	int			curxx=CURXX;											// カーソルＸ

	toplno=CUTPGYY+CUTCURYY;											// 先頭行番号
	curlno=PGYY+CURYY;													// カレント行番号
	if( CUTCURP && !CUTTYP && curlno>toplno ){							// 選択モード中である
		linp=CUTCURP;													// 行ポインタは選択先頭
		for( lno=toplno; lno<curlno; lno++ ){							// 行番号ループ
			strcpy( txta, (char*)dbKEYADR( linp ) );					// 一旦テキストバッファにコピー
			curxx=lmSetChr( txta, CURXX, xedk, g_tabstp, g_flgins );	// 一文字セット
			linp=linflush( linp, txta );								// テキスト更新
			linchg( linp );												// 行変更セット
			linp=(stLIN*)dbNxt( linp );									// 行ポインタ更新
		}
		CURXX=curxx;													// カーソルＸ変更
		linAdjScr();													// 横スクロールの正規化
		invalidateAll();												// 全て再描画
	}else{
		strcpy( txta, CURTXTP );										// 一旦テキストバッファにコピー
		CURXX=lmSetChr( txta, CURXX, xedk, g_tabstp, g_flgins );		// カーソル位置正規化
		renewtxt( CURP, CURYY, txta );									// カレントテキスト更新
		if( linAdjScr() ){												// 横スクロールの正規化
			invalidateAll();											// 全て再描画
		}
	}
}

/************************************************************************/
/*	カレントワードを得る												*/
/************************************************************************/

void	getcurword( char *dstp ){

	stLM		lm;														// 行マップ
	int			xx;														// Ｘ座標
	int			mapval;													// 行マップ値
	char		*txtp, *nxtp;											// テキストポインタ

	*dstp=0;															// とりあえずディストネーションクリア
	lmMak( &lm, CURTXTP, g_tabstp, 0 );									// 行マップ作成
	xx=lmPosAdj( &lm, CURXX, 0 );										// カーソル位置正規化
	if( xx!=lm.siz ){													// 行終端ではない
		txtp=CURTXTP+lm.txtofsa[xx];									// 先頭テキストポインタ
		lmMak( &lm, CURTXTP, g_tabstp, 1 );								// ワードモードで行マップ作成
		mapval=lm.txtofsa[xx];											// 行マップ値を得る
		while( lm.txtofsa[xx]==mapval ){								// 行マップ値が同じの間ループ
			xx++;														// カーソル位置インクリメント
		}
		nxtp=CURTXTP+lm.txtofsa[xx];									// 次のテキストポインタ
		memcpy( dstp, txtp, nxtp-txtp );								// 先頭から次のテキストポインタまでをコピー
		dstp[nxtp-txtp]=0;												// ０ターミネート
	}
}

/************************************************************************/
/*	検索文字列を得る													*/
/************************************************************************/

static int	scdbtrget( char *dstp ){

	int		res=0;														// リザルト
	stDB	*cbdbp;														// クリップボードDB
	char	*cbp;														// クリップボードポインタ

	*dstp=0;															// とりあえず０ターミネート
	cbdbp=clpbrdGet( CLPBRDP, CB_SEARCH );								// クリップボード取得
	if( cbdbp ){														// クリップボード取得成功
		cbp=(char*)dbTop( cbdbp );										// 先頭テキスト
		if( cbp ){														// 先頭テキストが存在する
			strcpy( dstp, cbp );										// ディストネーションへコピー
			res=1;														// リザルトセット
		}
		dbClose( cbdbp );												// クリップボードDBクローズ
//	}else{																// クリップボード取得失敗
//		printf( "クリップボードエラー\n" );
	}
	return( res );														// 0:失敗, 1:成功
}

/************************************************************************/
/*	検索文字列をクリップボードに書き込む								*/
/************************************************************************/

static void	scdbtrput( const char *soup ){

	stDB	*cbdbp;														// クリップボードDB

	cbdbp=dbOpen( 0, DBSIZ_STR, 0, 0 );									// クリップボードDBオープン
	dbAlc( cbdbp, soup );												// 文字列登録
	clpbrdPut( CLPBRDP, CB_SEARCH, cbdbp, 0, 0 );						// クリップボード書き込み
	dbClose( cbdbp );													// クリップボードDBクローズ
}

/************************************************************************/
/*	クリップボードにテキストを書き込む									*/
/************************************************************************/

static void	cbtxtput( const stDB *cbdbp, int rctmod ){

	stDB	*schdbp;													// 検索DB

	schdbp=clpbrdGet( CLPBRDP, CB_SEARCH );								// 検索文字列を得る
	clpbrdPut( CLPBRDP, CB_TXT, cbdbp, rctmod, 1 );						// クリップボードプット
	clpbrdPut( CLPBRDP, CB_SEARCH, schdbp, 0, 0 );						// クリップボードプット
	dbClose( schdbp );													// 検索DBクローズ
#	ifndef	_WIN32
#		ifndef	CURSES
	xselOwner( g_xselp, g_tim );										// セレクションオーナー宣言
#		endif
#	endif
}

/************************************************************************/
/*	シェル																*/
/************************************************************************/

static void	shell( const char txta[] ){

	stDBLNK			*bakpgp, *bakcurp;									// DBリンクポインタ
	stLIN			*linp;												// 行ポインタ
	char			*outp;												// カットポインタ
	stDB			*indbp, *outdbp;									// 入出力DB
	char			tmpa[4096];											// 環境変数変換用テンポラリバッファ

	cutchk();															// 選択モードで選択先頭より上の行で作業をしたら選択モードを解除する
	envcnv( tmpa, txta );												// 環境変数変換
	indbp=clpbrdGet( CLPBRDP, CB_TXT );									// クリップボードゲット
	outdbp=filterdb( tmpa, indbp );										// フィルター
	if( outdbp ){														// フィルター成功
		bakpgp=dbLNKP( PGP )->bakp;										// ページ先頭の前のリンクポインタ
		bakcurp=dbLNKP( CURP )->bakp;									// カレントの前のリンクポインタ
		dbFOR( outdbp, outp, char* ){									// カットバッファ総当り
			linp=linAlcStr( LINDBP, outp );								// 新行登録
			linchg( linp );												// 行変更セット
			dbMov( LINDBP, linp, CURP );								// カレントポインタの前に移動
		}
		PGP=(stLIN*)(bakpgp->nxtp+1);									// ページ先頭ポインタ
		CURP=(stLIN*)(bakcurp->nxtp+1);									// カレントポインタ
		invalidateAll();												// 全て再描画
		dbClose( outdbp );												// 出力DBクローズ
	}else{																// フィルターエラー
		msgSet( "FILTERDB ERROR!" );
	}
	dbClose( indbp );													// 入力DBクローズ
}

/************************************************************************/
/*	ESC+ESC コマンド													*/
/************************************************************************/

void	esc_esc(void){
#	ifdef	CURSES
	clear();
#	endif
	invalidateAll();													// 全て再描画
}

/************************************************************************/
/*	ESC+-	オプション													*/
/************************************************************************/

void	esc_minus(void){
}

/************************************************************************/
/*	ESC+NUM	行移動/タブ設定/ラベル認識									*/
/************************************************************************/

static void	esc_num( int keysym ){

	char	txta[256];													// テキストバッファ
	char	*txtp;														// テキストポインタ
	int		val;														// 評価値
	int		lno;														// 行番号

	txta[0]=(char)keysym;												// 一行編集バッファ
	txta[1]=0;															// 一行編集バッファ
	if( linedt( txta, "LINE NO or TAB(t),LBL(s),WILD[w],C2L[l],NUM[n],HEX[h]" ) ){
		val=strtol( txta, &txtp, 10 );									// １０進文字列から値に変換
		if( txtp>txta ){												// １０進文字列評価成功
			switch( *txtp ){
				case 0:													// 行番号
					lno=val-1;
					if( lno<0 ){										// 無効な行番号
						lno=0;											// 行番号は先頭
					}
					lnoset( lno );										// 行番号セット
					break;
				case 't':												// タブ設定
					if( val==1 || val==2 || val==4 || val==8 ){			// タブは1,2,4,8のいずれか
						g_tabstp=val;									// タブ設定
						invalidateAll();								// 全て再描画
					}else{
						msgSet( "Invalid Tab Step" );
					}
					break;
				case 's':												// ラベル認識
					g_flglbl=val;										// 検索、置換時のラベル認識
					break;
				case 'n':												// 行番号サイズ
					g_lnosiz=val;										// 行番号サイズ
					dspConfig( g_xw, g_yw );
					invalidateAll();									// 全て再描画
					break;
				case 'l':												// 検索時の大文字小文字区別
					g_flgc2l=val;										// 検索時の大文字小文字区別フラグ
					break;
				case 'w':												// ワイルドカード
					g_flgwld=val;										// 検索、置換時のワイルドカード有効フラグ
					break;
				case 'h':												// 行番号１６進表示
					g_flgh=val;											// 行番号１６進表示
					invalidateAll();									// 全て再描画
					break;
				default:												// その他の場合
					msgSet( "Syntax Error" );							// シンタックスエラー
					break;
			}
		}
	}
}

void	esc_0(void){
	esc_num( '0' );
}
void	esc_1(void){
	esc_num( '1' );
}
void	esc_2(void){
	esc_num( '2' );
}
void	esc_3(void){
	esc_num( '3' );
}
void	esc_4(void){
	esc_num( '4' );
}
void	esc_5(void){
	esc_num( '5' );
}
void	esc_6(void){
	esc_num( '6' );
}
void	esc_7(void){
	esc_num( '7' );
}
void	esc_8(void){
	esc_num( '8' );
}
void	esc_9(void){
	esc_num( '9' );
}

/************************************************************************/
/*	ESC+A	テキストの昇順切り替え										*/
/************************************************************************/

void	esc_a(void){
	ctxchg( 1 );														// コンテキスト変更
}

/************************************************************************/
/*	ESC+B	先頭へ移動													*/
/************************************************************************/

void	esc_b(void){
	PGP=CURP=(stLIN*)dbTop( LINDBP );									// 先頭行にセット
	PGYY=CURYY=0;														// カーソル座標クリア
	invalidateAll();													// 全て再描画
}

/************************************************************************/
/*	ESC+C	フィルター													*/
/************************************************************************/

void	esc_c(void){

	static char		txta[256]={ 0 };									// テキストバッファ

	if( linedt( txta, "SHELL" ) ){										// 一行編集
		shell( txta );													// シェル
	}
}

/************************************************************************/
/*	ESC+D	ファィルの降順切り替え										*/
/************************************************************************/

void	esc_d(void){
	ctxchg( -1 );														// コンテキスト変更
}

/************************************************************************/
/*	ESC+E	編集完了													*/
/************************************************************************/

void	esc_e(void){

	stCTX	*ctxp;														// コンテキストポインタ
	int		res;														// ファイルセーブのリザルト
	int		yn=-1;														// イエス、ノーリザルト

	dbFOR( g_ctxdbp, ctxp, stCTX* ){									// コンテキスト総当りループ
		if( ctxp->chgflg ){												// 変更されている
			res=ctxSave( ctxp );										// ファイルセーブ
			if( !res ){													// ファイルセーブ失敗
				if( yn<0 ){												// 最初の場合
					yn=YorN( "File Save Error Exit?(Y/N)" );			// 本気ですか？と聞く
				}
				if( yn!=YN_YES ){										// 本気ではない場合
					break;												// ループ終了
				}
			}
		}
	}
	g_flgend=( ctxp==0 );												// 終了フラグはコンテキスト総当りループを満足した場合
}

/************************************************************************/
/*	ESC+F	新しいテキストの編集										*/
/************************************************************************/

void	esc_f(void){

	static char	fnam[256]={ 0 };										// ファイル名
	char		enva[256];												// 環境変数変換バッファ
	stDB		*filedbp;												// ファイルDB
	stFILE		*filep;													// ファイル構造体ポインタ
	stCTX		*ctxp;													// コンテキストポインタ
	int			first=1;												// 最初フラグ

	if( linedt( fnam, "NEW FILE" ) ){									// 一行編集
		envcnv( enva, fnam );											// 環境変数変換
		filedbp=filedbAlc( 0, enva, FILEATR_REG|FILEATR_DOT|FILEATR_NON );		// ファイル名登録
		dbFOR( filedbp, filep, stFILE* ){								// 登録されたファイル名総当りループ
			ctxp=(stCTX*)dbSch( g_ctxdbp, dbKEYADR( filep ) );			// ファイル名検索
			if( !ctxp ){												// 見つからない
				ctxp=ctxOpen( (char*)dbKEYADR( filep ) );				// コンテキストオープン
			}
			if( ctxp && first ){										// コンテキストはオープンできてかつ最初の場合
				first=0;												// 最初フラグクリア
				g_ctxp=ctxp;											// カレントコンテキストポインタセット
			}
		}
		dbClose( filedbp );												// ファイルDBクローズ
		invalidateAll();												// 全て再描画
	}
}

/************************************************************************/
/*	ESC+H	現テキストのセーブ											*/
/************************************************************************/

void	esc_h(void){

	stLIN	*linp;														// 行ポインタ

	if( FILCHG ){														// カレントファイルは変更されている
		if( ctxSave( g_ctxp ) ){										// カレントファイルセーブ成功
			dbFOR( LINDBP, linp, stLIN* ){								// 行総当り
				linp->chgflg=0;											// 変更フラグクリア
			}
			FILCHG=0;													// ファイル変更フラグクリア
			invalidateAll();											// 全て再描画
		}else{															// カレントファイルセーブ失敗
			msgSet( "File Save Error" );								// ファイルセーブエラー
		}
	}
}

/************************************************************************/
/*	ESC+I	タブ文字の表示・非表示切り替え								*/
/************************************************************************/

void	esc_i(void){
	g_tabdsp=!g_tabdsp;													// タブ文字表示フラグ反転
	invalidateAll();													// 全て再描画
}

/************************************************************************/
/*	ESC+J	前方確認置換												*/
/************************************************************************/

void	esc_j(void){

	char	reporg[256]={ 0 }, repnew[256]={ 0 };						// 置換文字列
	int		yn;															// イエスノー
	int		chkflg;														// 置換確認フラグ

	if( linedt2( reporg, "REPLACE FORWORD", repnew, "NEW STRINGS" ) ){
		yn=YorN( "KAKUNIN ? (Y/N)" );									// 確認するか？
		if( yn!=YN_CANCEL ){											// キャンセルでない場合
			chkflg=( yn==YN_YES );										// イエスの場合置換確認フラグセット
			replace( reporg, repnew, 0, chkflg, !chkflg );				// 前方確認置換
		}
	}
}

/************************************************************************/
/*	ESC+K	現テキストの破棄											*/
/************************************************************************/

void	esc_k(void){

	int		doflg=0;													// 実行フラグ
	stCTX	*nxtp;														// コンテキストポインタ

	if( FILCHG ){														// カレントファイルは変更されている
		if( YorN( "Are You Sure(Y/N)" )==YN_YES ){						// 本気ですか？でイエスの場合
			doflg=1;													// 実行フラグを立てる
		}
	}else{																// 変更されていない
		doflg=1;														// 実行フラグを立てる
	}
	if( doflg ){														// 実行フラグが立っている
		nxtp=(stCTX*)dbNxt( g_ctxp );									// 次のコンテキストポインタをとっておく
		ctxClose( g_ctxp );												// カレントコンテキストクローズ
		if( !nxtp ){													// 次のコンテキストポインタは無効
			nxtp=(stCTX*)dbTop( g_ctxdbp );								// 次のコンテキストポインタは先頭
		}
		g_ctxp=nxtp;													// カレントコンテキストポインタ更新
		if( nxtp ){														// まだ編集中のファイルは存在する
			invalidateAll();											// 全て再描画
		}else{															// 編集中のファイルは存在しない
			g_flgend=1;													// 終了フラグを立てる
		}
	}
}

/************************************************************************/
/*	ESC+L	後方確認置換												*/
/************************************************************************/

void	esc_l(void){
	if( linedt2( g_reporg, "REPLACE BACK", g_repnew, "NEW STRINGS" ) ){
		replace( g_reporg, g_repnew, 1, 1, 0 );							// 後方確認置換
	}
}

/************************************************************************/
/*	ESC+M	改行文字の表示／非表示										*/
/************************************************************************/

void	esc_m(void){
	g_flgcr=!g_flgcr;													// 改行表示フラグ反転
	invalidateAll();													// 全て再描画
}

/************************************************************************/
/*	ESC+N	前方検索													*/
/************************************************************************/

void	esc_n(void){

	char	scdbtr[256];												// 検索文字列

	scdbtrget( scdbtr );												// 検索文字列を得る
	if( linedt( scdbtr, "SEARCH2 FORWORD" ) ){							// 一行編集
		if( scdbtr[0] ){												// 有効な文字列
			scdbtrput( scdbtr );										// 検索文字列をクリップボードに書き込む
			replace( scdbtr, 0, 0, 0, 0 );								// 検索または置換
		}
	}
}

/************************************************************************/
/*	ESC+O	現テキストの再ロード										*/
/************************************************************************/

void	esc_o(void){

	stCTX	*orgp;														// コンテキストポインタ

	if( FILCHG ){														// カレントファイルは変更されている
		if( YorN( "Are You Sure(Y/N)" )==YN_YES ){						// 本気ですか？でイエスの場合
			orgp=g_ctxp;												// カレントコンテキストポインタバックアップ
			g_ctxp=ctxOpen( FNAMP );									// コンテキストオープン
			dbMov( g_ctxdbp, g_ctxp, orgp );							// カレントに移動
			ctxClose( orgp );											// 球根テキスト破棄
			invalidateAll();											// 全て再描画
		}
	}
}

/************************************************************************/
/*	ESC+Q	全テキストを破棄し終了										*/
/************************************************************************/

void	esc_q(void){

	int		chgflg=0;													// 変更フラグ論理和
	int		flgend=1;													// 終了フラグ
	stCTX	*ctxp;														// コンテキストポインタ

	dbFOR( g_ctxdbp, ctxp, stCTX* ){									// コンテキスト総当りループ
		chgflg|=ctxp->chgflg;											// 変更フラグ論理和をとる
	}
	if( chgflg ){														// 変更されたものがある
		if( YorN( "Are You Sure(Y/N)" )!=YN_YES ){						// 本気ですか？でイエスではない場合
			flgend=0;													// 終了フラグクリア
		}
	}
	g_flgend=flgend;													// 終了フラグセット
}

/************************************************************************/
/*	ESC+R	前方置換													*/
/************************************************************************/

void	esc_r(void){
	if( linedt2( g_reporg, "REPLACE FORWORD", g_repnew, "NEW STRINGS" ) ){
		replace( g_reporg, g_repnew, 0, 0, 0 );							// 前方置換
	}
}

/************************************************************************/
/*	ESC+S	後方検索													*/
/************************************************************************/

void	esc_s(void){

	char	scdbtr[256];												// 検索文字列

	scdbtrget( scdbtr );												// 検索文字列を得る
	if( linedt( scdbtr, "SEARCH2 BACK" ) ){								// 一行編集
		if( scdbtr[0] ){												// 有効な文字列
			scdbtrput( scdbtr );										// 検索文字列をクリップボードに書き込む
			replace( scdbtr, 0, 1, 0, 0 );								// 検索または置換
		}
	}
}

/************************************************************************/
/*	ESC+T	ファイル名変更												*/
/************************************************************************/

void	esc_t(void){

	char	fnam[256];													// ファイル名
	stCTX	*ctxp;														// コンテキストポインタ

	strcpy( fnam, FNAMP );												// オリジナルファイル名セット
	if( linedt( fnam, "NEW NAME" ) ){									// 一行編集
		ctxp=(stCTX*)dbSch( g_ctxdbp, fnam );							// 検索
		if( !ctxp ){													// 一致するものはない
			g_ctxp=ctxRename( g_ctxp, fnam );							// コンテキストリネーム
			invalidateFil();											// ファイル名再表示
		}else{															// 一致するものがあった
			msgSet( "Same File Exist!" );								// 同じファイル名存在
		}
	}
}

/************************************************************************/
/*	ESC+U	後方置換													*/
/************************************************************************/

void	esc_u(void){
	if( linedt2( g_reporg, "REPLACE BACK", g_repnew, "NEW STRINGS" ) ){
		replace( g_reporg, g_repnew, 1, 0, 0 );							// 後方置換
	}
}

/************************************************************************/
/*	CYGWIN ドライブ表記から DOS 表記に変更する							*/
/************************************************************************/

#	ifdef	_WIN32

static char	*cyg2dos( char *dstadr, const char *txtadr ){

	const char	*txtp;
	char		*dstp;
	char		chr;													// テキストから読み込んだ文字
	char		drvchr;													// ドライブキャラクタ

	txtp=txtadr;
	if( *txtp++=='/' ){													// アブソリュートパス
		dstp=dstadr;													// ディストネーションバッファをテンポラリとして使用する
		while( ( chr=*txtp++ )!=0 ){
			*dstp++=chr;
			if( chr=='/' ){
				break;
			}
		}
		*dstp=0;
		if( strcmp( dstadr, "cygdrive/" ) ){							// CYGWIN ルート
			drvchr='c';
			dstp=dstadr+sprintf( dstadr, "%c:\\cygwin", drvchr );
			txtp=txtadr;
		}else{															// cygdrive/
			drvchr=*txtp++;
			dstp=dstadr+sprintf( dstadr, "%c:", drvchr );
		}
	}else{																// 相対パス
		txtp=txtadr;
		dstp=dstadr;
	}
	while( ( chr=*txtp++ )!=0 ){
//		if( chr=='/' ){
//			chr='\\';
//		}
		*dstp++=chr;
	}
	*dstp=0;
	return( dstp );
}

#	endif

/************************************************************************/
/*	ESC+V	タグジャンプ												*/
/************************************************************************/

void	esc_v(void){

	char	tmpa[256];													// ファイル名
	char	fnam[256];													// ファイル名
	int		lno=1;														// 行番号
	stCTX	*ctxp;														// コンテキストポインタ

	if( tagparse( &lno, tmpa, CURTXTP, FNAMP ) ){						// タグ名解析
#	ifdef	_WIN32
		cyg2dos( fnam, tmpa );											// CYGWIN ドライブ表記から DOS 表記に変更する
#	else
		strcpy( fnam, tmpa );
#	endif
		if( lno<=0 ){													// 行番号は無効
			lno=0;														// 先頭セット
		}else{															// 行番号は有効
			lno--;														// 内部形式は０が先頭のためデクリメント
		}
		ctxp=(stCTX*)dbSch( g_ctxdbp, fnam );							// ファイル名検索
		if( !ctxp ){													// 見つからない
			ctxp=ctxOpen( fnam );										// コンテキストオープン
		}
		if( ctxp ){														// コンテキストポインタは有効
			g_ctxp=ctxp;												// グローバルコンテキストポインタセット
			lnoset( lno );												// 行番号セット
			invalidateAll();											// 全て再描画
		}else{															// ファイルオープンエラー
			msgSet( "Can't Open File" );
		}
	}else{																// タグフォーマットエラー
		msgSet( "TAG Format Error" );
	}
}

/************************************************************************/
/*	ESC+X	現テキストの完了											*/
/************************************************************************/

void	esc_x(void){

	stCTX	*nxtp;														// コンテキストポインタ
	int		res=1;														// リザルト

	if( FILCHG ){														// カレントファイルは変更されている
		res=ctxSave( g_ctxp );											// ファイルセーブ
		if( !res ){														// ファイルセーブ失敗
			msgSet( "File Save Error" );
		}
	}
	if( res ){															// ファイルセーブ成功か変更されていなかった
		nxtp=(stCTX*)dbNxt( g_ctxp );									// 次のコンテキストポインタ
		ctxClose( g_ctxp );												// カレントコンテキストクローズ
		if( !nxtp ){													// 次のコンテキストポインタは無効
			nxtp=(stCTX*)dbTop( g_ctxdbp );								// 先頭コンテキストポインタ
		}
		g_ctxp=nxtp;													// カレントコンテキストポインタセット
		if( nxtp ){														// まだ編集中のファイルは存在する
			invalidateAll();											// 全て再描画
		}else{															// 編集中のファイルは存在しない
			g_flgend=1;													// 終了フラグを立てる
		}
	}
}

/************************************************************************/
/*	ESC+Y	ファィルの読み込み											*/
/************************************************************************/

void	esc_y(void){

	stDBLNK		*bakpgp, *bakcurp;										// DBリンクポインタ
	char		fnam[256]={ 0 };										// ファイル名
	char		enva[256];												// 環境変数変換バッファ
	stDB		*filedbp;												// ファイルDB
	stFILE		*filep;													// ファイル構造体ポインタ
	char		*fnamp;													// ファイル名ポインタ
	FILE		*fp;													// ファイルポインタ
	stLIN		*linp;													// 行ポインタ
	char		txta[0x10000], *txtp;									// テキストバッファ、ポインタ

	if( linedt( fnam, "INSERT FILE" ) ){								// 一行編集
		envcnv( enva, fnam );											// 環境変数変換
		bakpgp=dbLNKP( PGP )->bakp;										// ページ先頭の前のリンクポインタ
		bakcurp=dbLNKP( CURP )->bakp;									// カレントの前のリンクポインタ
		filedbp=filedbAlc( 0, enva, FILEATR_REG|FILEATR_DOT );			// ファイル登録
		dbFOR( filedbp, filep, stFILE* ){								// 登録されたファイル名総当りループ
			fnamp=(char*)dbKEYADR( filep );								// ファイル名ポインタ
			fp=fopen( fnamp, "r" );										// ファイルオープン
			if( fp ){													// ファイルオープン成功
				while( !feof( fp ) ){									// ファイル終端に達しない間ループ
					if( !fgets( txta, sizeof(txta), fp ) ){				// バッファは無効
						continue;
					}
					txtp=strpbrk( txta, "\r\n\x1a" );					// 改行コード検索
					if( txtp ){											// 見つかった
						*txtp=0;										// 改行コードクリア
					}
					linp=linAlcStr( LINDBP, txta );
					linchg( linp );										// 行変更セット
					dbMov( LINDBP, linp, CURP );						// カーソルポインタの前に挿入
				}
				fclose( fp );											// ファイルクローズ
			}
		}
		dbClose( filedbp );												// ファイルDBクローズ
		PGP=(stLIN*)(bakpgp->nxtp+1);									// ページ先頭ポインタ
		CURP=(stLIN*)(bakcurp->nxtp+1);									// カレントポインタ
		invalidateAll();												// 全て再描画
	}
}

/************************************************************************/
/*	ESC+Z	末尾へ移動													*/
/************************************************************************/

void	esc_z(void){

	int		seekn;														// 実際にシークした数
	stLIN	*endp;														// 終端ポインタ
	stLIN	*pgp;														// ページポインタ

	endp=(stLIN*)dbEnd( LINDBP );										// 終端ポインタを得る
	pgp=(stLIN*)dbSeek( &seekn, endp, 1-g_tyyd );						// 終端ポインタから表示行分後方にシークしてみる
	if( seekn==1-g_tyyd ){												// シークできた
		PGP=pgp;														// ページポインタセット
		PGYY=dbAlcN( LINDBP )-1+seekn;									// ページＹセット
		CURYY=-seekn;													// カーソルＹセット
	}else{																// シークできない
		PGP=(stLIN*)dbTop( LINDBP );									// ページポインタは先頭
		PGYY=0;															// ページＹセット
		CURYY=dbAlcN( LINDBP )-1;										// カーソルＹは終端
	}
	CURP=endp;															// カレントポインタセット
	invalidateAll();													// 全て再描画
}

/************************************************************************/
/*	ESC+[	カレントワード前方置換										*/
/************************************************************************/

void	esc_lbo(void){
	getcurword( g_reporg );												// カレントワードを得る
	if( linedt2( g_reporg, "REPLACE FORWORD", g_repnew, "NEW STRINGS" ) ){
		replace( g_reporg, g_repnew, 0, 0, 1 );							// カレントワード前方置換
	}
}

/************************************************************************/
/*	ESC+\	カレントワード後方置換										*/
/************************************************************************/

void	esc_yen(void){
	getcurword( g_reporg );												// カレントワードを得る
	if( linedt2( g_reporg, "REPLACE BACK", g_repnew, "NEW STRINGS" ) ){
		replace( g_reporg, g_repnew, 1, 0, 0 );							// カレントワード後方置換
	}
}

/************************************************************************/
/*	ESC+]	大文字・小文字変換切替										*/
/************************************************************************/

void	esc_lbc(void){
	g_flgl2c=!g_flgl2c;													// 小文字から大文字変換フラグ
	invalidateMod();													// 全て再描画
}

/************************************************************************/
/*	ESC+^	前方置換(表示なし)											*/
/************************************************************************/

void	esc_hat(void){
	if( linedt2( g_reporg, "REPLACE FORWORD NO-DISP", g_repnew, "NEW STRINGS" ) ){
		replace( g_reporg, g_repnew, 0, 0, 1 );							// 前方置換(表示なし)
	}
}

/************************************************************************/
/*	ESC+_	後方置換(表示なし)											*/
/************************************************************************/

void	esc__(void){
	if( linedt2( g_reporg, "REPLACE BACK NO-DISP", g_repnew, "NEW STRINGS" ) ){
		replace( g_reporg, g_repnew, 1, 0, 1 );							// 後方置換(表示なし)
	}
}

/************************************************************************/
/*	CTRL+[	ESC コマンド												*/
/************************************************************************/

void	esc(void){

	int		keysym;
	int		curbak;

	msgSet( "[ESC]" );													// メッセージ表示
	curbak=curSw( 1 );													// カーソルオン
	keysym=getKey()&0xffff;												// キー入力
	curSw( curbak );													// カーソル元の状態に
//	if( keysym>='a' && keysym<='z' ){									// 小文字の場合
//		keysym&=~0x20;													// 小文字から大文字変換
//	}
	msgSet( "" );														// メッセージクリア
	switch( keysym ){
		case '-' :														// オプション
			esc_minus();
			break;
		case '0' :
		case '1' :
		case '2' :
		case '3' :
		case '4' :
		case '5' :
		case '6' :
		case '7' :
		case '8' :
		case '9' :														// ESC+ 0-9
			esc_num( keysym );											// 行番号セット
			break;
		case '@' :														// キーボードマクロの定義
			f11();
			break;
		case 'a' :														// テキストの昇順切り替え
			esc_a();
			break;
		case 'b' :														// 先頭へ移動
			esc_b();
			break;
		case 'c' :														// 子プロセスの実行
			esc_c();
			break;
		case 'd' :														// ファィルの降順切り替え
			esc_d();
			break;
		case 'e' :														// 編集完了
			esc_e();
			break;
		case 'f' :														// 新しいテキストの編集
			esc_f();
			break;
		case 'h' :														// 現テキストのセーブ
			esc_h();
			break;
		case 'i' :														// タブ文字の表示切り替え
			g_tabdsp=!g_tabdsp;											// タブ文字表示フラグ反転
			invalidateAll();											// 全て再描画
			break;
		case 'j' :														// 前方確認置換
			esc_j();
			break;
		case 'k' :														// 現テキストの破棄
			esc_k();
			break;
		case 'l' :														// 後方確認置換
			esc_l();
			break;
		case 'm' :														// 改行文字の表示／非表示
			g_flgcr=!g_flgcr;											// 改行表示フラグ反転
			invalidateAll();											// 全て再描画
			break;
		case 'n' :														// 前方検索
			esc_n();
			break;
		case 'o' :														// 現テキストの再ロード
			esc_o();													// ESC+O	現テキストの再ロード
			break;
		case 'q' :														// 全テキストを破棄し終了
			esc_q();													// ESC+Q	全テキストを破棄し終了
			break;
		case 'r' :														// 前方置換
			esc_r();
			break;
		case 's' :														// 後方検索
			esc_s();
			break;
		case 't' :														// ファイル名変更
			esc_t();
			break;
		case 'u' :														// 後方置換
			esc_u();
			break;
		case 'v' :														// タグジャンプ
			esc_v();
			break;
		case 'w' :														// 子プロセスの実行
			esc_c();
			break;
		case 'x' :														// 現テキストの完了
			esc_x();
			break;
		case 'y' :														// ファィルの読み込み
			esc_y();
			break;
		case 'z' :														// 末尾へ移動
			esc_z();
			break;
		case '[' :														// カレントワード前方置換
			esc_lbo();
			break;
		case '\\':														// カレントワード後方置換
			esc_yen();
			break;
		case ']' :														// 大文字・小文字変換切替
			g_flgl2c=!g_flgl2c;											// 小文字から大文字変換フラグ
			invalidateMod();											// 全て再描画
			break;
		case '^' :														// 前方置換(表示なし)
			esc_hat();
			break;
		case '_' :														// 後方置換(表示なし)
			esc__();
			break;
		default:
#	ifdef	CURSES
			clear();
#	endif
			invalidateAll();											// 全て再描画
			break;
	}
}

/************************************************************************/
/*	CTRL+A	１語後方に移動												*/
/************************************************************************/

void	ctrl_a(void){
	curleft( 1 );														// カーソル←
}

/************************************************************************/
/*	CTRL+B	両行端に移動												*/
/************************************************************************/

void	ctrl_b(void){
	if( CURXX ){														// カーソルは左端ではない
		curtop();														// 左端に移動
	}else{																// カーソルは左端
		curend();														// 右端に移動
	}
}

/************************************************************************/
/*	CTRL+C	画面をロールアップ											*/
/************************************************************************/

void	ctrl_c(void){
	pgscroll( g_tyyd );													// スクロール
}

/************************************************************************/
/*	CTRL+D	１文字前方に移動											*/
/************************************************************************/

void	ctrl_d(void){
	curright( 0 );														// カーソル→
}

/************************************************************************/
/*	CTRL+E	１行上に移動												*/
/************************************************************************/

void	ctrl_e(void){
	curup();															// カーソル↑
}

/************************************************************************/
/*	CTRL+F	１語前方に移動												*/
/************************************************************************/

void	ctrl_f(void){
	curright( 1 );														// カーソル→
}

/************************************************************************/
/*	CTRL+G	１文字削除													*/
/************************************************************************/

void	ctrl_g(void){
	del( 0 );															// 削除
}

/************************************************************************/
/*	CTRL+H	バックスペース												*/
/************************************************************************/

void	ctrl_h(void){
	if( curleft( 0 ) ){													// カーソル←
		del( 0 );														// 削除
	}
}

/************************************************************************/
/*	CTRL+J	ヘルプ画面の表示											*/
/************************************************************************/

void	ctrl_j(void){
	help();																// ヘルプ処理
}

/************************************************************************/
/*	CTRL+K	行末まで削除												*/
/************************************************************************/

void	ctrl_k(void){

	stLM		lm;														// 行マップ
	char		txta[0x10000];											// テキストバッファ
	int			txtsiz;													// テキストサイズ

	lmMak( &lm, CURTXTP, g_tabstp, 0 );									// 行マップ作成
	CURXX=lmPosAdj( &lm, CURXX, 0 );									// カーソル位置正規化
	txtsiz=lm.txtofsa[CURXX];											// 新しいテキストサイズ
	memcpy( txta, CURTXTP, txtsiz );									// テキストバッファに新しいサイズでコピー
	txta[txtsiz]=0;														// ０ターミネート
	renewtxt( CURP, CURYY, txta );										// カレントテキスト更新
}

/************************************************************************/
/*	CTRL+L	削除バッファから挿入										*/
/************************************************************************/

void	ctrl_l(void){
	inslin( g_cutlin );													// 行挿入
}

/************************************************************************/
/*	CTRL+M	改行(行分割)												*/
/************************************************************************/

void	ctrl_m(void){
	cr();																// 改行
}

/************************************************************************/
/*	CTRL+N	１行挿入													*/
/************************************************************************/

void	ctrl_n(void){
	inslin( "" );														// 空行挿入
}

/************************************************************************/
/*	CTRL+O	挿入モード切り替え											*/
/************************************************************************/

void	ctrl_o(void){
	g_flgins=!g_flgins;													// 挿入モードフラグ論理反転
	invalidateAll();													// 全て再描画
}

/************************************************************************/
/*	CTRL+P	右端に移動													*/
/************************************************************************/

void	ctrl_p(void){
	curend();															// 右端に移動
}

/************************************************************************/
/*	CTRL+Q	左端に移動													*/
/************************************************************************/

void	ctrl_q(void){
	curtop();															// 左端に移動
}

/************************************************************************/
/*	CTRL+R	画面をロールダウン											*/
/************************************************************************/

void	ctrl_r(void){
	pgscroll( -g_tyyd );												// スクロール
}

/************************************************************************/
/*	CTRL+S	１文字後方に移動											*/
/************************************************************************/

void	ctrl_s(void){
	curleft( 0 );														// カーソル←
}

/************************************************************************/
/*	CTRL+T	１語削除													*/
/************************************************************************/

void	ctrl_t(void){
	del( 1 );															// 削除
}

/************************************************************************/
/*	CTRL+U	行頭から削除												*/
/************************************************************************/

void	ctrl_u(void){

	stLM		lm;														// 行マップ

	lmMak( &lm, CURTXTP, g_tabstp, 0 );									// 行マップ作成
	CURXX=lmPosAdj( &lm, CURXX, 0 );									// カーソル位置正規化
	renewtxt( CURP, CURYY, CURTXTP+lm.txtofsa[CURXX] );					// カレントテキスト更新
	CURXX=0;															// カーソルを左端に移動
}

/************************************************************************/
/*	CTRL+V	直接コード入力												*/
/************************************************************************/

void	ctrl_v(void){

	int		keysym;
	int		curbak;

	msgSet( "CODE ?" );													// メッセージ表示
	curbak=curSw( 1 );													// カーソルオン
	keysym=getKey()&0xffff;												// キー入力
	curSw( curbak );													// カーソル元の状態に
	setchr( keysym );													// 一文字セット
}

/************************************************************************/
/*	CTRL+W	画面を１行ロールダウン										*/
/************************************************************************/

void	ctrl_w(void){
	linscroll( -1 );													// スクロール
}

/************************************************************************/
/*	CTRL+X	１行下に移動												*/
/************************************************************************/

void	ctrl_x(void){
	curdown();															// カーソル↓
}

/************************************************************************/
/*	CTRL+Y	１行削除													*/
/************************************************************************/

void	ctrl_y(void){

	stLIN		*linp;													// 行ポインタ

	linp=(stLIN*)dbNxt( CURP );											// 次の行
	if( linp ){															// 最終行ではない
		strcpy( g_cutlin, CURTXTP );									// カットバッファにコピー
		if( PGP==CURP ){												// ページ先頭
			PGP=linp;													// ページポインタ更新
		}
		if( CUTCURP==CURP ){
			CUTCURP=0;
		}
		dbFre( LINDBP, CURP );											// カレント行破棄
		CURP=linp;														// カレント行更新
		FILCHG=1;														// ファイル変更セット
		invalidateAll();												// 全て再描画
	}
}

/************************************************************************/
/*	CTRL+Z	１行ロールアップ											*/
/************************************************************************/

void	ctrl_z(void){
	linscroll( 1 );														// スクロール
}

/************************************************************************/
/*	CTRL+\	カレントワード後方検索										*/
/************************************************************************/

void	ctrl_yen(void){

	char	scdbtr[256];												// 検索文字列

	getcurword( scdbtr );												// カレントワードを得る
	if( linedt( scdbtr, "SEARCH2 BACK" ) ){								// 一行編集
		if( scdbtr[0] ){												// 有効な文字列
			scdbtrput( scdbtr );										// 検索文字列をクリップボードに書き込む
			replace( scdbtr, 0, 1, 0, 0 );								// 検索または置換
		}
	}
}

/************************************************************************/
/*	CTRL+]	大文字・小文字変換											*/
/************************************************************************/

void	ctrl_lbktc(void){

	stLM		lm;														// 行マップ
	char		*txtp;													// テキストポインタ
	int			chr;													// 文字

	lmMak( &lm, CURTXTP, g_tabstp, 0 );									// 行マップ作成
	CURXX=lmPosAdj( &lm, CURXX, 0 );									// カーソルＸ正規化
	txtp=CURTXTP+lm.txtofsa[CURXX];										// 現在のテキストポインタ
	chr=*txtp;															// 現在のテキストポインタから一文字を得る
	if( !g_flgl2c ){													// 大文字から小文字変換
		if( chr>='A' && chr<='Z' ){										// 大文字
			chr |= 0x20;												// 小文字化
		}
	}else{																// 小文字から大文字変換
		if( chr>='a' && chr<='z' ){										// 小文字
			chr &= 0xdf;												// 大文字化
		}
	}
	if( chr!=*txtp ){													// 文字が変更された
		*txtp=(char)chr;												// 文字セット
		linchg( CURP );													// 行変更セット
	}
	curright( 0 );														// カーソル→
	if( linAdjScr() ){													// 横スクロールの正規化
		invalidateAll();												// 全て再描画
	}
}

/************************************************************************/
/*	CTRL+^	カレントワード前方検索										*/
/************************************************************************/

void	ctrl_hat(void){

	char	scdbtr[256];												// 検索文字列

	getcurword( scdbtr );												// カレントワードを得る
	if( linedt( scdbtr, "SEARCH2 BACK" ) ){								// 一行編集
		if( scdbtr[0] ){												// 有効な文字列
			scdbtrput( scdbtr );										// 検索文字列をクリップボードに書き込む
			replace( scdbtr, 0, 0, 0, 0 );								// 検索または置換
		}
	}
}

/************************************************************************/
/*	CTRL+_	ファイル終端記号											*/
/************************************************************************/

void	ctrl__(void){
	g_flgeof=!g_flgeof;													// [EOF]表示フラグ
	invalidateAll();													// 全て再描画
}

/************************************************************************/
/*	F5		継続前方検索												*/
/************************************************************************/

void	f5(void){

	char	scdbtr[256];												// 検索文字列

	if( scdbtrget( scdbtr ) ){											// 検索文字列を得る
		replace( scdbtr, 0, 0, 0, 0 );									// 検索または置換
	}
}

/************************************************************************/
/*	範囲指定開始														*/
/************************************************************************/

static void	cutstart( int cuttyp ){
	if( !CUTCURP ){														// 範囲指定中ではない
		CUTTYP=cuttyp;													// 範囲指定タイプ
		CUTPGP=PGP;														// 範囲指定開始ページポインタ
		CUTCURP=CURP;													// 範囲指定開始カーソルポインタ
		CUTPGYY=PGXX;													// 範囲指定開始ページＸ
		CUTPGYY=PGYY;													// 範囲指定開始ページＹ
		CUTCURXX=CURXX;													// 範囲指定開始カーソルＸ
		CUTCURYY=CURYY;													// 範囲指定開始カーソルＹ
	}else{																// 範囲指定中
		CUTCURP=0;														// 範囲指定終了
		invalidateAll();												// 全て再描画
	}
}

/************************************************************************/
/*	行モードカットバッファにコピー										*/
/************************************************************************/

static void	cutcpylin( int delflg ){

	stDBLNK		*bakpgp, *bakcurp;										// DBリンクポインタ
	stLIN		*linp, *nxtp;											// 行ポインタ
	int			toplno, curlno;											// 先頭行番号, カレント行番号
	int			lno;													// 行番号
	stDB		*cbdbp;													// クリップボードDB

	if( CUTCURP ){														// 選択モード中である
		toplno=CUTPGYY+CUTCURYY;										// 先頭行番号
		curlno=PGYY+CURYY;												// カレント行番号
		if( curlno>toplno ){											// カット先頭より後方にある
			bakpgp=dbLNKP( CUTPGP )->bakp;								// ページ先頭の前のリンクポインタ
			bakcurp=dbLNKP( CUTCURP )->bakp;							// カレントの前のリンクポインタ
			cbdbp=dbOpen( 0, DBSIZ_STR, 0, 0x4000 );					// クリップボードDBオープン
			linp=CUTCURP;												// 選択先頭行
			for( lno=toplno; lno<curlno && linp; lno++ ){				// 行番号ループ
				dbAlc( cbdbp, dbKEYADR( linp ) );						// 登録
				nxtp=(stLIN*)dbNxt( linp );								// 次の行
				if( delflg ){											// 削除
					dbFre( LINDBP, linp );								// 行開放
				}
				linp=nxtp;												// 行ポインタ更新
			}
			if( delflg ){												// 削除
				PGP=(stLIN*)(bakpgp->nxtp+1);							// ページ先頭ポインタ
				CURP=(stLIN*)(bakcurp->nxtp+1);							// カレントポインタ
				PGYY=CUTPGYY;											// ページ行番号
				CURYY=CUTCURYY;											// カーソルＹ座標
				FILCHG=1;												// ファイル変更セット
			}
			CUTCURP=0;													// 選択モード解除
			cbtxtput( cbdbp, 0 );										// クリップボードにテキストを書き込む
			dbClose( cbdbp );											// クリップボードDBクローズ
			invalidateAll();											// 全て再描画
		}
	}
}

/************************************************************************/
/*	テキスト範囲削除													*/
/************************************************************************/

static void	txtcut( char *cutp, char *jntp, const char *txtp, int xx1, int xx2 ){

	stLM		lm;														// 行マップ
	int			axx1, axx2;												// 正規化されたカーソル位置
	int			o1, o2, o3;												// オフセット

	lmMak( &lm, txtp, g_tabstp, 0 );									// 行マップ作成
	axx1=lmPosAdj( &lm, xx1, 0 );										// カーソル位置正規化
	axx2=lmPosAdj( &lm, xx2, 0 );										// カーソル位置正規化
	o1=lm.txtofsa[axx1];												// オフセット１
	o2=lm.txtofsa[axx2];												// オフセット２
	o3=strlen( txtp );													// オフセット３
	memcpy( cutp, txtp+o1, o2-o1 );										// カットバッファにコピー
	cutp[o2-o1]=0;														// カットバッファ０ターミネート
	if( jntp ){															// 残りをジョイントする
		memcpy( jntp, txtp, o1 );										// テキスト先頭からオフセット１サイズコピー
		memcpy( jntp+o1, txtp+o2, o3-o2 );								// オフセット２から残り全てをコピー
		jntp[o1+o3-o2]=0;												// ０ターミネート
	}
}

/************************************************************************/
/*	矩形モードカットバッファにコピー									*/
/************************************************************************/

static void	cutcpybox( int delflg ){

	stDBLNK		*bakpgp, *bakcurp;										// DBリンクポインタ
	int			xx1, yy1, xx2, yy2;										// 矩形選択エリア
	stLIN		*linp;													// 行ポインタ
	int			lno;													// 行番号
	char		cuta[0x10000];											// カットバッファ
	char		jnta[0x10000];											// ジョイントバッファ
	stDB		*cbdbp;													// クリップボードDB

	if( CUTCURP ){														// 選択モード中である
		xx1=CUTCURXX;													// 矩形選択エリア
		yy1=CUTPGYY+CUTCURYY;											// 矩形選択エリア
		xx2=CURXX;														// 矩形選択エリア
		yy2=CURYY+PGYY;													// 矩形選択エリア
		if( xx1<=xx2 && yy1<=yy2 ){										// 正常矩形エリア
			cbdbp=dbOpen( 0, DBSIZ_STR, 0, 0x4000 );					// クリップボードDBオープン
			bakpgp=dbLNKP( CUTPGP )->bakp;								// ページ先頭の前のリンクポインタ
			bakcurp=dbLNKP( CUTCURP )->bakp;							// カレントの前のリンクポインタ
			linp=CUTCURP;												// 行ポインタは選択先頭
			for( lno=yy1; lno<=yy2 && linp; lno++ ){					// 行番号ループ
				txtcut( cuta, jnta, (char*)dbKEYADR( linp ), xx1, xx2 );// テキスト範囲削除
				dbAlc( cbdbp, cuta );									// カットバッファ登録
				if( delflg ){											// 削除
					linp=linflush( linp, jnta );						// テキスト更新
					linchg( linp );										// 行変更セット
				}
				linp=(stLIN*)dbNxt( linp );								// 作業行更新
			}
			if( delflg ){												// 削除
				PGP=(stLIN*)(bakpgp->nxtp+1);							// ページ先頭ポインタ
				CURP=(stLIN*)(bakcurp->nxtp+1);							// カレントポインタ
				PGXX=CUTPGXX;											// ページＸ
				PGYY=CUTPGYY;											// ページＹ
				CURXX=CUTCURXX;											// カーソルＸ
				CURYY=CUTCURYY;											// カーソルＹ
				linAdjScr();											// 横スクロールの正規化
			}
			CUTCURP=0;													// 範囲指定モード終了
			cbtxtput( cbdbp, 1 );										// クリップボードにテキストを書き込む
			dbClose( cbdbp );											// クリップボードDBクローズ
			invalidateAll();											// 全て再描画
		}
	}
}

/************************************************************************/
/*	カットバッファにコピー												*/
/************************************************************************/

static void	cutcpy( int delflg ){
	if( !CUTTYP ){														// 行選択モード
		cutcpylin( delflg );											// 行モードカットバッファにコピー
	}else{																// 矩形選択モード
		cutcpybox( delflg );											// 矩形モードカットバッファにコピー
	}
}

/************************************************************************/
/*	F3	カレントワード前方置換											*/
/************************************************************************/

void	f3(void){

	int		yn;															// イエスノー
	int		chkflg;														// 置換確認フラグ

	getcurword( g_reporg );												// カレントワードを得る
	if( linedt2( g_reporg, "REPLACE FORWORD", g_repnew, "NEW STRINGS" ) ){
		yn=YorN( "KAKUNIN ? (Y/N)" );									// 確認するか？
		if( yn!=YN_CANCEL ){											// キャンセルでない場合
			chkflg=( yn==YN_YES );										// イエスの場合置換確認フラグセット
			replace( g_reporg, g_repnew, 0, chkflg, !chkflg );			// 前方確認置換
		}
	}
}

/************************************************************************/
/*	F6		範囲指定開始												*/
/************************************************************************/

void	f6(void){
	cutstart( 0 );														// 範囲指定開始
}

/************************************************************************/
/*	F7		カットバッファへ移動										*/
/************************************************************************/

void	f7(void){
	cutcpy( 1 );														// カットバッファにコピー
}

/************************************************************************/
/*	F8		カットバッファへ複写										*/
/************************************************************************/

void	f8(void){
	cutcpy( 0 );														// カットバッファにコピー
}

/************************************************************************/
/*	F9		カットバッファから挿入										*/
/************************************************************************/

void	f9(void){

	stDBLNK		*bakpgp, *bakcurp;										// DBリンクポインタ
	stLIN		*linp;													// 行ポインタ
	char		*cutp;													// カットポインタ
	stDB		*cbdbp;													// クリップボードDB

	cbdbp=clpbrdGet( CLPBRDP, CB_TXT );									// クリップボードゲット
	if( cbdbp ){														// クリップボードゲット成功
		bakpgp=dbLNKP( PGP )->bakp;										// ページ先頭の前のリンクポインタ
		bakcurp=dbLNKP( CURP )->bakp;									// カレントの前のリンクポインタ
		dbFOR( cbdbp, cutp, char* ){									// カットバッファ総当り
			linp=linAlcStr( LINDBP, cutp );								// 新行登録
			linchg( linp );												// 行変更セット
			dbMov( LINDBP, linp, CURP );								// カレントポインタの前に移動
		}
		PGP=(stLIN*)(bakpgp->nxtp+1);									// ページ先頭ポインタ
		CURP=(stLIN*)(bakcurp->nxtp+1);									// カレントポインタ
		invalidateAll();												// 全て再描画
		dbClose( cbdbp );												// クリップボードDBクローズ
	}
}

/************************************************************************/
/*	F10		行複写														*/
/************************************************************************/

void	f10(void){

	stDBLNK		*bakpgp;												// DBリンクポインタ
	stLIN		*newp;													// 行ポインタ

	bakpgp=dbLNKP( PGP )->bakp;											// ページ先頭の前のリンクポインタ
	newp=linAlcStr( LINDBP, CURTXTP );									// 行作成
	linchg( newp );														// 行変更セット
	dbMov( LINDBP, newp, CURP );										// カレント行の前に移動
	PGP=(stLIN*)(bakpgp->nxtp+1);										// ページ先頭ポインタ
	CURP=newp;															// カレントポインタを新しい行にセット
	invalidateAll();													// 全て再描画
}

/************************************************************************/
/*	F11		キーボードマクロの定義										*/
/************************************************************************/

void	f11(void){

	int		*keyp;														// キーポインタ

	if( !g_macalcdbp ){													// マクロ登録中ではない
		g_macalcdbp=dbOpen( 0, sizeof(int), 0, 4096 );					// マクロ登録開始
		msgSet( "MACRO SET START" );
	}else{																// マクロ登録中
		keyp=(int*)dbEnd( g_macalcdbp );								// 最終キーを得る
		if( *keyp==MKEY_F11 ){											// マクロ定義終了キー
			dbFre( g_macalcdbp, keyp );									// F11 削除
		}else if( *keyp=='@' ){											// マクロ定義終了キー
			dbFre( g_macalcdbp, keyp );									// @ 削除
			keyp=(int*)dbEnd( g_macalcdbp );							// 最終キーを得る
			dbFre( g_macalcdbp, keyp );									// [ESC] 削除
		}
		dbClose( g_macrsvdbp );											// マクロ確保DBクローズ
		g_macrsvdbp=g_macalcdbp;										// マクロ確保DB設定
		g_macalcdbp=0;													// マクロ登録DBクリア
		msgSet( "MACRO SET END" );										// マクロ登録終了
	}
}

/************************************************************************/
/*	UNDO		マクロ実行												*/
/************************************************************************/

void	f12(void){
	dbClose( g_macexedbp );												// マクロ実行DBクローズ
	g_macexedbp=dbDup( g_macrsvdbp );									// マクロ確保DBからマクロ実行DBにコピー
	msgSet( "MACRO EXECUTE" );											// マクロ実行
}

/************************************************************************/
/*	SHIFT+F5	継続後方検索											*/
/************************************************************************/

void	shift_f5(void){

	char	scdbtr[256];												// 検索文字列

	if( scdbtrget( scdbtr ) ){											// 検索文字列を得る
		replace( scdbtr, 0, 1, 0, 0 );									// 検索または置換
	}
}

/************************************************************************/
/*	SHIFT+F6	矩形モード範囲指定開始									*/
/************************************************************************/

void	shift_f6(void){
	cutstart( 1 );														// 範囲指定開始
	msgSet( "矩形選択中" );
}

/************************************************************************/
/*	SHIFT+F7	矩形モードカットバッファへ移動							*/
/************************************************************************/

void	shift_f7(void){
	cutcpy( 1 );														// カットバッファにコピー
}

/************************************************************************/
/*	SHIFT+F8	矩形モードカットバッファへ複写							*/
/************************************************************************/

void	shift_f8(void){
	cutcpy( 0 );														// カットバッファにコピー
}

/************************************************************************/
/*	SHIFT+F9	矩形モードカットバッファから挿入						*/
/************************************************************************/

void	shift_f9(void){

	stLIN		*linp, *newp;											// 行ポインタ
	char		tmpa[0x10000];											// テンポラリバッファ
	char		*cutp;													// カットポインタ
	char		*txtp;													// テキストポインタ
	stDB		*cbdbp;													// クリップボードDB

	cbdbp=clpbrdGet( CLPBRDP, CB_TXT );									// クリップボードゲット
	if( cbdbp ){														// クリップボードゲット成功
		linp=CURP;														// 変更はカレントから
		dbFOR( cbdbp, cutp, char* ){									// カットバッファ総当り
			if( linp ){													// 有効行が存在する
				if( strlen( cutp ) ){									// 挿入すべき文字列は存在する
					txtp=(char*)dbKEYADR( linp );						// テキストポインタ
					if( strlen( txtp )+strlen( cutp )+1<sizeof(tmpa) ){	// 挿入すべき有効な文字列が存在する
						strcpy( tmpa, txtp );							// テンポラリバッファに一旦コピー
						lmSetStr( tmpa, CURXX, cutp, g_tabstp, g_flgins );		// 文字列作成
						linp=linflush( linp, tmpa );					// テキスト更新
						linchg( linp );									// 行変更セット
					}else{												// テキストが長すぎる
						msgSet( "Line Too Long" );
					}
				}
				linp=(stLIN*)dbNxt( linp );								// 処理行更新
			}else{														// 有効行は存在しない
				newp=linAlcStr( LINDBP, cutp );							// ただの行登録
				linchg( newp );											// 行変更セット
			}
		}
		invalidateAll();												// 全て再描画
		dbClose( cbdbp );												// クリップボードDBクローズ
	}
}

/************************************************************************/
/*	CTRL+TAB	水平タブ												*/
/************************************************************************/

void	ctrl_tab(void){

	stLIN		*linp;													// 行ポインタ
	int			toplno, curlno;											// 先頭行番号, カレント行番号
	int			lno;													// 行番号
	int			curxx;													// カーソルＸ座標
	stLM		lm;														// 行マップ
	char		*txtp;													// 行テキストポインタ
	char		*a0;													// 削除ポインタ

	toplno=CUTPGYY+CUTCURYY;											// 選択先頭行番号
	curlno=PGYY+CURYY;													// カレント行番号
	if( CUTCURP && !CUTTYP && curlno>toplno ){							// 選択モード中である
		linp=CUTCURP;													// 行ポインタは選択先頭
		for( lno=toplno; lno<curlno; lno++ ){							// 行番号ループ
			txtp=(char*)dbKEYADR( linp );								// 行テキストポインタ
			lmMak( &lm, txtp, g_tabstp, 0 );							// 行マップ作成
			curxx=lmPosAdj( &lm, CURXX, 0 );							// カーソルＸ座標正規化
			a0=txtp+lm.txtofsa[curxx];									// 削除ポインタセット
			if( *a0=='\t' || *a0==' ' ){								// タブ又はスペースの場合
				strcpy( a0, a0+1 );										// タブ又はスペース削除
				linp=linflush( linp, txtp );							// テキスト更新
				linchg( linp );											// 行変更セット
			}
			linp=(stLIN*)dbNxt( linp );									// 行ポインタ更新
		}
		invalidateAll();												// 全て再描画
	}
}

/************************************************************************/
/*	ウィンドウ元のサイズに戻す											*/
/************************************************************************/

void	alt_f5(void){
#	ifdef	_WIN32
	ShowWindow( g_hwnd, SW_RESTORE );									// ウィンドウ元のサイズに戻す
#	endif
}

/************************************************************************/
/*	ウィンドウ最小化													*/
/************************************************************************/

void	alt_f9(void){
#	ifdef	_WIN32
	ShowWindow( g_hwnd, SW_SHOWMAXIMIZED );								// ウィンドウ最小化
#	endif
}

/************************************************************************/
/*	ウィンドウ最大化													*/
/************************************************************************/

void	alt_f10(void){
#	ifdef	_WIN32
	ShowWindow( g_hwnd, SW_SHOWMAXIMIZED );								// ウィンドウ最大化
#	endif
}

/************************************************************************/
/*	電卓																*/
/************************************************************************/

void	alt_slash(void){

	stLIN		*linp;													// 行ポインタ
	const char	*txtp;													// テキストポインタ
	LLONG		val;
	char		srca[LM_MAX];
	char		tmpa[LM_MAX];

	txtp=SKPSP( CURTXTP );												// テキストポインタはカレントテキストポインタ
	if( txtp[0]=='/' && txtp[1]=='/' ){									// 先頭はコメント
		txtp=SKPSP( txtp+2 );											// コメントスキップ
	}
	txtp=eval( &val, txtp, 0, 0 );
	if( txtp ){
		memcpy( srca, CURTXTP, txtp-CURTXTP );
		srca[txtp-CURTXTP]=0;
		sprintf( tmpa, "%s	%lld 0x%I64x", srca, val, val );
		linp=linflush( CURP, tmpa );									// テキスト更新
		linchg( linp );													// 行変更セット
		invalidateAll();												// 全て再描画
	}
}

/************************************************************************/
/*	浮動小数電卓														*/
/************************************************************************/

void	alt_ast(void){

	stLIN		*linp;													// 行ポインタ
	const char	*txtp;													// テキストポインタ
	FPTYP		val;
	char		srca[LM_MAX];
	char		tmpa[LM_MAX];

	txtp=SKPSP( CURTXTP );												// テキストポインタはカレントテキストポインタ
	if( txtp[0]=='/' && txtp[1]=='/' ){									// 先頭はコメント
		txtp=SKPSP( txtp+2 );											// コメントスキップ
	}
	txtp=evalf( &val, txtp, 0, 0 );
	if( txtp ){
		memcpy( srca, CURTXTP, txtp-CURTXTP );
		srca[txtp-CURTXTP]=0;
		sprintf( tmpa, "%s	%f 0x%08x", srca, val, (int)val );
		linp=linflush( CURP, tmpa );									// テキスト更新
		linchg( linp );													// 行変更セット
		invalidateAll();												// 全て再描画
	}
}

/************************************************************************/
/*	シェル																*/
/************************************************************************/

void	alt_plus(void){

	const char	*txtp;													// テキストポインタ

	txtp=SKPSP( CURTXTP );												// テキストポインタはカレントテキストポインタ
	if( txtp[0]=='/' && txtp[1]=='/' ){									// 先頭はコメント
		txtp=SKPSP( txtp+2 );											// コメントスキップ
	}
	shell( txtp );														// シェル
	invalidateAll();													// 全て再描画
}

/************************************************************************/
/*	SHIFT+SPACE	FEPオープン												*/
/************************************************************************/

void	shift_space(void){
#	ifndef	_WIN32
#		ifndef	CURSES
	fepopen();															// FEPオープン
#		endif
#	endif
}

/************************************************************************/
/*	CTRL+F9		セレクション読み込み									*/
/************************************************************************/

void	ctrl_f9calbak( stDB *cbdbp ){

	stDBLNK		*bakpgp, *bakcurp;										// DBリンクポインタ
	stLIN		*linp;													// 行ポインタ
	char		*cutp;													// カットポインタ

	bakpgp=dbLNKP( PGP )->bakp;											// ページ先頭の前のリンクポインタ
	bakcurp=dbLNKP( CURP )->bakp;										// カレントの前のリンクポインタ
	dbFOR( cbdbp, cutp, char* ){										// カットバッファ総当り
		linp=linAlcStr( LINDBP, cutp );									// 新行登録
		linchg( linp );													// 行変更セット
		dbMov( LINDBP, linp, CURP );									// カレントポインタの前に移動
	}
	PGP=(stLIN*)(bakpgp->nxtp+1);										// ページ先頭ポインタ
	CURP=(stLIN*)(bakcurp->nxtp+1);										// カレントポインタ
	invalidateAll();													// 全て再描画
}

void	ctrl_f9(void){
#	ifndef	_WIN32
#		ifndef	CURSES
	xselDemand( g_xselp, g_tim );										// セレクション要求
#		endif
#	endif
}

