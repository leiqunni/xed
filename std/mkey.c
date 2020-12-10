/************************************************************************/
/*																		*/
/*	マルチキー管理				by H.Watanabe							*/
/*	Ver1.00	2009/04/05 13:52:04	作成									*/
/*																		*/
/************************************************************************/

#	include	<assert.h>													/* アサーション*/
#	include	<stdio.h>													/* 標準入出力関数*/
#	include	<stdlib.h>													/* 標準ライブラリ関数*/
#	include	<string.h>													/* 標準文字列関数*/
#	include	"db2.h"														/* プログラム内汎用データベースシステム*/
#	include	"mkey.h"													/* マルチキー管理*/

/************************************************************************/
/*	ローカル定義														*/
/************************************************************************/

#	define	RNGTSTB(C,L,H)	((unsigned char)((C)-(L))<=(unsigned char)((H)-(L)))				/* 符号無し８ビットでレンジチェックをする*/

#	define	KEYNMAX			6

	typedef struct{
		stDB				*noddbp;									// ノードDB
		int					mkey;										// マルチキー
	}	stNOD;															// ノード構造体

	struct	tgMKEY{
		stNOD				nod;										// ルートノード
		int					keya[KEYNMAX];								// キーバッファ
		int					keyn;										// キーバッファ数
		int					kcod;										// 漢字フラグ
		stDB				*keydbp;									// 出力キーDB
	};

	typedef struct{
		int					mkey;										// マルチキー
		int					keya[KEYNMAX];								// キーデータ
	}	stKTAB;															// キーテーブル構造体

	static const stKTAB		g_ktaba[]={

		{	MKEY_DELETE,			{	0x7f, 0							}	},

#	ifdef	CURSES

		{	MKEY_ESC_ESC,			{	0x1b, 0x1b, 0					}	},
		{	MKEY_ESC_ESC,			{	0x1b, MKEY_PGUP, 0				}	},
		{	MKEY_ESC_ESC,			{	0x1b, MKEY_PGDOWN, 0			}	},
		{	MKEY_ESC_ESC,			{	0x1b, MKEY_END, 0				}	},
		{	MKEY_ESC_ESC,			{	0x1b, MKEY_HOME, 0				}	},
		{	MKEY_ESC_ESC,			{	0x1b, MKEY_LEFT, 0				}	},
		{	MKEY_ESC_ESC,			{	0x1b, MKEY_UP, 0				}	},
		{	MKEY_ESC_ESC,			{	0x1b, MKEY_RIGHT, 0				}	},
		{	MKEY_ESC_ESC,			{	0x1b, MKEY_DOWN, 0				}	},
		{	MKEY_ESC_ESC,			{	0x1b, MKEY_INSERT, 0			}	},
		{	MKEY_ESC_ESC,			{	0x1b, MKEY_DELETE, 0			}	},
		{	MKEY_ESC_ESC,			{	0x1b, MKEY_F1, 0				}	},
		{	MKEY_ESC_ESC,			{	0x1b, MKEY_F2, 0				}	},
		{	MKEY_ESC_ESC,			{	0x1b, MKEY_F3, 0				}	},
		{	MKEY_ESC_ESC,			{	0x1b, MKEY_F4, 0				}	},
		{	MKEY_ESC_ESC,			{	0x1b, MKEY_F5, 0				}	},
		{	MKEY_ESC_ESC,			{	0x1b, MKEY_F6, 0				}	},
		{	MKEY_ESC_ESC,			{	0x1b, MKEY_F7, 0				}	},
		{	MKEY_ESC_ESC,			{	0x1b, MKEY_F8, 0				}	},
		{	MKEY_ESC_ESC,			{	0x1b, MKEY_F9, 0				}	},
		{	MKEY_ESC_ESC,			{	0x1b, MKEY_F10, 0				}	},
		{	MKEY_ESC_ESC,			{	0x1b, MKEY_F11, 0				}	},
		{	MKEY_ESC_ESC,			{	0x1b, MKEY_F12, 0				}	},

		{	MKEY_ESC_0,				{	0x1b, 0x30, 0					}	},
		{	MKEY_ESC_1,				{	0x1b, 0x31, 0					}	},
		{	MKEY_ESC_2,				{	0x1b, 0x32, 0					}	},
		{	MKEY_ESC_3,				{	0x1b, 0x33, 0					}	},
		{	MKEY_ESC_4,				{	0x1b, 0x34, 0					}	},
		{	MKEY_ESC_5,				{	0x1b, 0x35, 0					}	},
		{	MKEY_ESC_6,				{	0x1b, 0x36, 0					}	},
		{	MKEY_ESC_7,				{	0x1b, 0x37, 0					}	},
		{	MKEY_ESC_8,				{	0x1b, 0x38, 0					}	},
		{	MKEY_ESC_9,				{	0x1b, 0x39, 0					}	},

		{	MKEY_ESC_AT,			{	0x1b, 0x40, 0					}	},
		{	MKEY_ESC_A,				{	0x1b, 0x41, 0					}	},
		{	MKEY_ESC_B,				{	0x1b, 0x42, 0					}	},
		{	MKEY_ESC_C,				{	0x1b, 0x43, 0					}	},
		{	MKEY_ESC_D,				{	0x1b, 0x44, 0					}	},
		{	MKEY_ESC_E,				{	0x1b, 0x45, 0					}	},
		{	MKEY_ESC_F,				{	0x1b, 0x46, 0					}	},
		{	MKEY_ESC_G,				{	0x1b, 0x47, 0					}	},
		{	MKEY_ESC_H,				{	0x1b, 0x48, 0					}	},
		{	MKEY_ESC_I,				{	0x1b, 0x49, 0					}	},
		{	MKEY_ESC_J,				{	0x1b, 0x4a, 0					}	},
		{	MKEY_ESC_K,				{	0x1b, 0x4b, 0					}	},
		{	MKEY_ESC_L,				{	0x1b, 0x4c, 0					}	},
		{	MKEY_ESC_M,				{	0x1b, 0x4d, 0					}	},
		{	MKEY_ESC_N,				{	0x1b, 0x4e, 0					}	},
		{	MKEY_ESC_O,				{	0x1b, 0x4f, 0					}	},
		{	MKEY_ESC_P,				{	0x1b, 0x50, 0					}	},
		{	MKEY_ESC_Q,				{	0x1b, 0x51, 0					}	},
		{	MKEY_ESC_R,				{	0x1b, 0x52, 0					}	},
		{	MKEY_ESC_S,				{	0x1b, 0x53, 0					}	},
		{	MKEY_ESC_T,				{	0x1b, 0x54, 0					}	},
		{	MKEY_ESC_U,				{	0x1b, 0x55, 0					}	},
		{	MKEY_ESC_V,				{	0x1b, 0x56, 0					}	},
		{	MKEY_ESC_W,				{	0x1b, 0x57, 0					}	},
		{	MKEY_ESC_X,				{	0x1b, 0x58, 0					}	},
		{	MKEY_ESC_Y,				{	0x1b, 0x59, 0					}	},
		{	MKEY_ESC_Z,				{	0x1b, 0x5a, 0					}	},
//		{	MKEY_ESC_LBO,			{	0x1b, 0x5b, 0					}	},
		{	MKEY_ESC_YEN,			{	0x1b, 0x5c, 0					}	},
		{	MKEY_ESC_LBC,			{	0x1b, 0x5d, 0					}	},
		{	MKEY_ESC_HAT,			{	0x1b, 0x5e, 0					}	},
		{	MKEY_ESC__,				{	0x1b, 0x5f, 0					}	},

		{	MKEY_ESC_a,				{	0x1b, 0x61, 0					}	},
		{	MKEY_ESC_b,				{	0x1b, 0x62, 0					}	},
		{	MKEY_ESC_c,				{	0x1b, 0x63, 0					}	},
		{	MKEY_ESC_d,				{	0x1b, 0x64, 0					}	},
		{	MKEY_ESC_e,				{	0x1b, 0x65, 0					}	},
		{	MKEY_ESC_f,				{	0x1b, 0x66, 0					}	},
		{	MKEY_ESC_g,				{	0x1b, 0x67, 0					}	},
		{	MKEY_ESC_h,				{	0x1b, 0x68, 0					}	},
		{	MKEY_ESC_i,				{	0x1b, 0x69, 0					}	},
		{	MKEY_ESC_j,				{	0x1b, 0x6a, 0					}	},
		{	MKEY_ESC_k,				{	0x1b, 0x6b, 0					}	},
		{	MKEY_ESC_l,				{	0x1b, 0x6c, 0					}	},
		{	MKEY_ESC_m,				{	0x1b, 0x6d, 0					}	},
		{	MKEY_ESC_n,				{	0x1b, 0x6e, 0					}	},
		{	MKEY_ESC_o,				{	0x1b, 0x6f, 0					}	},
		{	MKEY_ESC_p,				{	0x1b, 0x70, 0					}	},
		{	MKEY_ESC_q,				{	0x1b, 0x71, 0					}	},
		{	MKEY_ESC_r,				{	0x1b, 0x72, 0					}	},
		{	MKEY_ESC_s,				{	0x1b, 0x73, 0					}	},
		{	MKEY_ESC_t,				{	0x1b, 0x74, 0					}	},
		{	MKEY_ESC_u,				{	0x1b, 0x75, 0					}	},
		{	MKEY_ESC_v,				{	0x1b, 0x76, 0					}	},
		{	MKEY_ESC_w,				{	0x1b, 0x77, 0					}	},
		{	MKEY_ESC_x,				{	0x1b, 0x78, 0					}	},
		{	MKEY_ESC_y,				{	0x1b, 0x79, 0					}	},
		{	MKEY_ESC_z,				{	0x1b, 0x7a, 0					}	},


		{	MKEY_F1,				{	0x1b, 0x5b, 0x31, 0x31, 0x7e, 0	}	},
		{	MKEY_F2,				{	0x1b, 0x5b, 0x31, 0x32, 0x7e, 0	}	},
		{	MKEY_F3,				{	0x1b, 0x5b, 0x31, 0x33, 0x7e, 0	}	},
		{	MKEY_F4,				{	0x1b, 0x5b, 0x31, 0x34, 0x7e, 0	}	},
		{	MKEY_F5,				{	0x1b, 0x5b, 0x31, 0x35, 0x7e, 0	}	},

		{	MKEY_F6,				{	0x1b, 0x5b, 0x31, 0x37, 0x7e, 0	}	},
		{	MKEY_F7,				{	0x1b, 0x5b, 0x31, 0x38, 0x7e, 0	}	},
		{	MKEY_F8,				{	0x1b, 0x5b, 0x31, 0x39, 0x7e, 0	}	},
		{	MKEY_F9,				{	0x1b, 0x5b, 0x32, 0x30, 0x7e, 0	}	},
		{	MKEY_F10,				{	0x1b, 0x5b, 0x32, 0x31, 0x7e, 0	}	},
		{	MKEY_F11,				{	0x1b, 0x5b, 0x32, 0x33, 0x7e, 0	}	},
		{	MKEY_F12,				{	0x1b, 0x5b, 0x32, 0x34, 0x7e, 0	}	},

		{	MKEY_F3  | MKEY_SHIFT,	{	0x1b, 0x5b, 0x32, 0x35, 0x7e, 0	}	},
		{	MKEY_F4  | MKEY_SHIFT,	{	0x1b, 0x5b, 0x32, 0x36, 0x7e, 0	}	},

		{	MKEY_F5  | MKEY_SHIFT,	{	0x1b, 0x5b, 0x32, 0x38, 0x7e, 0	}	},
		{	MKEY_F6  | MKEY_SHIFT,	{	0x1b, 0x5b, 0x32, 0x39, 0x7e, 0	}	},
		{	MKEY_F7  | MKEY_SHIFT,	{	0x1b, 0x5b, 0x33, 0x31, 0x7e, 0	}	},
		{	MKEY_F8  | MKEY_SHIFT,	{	0x1b, 0x5b, 0x33, 0x32, 0x7e, 0	}	},
		{	MKEY_F9  | MKEY_SHIFT,	{	0x1b, 0x5b, 0x33, 0x33, 0x7e, 0	}	},
		{	MKEY_F10 | MKEY_SHIFT,	{	0x1b, 0x5b, 0x33, 0x34, 0x7e, 0	}	},

		{	MKEY_PGUP,				{	0x1b, 0x5b, 0x33, 0x7e, 0		}	},

		{	MKEY_PGDOWN,			{	0x1b, 0x5b, 0x36, 0x7e, 0		}	},
		{	MKEY_END,				{	0x1b, 0x5b, 0x35, 0x7e, 0		}	},
		{	MKEY_HOME,				{	0x1b, 0x5b, 0x32, 0x7e, 0		}	},
		{	MKEY_LEFT,				{	0x104, 0						}	},
		{	MKEY_UP,				{	0x103, 0						}	},
		{	MKEY_RIGHT,				{	0x105, 0						}	},
		{	MKEY_DOWN,				{	0x102, 0						}	},
		{	MKEY_INSERT,			{	0x1b, 0x5b, 0x31, 0x7e, 0		}	},
		{	MKEY_DELETE,			{	0x1b, 0x5b, 0x34, 0x7e, 0		}	},

		{	MKEY_ESC_0,				{	0x1b, 0x15f, 0					}	},
		{	MKEY_ESC_1,				{	0x1b, 0x15c, 0					}	},
		{	MKEY_ESC_2,				{	0x1b, 0x15e, 0					}	},
		{	MKEY_ESC_3,				{	0x1b, 0x15d, 0					}	},
		{	MKEY_ESC_4,				{	0x1b, 0x10d, 0					}	},
		{	MKEY_ESC_5,				{	0x1b, 0x10e, 0					}	},
		{	MKEY_ESC_6,				{	0x1b, 0x10f, 0					}	},
		{	MKEY_ESC_7,				{	0x1b, 0x111, 0					}	},
		{	MKEY_ESC_8,				{	0x1b, 0x112, 0					}	},
		{	MKEY_ESC_9,				{	0x1b, 0x108, 0					}	},

		{	'0',					{	0x15f, 0						}	},
		{	'1',					{	0x15c, 0						}	},
		{	'2',					{	0x15e, 0						}	},
		{	'3',					{	0x15d, 0						}	},
		{	'4',					{	0x10d, 0						}	},
		{	'5',					{	0x10e, 0						}	},
		{	'6',					{	0x10f, 0						}	},
		{	'7',					{	0x111, 0						}	},
		{	'8',					{	0x112, 0						}	},
		{	'9',					{	0x108, 0						}	},
		{	'.',					{	0x160, 0						}	},
		{	0x0d,					{	0x157, 0						}	},
		{	'+',					{	0x110, 0						}	},
		{	'-',					{	0x10c, 0						}	},
		{	'*',					{	0x10b, 0						}	},
		{	'/',					{	0x10a, 0						}	},

		{	'0',					{	0x1b, 0x4f, 0x70 , 0			}	},
		{	'1',					{	0x1b, 0x4f, 0x71 , 0			}	},
		{	'2',					{	0x1b, 0x4f, 0x72 , 0			}	},
		{	'3',					{	0x1b, 0x4f, 0x73 , 0			}	},
		{	'4',					{	0x1b, 0x4f, 0x74 , 0			}	},
		{	'5',					{	0x1b, 0x4f, 0x75 , 0			}	},
		{	'6',					{	0x1b, 0x4f, 0x76 , 0			}	},
		{	'7',					{	0x1b, 0x4f, 0x77 , 0			}	},
		{	'8',					{	0x1b, 0x4f, 0x78 , 0			}	},
		{	'9',					{	0x1b, 0x4f, 0x79 , 0			}	},
		{	'.',					{	0x1b, 0x4f, 0x6e , 0			}	},
		{	0x0d,					{	0x1b, 0x4f, 0x4d , 0			}	},
		{	'+',					{	0x1b, 0x4f, 0x6b , 0			}	},
		{	'-',					{	0x1b, 0x4f, 0x6d , 0			}	},
		{	'*',					{	0x1b, 0x4f, 0x6a , 0			}	},
		{	'/',					{	0x1b, 0x4f, 0x6f , 0			}	},

#	else

		{	0x1b,					{	0x1b, 0							}	},

#	endif

		{	0,						{	0								}	}
	};

/************************************************************************/
/*	登録																*/
/************************************************************************/

static void	alloc( stNOD *nodp, const int *keyp, int mkey ){

	stNOD	*p;

	if( !*keyp ){														// キーテーブルの終わり
		nodp->mkey=mkey;
	}else{
		if( !nodp->noddbp ){
			nodp->noddbp=dbOpen( sizeof(int), sizeof(stNOD), 256, 0 );
		}
		p=(stNOD*)dbSch( nodp->noddbp, keyp );
		if( !p ){
			p=(stNOD*)dbAlc( nodp->noddbp, keyp, 0 );
		}
		alloc( p, keyp+1, mkey );
	}
}

/************************************************************************/
/*	検索																*/
/************************************************************************/

static int	search( stNOD *nodp, const int *keyp ){

	int		mkey=0;
	stNOD	*p;

	if( nodp->mkey ){
		mkey=nodp->mkey;
	}else{
		if( *keyp ){
			p=(stNOD*)dbSch( nodp->noddbp, keyp );
			if( p ){
				mkey=search( p, keyp+1 );
			}else{
				mkey=-1;
			}
		}
	}
	return( mkey );
}

/************************************************************************/
/*	開放																*/
/************************************************************************/

static void	close( stNOD *nodp ){

	stNOD	*p;

	dbFOR( nodp->noddbp, p, stNOD* ){
		close( p );
	}
	dbClose( nodp->noddbp );
}

/************************************************************************/
/*	オープン															*/
/************************************************************************/

stMKEY	*mkeyOpen(void){

	stMKEY	*mkp;														// 管理構造体
	int		i;

	mkp=(stMKEY*)malloc( sizeof(stMKEY) );								// 管理構造体メモリ確保
	memset( mkp, 0, sizeof(stMKEY) );									// 管理構造体クリア
	mkp->keydbp=dbOpen( 0, sizeof(int), 0, 4096 );
	for( i=0; g_ktaba[i].mkey; i++ ){
		alloc( &mkp->nod, g_ktaba[i].keya, g_ktaba[i].mkey );
	}
	return( mkp );														// 管理構造体を持ってリターン
}

/************************************************************************/
/*	クローズ															*/
/************************************************************************/

void	mkeyClose( stMKEY *mkp ){
	if( mkp ){
		close( &mkp->nod );
		dbClose( mkp->keydbp );
		free( mkp );													// 管理構造体メモリ開放
	}
}

/************************************************************************/
/*	プット																*/
/************************************************************************/

void	mkeyPut( stMKEY *mkp, int key ){

	int		mkey;														// 0:検索中
	int		ksiz=1;
	int		i;

	if( !mkp->kcod ){
#	ifdef	_WIN32
		if( RNGTSTB( key, 0xa0, 0xdf ) ){								// 半角
			ksiz=1;
		}else if( RNGTSTB( key, 0x81, 0xfc ) ){							// SJIS漢字1バイト目チェック( すでに半角かなではないことがわかっている )
			ksiz=2;
		}
#	else
		if( !(key&0xff00) && key&0x80 ){
			ksiz=2;
		}
#	endif
		if( ksiz==2 ){
			mkp->kcod=key<<8;
		}else{
			mkp->keya[mkp->keyn++]=key;
			assert( mkp->keyn<KEYNMAX );
			mkp->keya[mkp->keyn]=0;
			mkey=search( &mkp->nod, mkp->keya );
			if( mkey ){
				if( mkey==-1 ){
					for( i=0; i<mkp->keyn; i++ ){
						dbAlc( mkp->keydbp, &mkp->keya[i] );
					}
				}else{
					dbAlc( mkp->keydbp, &mkey );
				}
				mkp->keyn=0;
			}
		}
	}else{
		mkey=mkp->kcod|key;
		dbAlc( mkp->keydbp, &mkey );
		mkp->kcod=0;
	}
}

/************************************************************************/
/*	ダイレクト登録														*/
/************************************************************************/

void	mkeyAlc( stMKEY *mkp, int mkey ){
	dbAlc( mkp->keydbp, &mkey );
}

/************************************************************************/
/*	ゲット																*/
/************************************************************************/

int		mkeyGet( stMKEY *mkp ){

	int		key=0;
	int		*keyp;

	keyp=(int*)dbTop( mkp->keydbp );
	if( keyp ){
		key=*keyp;
		dbFre( mkp->keydbp, keyp );
	}
	return( key );
}

/************************************************************************/
/*	覗く																*/
/************************************************************************/

int		mkeyPeek( stMKEY *mkp ){

	int		key=0;
	int		*keyp;

	keyp=(int*)dbTop( mkp->keydbp );
	if( keyp ){
		key=*keyp;
	}
	return( key );
}

