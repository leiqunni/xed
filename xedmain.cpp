/************************************************************************/
/*																		*/
/*	メインループ				by H.Watanabe							*/
/*	Ver0.00	2007/05/09 21:51:53	作成開始								*/
/*																		*/
/************************************************************************/

#	include	"main.h"													/* メインインクルードファイル*/

/************************************************************************/
/*	コマンドテーブル													*/
/************************************************************************/

	typedef struct{
		int				xedk;											// キー
		void			(*prcp)(void);									// 処理アドレス
	}	stCMD;

	static const stCMD	s_cmda[]={
		{	0x0001,							ctrl_a		},				// CTRL+A		１語後方に移動
		{	0x0002,							ctrl_b		},				// CTRL+B		両行端に移動
		{	0x0003,							ctrl_c		},				// CTRL+C		画面をロールアップ
		{	0x0004,							ctrl_d		},				// CTRL+D		１文字前方に移動
		{	0x0005,							ctrl_e		},				// CTRL+E		１行上に移動
		{	0x0006,							ctrl_f		},				// CTRL+F		１語前方に移動
		{	0x0007,							ctrl_g		},				// CTRL+G		１文字削除
		{	0x0008,							ctrl_h		},				// CTRL+H		バックスペース
		{	0x000a,							ctrl_j		},				// CTRL+J		ヘルプ画面の表示
		{	0x000b,							ctrl_k		},				// CTRL+K		行末まで削除
		{	0x000c,							ctrl_l		},				// CTRL+L		削除バッファから挿入
		{	0x000d,							ctrl_m		},				// CTRL+M		改行(行分割)
		{	0x000e,							ctrl_n		},				// CTRL+N		１行挿入
		{	0x000f,							ctrl_o		},				// CTRL+O		挿入モード切り替え
		{	0x0010,							ctrl_p		},				// CTRL+P		右端に移動
		{	0x0011,							ctrl_q		},				// CTRL+Q		左端に移動
		{	0x0012,							ctrl_r		},				// CTRL+R		画面をロールダウン
		{	0x0013,							ctrl_s		},				// CTRL+S		１文字後方に移動
		{	0x0014,							ctrl_t		},				// CTRL+T		１語削除
		{	0x0015,							ctrl_u		},				// CTRL+U		行頭から削除
		{	0x0016,							ctrl_v		},				// CTRL+V		直接コード入力
		{	0x0017,							ctrl_w		},				// CTRL+W		画面を１行ロールダウン
		{	0x0018,							ctrl_x		},				// CTRL+X		１行下に移動
		{	0x0019,							ctrl_y		},				// CTRL+Y		１行削除
		{	0x001a,							ctrl_z		},				// CTRL+Z		１行ロールアップ
//		{	0x001b,							esc			},				// CTRL+[		ESC コマンド
		{	0x001c,							ctrl_yen	},				// CTRL+\		カレントワード後方検索
		{	0x001d,							ctrl_lbktc	},				// CTRL+]		大文字・小文字変換
		{	0x001e,							ctrl_hat	},				// CTRL+^		カレントワード前方検索
		{	0x001f,							ctrl__		},				// CTRL+_		ファイル終端記号

		{	MKEY_ESC,						esc			},
		{	MKEY_ESC_ESC,					esc_esc		},

		{	MKEY_ESC_0,						esc_0		},
		{	MKEY_ESC_1,						esc_1		},
		{	MKEY_ESC_2,						esc_2		},
		{	MKEY_ESC_3,						esc_3		},
		{	MKEY_ESC_4,						esc_4		},
		{	MKEY_ESC_5,						esc_5		},
		{	MKEY_ESC_6,						esc_6		},
		{	MKEY_ESC_7,						esc_7		},
		{	MKEY_ESC_8,						esc_8		},
		{	MKEY_ESC_9,						esc_9		},

		{	MKEY_ESC_AT,					f11			},
		{	MKEY_ESC_A,						esc_a		},
		{	MKEY_ESC_B,						esc_b		},
		{	MKEY_ESC_C,						esc_c		},
		{	MKEY_ESC_D,						esc_d		},
		{	MKEY_ESC_E,						esc_e		},
		{	MKEY_ESC_F,						esc_f		},
//		{	MKEY_ESC_G,						esc_g		},
		{	MKEY_ESC_H,						esc_h		},
		{	MKEY_ESC_I,						esc_i		},
		{	MKEY_ESC_J,						esc_j		},
		{	MKEY_ESC_K,						esc_k		},
		{	MKEY_ESC_L,						esc_l		},
		{	MKEY_ESC_M,						esc_m		},
		{	MKEY_ESC_N,						esc_n		},
		{	MKEY_ESC_O,						esc_o		},
//		{	MKEY_ESC_P,						esc_p		},
		{	MKEY_ESC_Q,						esc_q		},
		{	MKEY_ESC_R,						esc_r		},
		{	MKEY_ESC_S,						esc_s		},
		{	MKEY_ESC_T,						esc_t		},
		{	MKEY_ESC_U,						esc_u		},
		{	MKEY_ESC_V,						esc_v		},
//		{	MKEY_ESC_W,						esc_c		},
		{	MKEY_ESC_X,						esc_x		},
		{	MKEY_ESC_Y,						esc_y		},
		{	MKEY_ESC_Z,						esc_z		},
		{	MKEY_ESC_LBO,					esc_lbo		},
		{	MKEY_ESC_YEN,					esc_yen		},
		{	MKEY_ESC_LBC,					esc_lbc		},
		{	MKEY_ESC_HAT,					esc_hat		},
		{	MKEY_ESC__,						esc__		},
	
		{	MKEY_ESC_a,						esc_a		},
		{	MKEY_ESC_b,						esc_b		},
		{	MKEY_ESC_c,						esc_c		},
		{	MKEY_ESC_d,						esc_d		},
		{	MKEY_ESC_e,						esc_e		},
		{	MKEY_ESC_f,						esc_f		},
//		{	MKEY_ESC_g,						esc_g		},
		{	MKEY_ESC_h,						esc_h		},
		{	MKEY_ESC_i,						esc_i		},
		{	MKEY_ESC_j,						esc_j		},
		{	MKEY_ESC_k,						esc_k		},
		{	MKEY_ESC_l,						esc_l		},
		{	MKEY_ESC_m,						esc_m		},
		{	MKEY_ESC_n,						esc_n		},
		{	MKEY_ESC_o,						esc_o		},
//		{	MKEY_ESC_p,						esc_p		},
		{	MKEY_ESC_q,						esc_q		},
		{	MKEY_ESC_r,						esc_r		},
		{	MKEY_ESC_s,						esc_s		},
		{	MKEY_ESC_t,						esc_t		},
		{	MKEY_ESC_u,						esc_u		},
		{	MKEY_ESC_v,						esc_v		},
		{	MKEY_ESC_w,						esc_c		},
		{	MKEY_ESC_x,						esc_x		},
		{	MKEY_ESC_y,						esc_y		},
		{	MKEY_ESC_z,						esc_z		},

		{	MKEY_INSERT,					ctrl_o		},				// CTRL+O		挿入モード切り替え
		{	MKEY_DELETE,					ctrl_g		},				// CTRL+G		１文字削除
		{	MKEY_LEFT,						ctrl_s		},				// CTRL+S		１文字後方に移動
		{	MKEY_UP,						ctrl_e		},				// CTRL+E		１行上に移動
		{	MKEY_RIGHT,						ctrl_d		},				// CTRL+D		１文字前方に移動
		{	MKEY_DOWN,						ctrl_x		},				// CTRL+X		１行下に移動
		{	MKEY_HOME,						esc_a		},				// HOME			テキストの昇順切り替え
		{	MKEY_END,						ctrl_j		},				// CTRL+J		ヘルプ画面の表示
		{	MKEY_PGDOWN,					ctrl_c		},				// CTRL+C		画面をロールアップ
		{	MKEY_PGUP,						ctrl_r		},				// CTRL+R		画面をロールダウン
		{	MKEY_DELETE,					ctrl_g		},				// CTRL+G		１文字削除
		{	MKEY_F1,						esc_b		},				// ESC+B		先頭へ移動
		{	MKEY_F2,						esc_z		},				// ESC+Z		末尾へ移動
		{	MKEY_F3,						f3			},				// F3			カレントワード前方置換
		{	MKEY_F4,						ctrl_hat	},				// CTRL+^		カレントワード前方検索
		{	MKEY_F5,						f5			},				// F5			継続前方検索
		{	MKEY_F6,						f6			},				// F6			範囲指定開始
		{	MKEY_F7,						f7			},				// F7			カットバッファへ移動
		{	MKEY_F8,						f8			},				// F8			カットバッファへ複写
		{	MKEY_F9,						f9			},				// F9			カットバッファから挿入
		{	MKEY_F10,						f10			},				// F10			行複写
		{	MKEY_F11,						f11			},				// ESC+@		キーボードマクロの定義
		{	MKEY_F12,						f12			},				// UNDO			マクロ実行
		{	MKEY_F5		| MKEY_SHIFT,		shift_f5	},				// SHIFT+F5		継続前方検索
		{	MKEY_F6		| MKEY_SHIFT,		shift_f6	},				// SHIFT+F6		範囲指定開始
		{	MKEY_F7		| MKEY_SHIFT,		shift_f7	},				// SHIFT+F7		カットバッファへ移動
		{	MKEY_F8		| MKEY_SHIFT,		shift_f8	},				// SHIFT+F8		カットバッファへ複写
		{	MKEY_F9		| MKEY_SHIFT,		shift_f9	},				// SHIFT+F9		カットバッファから挿入
		{	0x0009		| MKEY_SHIFT,		ctrl_tab	},				// SHIFT+TAB	水平タブ
		{	0x0009		| MKEY_CTRL,		ctrl_tab	},				// CTRL+I		水平タブ
		{	MKEY_F5		| MKEY_ALT,			alt_f5		},				// ALT+F5		ウィンドウ元のサイズに戻す
		{	MKEY_F9		| MKEY_ALT,			alt_f9		},				// ALT+F9		ウィンドウ最小化
		{	MKEY_F10	| MKEY_ALT,			alt_f10		},				// ALT+F10		ウィンドウ最大化
		{	'/'			| MKEY_ALT,			alt_slash	},				// ALT+/		電卓
		{	'*'			| MKEY_ALT,			alt_ast		},				// ALT+*		浮動小数電卓
		{	'+'			| MKEY_ALT,			alt_plus	},				// ALT++		シェルコマンド
#	ifndef	_WIN32
		{	0x0020		| MKEY_SHIFT,		shift_space	},				// SHIFT+SPACE	FEPオープン
		{	0x0020		| MKEY_CTRL,		shift_space	},				// SHIFT+SPACE	FEPオープン
		{	MKEY_F9		| MKEY_CTRL,		ctrl_f9		},				// CTRL+F9		セレクション読み込み
#	endif
		{	0,								0			}				// 終端コード
	};

/************************************************************************/
/*	メインループ														*/
/************************************************************************/

void	xedmain(void){

	stDB		*cmddbp;												// コマンドDB
	int			keyatrsym;												// アトリビュート＋キー
	int			keysym;													// キー
	int			curbak;													// カーソルスイッチバックアップ
	const stCMD	*cmdp;													// コマンドポインタ

	cmddbp=dbOpen( sizeof(int), sizeof(stCMD), 4096, 4096 );			// コマンドDBオープン
	for( cmdp=s_cmda; cmdp->xedk; cmdp++ ){								// テーブル総当りループ
		dbAlc( cmddbp, &cmdp->xedk, cmdp );								// コマンド登録
	}
	if( g_optlno ){														// オプションで行番号が指定されている
		lnoset( g_optlno-1 );											// 行番号セット
	}
	curSw( 1 );															// カーソルオン
	while( !g_flgend ){													// 終了フラグが立っていない間ループ
		keyatrsym=getKey();												// キーを得る
		msgSet( "" );													// メッセージクリア
		if( !keyatrsym ){												// キーが無かった
			break;														// 終了
		}
		curbak=curSw( 0 );												// カーソルオフ
		keysym=keyatrsym&0x0000ffff;									// アトリビュートを除いたキーシンボル
		cmdp=(stCMD*)dbSch( cmddbp, &keyatrsym );						// アトリビュート＋キー検索
		if( !cmdp ){													// 見つからない
			cmdp=(stCMD*)dbSch( cmddbp, &keysym );						// キーのみで検索
		}
		if( cmdp ){														// コマンドが見つかった
			cmdp->prcp();												// コマンド処理
		}else{															// コマンドが見つからない
			if( !( keyatrsym&0xffff0000 ) ){							// アトリビュートはない
				setchr( keysym );										// 一文字セット
			}
		}
		if( !g_flgend ){												// 終了ではない
			curSw( curbak );											// カーソルオン
			if( CUTCURP && CUTTYP ){									// 矩形選択モード
				invalidateAll();										// 全て表示
			}else{
				invalidateInf();										// インフォメーション再描画
			}
		}
		g_scdbiz=0;														// 検索表示サイズクリア
	}
	dbClose( cmddbp );													// コマンドDBクローズ
}

