/************************************************************************/
/*																		*/
/*	バージョン情報ボックス用メッセージハンドラ							*/
/*	Ver1.00	2009/04/05 13:52:04	作成									*/
/*																		*/
/************************************************************************/

#	ifndef	ABOUT2_H
#	define	ABOUT2_H
#	ifdef	__cplusplus
	extern "C"{
#	endif

#	ifdef	_WIN32

#	pragma	warning(disable:4115)
#	include	<windows.h>													/* Windows ヘッダーファイル*/

	LRESULT CALLBACK	about( HWND hdlg, unsigned int message, unsigned long wparam, unsigned long lparam );

#	endif

#	ifdef	__cplusplus
	}
#	endif
#	endif

