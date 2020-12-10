/************************************************************************/
/*																		*/
/*	バージョン情報ボックス用メッセージハンドラ							*/
/*	Ver1.00	2009/04/05 13:52:04	作成									*/
/*																		*/
/************************************************************************/

#	ifdef	_WIN32

#	include	"about2.h"													/* バージョン情報ボックス用メッセージハンドラ*/

/************************************************************************/
/*	バージョン情報ボックス用メッセージハンドラ							*/
/************************************************************************/

LRESULT CALLBACK	about( HWND hdlg, unsigned int message, unsigned long wparam, unsigned long lparam ){

	(void)&lparam;														// 未使用
	switch( message ){
		case WM_INITDIALOG:{											// ダイアログの初期化
			return( FALSE );											// WM_INITDIALOG の場合のみ例外で FALSE を帰す
		}
		case WM_COMMAND:{												// コマンド
			if( LOWORD( wparam ) == IDOK || LOWORD( wparam ) == IDCANCEL ){
				EndDialog( hdlg, LOWORD( wparam ) );					// モーダルダイアログボックスを破棄し、そのダイアログボックスに関係するすべての処理を終了させます。
				return( TRUE );											// TRUE=処理した, FALSE=未処理
			}
			break;
		}
	}
	return( FALSE );													// メッセージを処理した場合に 1 を、処理しなかった場合に 0 を返す
}

#	endif

