/************************************************************************/
/*																		*/
/*	�o�[�W�������{�b�N�X�p���b�Z�[�W�n���h��							*/
/*	Ver1.00	2009/04/05 13:52:04	�쐬									*/
/*																		*/
/************************************************************************/

#	ifndef	ABOUT2_H
#	define	ABOUT2_H
#	ifdef	__cplusplus
	extern "C"{
#	endif

#	ifdef	_WIN32

#	pragma	warning(disable:4115)
#	include	<windows.h>													/* Windows �w�b�_�[�t�@�C��*/

	LRESULT CALLBACK	about( HWND hdlg, unsigned int message, unsigned long wparam, unsigned long lparam );

#	endif

#	ifdef	__cplusplus
	}
#	endif
#	endif

