/************************************************************************/
/*																		*/
/*	テキストエディタのためのディスプレイリスト変換	by H.Watanabe		*/
/*	Ver1.00	2009/04/05 13:52:04	作成									*/
/*																		*/
/************************************************************************/

#	ifndef	TXTDL_H
#	define	TXTDL_H
#	ifdef	__cplusplus
	extern "C"{
#	endif

#	include	"db2.h"														/* プログラム内汎用データベースシステム*/

	typedef struct{
		int					dspsiz;										// 表示サイズ
		int					rvs;										// 反転フラグ
		int					cur;										// カーソルフラグ
		int					col;										// カラー( 0:テキスト, 1:[EOF]等 )
	}	stTXTDL;														// テキストディスプレイリスト構造体

	stDB	*txtdl( const char *txtp, int curxx, int tabstp, int tabdsp, int eof, int eofdsp, int pgxx, int findsiz, int xxw );

#	ifdef	__cplusplus
	}
#	endif
#	endif

