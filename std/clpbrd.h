/************************************************************************/
/*																		*/
/*	クリップボード管理			by H.Watanabe							*/
/*	Ver1.00	2009/04/05 13:52:04	作成									*/
/*																		*/
/************************************************************************/

#	ifndef	CLPBRD_H
#	define	CLPBRD_H
#	ifdef	__cplusplus
	extern "C"{
#	endif

#	ifndef	_WIN32
#		ifndef	CURSES
#			include <X11/Xlib.h>
#			include <X11/Xutil.h>
#		endif
#	endif

#	include	"db2.h"														/* プログラム内汎用データベースシステム*/

#	ifdef	_WIN32
#		define	tpCLPBRD	struct HWND__ *
#	else
#		ifndef	CURSES
#			define	tpCLPBRD	Display *
#		else
#			define	tpCLPBRD	void *
#		endif
#	endif

	enum{
		CB_TXT,															// テキスト
		CB_SEARCH,														// 検索
		CB_N
	};

#	ifdef	CURSES

	void	clpbrdStartUp(void);
	void	clpbrdCleanUp(void);

#	endif

	stDB	*clpbrdGet( tpCLPBRD hwnd, int cbno );
	int		clpbrdPut( tpCLPBRD hwnd, int cbno, const stDB *dbp, int rctmod, int clrflg );

#	ifdef	__cplusplus
	}
#	endif
#	endif

