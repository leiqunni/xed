/************************************************************************/
/*																		*/
/*	ヘルプ						by H.Watanabe							*/
/*	Ver1.00	2009/04/04 13:49:05	作成開始								*/
/*																		*/
/************************************************************************/

#	include	<assert.h>													/* アサーション*/
#	include	<stdio.h>													/* 標準入出力関数*/
#	include	<string.h>													/* 標準文字列関数*/
#	include	"std/txtdl.h"												/* テキストエディタのためのディスプレイリスト変換*/
#	include	"main.h"													/* メインインクルードファイル*/

/************************************************************************/
/*	ローカル定義														*/
/************************************************************************/

#	define	HELPXW	64													/* ヘルプ横幅*/
#	define	HELPYW	22													/* ヘルプ縦幅*/

	static 	const char		**g_helppp;									// ヘルプポインタ

/************************************************************************/
/*	ヘルプデータ														*/
/************************************************************************/

	static const char *helpAsc[]={
	   "				◆ ＡＳＣＩＩコード一覧 ◆",
	   "",
	   "		0x 1x 2x 3x 4x 5x 6x 7x 8x 9x Ax Bx Cx Dx Ex Fx",
	   "	0     dle     0  @  P  `  p  .  .  .  ｰ  ﾀ  ﾐ  .  .",
	   "	1 INS dc1  !  1  A  Q  a  q  .  .  ｡  ｱ  ﾁ  ﾑ  .  .",
	   "	2 stx dc2  \"  2  B  R  b  r  .  .  ｢  ｲ  ﾂ  ﾒ  .  .",
	   "	3 BRK dc3  #  3  C  S  c  s  .  .  ｣  ｳ  ﾃ  ﾓ  .  .",
	   "	4 eot dc4  $  4  D  T  d  t  .  .  ､  ｴ  ﾄ  ﾔ  .  .",
	   "	5 enq UND  %  5  E  U  e  u  .  .  ･  ｵ  ﾅ  ﾕ  .  .",
	   "	6 ack syn  &  6  F  V  f  v  .  .  ｦ  ｶ  ﾆ  ﾖ  .  .",
	   "	7 BEL etb  '  7  G  W  g  w  .  .  ｧ  ｷ  ﾇ  ﾗ  .  .",
	   "	8 BS  can  (  8  H  X  h  x  .  .  ｨ  ｸ  ﾈ  ﾘ  .  .",
	   "	9 HT  em   )  9  I  Y  i  y  .  .  ｩ  ｹ  ﾉ  ﾙ  .  .",
	   "	A LF  sub  *  :  J  Z  j  z  .  .  ｪ  ｺ  ﾊ  ﾚ  .  .",
	   "	B HOM ESC  +  ;  K  [  k  {  .  .  ｫ  ｻ  ﾋ  ﾛ  .  .",
	   "	C CLR →   ,  <  L  \\  l  |  .  .  ｬ  ｼ  ﾌ  ﾜ  .  .",
	   "	D CR  ←   -  =  M  ]  m  }  .  .  ｭ  ｽ  ﾍ  ﾝ  .  .",
	   "	E RUP ↑   .  >  N  ^  n  ~  .  .  ｮ  ｾ  ﾎ  ﾞ  .  .",
	   "	F RDO ↓   /  ?  O  _  o  DEL.  .  ｯ  ｿ  ﾏ  ﾟ  .  .",
		0
	};

	static const char *helpCtrl[]={
	   "				◆ ＣＴＲＬキー操作一覧 ◆",
	   "",
	   "CTRL+A	１語後方に移動			CTRL+Q	左端に移動",
	   "CTRL+B	両行端に移動			CTRL+R	画面をロールダウン",
	   "CTRL+C	画面をロールアップ		CTRL+S	１文字後方に移動",
	   "CTRL+D	１文字前方に移動		CTRL+T	１語削除",
	   "CTRL+E	１行上に移動			CTRL+U	行頭から削除",
	   "CTRL+F	１語前方に移動			CTRL+V	直接コード入力",
	   "CTRL+G	１文字削除				CTRL+W	１行ロールダウン",
	   "CTRL+H	バックスペース			CTRL+X	１行下に移動",
	   "CTRL+I	水平タブ				CTRL+Y	１行削除",
	   "CTRL+J	ヘルプ画面の表示		CTRL+Z	１行ロールアップ",
	   "CTRL+K	行末まで削除			CTRL+[	ＥＳＣコマンド",
	   "CTRL+L	削除バッファから挿入	CTRL+\\	日本語入力",
	   "CTRL+M	改行(行分割)			CTRL+]	大文字・小文字変換",
	   "CTRL+N	１行挿入				CTRL+^	カレントワード前方検索",
	   "CTRL+O	挿入モード切り替え		CTRL+_	ファイル終端記号",
	   "CTRL+P	右端に移動",
		0
	};

	static const char *helpEsc[]={
	   "				◆ ＥＳＣキー操作一覧 ◆",
	   "",
	   "ESC+NUM	指定行番号への移動		ESC+<数値>T タブステップ変更",
	   "ESC+@	キーボードマクロの定義	ESC+<数値>N 行番号表示",
	   "ESC+A	テキストの昇順切り替え	ESC+Q	全テキストを破棄し終了",
	   "ESC+B	先頭へ移動				ESC+R	前方置換",
	   "ESC+C	子プロセスの実行		ESC+S	後方検索",
	   "ESC+D	ファィルの降順切り替え	ESC+T	ファイル名変更",
	   "ESC+E	編集完了				ESC+U	後方置換",
	   "ESC+F	新しいテキストの編集	ESC+V	タグジャンプ",
	   "ESC+G	未使用					ESC+W	未使用",
	   "ESC+H	現テキストのセーブ		ESC+X	現テキストの完了",
	   "ESC+I	タブ文字の表示切り替え	ESC+Y	ファィルの読み込み",
	   "ESC+J	前方確認置換			ESC+Z	末尾へ移動",
	   "ESC+K	現テキストの破棄		ESC+[	カレントワード前方置換",
	   "ESC+L	後方確認置換			ESC+\\	カレントワード後方置換",
	   "ESC+M	改行文字の表示／非表示	ESC+]	大文字・小文字変換切替",
	   "ESC+N	前方検索				ESC+^	前方置換(表示なし)",
	   "ESC+O	現テキストの再ロード	ESC+_	後方置換(表示なし)",
	   "ESC+P	未使用					ESC+-	オプション",
		0
	};

	static const char *helpAlt[]={
	   "				◆ ＡＬＴキー操作一覧 ◆",
	   "",
#	ifdef	_WIN32
	   "ALT+F5			ウィンドウ元のサイズに戻す",
	   "ALT+F9			ウィンドウ最小化",
	   "ALT+F10			ウィンドウ最大化",
	   "",
#	endif
	   "ALT+'/'			16進電卓",
	   "ALT+'*'			浮動小数電卓",
	   "ALT+'+'			カレント行シェル実行",
		0
	};

	static const char *helpFn[]={
	   "				◆ ファンクションキー操作一覧 ◆",
	   "",
	   "F1		先頭へ移動				F11		キーボードマクロの定義",
	   "F2		末尾へ移動				F12		キーボードマクロの実行",
	   "F3		前方連続置換			F13		未使用",
	   "F4		カレントワード前方検索	F14		未使用",
	   "F5		継続前方検索			F15		未使用",
	   "F6		範囲指定開始			F16		矩形範囲指定開始",
	   "F7		カットバッファへ移動	F17		F7 と同じ",
	   "F8		カットバッファへ複写	F18		F8 と同じ",
	   "F9		カットバッファから挿入	F19		矩形挿入",
	   "F10		行の２重化				F20		未使用",
	   "TAB		水平タブ				↑		１行上に移動",
	   "BS		バックスペース			←		１文字右に移動",
	   "CR		改行（行分割）			→		１文字を右に移動",
	   "ESC		ＥＳＣコマンド			↓		１行下に移動",
	   "R_UP	画面をロールアップ",
	   "R_DOWN	画面をロールダウン		END		説明画面の表示",
	   "INS		挿入モード切替			HOME	テキストの昇順切り替え",
	   "DEL		１文字削除",
		0
	};

	static const char *helpOpt[]={
	   "				◆ 起動時のオプション ◆",
	   "",
	   "-h<数値>	タブ幅を指定 ( 2 / 4 / [8] )",
	   "-n			.bak ファイルを作成しない",
	   "-u			強制英語モード",
	   "-e			ファィル終端記号を表示しない",
	   "-l			改行文字を表示しない",
	   "-lno<数値>	編集行指定",
		0
	};

	static const char *helpGreek[]={
	   "				◆ ギリシャ文字の綴り ◆",
	   "",
	   "Αα:alpha	 アルファ	Νν:nu		 ニュー",
	   "Ββ:beta	 ベータ		Ξξ:xi		 グザイ",
	   "Γγ:gamma	 ガンマ		Οο:omicron オミクロン",
	   "Δδ:delta	 デルタ		Ππ:pi		 パイ",
	   "Εε:epsilon イプシロン	Ρρ:rho	 ロー",
	   "Ζζ:zeta	 ジータ		Σσ:sigma	 シグマ",
	   "Ηη:eta	 イータ		Ττ:tau	 タウ",
	   "Θθ:theta	 シータ		Υυ:upsilon ウプシロン",
	   "Ιι:iota	 イオタ		Φφ:phi	 ファイ",
	   "Κκ:kappa	 カッパ		Χχ:chi	 カイ",
	   "Λλ:lambda	 ラムダ		Ψψ:psi	 プサイ",
	   "Μμ:mu		 ミュー		Ωω:omega	 オメガ",
		0
	};

	static const char	**HlpStr[]={
		helpAsc,														// ＡＳＣＩＩコード一覧
		helpGreek,														// ギリシャ文字の綴り
		helpCtrl,														// ＣＴＲＬキー操作一覧
		helpEsc,														// ＥＳＣキー操作一覧
		helpAlt,														// ＡＬＴキー操作一覧
		helpFn,															// ファンクションキー操作一覧
		helpOpt,														// 起動時のオプション
		0																// 終端コード
	};

/************************************************************************/
/*	ヘルプ座標を得る													*/
/************************************************************************/

static void	helpCrd( int *xxcp, int *yycp, int *xxdp, int *yydp ){
	*xxdp=HELPXW;
	*xxcp=g_xxw-1-HELPXW;
	*yydp=HELPYW;
	*yycp=g_yyw-1-HELPYW;
}

/************************************************************************/
/*	一画面表示															*/
/************************************************************************/

static void	helps( int xxc, int yyc, int xxd, int yyd, const char **helppp ){

	const char	*helpp="";												// ヘルプ一行ポインタ
	int			xx, yy;													// 座標
	stDB		*dldbp;													// テキストディスプレイリストDB
	stTXTDL		*dlp;													// テキストディスプレイリストポインタ

	for( yy=0; yy<yyd; yy++ ){
		if( helpp ){													// まだ終端に達していない
			helpp=helppp[yy];											// ポインタを得る
		}
		xx=0;															// Ｘ座標クリア
		dldbp=txtdl( helpp?helpp:"", -1, g_tabstp, g_tabdsp, 0, 0, 0, 0, xxd );	// テキストディスプレイリストコンパイル
		dbFOR( dldbp, dlp, stTXTDL* ){									// ディスプレイリスト総当り
			if( dbKEYSIZ( dlp ) ){
				drwTxt( xxc+xx, yyc+yy, (char*)dbKEYADR( dlp ), dbKEYSIZ( dlp ), CR_BLACK, CR_WHITE );
			}else{
				drwFil( xxc+xx, yyc+yy, xxc+xx+dlp->dspsiz, yyc+yy+1, CR_BLACK );
			}
			xx+=dlp->dspsiz;											// 座標更新
		}
		dbClose( dldbp );												// ディスプレイリストDBクローズ
		if( xx<xxd ){													// 空きエリアが存在する
			drwFil( xxc+xx, yyc+yy, xxc+xxd, yyc+yy+1, CR_BLACK );		// クリア
		}
	}
}

/************************************************************************/
/*	ヘルプ再表示														*/
/************************************************************************/

void	helpReDsp(void){

	int			xxc, yyc, xxd, yyd;										// ヘルプ座標

	if( g_helppp ){														// ヘルプ表示最中
		helpCrd( &xxc, &yyc, &xxd, &yyd );								// ヘルプ座標を得る
		displayChrBox( xxc, yyc, xxd, yyd, "Hlep" );					// キャラクタボックス描画
		helps( xxc+1, yyc+1, xxd-2, yyd-2, g_helppp );					// ヘルプ描画
	}
}

/************************************************************************/
/*	ヘルプ画面の表示													*/
/************************************************************************/

void	help(void){

	int			helpno=0;												// ヘルプ番号
	const char	**helppp;												// ヘルプページポインタ
	int			key;

	while( !g_flgend ){													// 終了フラグが立っていない間ループ
		helppp=HlpStr[helpno];											// ヘルプ番号からヘルプページポインタを得る
		if( !helppp ){													// 終端
			helpno--;
			continue;
		}
		g_helppp=helppp;												// グローバルワークセット
		invalidateHlp();												// ヘルプ再表示
		key=getKey();													// キーを得る
		if( key==MKEY_PGUP || key==MKEY_UP || key==MKEY_LEFT ){
			helpno--;
			if( helpno<0 ){
				helpno=0;
			}
		}else if( key==' ' || key==MKEY_PGDOWN || key==MKEY_RIGHT || key==MKEY_DOWN ){
			helpno++;													// ヘルプ番号加算
		}else{
			break;
		}
	}
	g_helppp=0;															// グローバルワーククリア
	invalidateAll();													// 全て再表示
}

