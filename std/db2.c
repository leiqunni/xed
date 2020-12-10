/************************************************************************/
/*																		*/
/*	プログラム内汎用データベースシステム by H.Watanabe					*/
/*	Ver0.00	2009/01/09 21:15:32	作成開始								*/
/*																		*/
/************************************************************************/
/*
┌───────────────────────────────────┐
│	概略																│
└───────────────────────────────────┘
	このモジュールは、プログラム内で使用する汎用データベースシステムです。
	もともとは、アセンブラのためのハッシュテーブル管理を独立させたものです。
	しかし、あまりに使い勝手がよいため、非常に多くの場面で使用しています。
	Windows, unix どちらでもコンパイルできます。
	また、malloc, free を実装してあれば組み込み用でも気軽に使用できます。
┌───────────────────────────────────┐
│	用途																│
└───────────────────────────────────┘
	・可変長配列管理。
		単純な可変長配列管理として使用できます。
	・ローカルヒープ管理
		グローバルヒープから一定サイズでメモリを確保して
		それをローカルヒープとして使用することが出来ます。
		これによりグローバルヒープのフラグメンテーション(断片化)が防止できます。
		またmallocを使用するよりも高速です。
	・メモリ管理。
		dbMemAlcを使用してメモリ確保をすると
		dbClose が呼ばれた時点で責任を持って自動開放をします。
	・名前付スタック管理。
		同じキーで登録すると最後に登録したものが有効になります。
		最後に登録したデータを開放すると、その前のデータが有効になります。
	・検索。
		アセンブラを作る際には無くてはなりません。
		ラベル予約語の高速検索、登録、削除が簡単に行えます。
	・複数のデータを効率よく扱えるため、関数の引数、帰り値にも気楽に使用できます。
┌───────────────────────────────────┐
│	使用法																│
└───────────────────────────────────┘
	まず、dbOpen 関数でデータベース管理構造体をオープンします。
	dbAlc 関数でデータを登録します( dbFre関数で削除できます )。
	dbSch 関数でキーからリストを検索できます。
	また、dbTop,dbEnd,dbNxt,dbBak 関数で登録されているものを総当りできます。
	リストからキーのポインタやリストサイズ、キーサイズを求めるには,
	dbKEYADR,dbLSTSIZ,dbKEYSIZマクロを使用します。
	用が済んだら
	dbClose 関数をコールして確保していたメモリを開放します。
┌───────────────────────────────────┐
│	キーとリストの管理													│
└───────────────────────────────────┘
	キーとリストを一組にして管理しています。
	 (オープン時のオプションによって、キー無、リスト無も選択できます。)
	メモリ上のイメージは以下のようになっています。
	  ┌────────────────────── 一つ前のポインタ( 双方向リスト管理 )
	  │  ┌──────────────────── 一つ後のポインタ( 双方向リスト管理 )
	  │  │  ┌────────────────── リストサイズ
	  │  │  │  ┌──────────────── キーサイズ
	  │  │  │  │				┌─────── アライン( キーデータの先頭がＮバイト境界に合うように )
	  │  │  │  │				│
	  ↓  ↓  ↓  ↓				↓
	┌─┬─┬─┬─┬──────┬─┬─────┐
	│　│　│　│　│リストデータ│　│キーデータ│
	└─┴─┴─┴─┴──────┴─┴─────┘
					↑
					│
					└─────────────── ユーザーに渡されるリストポインタ
┌───────────────────────────────────┐
│	マクロリファレンス													│
└───────────────────────────────────┘
	──────────────────────────────────
	dbLSTSIZ(P)		ユーザリストサイズを得る
	──────────────────────────────────
	引数:
		P:		リストポインタ
	解説:
		リストポインタからリストのサイズを参照します。

	──────────────────────────────────
	dbKEYSIZ(P)		ユーザキーサイズを得る
	──────────────────────────────────
	引数:
		P:		リストポインタ
	解説:
		リストポインタからキーのサイズを参照します。

	──────────────────────────────────
	dbKEYADR(P)		ユーザキーアドレスを得る
	──────────────────────────────────
	引数:
		P:		リストポインタ
	解説:
		リストポインタからキーアドレスを参照します。

	──────────────────────────────────
	dbFOR(H,P,C)	ユーザーデータ総当りループ
	──────────────────────────────────
	引数:
		H:		データベースポインタ
		P:		リストポインタ変数
		C:		型名
	解説:
		for( P=( C )dbTop( H );P;P=( C )dbNxt( P ) ) に展開されます。
		登録されているデータを順番に総当りする場合に使用します。
		但し、リストポインタ変数を使用して、次のリストポインタを求めているため
		このループの中で、dbFre( リストポインタ変数 )は指定できません。
		その場合は dbFOR2 マクロを使用してください。

	──────────────────────────────────
	dbFOR2(H,P,N,C)	ユーザーデータ総当りループ
	──────────────────────────────────
	引数:
		H:		データベースポインタ
		P:		リストポインタ変数
		N:		テンポラリリストポインタ変数( 次のリストポインタを示します )
		C:		型名
	解説:
		for( P=( C )dbTop( H ); N=( C )dbNxt( P ), P; P=N ) に展開されます。
		登録されているデータを順番に総当りする場合に使用します。
		dbFOR* マクロの中では一番汎用性が高いのですが、
		テンポラリリストポインタを準備しなければならないので少々面倒です。
		このループの中で、dbFre( リストポインタ変数 )を実行する場合のみ使用しています。
*/

#	include	<assert.h>													/* アサーション*/
#	include	<stdarg.h>													/* 可変引数処理定義*/
#	include	<stdlib.h>													/* 標準ライブラリ関数*/
#	include	<string.h>													/* 標準文字列関数*/
#	include	"db2.h"														/* プログラム内汎用データベースシステム*/

/************************************************************************/
/*	メモリ確保・開放定義												*/
/************************************************************************/

#	ifdef	DB_MALLOC
		void	*DB_MALLOC( int );
		void	DB_FREE( void * );
#	else
#		define	DB_MALLOC	malloc
#		define	DB_FREE		free
#	endif

/************************************************************************/
/*	ローカル定義														*/
/************************************************************************/

#	define	dbLNK(BAK,NXT)		((BAK)->nxtp=(NXT),(NXT)->bakp=(BAK))	/* 双方向リストリンク*/
#	define	dbDEL(LST)			dbLNK((LST)->bakp,(LST)->nxtp)			/* 双方向リスト削除*/
#	define	dbINI(TOP,END)		((TOP)->bakp=(END)->nxtp=0,dbLNK(TOP,END))		/* 双方向リスト初期化*/
#	define	dbINSBAK(NEW,NXT)	(dbLNK((NXT)->bakp,NEW),dbLNK(NEW,NXT))	/* 双方向リスト挿入*/
#	define	dbINSNXT(BAK,NEW)	(dbLNK(NEW,(BAK)->nxtp),dbLNK(BAK,NEW))	/* 双方向リスト後方挿入*/
#	define	dbUDELNXT(BAK)		((BAK)->slp=(BAK)->slp->slp)			/* 単方向リスト削除*/
#	define	dbUINSNXT(BAK,NEW)	((NEW)->slp=(BAK)->slp,(BAK)->slp=(NEW))/* 単方向リスト挿入*/

	typedef struct	tgMCB{												// メモリコントロールブロック構造体
		struct tgMCB	*nxtp, *bakp;									// 双方向リスト
		stDB			*dbp;											// このブロックを確保したヒープ管理構造体ポインタ
		int				blk;											// ブロック数( 一部でバイトサイズとして使用している )
	}	stMCB;

	typedef struct tgMLST	stMLST;
	struct	tgMLST{														// メモリ管理のための構造体
		stMLST			*nxtp, *bakp;									// 双方向リストポインタ
		stDB			*dbp;											// このブロックを確保したヒープ管理構造体ポインタ
		int				siz;											// 確保サイズ
	};

	typedef struct tgSL	stSL;
	struct	tgSL{														// 単方向リスト管理
		stSL			*slp;											// 単方向リストポインタ
	};

	struct	tgDB{														// データベースポインタ
		stSL			*slpadr;										// ハッシュテーブルアドレス
		stDBLNK			lnktop, lnkend;									// 双方向リストダミー先頭、終端
		stMLST			mlsttop, mlstend;								// メモリ確保双方向リストダミー先頭、終端
		int				tblmsk;											// ハッシュ値マスク
		int				keysiz;											// ユーザーキーサイズ
		int				lstsiz;											// ユーザーリストサイズ
		int				alcn;											// 登録個数
		unsigned int	hpsiz;											// ローカルヒープサイズ
		stMCB			fretop, freend;									// 空き	 ( 双方向リストダミー先頭・終端 )
		stMCB			usetop, useend;									// 使用中( 双方向リストダミー先頭・終端 )
		stMCB			alctop, alcend;									// グローバルメモリ確保( 双方向リストダミー先頭・終端 )
	};

/************************************************************************/
/*	ヒープ解放															*/
/************************************************************************/

static void	memFre( stDB *dbp, void *memp ){

	stMCB		*mcbp, *bakp, *p, *nxtp;								// 双方向リストアクセスワーク

	if( memp ){															// 有効なメモリアドレス
		if( dbp->hpsiz ){												// ローカルヒープ有効
			mcbp=(stMCB*)memp-1;										// ユーザに渡す値は管理ポインタ＋１
			assert( mcbp->dbp==dbp );									// ヒープ管理ポインタチェック
			assert( mcbp->nxtp->dbp==dbp );								// 次のヒープ管理ポインタチェック
			assert( mcbp->nxtp->bakp==mcbp );							// 次の双方向リストリンクチェック
			assert( mcbp->bakp->dbp==dbp );								// 前のヒープ管理ポインタチェック
			assert( mcbp->bakp->nxtp==mcbp );							// 前の双方向リストリンクチェック
			dbDEL( mcbp );												// 使用中リストからの開放
			for( p=dbp->fretop.nxtp; ( nxtp=p->nxtp )!=0; p=nxtp ){		// 空きエリア検索
				if( mcbp<p ){
					break;
				}
			}
			bakp=p->bakp;
			if( nxtp ){													// 最後尾ではない
				assert( !bakp->bakp || bakp+bakp->blk<=mcbp );
				dbINSBAK( mcbp, p );									// とりあえず挿入
				if( mcbp+mcbp->blk==p ){								// 後ブロックの直前
					dbLNK( mcbp, p->nxtp );								// 後ブロック削除
					mcbp->blk+=p->blk;									// カレントブロックサイズ加算
				}
				if( bakp+bakp->blk==mcbp ){								// 前ブロックの直後
					dbLNK( bakp, mcbp->nxtp );							// カレントブロック削除
					bakp->blk+=mcbp->blk;								// 前ブロックサイズ加算
				}
			}else{														// 最後尾
				if( bakp+bakp->blk==mcbp ){								// 前ブロックの直後
					bakp->blk+=mcbp->blk;								// 前ブロックサイズ加算
				}else{
					dbINSBAK( mcbp, p );								// 最後尾に挿入
				}
			}
		}else{															// ローカルヒープ無効
			DB_FREE( memp );											// グローバルメモリ開放
		}
	}
}

/************************************************************************/
/*	ヒープフリー領域を追加												*/
/************************************************************************/

static void	memAddFre( stDB *dbp, stMCB *mcbp, int siz ){

	mcbp->blk=siz/sizeof(stMCB);										// ブロックサイズの計算
	mcbp->dbp=dbp;														// このブロックを確保したヒープ管理ポインタ
	dbINSBAK( mcbp, &dbp->useend );										// 後で解放されるのでダミー登録
	memFre( dbp, mcbp+1 );												// 解放
}

/************************************************************************/
/*	ヒープ確保															*/
/************************************************************************/

static void	*memAlc( stDB *dbp, unsigned int siz ){

	stMCB			*mcbp=0;											// メモリコントロールブロックポインタ
	stMCB			*p, *nxtp;											// 双方向リストアクセスワーク
	int				blk, modblk;										// ブロック数
	unsigned int	alcsiz;												// 確保サイズ

	if( siz ){															// 有効なサイズ
		if( dbp->hpsiz ){												// ローカルヒープ有効
			blk=( siz+(sizeof(stMCB)*2-1) )/sizeof( stMCB );			// 必要サイズの計算
			for( p=dbp->fretop.nxtp; ( nxtp=p->nxtp )!=0; p=nxtp ){		// 先頭から検索
				if( p->blk>=blk ){										// 必要サイズ以上
					break;												// ループ終了
				}
			}
			if( nxtp ){													// 空きあり
				mcbp=p;													// メモリコントロールブロックポインタセット
				modblk=p->blk-blk;										// 余り空きサイズ
				if( modblk>=2 ){										// ２ブロック以上余計にあるので分割する
					nxtp=mcbp+blk;										// 新しく作成するブロック
					nxtp->blk=modblk;									// 新しく作成するブロックサイズ
					mcbp->blk=blk;										// 現在のブロックサイズ変更
					dbINSNXT( mcbp, nxtp );								// 新ブロック挿入
				}
				dbDEL( mcbp );											// 空きリストから削除
				dbINSBAK( mcbp, &dbp->useend );							// 使用中リストに追加
				mcbp->dbp=dbp;											// 確保した管理ワークアドレスセット
				mcbp++;													// ユーザに渡す値はメモリコントロールブロックポインタ＋１
			}else{														// 空き無し
				alcsiz=((siz+3*sizeof(stMCB)-1)/sizeof(stMCB))*sizeof(stMCB);
				if( alcsiz<dbp->hpsiz ){
					alcsiz=dbp->hpsiz;
				}
				mcbp=(stMCB*)DB_MALLOC( alcsiz );						// グローバルメモリ確保
				mcbp->blk=alcsiz;										// メモリ確保サイズとして使用している
				mcbp->dbp=0;											// 念のため
				dbINSBAK( mcbp, &dbp->alcend );							// 確保中リストに追加
				memAddFre( dbp, mcbp+1, alcsiz-sizeof(stMCB) );			// ヒープフリー領域を追加
				mcbp=(stMCB*)memAlc( dbp, siz );						// 再帰コール
			}
		}else{															// ローカルヒープ無効
			mcbp=(stMCB*)DB_MALLOC( siz );								// グローバルメモリ確保
		}
	}
	return( mcbp );														// 0:確保不能, 0以外確保したメモリアドレス
}

/************************************************************************/
/*	ハッシュ値計算してテーブルの先頭を求める							*/
/************************************************************************/

static stSL	*dbHash( const stDB *dbp, const char *keyp, int keysiz ){

	int		hash=0;														// ハッシュ値計算用ワーク
	int		i;															// ループカウンタ

	if( dbp->keysiz==DBSIZ_RSV ){										// 大文字小文字を区別しない文字列
		for( i=0; i<keysiz; i++ ){										// ユーザーキーサイズ分ループ
			hash=((*keyp++)|0x20)+(hash<<6)-hash;						// 元の値を63倍して新しい値を足す
		}
	}else{																// バイナリーまたは大文字小文字を区別する文字列
		for( i=0; i<keysiz; i++ ){										// ユーザーキーサイズ分ループ
			hash=(*keyp++)+(hash<<6)-hash;								// 元の値を63倍して新しい値を足す
		}
	}
	return( &dbp->slpadr[hash&dbp->tblmsk] );							// 単方向リスト先頭
}

/************************************************************************/
/*	データベースをオープンします										*/
/************************************************************************/

//	引数:
//		keysiz:	キーサイズまたはキータイプを指定します。
//			正の値の場合はキーサイズの指定です。
//				0を指定するとキー無になります。
//				それ以外は固定長のキーサイズとして認識されます。
//			負の値の場合は予約値です:
//				DBSIZ_VAR:	登録、検索の場合にアドレスの後にサイズを指定します。
//				DBSIZ_STR:	文字列として扱います。
//				DBSIZ_RSV:	大文字小文字を区別しない文字列として扱います( Windowsのファイル名管理によく使用します )。
//				DBSIZ_LBL:	ラベル文字列として扱います
//				DBSIZ_INT:	直値として扱います
//				DBSIZ_PTR:	ポインタとして扱います
//		lstsiz:	リストサイズまたはリストタイプを指定します。
//			正の値の場合はリストサイズの指定です。
//				0を指定するとリスト無になります。
//				それ以外は固定長のリストサイズとして認識されます。
//			負の値の場合は予約値です:
//				DBSIZ_VAR:	登録、検索の場合にアドレスの後にサイズを指定します。
//				DBSIZ_STR:	文字列として扱います。
//				DBSIZ_RSV:	大文字小文字を区別しない文字列として扱います。
//				DBSIZ_LBL:	ラベル文字列として扱います
//				DBSIZ_INT:	直値として扱います
//				DBSIZ_PTR:	ポインタとして扱います
//		hashn:	ハッシュテーブル個数の指定です。
//				0を指定するとハッシュ管理を行いません。
//				( その場合に検索するとリニア検索をします )
//				0でない場合は２のＮ乗数でなければなりません。
//				2のＮ乗数でない場合はアサーションとなります。
//				オープンハッシュ法を使用していますので、
//				テーブルサイズは登録個数よりも小さくてもかまいません。
//				私の場合、256,1024,4096をよく使用します。
//		hpsiz:	ローカルヒープサイズ
//				0を指定すると、ローカルヒープを使用しません。
//				( 登録のたびにグローバルからメモリ確保します )
//				0でない場合は hpsiz 分まとめてローカルヒープを確保して
//				登録はローカルヒープを使用します。
//	戻り値:
//		成功した場合はデータベースポインタが帰ります。
//		失敗した場合はNULLが帰ります。
//	解説:
//		指定されたキーサイズ、リストサイズ、ハッシュテーブル個数に従って
//		データベースを確保して初期化します。
//	例:
//		typedef struct{
//			int		val;
//		}	stLST;
//		stDB	*dbp;
//
//		・キーは文字列、リストサイズは固定サイズ、ハッシュテーブル個数は256でオープンする
//
//		dbp=dbOpen( DBSIZ_STR, sizeof(stLST), 256, 0 );
//
//		・キーは大文字小文字を区別しない文字列、リストサイズは無、ハッシュテーブル個数は4096でオープンする
//
//		dbp=dbOpen( DBSIZ_RSV, 0, 4096, 0 );
//
//		・キーは無、リストサイズは登録・検索時に指定、ハッシュテーブル無、ブロックサイズ0x4000バイトでオープンする
//		  ( 単なる可変長配列管理として使用したい場合に多い指定です。)
//
//		dbp=dbOpen( 0, DBSIZ_VAR, 0, 0x4000 );

stDB	*dbOpen( int keysiz, int lstsiz, int hashn, int hpsiz ){

	stDB	*dbp;														// データベースポインタ

	assert( hashn==(hashn&-hashn) );									// ハッシュテーブルサイズは２のＮ乗数である
	dbp=(stDB*)DB_MALLOC( sizeof(stDB)+sizeof(stSL*)*hashn );			// データベース管理構造体とハッシュテーブルメモリ確保
	assert( dbp );														// メモリ確保チェック
	memset( dbp, 0, sizeof(stDB)+sizeof(stSL*)*hashn );					// データベース管理構造体とハッシュテーブルメモリクリア
	dbINI( &dbp->lnktop, &dbp->lnkend );								// 双方向リスト初期化
	dbINI( &dbp->mlsttop, &dbp->mlstend );								// 双方向リスト初期化
	dbp->keysiz=keysiz;													// ユーザーキーサイズ
	dbp->lstsiz=lstsiz;													// ユーザーリストサイズ
	dbp->tblmsk=hashn-1;												// ハッシュテーブルマスクセット
	if( hashn ){														// ハッシュテーブルを使用する
		dbp->slpadr=(stSL*)(dbp+1);										// ハッシュテーブルアドレスセット
	}
	if( hpsiz ){														// ローカルヒープ有効
		dbp->hpsiz=hpsiz;												// ローカルヒープ
		dbINI( &dbp->fretop, &dbp->freend );							// 空きメモリコントロールブロックリスト初期化
		dbINI( &dbp->usetop, &dbp->useend );							// 使用メモリコントロールブロックリスト初期化
		dbINI( &dbp->alctop, &dbp->alcend );							// ローカルヒープメモリコントロールブロック初期化
		dbp->fretop.dbp=dbp;											// メモリ開放時のチェック用
		dbp->freend.dbp=dbp;											// メモリ開放時のチェック用
		dbp->usetop.dbp=dbp;											// メモリ開放時のチェック用
		dbp->useend.dbp=dbp;											// メモリ開放時のチェック用
	}
	return( dbp );														// データベースポインタを持ってリターン
}

/************************************************************************/
/*	データベースをクローズします										*/
/************************************************************************/

//	引数:
//		dbp:	データベースポインタ
//	解説:
//		確保していたメモリを全て開放します。
//		dbp に NULL を指定すると何もしません。

void	dbClose( stDB *dbp ){

	stDBLNK		*p, *nxtp;												// 双方向リストポインタ
	stMLST		*mp, *nxtmp;											// 双方向リストポインタ
	stMCB		*mcbp, *nxtmcbp;										// ローカルヒープ確保双方向リストポインタ

	if( dbp ){															// 有効なデータベースポインタ
		if( dbp->hpsiz ){												// ローカルヒープ有効
			for( mcbp=dbp->alctop.nxtp; (nxtmcbp=mcbp->nxtp)!=0; mcbp=nxtmcbp ){				// メモリ確保総当りループ
				DB_FREE( mcbp );										// グローバルメモリ開放
			}
		}else{															// ローカルヒープ無効
			for( mp=dbp->mlsttop.nxtp; (nxtmp=mp->nxtp)!=0; mp=nxtmp ){	// ローカルメモリ管理構造体ループ
				DB_FREE( mp );											// ローカルメモリ開放
			}
			for( p=dbp->lnktop.nxtp; (nxtp=p->nxtp)!=0; p=nxtp ){		// 双方向リストループ
				if( dbp->slpadr ){										// ハッシュ表が使用されている
					DB_FREE( ((char *)p)-dbALG(sizeof(stSL)) );			// 単方向リストを使用しているので-1した値を開放
				}else{													// ハッシュ表は使用していない
					DB_FREE( p );										// リンクポインタそのまま開放
				}
			}
		}
#	ifndef	NDEBUG
		memset( dbp, 0, sizeof(stDB) );
#	endif
		DB_FREE( dbp );													// データベース管理構造体メモリ開放
	}
}

/************************************************************************/
/*	データベースにデータをフルスペックで登録します						*/
/************************************************************************/

//	引数:
//		dbp:	データベースポインタ
//		keyadr:	キーアドレス
//		keysiz:	キーサイズ
//		lstadr:	リストアドレス
//		lstsiz:	リストサイズ
//	戻り値:
//		登録したリストのポインタが帰ります。
//	解説:
//		データベースにフルスペックでデータを登録します。
//		dbAlc 関数の下請けです。
//		この関数を使用することにより、dbOpen で指定したキーやリストを無視して
//		登録することが出来ます。

void	*dbAlcS( stDB *dbp, const void *keyadr, int keysiz, const void *lstadr, int lstsiz ){

	void		*lstp;													// ユーザーリストポインタ
	int			keyalg, lstalg;											// アラインされたユーザーキーサイズ、ユーザーリストサイズ
	stSL		*bakp, *slp;											// 単方向リストポインタ
	stDBLNK		*lnkp;													// リンクポインタ
	char		*wrtp;													// メモリ書き込みのためのテンポラリポインタ

	keyalg=dbALG( keysiz );												// アライン済みのユーザーキーサイズ
	lstalg=dbALG( lstsiz );												// アライン済みのユーザーリストサイズ
	if( dbp->slpadr ){													// ハッシュ表が使用されている
		slp=(stSL*)memAlc( dbp, dbALG(sizeof(stSL))+sizeof(stDBLNK)+lstalg+keyalg );			// 単方向リストを先頭にメモリ確保
		assert( slp );
		lnkp=(stDBLNK*)((char*)slp+dbALG(sizeof(stSL)));				// リンクポインタは単方向リストポインタ＋１
		bakp=dbHash( dbp, (char*)keyadr, keysiz );						// ハッシュ値計算してテーブルの先頭を求める
		dbUINSNXT( bakp, slp );											// 単方向リスト挿入
	}else{																// ハッシュ表は使用していない
		lnkp=(stDBLNK*)memAlc( dbp, sizeof(stDBLNK)+lstalg+keyalg );	// リンクポインタを先頭にしてメモリ確保
		assert( lnkp );
	}
	lstp=wrtp=(char*)(lnkp+1);											// 帰り値
	if( (lnkp->lstsiz=lstsiz)!=0 ){										// ユーザーリストサイズは有功
		if( lstadr ){													// ユーザーリストアドレスは有功
			memcpy( wrtp, lstadr, lstsiz );								// ユーザーリストデータコピー
			memset( wrtp+lstsiz, 0, lstalg-lstsiz );					// アラインエリアクリア
		}else{															// ユーザーリストアドレスは無効
			memset( wrtp, 0, lstalg );									// ユーザーリストメモリクリア
		}
		wrtp+=lstalg;													// 書き込みアドレス加算
	}
	if( (lnkp->keysiz=keysiz)!=0 ){										// ユーザーキーサイズは有功
		if( keyadr ){													// ユーザーキーアドレスは有功
			memcpy( wrtp, keyadr, keysiz );								// ユーザーキーデータコピー
			memset( wrtp+keysiz, 0, keyalg-keysiz );					// アラインエリアクリア
		}else{															// ユーザーキーアドレスは無効
			memset( wrtp, 0, keyalg );									// ユーザーキーメモリクリア
		}
	}
	dbINSBAK( lnkp, &dbp->lnkend );										// 双方向リスト挿入
	dbp->alcn++;														// 登録個数加算
	return( lstp );														// ユーザーリストポインタを帰り値とする
}

/************************************************************************/
/*	可変引数を処理してデータベースにデータを登録します					*/
/************************************************************************/

//	引数:
//		dbp:	データベースポインタ
//		...:	キーとリストの情報を順番で指定します。
//				ただし、どの順番で何を指定するかは、dbOpen で指定したキーとリストのサイズによって変わります。
//				サイズが０の場合:
//					無いので<データアドレス>、<データサイズ>とも指定しません。
//				サイズが１以上の正の値の場合:
//					<データサイズ>は固定サイズですでに分かっているので、
//					<データアドレス>だけを指定します。
//				サイズが DBSIZ_VAR の場合
//					<データアドレス>と<データサイズ>を指定します。
//				サイズが DBSIZ_STR, DBSIZ_RSVの場合
//					文字列の<データアドレス>を指定します。
//
//				また、<データアドレス>がNULLの場合にはゼロクリアされたメモリが指定されたものとみなします。
//
//				例:
//					・dbp=dbOpen( DBSIZ_VAR, DBSIZ_VAR, ???, ??? )　と指定した場合
//
//					dbAlc( dbp, <キーアドレス>, <キーサイズ>, <リストアドレス>, <リストサイズ> )
//
//					・dbp=dbOpen( 0, DBSIZ_STR, ???, ??? )　と指定した場合
//
//					dbAlc( dbp, <リストアドレス> )
//
//	戻り値:
//		登録したリストのポインタが帰ります。
//	解説:
//		データベースにデータを登録します。
//		ユーザーデータは内部管理メモリにコピーされますので、
//		ユーザーデータは保持しておく必要はありません。

void	*dbAlc( stDB *dbp, ... ){

	va_list		ap;														// 可変引数ワーク
	void		*lstp=0;												// 帰り値
	char		*keyadr=0, *lstadr=0;									// ユーザーキーアドレス、ユーザーリストアドレス
	int			keysiz, lstsiz;											// ユーザーキーサイズ、ユーザーリストサイズ
	char		keybuf[1024], lstbuf[1024];								// ラベルバッファ
	int			keyint, lstint;											// 数値

	if( dbp ){															// データベースポインタチェック
		va_start( ap, dbp );											// 可変引数オープン
		if( ( keysiz=dbp->keysiz )!=0 ){								// キーサイズが存在する
			keyadr=va_arg( ap, char * );								// 引数からユーザーキーアドレス読み込み
			switch( keysiz ){											// 特殊ユーザーキーサイズの判定
				case DBSIZ_VAR:											// キーサイズは可変
					keysiz=va_arg( ap, int );							// 可変引数からキーサイズを得る
					break;
				case DBSIZ_STR:											// 文字列として扱います。
				case DBSIZ_RSV:											// 大文字小文字を区別しない文字列として扱います
					keysiz=1;											// キーアドレスが0の場合のキーサイズ
					if( keyadr ){										// キーアドレスが指定されている
						keysiz=strlen( keyadr )+1;						// ユーザーキーサイズは文字列長＋１
					}
					break;
				case DBSIZ_LBL:											// ラベル
					keysiz=dbLblLen( keyadr, sizeof(keybuf) )+1;		// キーサイズ
					memcpy( keybuf, keyadr, keysiz-1 );					// バッファにコピー
					keybuf[keysiz-1]=0;									// ０ターミネート
					keyadr=keybuf;										// キーアドレスはバッファ
					break;
				case DBSIZ_INT:											// 数値
				case DBSIZ_PTR:											// ポインタ
					keyint=(int)keyadr;									// テンポラリ
					keyadr=(char*)&keyint;								// キーアドレス
					keysiz=sizeof(int);									// キーサイズ
					break;
			}
		}
		if( ( lstsiz=dbp->lstsiz )!=0 ){								// リストサイズが存在する
			lstadr=va_arg( ap, char * );								// 引数からユーザーリストアドレス読み込み
			switch( lstsiz ){											// 特殊ユーザーリストサイズの判定
				case DBSIZ_VAR:											// ユーザーリストサイズは登録時に指定
					lstsiz=va_arg( ap, int );							// 可変引数からリストサイズを得る
					break;
				case DBSIZ_STR:											// 文字列として扱います。
				case DBSIZ_RSV:											// 大文字小文字を区別しない文字列として扱います
					lstsiz=1;											// リストアドレスが0の場合のリストサイズ
					if( lstadr ){										// リストアドレスが指定されている
						lstsiz=strlen( lstadr )+1;						// ユーザーリストサイズは文字列長＋１
					}
					break;
				case DBSIZ_LBL:											// ラベル
					lstsiz=dbLblLen( lstadr, sizeof(lstbuf) )+1;		// リストサイズ
					memcpy( lstbuf, lstadr, lstsiz-1 );					// バッファにコピー
					lstbuf[lstsiz-1]=0;									// ０ターミネート
					lstadr=lstbuf;										// リストアドレスはバッファ
					break;
				case DBSIZ_INT:											// 数値
				case DBSIZ_PTR:											// ポインタ
					lstint=(int)lstadr;									// テンポラリ
					lstadr=(char*)&lstint;								// キーアドレス
					lstsiz=sizeof(int);									// キーサイズ
					break;
			}
		}
		va_end( ap );													// 可変引数クローズ
		lstp=dbAlcS( dbp, keyadr, keysiz, lstadr, lstsiz );				// データベースにデータを登録します
	}
	return( lstp );
}

/************************************************************************/
/*	キー比較															*/
/************************************************************************/

static int	dbKeyCmp( const stDB *dbp, const void *dstadr, int dstsiz, const void *souadr, int sousiz ){

	int			res=0;													// 0:違う, 1:同じ

	if( dstsiz==sousiz ){												// 同じサイズである
		if( dbp->keysiz==DBSIZ_RSV ){									// 予約語( 大文字小文字を区別しない )
			res=!dbMemCaseCmp( dstadr, souadr, sousiz );				// 大文字小文字を区別しない漢字対応メモリ比較
		}else{															// 通常データ列
			res=!memcmp( dstadr, souadr, sousiz );						// べた比較をする
		}
	}
	return( res );														// 帰り値( 0:違う, 1:同じ )
}

/************************************************************************/
/*	データベースからキーにしたがって検索します							*/
/************************************************************************/

static void	*dbSchS( const stDB *dbp, const void *keyadr, int keysiz ){

	void		*lstp=0;												// ユーザーリストポインタ
	int			lstalg;													// アラインされたユーザーリストサイズ
	stSL		*bakp, *slp;											// 単方向リストポインタ
	stDBLNK		*p, *nxtp;												// 双方向リストポインタ
	int			find;													// 見つかったフラグ

	if( dbp->slpadr ){													// ハッシュ表が使用されている
		bakp=dbHash( dbp, (char*)keyadr, keysiz );						// ハッシュ値計算してテーブルの先頭を求める
		while( (slp=bakp->slp)!=0 ){									// 単方向リスト検査クループ
			p=(stDBLNK *)((char*)slp+dbALG(sizeof(stSL)));				// リンクポインタ
			lstalg=dbALG( p->lstsiz );									// アラインされたリストサイズ
			find=dbKeyCmp( dbp, (char*)(p+1)+lstalg, p->keysiz, keyadr, keysiz );
			if( find ){													// 見つかった
				lstp=(p+1);												// 帰り値セット
				break;													// ループ終了
			}
			bakp=slp;													// 単方向リストポインタ更新
		}
	}else{																// ハッシュ表は使用していないのでベタ探索
		for( p=dbp->lnkend.bakp; (nxtp=p->bakp)!=0; p=nxtp ){			// 双方向リストループ
			lstalg=dbALG( p->lstsiz );									// アラインされたリストサイズ
			find=dbKeyCmp( dbp, (char*)(p+1)+lstalg, p->keysiz, keyadr, keysiz );
			if( find ){
				lstp=(p+1);												// 帰り値セット
				break;													// ループ終了
			}
		}
	}
	return( lstp );														// リストポインタを帰り値とする
}

/************************************************************************/
/*	可変引数を処理してデータベースからキーにしたがって検索します		*/
/************************************************************************/

//	引数:
//		dbp:	データベースポインタ
//		...:	キーの情報を指定します。
//				ただし、どの順番で何を指定するかは、dbOpen で指定したキーのサイズによって変わります。
//				サイズが０の場合:
//					無効です。
//				サイズが１以上の正の値の場合:
//					<データサイズ>は固定サイズですでに分かっているので、
//					<データアドレス>だけを指定します。
//				サイズが DBSIZ_VAR の場合
//					<データアドレス>と<データサイズ>を指定します。
//				サイズが DBSIZ_STR, DBSIZ_RSVの場合
//					文字列の<データアドレス>を指定します。
//				例:
//					・dbp=dbOpen( DBSIZ_VAR, ???, ???, ??? )　と指定した場合
//
//					dbSch( dbp, <キーアドレス>, <キーサイズ> )
//
//					・dbp=dbOpen( DBSIZ_STR, ???, ???, ??? )　と指定した場合
//
//					dbSch( dbp, <文字列アドレス> )
//
//	戻り値:
//		検索したリストのポインタが帰ります。
//		見つからない場合にはNULLが帰ります。
//	解説:
//		データベースにからキーに当てはまるデータを検索します。
//		もし、ハッシュ表を使用していない場合は線形探索を行います。

void	*dbSch( const stDB *dbp, ... ){

	void		*lstp=0;												// ユーザーリストポインタ
	va_list		ap;														// 可変引数ワーク
	char		*keyadr=0;												// ユーザーキーアドレス
	int			keysiz;													// ユーザーキーサイズ
	char		label[1024];											// ラベル
	int			keyint;													// 数値

	if( dbp ){															// データベースポインタチェック
		va_start( ap, dbp );											// 可変引数オープン
		if( ( keysiz=dbp->keysiz )!=0 ){								// ユーザーキーサイズは有功
			keyadr=va_arg( ap, char * );								// 引数からユーザーキーアドレス読み込み
			switch( keysiz ){											// 特殊ユーザーキーサイズの判定
				case DBSIZ_VAR:											// ユーザーキーサイズは登録時に指定
					keysiz=va_arg( ap, int );
					break;
				case DBSIZ_STR:											// 文字列
				case DBSIZ_RSV:											// 大文字小文字を区別しない文字列
					keysiz=strlen( keyadr )+1;							// ユーザーキーサイズは文字列長＋１
					break;
				case DBSIZ_LBL:											// ラベル
					keysiz=dbLblLen( keyadr, sizeof(label) )+1;			// キーサイズ
					memcpy( label, keyadr, keysiz-1 );					// バッファにコピー
					label[keysiz-1]=0;									// ０ターミネート
					keyadr=label;										// キーアドレスはバッファ
					break;
				case DBSIZ_INT:											// 数値
				case DBSIZ_PTR:											// ポインタ
					keyint=(int)keyadr;
					keyadr=(char*)&keyint;
					keysiz=sizeof(int);
					break;
			}
		}
		va_end( ap );													// 可変引数クローズ
		lstp=dbSchS( dbp, keyadr, keysiz );								// データベースからキーにしたがって検索します
	}
	return( lstp );														// リストポインタを帰り値とする
}

/************************************************************************/
/*	データベースからデータを開放します									*/
/************************************************************************/

//	引数:
//		dbp:	データベースポインタ
//		lstp:	リストポインタを指定します。
//	解説:
//		データベースにからデータを削除します。

void	dbFre( stDB *dbp, void *lstp ){

	stSL		*bakp, *slp;											// 単方向リストポインタ
	stDBLNK		*lnkp;													// リンクポインタ
	int			lstalg;													// アラインされたユーザーリストサイズ

	assert( dbp );														// データベースポインタチェック
	assert( lstp );														// ユーザーリストポインタチェック
	lnkp=dbLNKP(lstp);													// ユーザーリストポインタからリンクポインタに変換
	assert( lnkp->nxtp->bakp==lnkp );									// 次の双方向リストリンクチェック
	assert( lnkp->bakp->nxtp==lnkp );									// 前の双方向リストリンクチェック
	if( dbp->slpadr ){													// ハッシュ表が使用されている
		lstalg=dbALG( lnkp->lstsiz );									// アラインされたユーザーリストサイズ
		bakp=dbHash( dbp, ((char*)lstp)+lstalg, lnkp->keysiz );			// ハッシュ値計算してテーブルの先頭を求める
		while( (slp=bakp->slp)!=0 ){									// 単方向リスト検索ループ
			if( lnkp==(stDBLNK *)(((char*)slp)+dbALG(sizeof(stSL))) ){	// 見つかった
				dbUDELNXT( bakp );										// 単方向リスト削除
				dbDEL( lnkp );											// 双方向リスト削除
				memFre( dbp, slp );										// メモリ開放
				break;													// ループ終了
			}
			bakp=slp;													// 単方向リストポインタ更新
		}
		assert( slp );													// 見つからなかった
	}else{																// ハッシュ表は使用していない
		dbDEL( lnkp );													// 双方向リスト削除
		memFre( dbp, lnkp );											// メモリ開放
	}
	dbp->alcn--;														// 登録個数減算
}

/************************************************************************/
/*	データベースに登録されている個数を返します							*/
/************************************************************************/

//	引数:
//		dbp:	データベースポインタ
//	戻り値:
//		現在登録されているデータ数が帰ります。
//	解説:
//		現在登録されているデータ数を返します。
//		dbpがNULLの場合は常に0が帰ります。

int		dbAlcN( const stDB *dbp ){

	int		alcn=0;														// 登録個数

	if( dbp ){															// 有効なデータベースポインタチェック
		alcn=dbp->alcn;													// 個数をセット
	}
	return( alcn );														// 登録個数
}

/************************************************************************/
/*	データベースに登録されている先頭データのポインタを返します			*/
/************************************************************************/

//	引数:
//		dbp:	データベースポインタ
//	戻り値:
//		現在登録されている先頭のリストポインタが帰ります。
//	解説:
//		現在登録されている先頭のリストポインタを返します。
//		dbp が NULL の場合は常に NULL が帰ります。

void	*dbTop( const stDB *dbp ){

	stDBLNK		*lnkp;													// リンクポインタ
	void		*topp=0;												// 先頭ユーザーリストポインタ

	if( dbp ){															// 有効なデータベースポインタチェック
		lnkp=dbp->lnktop.nxtp;											// リストの先頭を得る
		if( lnkp->nxtp ){												// ダミー終端ではない
			topp=lnkp+1;												// ユーザに渡すのはリンクポインタ+1
		}
	}
	return( topp );														// 0:無し, 0以外:有効なデータポインタ
}

/************************************************************************/
/*	データベースに登録されている終端データのポインタを返します			*/
/************************************************************************/

//	引数:
//		dbp:	データベースポインタ
//	戻り値:
//		現在登録されている終端のリストポインタが帰ります。
//	解説:
//		現在登録されている終端のリストポインタを返します。
//		dbp が NULL の場合は常に NULL が帰ります。

void	*dbEnd( const stDB *dbp ){

	stDBLNK		*lnkp;													// リンクポインタ
	void		*endp=0;												// 終端ユーザーリストポインタ

	if( dbp ){															// 有効なデータベースポインタチェック
		lnkp=dbp->lnkend.bakp;											// リストの先頭を得る
		if( lnkp->bakp ){												// ダミー先頭ではない
			endp=lnkp+1;												// ユーザに渡すのはリンクポインタ+1
		}
	}
	return( endp );														// 0:無し, 0以外:有効なデータポインタ
}

/************************************************************************/
/*	次のデータポインタを返します										*/
/************************************************************************/

//	引数:
//		lstp:	リストポインタ
//	戻り値:
//		次のリストポインタが帰ります。
//	解説:
//		次のリストポインタを返します。
//		lstp が NULL の場合は常にNULLが帰ります。

void	*dbNxt( const void *lstp ){

	stDBLNK		*lnkp;													// リンクポインタ
	void		*nxtlstp=0;												// 一つ前のユーザーポインタ

	if( lstp ){															// 有効なdb2管理ポインタ
		lnkp=dbLNKP(lstp)->nxtp;										// ユーザーリストポインタからリンクポインタに変換
		if( lnkp->nxtp ){												// ダミー終端ではない
			nxtlstp=lnkp+1;												// ユーザに渡すのはリンクポインタ+1
		}
	}
	return( nxtlstp );													// 0:無し, 0以外:有効なデータポインタ
}

/************************************************************************/
/*	前のデータポインタを返します										*/
/************************************************************************/

//	引数:
//		lstp:	リストポインタ
//	戻り値:
//		前のリストポインタが帰ります。
//	解説:
//		前のリストポインタを返します。
//		lstp が NULL の場合は常にNULLが帰ります。

void	*dbBak( const void *lstp ){

	stDBLNK		*lnkp;													// リンクポインタ
	void		*baklstp=0;												// 一つ前のユーザーポインタ

	if( lstp ){															// 有効なdb2管理ポインタ
		lnkp=dbLNKP(lstp)->bakp;										// ユーザーリストポインタからリンクポインタに変換
		if( lnkp->bakp ){												// ダミー先頭ではない
			baklstp=lnkp+1;												// ユーザに渡すのはリンクポインタ+1
		}
	}
	return( baklstp );													// 0:無し, 0以外:有効なデータポインタ
}

/************************************************************************/
/*	データポインタの配列を作成します									*/
/************************************************************************/

//	引数:
//		dbp:	データベースポインタ
//		lstpa:	リストポインタの配列
//	戻り値:
//		lstpa の値が帰ります。
//	解説:
//		もし、lstpa が NULL の場合は内部で登録データ数+1のメモリ確保をして
//		lstpaに確保したメモリのポインタがセットされます。
//		lstpa 配列に、リストポインタを格納し、終端に NULL をセットします。
//		( lstpa のサイズは登録データ数+1が必要です )
//		通常は、メモリ確保した場合、dbAryClose で開放が必要ですが、
//		もし、開放していない場合でも、dbClose がコールされた時点で自動的に開放されます。

void	*dbAryOpen( stDB *dbp, void *lstpa ){

	stDBLNK	*p, *nxtp;													// 双方向リストアクセスワーク
	void	**arypp;													// リストポインタのポインタ

	assert( dbp );														// データベースポインタチェック
	if( !lstpa ){														// 配列アドレスがしていされていない
		lstpa=dbMemAlc( dbp, ( dbAlcN( dbp )+1 )*sizeof(void*) );		// 必要メモリ確保
	}
	arypp=(void**)lstpa;												// リストポインタのポインタ
	for( p=dbp->lnktop.nxtp; (nxtp=p->nxtp)!=0; p=nxtp ){				// 登録されているデータの総当り
		*arypp++=( p+1 );												// ユーザーデータポインタ書き込み
	}
	*arypp=0;															// 門番
	return( lstpa );
}

/************************************************************************/
/*	データポインタの配列を開放します									*/
/************************************************************************/

//	引数:
//		lstpa:	リストポインタの配列
//	解説:
//		dbAryOpen で lstpa に NULL を指定した場合で、
//		明示的に、確保したメモリの開放をしたい場合にコールします。

void	dbAryClose( void *lstpa ){
	dbMemFre( lstpa );													// データベースのローカルメモリ開放
}

/************************************************************************/
/*	ローカルメモリ確保( データベースをクローズすると自動的に開放します )*/
/************************************************************************/

//	引数:
//		dbp:	データベースポインタ
//		siz:	確保するサイズ
//	戻り値:
//		確保したメモリポインタが帰ります。
//	解説:
//		データベースを通じてメモリ確保をします。
//		単にmallocをコールするより遅くなりますが、
//		dbClose がコールされた時点で自動的に開放されるので
//		メモリリークの心配がなくなります。

void	*dbMemAlc( stDB *dbp, int siz ){

	void	*alcp=0;													// ユーザーに渡すメモリ確保ポインタ
	stMLST	*mlstp=0;													// メモリ管理構造体ポインタ

	assert( dbp );														// データベースポインタチェック
	if( dbp ){															// 有効なデータベースポインタチェック
		mlstp=(stMLST*)memAlc( dbp, dbALG(sizeof(stMLST))+siz );		// メモリ管理構造体サイズ+ユーザー確保サイズを確保する
		assert( mlstp );												// メモリ確保が成功したかどうかのチェック
	}
	if( mlstp ){														// メモリ確保成功
		mlstp->dbp=dbp;													// このメモリを確保したデータベースポインタ
		mlstp->siz=siz;													// 確保メモリサイズ
		dbINSBAK( mlstp, &dbp->mlstend );								// メモリ管理用双方向リストに管理ポインタを挿入する
		alcp=((char*)mlstp)+dbALG(sizeof(stMLST));						// ユーザーに渡すメモリ確保ポインタ
	}
	return( alcp );														// ユーザーポインタを返す
}

/************************************************************************/
/*	ローカルメモリ開放													*/
/************************************************************************/

//	引数:
//		usrp:	dbMemAlcまたはdbMemAlcClrで確保したメモリポインタ
//	解説:
//		メモリを開放します。

void	dbMemFre( void *usrp ){

	stMLST	*mlstp;														// メモリ管理構造体ポインタ

	if( usrp ){															// 有効なメモリポインタ
		mlstp=(stMLST*)(((char*)usrp)-dbALG(sizeof(stMLST)));			// メモリ管理構造体ポインタはユーザーポインタ-1
		assert( mlstp->nxtp->bakp==mlstp );								// 次の双方向リストリンクチェック
		assert( mlstp->bakp->nxtp==mlstp );								// 前の双方向リストリンクチェック
		dbDEL( mlstp );													// メモリ管理用双方向リストから削除する
		memFre( mlstp->dbp, mlstp );									// メモリ開放
	}
}

/************************************************************************/
/*	ローカルメモリ確保をしてクリアする									*/
/************************************************************************/

//	引数:
//		dbp:	データベースポインタ
//		siz:	確保するサイズ
//	戻り値:
//		確保したメモリポインタが帰ります。
//	解説:
//		データベースを通じてメモリ確保をしクリアします。
//		( dbMemAlcをコールしてメモリをクリアしてからリターンします。)

void	*dbMemAlcClr( stDB *dbp, int siz ){

	void	*alcp;														// ローカルメモリ確保ポインタ

	alcp=dbMemAlc( dbp, siz );											// ローカルメモリ確保( データベースをクローズすると自動的に開放します )
	if( alcp ){															// 確保できた
		memset( alcp, 0, siz );											// メモリクリア
	}
	return( alcp );														// ローカルメモリ確保ポインタを返す
}

/************************************************************************/
/*	ローカルメモリ確保をしてコピーする( bufadr が NULL の場合はクリア )	*/
/************************************************************************/

void	*dbMemDup( stDB *dbp, const void *bufadr, int bufsiz ){

	void	*alcp;														// ローカルメモリ確保ポインタ

	alcp=dbMemAlc( dbp, bufsiz );										// ローカルメモリ確保( データベースをクローズすると自動的に開放します )
	if( alcp ){															// 確保できた
		if( bufadr ){
			memcpy( alcp, bufadr, bufsiz );
		}else{
			memset( alcp, 0, bufsiz );									// メモリクリア
		}
	}
	return( alcp );														// ローカルメモリ確保ポインタを返す
}

/************************************************************************/
/*	メモリアロケーションしたサイズ合計を求める							*/
/************************************************************************/

//	引数:
//		dbp:	データベースポインタ
//	解説:
//		確保しているメモリの総量を返します。
//		デバッグ時などによく使用します。

int		dbMemSiz( const stDB *dbp ){

	int		hashn;														// ハッシュテーブ個数
	int		slpsiz=0;													// ハッシュを使用している場合には単方向リストポインタサイズが必要
	int		res=0;														// メモリアロケーションしたサイズ合計
	void	*p;															// 双方向リストアクセスポインタ
	stMLST	*mp, *nxtmp;												// 双方向リストポインタ
	stMCB	*mcbp, *nxtmcbp;											// ローカルヒープ確保双方向リストポインタ

	if( dbp ){															// 有効なデータベースポインタチェック
		hashn=dbp->tblmsk+1;											// ハッシュテーブル個数
		if( dbp->slpadr ){												// ハッシュテーブルを使用している
			slpsiz=dbALG(sizeof(stSL));									// 単方向リストポインタサイズ
		}
		res=sizeof(stDB)+hashn*sizeof(stSL*);							// データベース構造体サイズ＋ハッシュテーブルサイズ
		if( dbp->hpsiz ){												// ローカルヒープ有効
			for( mcbp=dbp->alctop.nxtp; (nxtmcbp=mcbp->nxtp)!=0; mcbp=nxtmcbp ){				// グローバルメモリ確保総当りループ
				res+=mcbp->blk;											// メモリ確保サイズ加算( ここでは blk をバイトサイズとして使用している )
			}
		}else{															// ローカルヒープ無効
			dbFOR( dbp, p, void* ){										// 登録データ総当りループ
				res	+=
					slpsiz+												// 単方向リストポインタサイズ
					sizeof(stDBLNK)+									// リンク管理構造体
					dbALG(dbKEYSIZ(p))+									// キーサイズ
					dbALG(dbLSTSIZ(p));									// リストサイズ
			}
			for( mp=dbp->mlsttop.nxtp; (nxtmp=mp->nxtp)!=0; mp=nxtmp ){	// ローカルメモリ双方向リストループ
				res	+=mp->siz;											// メモリサイズ加算
			}
		}
	}
	return( res );														// メモリアロケーションしたサイズ合計をもってリターン
}

/************************************************************************/
/*	文字列登録															*/
/************************************************************************/

//	引数:
//		dbp:	データベースポインタ
//		strp:	文字列ポインタ
//	解説:
//		文字列を確保してそのポインタを帰します。

char	*dbStrAlc( stDB *dbp, const char *strp ){

	char	*p=0;														// メモリ確保ポインタ

	assert( dbp );														// 有効なデータベースポインタチェック
	if( strp ){															// 有効な文字列ポインタ
		p=(char*)dbMemAlc( dbp, strlen( strp )+1 );						// メモリ確保( 文字列サイズ+1 )
		assert( p );													// メモリ確保が成功したかどうかのチェック
		strcpy( p, strp );												// 文字列コピー
	}
	return( p );														// メモリ確保ポインタを持ってリターン
}

/************************************************************************/
/*	データベースの複製を作成する										*/
/************************************************************************/

//	引数:
//		dbp:	データベースポインタ
//	解説:
//		データベースの複製を作成します。
//		dbp が NULL の場合は常にNULLが帰ります。

stDB	*dbDup( const stDB *dbp ){

	stDB		*newdbp=0;												// 新しいデーターベースポインタ
	const void	*p;														// データアクセス用ワーク

	if( dbp ){															// 有効なデータベースポインタチェック
		newdbp=dbOpen( dbp->keysiz, dbp->lstsiz, dbp->tblmsk+1, dbp->hpsiz );	// 新しいデータベースオープン
		dbFOR( dbp, p, const void* ){									// オリジナルデータ総当り
			dbAlcS( newdbp, dbKEYADR( p ), dbKEYSIZ( p ), p, dbLSTSIZ( p ) );
		}
	}
	return( newdbp );													// 新しいデータベースを持ってリターン
}

/************************************************************************/
/*	Ｎ番目のリストアドレスを得る										*/
/************************************************************************/

//	引数:
//		dbp:	データベースポインタ
//		no:		登録番号
//	解説:
//		データベースの no 番目のリストアドレスを返します。
//		no が登録個数以上、または dbp が NULL の場合はNULLが帰ります。

void	*dbLstAdr( const stDB *dbp, int lstno ){

	void			*lstp=0;											// ユーザーリストポインタ
	stDBLNK			*p;													// リンクポインタ
	unsigned int	lstnou=(unsigned int)lstno;							// 符号無し整数にキャストしたリスト番号
	unsigned int	lstn;												// 登録個数
	unsigned int	i;													// ループカウンタ

	if( dbp ){															// 有効なデータベースポインタチェック
		lstn=dbAlcN( dbp );												// 登録個数
		if( lstnou<lstn ){												// 番号が登録個数よりも小さい
			if( lstnou<(lstn>>1) ){										// データは前半に存在する
				p=dbp->lnktop.nxtp;										// リンクポインタを先頭にセット
				for( i=0; i<lstnou; i++ ){								// 先頭から順番
					p=p->nxtp;											// 次のリンクポインタ
				}
			}else{														// データは後半に存在する
				p=dbp->lnkend.bakp;										// リンクポインタを終端にセット
				for( i=lstn-1; i>lstnou; i-- ){							// 終端から順番
					p=p->bakp;											// 前のリンクポインタ
				}
			}
			lstp=p+1;													// ユーザーリストポインタセット
		}
	}
	return( lstp );														// ユーザーリストポインタを持ってリターン
}

/************************************************************************/
/*	データベースの登録順番を変更する									*/
/************************************************************************/

//	引数:
//		dbp:	データベースポインタ
//		lstp:	移動するデータポインタ
//		nxtp:	移動先の次のデータポインタ( 0 の場合は終端の次が指定されたものとみなします )
//	解説:
//		データベースの登録順番を変更します。
//		lstp を nxtp の前に移動します。

void	dbMov( stDB *dbp, void *lstp, void *nxtp ){

	stDBLNK		*lnkp, *nxtlnkp;										// リンクポインタ

	assert( dbp );
	assert( lstp );
	lnkp=dbLNKP(lstp);													// ユーザーリストポインタからリンクポインタに変換
	if( nxtp ){															// 次が指定されている
		nxtlnkp=dbLNKP(nxtp);											// ユーザーリストポインタからリンクポインタに変換
	}else{																// 次は指定されていない
		nxtlnkp=&dbp->lnkend;											// 終端
	}
	dbDEL( lnkp );														// 双方向リスト削除
	dbINSBAK( lnkp, nxtlnkp );											// 双方向リスト挿入
}

/************************************************************************/
/*	指定したオフセット分シークする										*/
/************************************************************************/

//	引数:
//		seeknp:	実際にシークできた個数が書き込まれるポインタ( 0 ならば無視されます )
//		lstp:	リストポインタ
//		seekofs:シーク数指定
//	解説:
//		指定したオフセット分シークしたリストポインタを帰します。

void	*dbSeek( int *seeknp, const void *lstp, int seekofs ){

	const stDBLNK	*p, *np;											// リンクポインタ
	int				seekn=0;											// 実際にシークした数

	if( lstp ){															// リストポインタは有効
		p=dbLNKP(lstp);													// リストポインタからリンクポインタに変換
		if( seekofs<0 ){												// 後方
			for( seekn=0; seekn>seekofs; seekn-- ){
				np=p->bakp;												// 前のリンクポインタ
				if( !np->bakp ){										// もうない
					break;												// ループ終了
				}
				p=np;													// リンクポインタ更新
			}
		}else{															// 前方
			for( seekn=0; seekn<seekofs; seekn++ ){
				np=p->nxtp;												// 次のリンクポインタ
				if( !np->nxtp ){										// もうない
					break;												// ループ終了
				}
				p=np;													// リンクポインタ更新
			}
		}
		lstp=p+1;														// リンクポインタからリストポインタに変換
	}
	if( seeknp ){														// 実際にシークした数が書き込まれるポインタは有効
		*seeknp=seekn;													// 実際にシークした数を書き込む
	}
	return( (void*)lstp );												// リストポインタを持ってリターン
}

/************************************************************************/
/*	次に一致するものを検索												*/
/************************************************************************/

//	引数:
//		dbp:	データベースポインタ
//		lstp:	リストポインタ
//	解説:
//		次に一致するものを検索します。
//		一般的にはあまり使用しませんが、
//		同じキーを複数登録しておいて総当りするといった使い方をします。

void	*dbSchNxt( const stDB *dbp, const void *lstp ){

	void		*nxtlstp=0;												// ユーザーリストポインタ
	void		*keyadr;												// キーアドレス
	int			keysiz;													// キーサイズ
	int			lstalg;													// アラインされたユーザーリストサイズ
	stSL		*bakp, *slp;											// 単方向リストポインタ
	stDBLNK		*lnkp;													// 双方向リストポインタ
	int			find;													// 見つかったフラグ

	if( lstp ){															// リストポインタは有効
		keyadr=dbKEYADR( lstp );										// キーアドレス
		keysiz=dbKEYSIZ( lstp );										// キーサイズ
		if( dbp->slpadr ){												// ハッシュ表が使用されている
			bakp=(stSL*)( ((char*)lstp)-(dbALG(sizeof(stSL))+sizeof(stDBLNK)) );
			while( (slp=bakp->slp)!=0 ){								// 単方向リスト検査クループ
				lnkp=(stDBLNK*)((char*)slp+dbALG(sizeof(stSL)));		// リンクポインタ
				lstalg=dbALG( lnkp->lstsiz );							// アラインされたリストサイズ
				find=dbKeyCmp( dbp, (char*)(lnkp+1)+lstalg, lnkp->keysiz, keyadr, keysiz );
				if( find ){												// 見つかった
					nxtlstp=(lnkp+1);									// 帰り値セット
					break;												// ループ終了
				}
				bakp=slp;												// 単方向リストポインタ更新
			}
		}else{															// ハッシュ表は使用していないのでベタ探索
			lnkp=(stDBLNK*)lstp-1;										// カレントリンクポインタセット
			for(;;){
				lnkp=lnkp->bakp;										// 一つ前に更新
				if( !lnkp ){											// 前はない
					break;												// ループ終了
				}
				lstalg=dbALG( lnkp->lstsiz );							// アラインされたリストサイズ
				find=dbKeyCmp( dbp, (char*)(lnkp+1)+lstalg, lnkp->keysiz, keyadr, keysiz );
				if( find ){												// 見つかった
					nxtlstp=(lnkp+1);									// 帰り値セット
					break;												// ループ終了
				}
			}
		}
	}
	return( nxtlstp );													// リストポインタを帰り値とする
}

/************************************************************************/
/*	追加																*/
/************************************************************************/

//	引数:
//		dstdbp:	ディストネーションデータベースポインタ
//		adddbp:	ソースデータベースポインタ
//	解説:
//		adddbp の内容を dstdbp に付加します。

void	dbAddDB( stDB *dstdbp, const stDB *adddbp ){

	const void	*p;														// データアクセス用ワーク

	dbFOR( adddbp, p, const void* ){									// オリジナルデータ総当り
		dbAlcS( dstdbp, dbKEYADR( p ), dbKEYSIZ( p ), p, dbLSTSIZ( p ) );
	}
}

/************************************************************************/
/*	クリア																*/
/************************************************************************/

//	引数:
//		dbp:	データベースポインタ
//	解説:
//		dbp をクリアします。
//		dbClose してから dbOpen するよりも高速です。

void	dbClear( stDB *dbp ){

	void		*p, *nxtp;												// リストポインタ
	stMLST		*mp, *nxtmp;											// 双方向リストポインタ
	stMCB		*mcbp, *nxtmcbp;										// ローカルヒープ確保双方向リストポインタ

	if( dbp ){															// 有効なデータベースポインタ
		dbFOR2( dbp, p, nxtp, void* ){
			dbFre( dbp, p );
		}
		if( dbp->hpsiz ){												// ローカルヒープ有効
			for( mcbp=dbp->alctop.nxtp; (nxtmcbp=mcbp->nxtp)!=0; mcbp=nxtmcbp ){				// メモリ確保総当りループ
				DB_FREE( mcbp );										// グローバルメモリ開放
			}
			dbINI( &dbp->fretop, &dbp->freend );						// 空きメモリコントロールブロックリスト初期化
			dbINI( &dbp->usetop, &dbp->useend );						// 使用メモリコントロールブロックリスト初期化
			dbINI( &dbp->alctop, &dbp->alcend );						// ローカルヒープメモリコントロールブロック初期化
		}else{															// ローカルヒープ無効
			for( mp=dbp->mlsttop.nxtp; (nxtmp=mp->nxtp)!=0; mp=nxtmp ){	// ローカルメモリ管理構造体ループ
				DB_FREE( mp );											// ローカルメモリ開放
			}
			dbINI( &dbp->mlsttop, &dbp->mlstend );						// 双方向リスト初期化
		}
	}
}

/************************************************************************/
/*	リストの番号を得る													*/
/************************************************************************/

//	引数:
//		dbp:	データベースポインタ
//		lstp:	リストポインタ
//	解説:
//		リストポインタからリスト番号を得ます。
//		見つからなかった場合には-1を帰します。

int		dbLstNo( stDB *dbp, const void *lstp ){

	void	*p;															// リストポインタ
	int		i=0;														// リスト番号

	dbFOR( dbp, p, void* ){
		if( p==lstp ){													// 見つかった
			break;
		}
		i++;															// リスト番号インクリメント
	}
	if( !p ){															// 見つからなかった
		i=-1;
	}
	return( i );														// リスト番号を持ってリターン
}

/************************************************************************/
/*	漢字ラベル長を求める												*/
/************************************************************************/

	static const char	lblchrlena[2][256]={							// ラベル文字サイズテーブル
#	if	defined(_WIN32) || defined(__CYGWIN__)							/* SJIS*/
		{
			0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
			0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,1, 0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,
			1,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2, 2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2, 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
			1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1, 2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2, 2,2,2,2,2,2,2,2,2,2,2,2,2,1,1,1,
		},{
			0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,
			0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,1, 0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,
			1,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2, 2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2, 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
			1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1, 2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2, 2,2,2,2,2,2,2,2,2,2,2,2,2,1,1,1,
		}
#	else																/* EUC*/
		{
			0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
			0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,1, 0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,
			2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2, 2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2, 2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2, 2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
			2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2, 2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2, 2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2, 2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
		},{
			0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,
			0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,1, 0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,
			2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2, 2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2, 2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2, 2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
			2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2, 2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2, 2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2, 2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
		}
#	endif
	};

//	引数:
//		txtadr:	文字列アドレス
//		bufsiz:	バッファサイズ
//	解説:
//		漢字ラベル長を得ます。
//		もし、漢字ラベル長が bufsiz よりも大きい場合はクランプされます。

int		dbLblLen( const char *txtadr, int bufsiz ){

	const char	*txtp;													// テキストポインタ
	const char	*lblp;													// ラベルポインタ
	int			len;													// 文字サイズ
	int			lbllen;													// ラベルサイズ

	txtp=txtadr;
	lblp=lblchrlena[0];													// ラベル先頭文字テーブル
	for(;;){
		len=lblp[*(unsigned char*)txtp];								// 文字サイズ
		if( !len ){														// ラベルではない
			break;
		}
		txtp+=len;
		lblp=lblchrlena[1];												// ラベル文字テーブル
	}
	lbllen=txtp-txtadr;													// ラベルサイズ
	if( lbllen>bufsiz-1 ){												// ラベルサイズがバッファサイズより大きい
		lbllen=bufsiz-1;												// クランプ
	}
	return( lbllen );
}

/************************************************************************/
/*	漢字ラベルを得る													*/
/************************************************************************/

//	bufadr:	バッファアドレス
//	bufsiz:	バッファサイズ
//	txtp:	文字列アドレス

char	*dbGetLbl( char *bufadr, int bufsiz, const char *txtp ){

	int		lbllen;														// 漢字ラベル長

	lbllen=dbLblLen( txtp, bufsiz );									// 漢字ラベル長を求める
	memcpy( bufadr, txtp, lbllen );										// バッファにコピー
	bufadr[lbllen]=0;													// ０ターミネート
	return( (char*)txtp+lbllen );
}

/************************************************************************/
/*	ダブルクォーテーションで囲まれた文字列を得る						*/
/************************************************************************/

//	引数:
//		dstp:	バッファアドレス
//		dstsiz:	バッファサイズ
//		txtp:	文字列アドレス
//	解説:
//		もし、文字列がダブルクォーテーションで囲まれている場合はその中身を得ます。
//		そうでない場合はタブ、スペース、改行までを得ます。

char	*dbGetDQ( char *dstp, int dstsiz, const char *txtp ){

	static const char * const	sepa[2]={ "\r\n\t ", "\"" };			// ターミネータ文字列配列
	int							dq;										// ダブルコーテーションフラグ
	const char					*nxtp;									// 次のテキストポインタ
	int							siz;

	dq	=( *txtp=='"' );												// ダブルコーテーションフラグセット
	txtp+=dq;															// もし、先頭がダブルクォーテーションならばテキストポインタをインクリメント
	nxtp=strpbrk( txtp, sepa[dq] );										// セパレータ検索
	if( !nxtp ){														// セパレータが見つからない
		nxtp=txtp+strlen( txtp );										// ネクストポインタを文字列終端にセット
		dq	=0;															// セパレータが見つからない場合はダブルクォーテーション無視
	}
	siz=nxtp-txtp;														// 転送サイズ
	if( siz>dstsiz-1 ){													// バッファサイズ－１より大きい
		siz=dstsiz-1;													// クランプ
	}
	if( dstp ){
		memcpy( dstp, txtp, siz );										// ディストネーションバッファに文字列をコピー
		dstp[siz]=0;													// ディストネーションバッファ０ターミネート
	}
	return( (char*)( nxtp+dq ) );										// テキストポインタを持ってリターン
}

/************************************************************************/
/*	大文字小文字を区別しない漢字対応メモリ比較							*/
/************************************************************************/

//	dstadr:	比較メモリアドレス
//	souadr:	比較メモリアドレス
//	siz:	メモリサイズ

int		dbMemCaseCmp( const void *dstadr, const void *souadr, int siz ){

	int					res=0;
	const unsigned char	*dstp=(const unsigned char*)dstadr;
	const unsigned char	*soup=(const unsigned char*)souadr;
	unsigned char		sou, dst;
	int					i;

	for( i=0; i<siz; i++ ){
		dst=*dstp++;													// ディストネーション
		sou=*soup++;													// ソース
		res=dst-sou;													// リザルト
		if( res ){														// 違う
			if( dst>='A' && dst<='Z' ){									// 大文字
				dst|='a'-'A';											// 小文字変換
			}
			if( sou>='A' && sou<='Z' ){									// 大文字
				sou|='a'-'A';											// 小文字変換
			}
			res=dst-sou;												// リザルト
			if( res ){													// 違う
				break;
			}
		}else{
			if( lblchrlena[0][sou]==2 && i<siz-1 ){						// 漢字１バイト目
				dst=*dstp++;											// ディストネーション
				sou=*soup++;											// ソース
				res=dst-sou;											// リザルト
				if( res ){
					break;
				}
				i++;
			}
		}
	}
	return( res );
}

/************************************************************************/
/*	大文字小文字を区別しない漢字対応文字列比較							*/
/************************************************************************/

//	dstp:	比較文字列アドレス
//	soup:	比較文字列アドレス

int		dbStrCaseCmp( const char *dstp, const char *soup ){

	int		dstsiz;														// ディストネーションサイズ
	int		sousiz;														// ソースサイズ
	int		siz;														// 比較サイズ

	dstsiz=strlen( dstp );												// ディストネーションサイズ
	sousiz=strlen( soup );												// ソースサイズ
	if( dstsiz==sousiz ){												// 同じ
		siz=dstsiz;
	}else if( dstsiz>sousiz ){
		siz=sousiz+1;													// 小さいほう+1
	}else{
		siz=dstsiz+1;													// 小さいほう+1
	}
	return( dbMemCaseCmp( dstp, soup, siz ) );							// 大文字小文字を区別しない漢字対応メモリ比較
}

