/************************************************************************/
/*																		*/
/*	�o�[�W�������{�b�N�X�p���b�Z�[�W�n���h��							*/
/*	Ver1.00	2009/04/05 13:52:04	�쐬									*/
/*																		*/
/************************************************************************/

#	ifdef	_WIN32

#	include	"about2.h"													/* �o�[�W�������{�b�N�X�p���b�Z�[�W�n���h��*/

/************************************************************************/
/*	�o�[�W�������{�b�N�X�p���b�Z�[�W�n���h��							*/
/************************************************************************/

LRESULT CALLBACK	about( HWND hdlg, unsigned int message, unsigned long wparam, unsigned long lparam ){

	(void)&lparam;														// ���g�p
	switch( message ){
		case WM_INITDIALOG:{											// �_�C�A���O�̏�����
			return( FALSE );											// WM_INITDIALOG �̏ꍇ�̂ݗ�O�� FALSE ���A��
		}
		case WM_COMMAND:{												// �R�}���h
			if( LOWORD( wparam ) == IDOK || LOWORD( wparam ) == IDCANCEL ){
				EndDialog( hdlg, LOWORD( wparam ) );					// ���[�_���_�C�A���O�{�b�N�X��j�����A���̃_�C�A���O�{�b�N�X�Ɋ֌W���邷�ׂĂ̏������I�������܂��B
				return( TRUE );											// TRUE=��������, FALSE=������
			}
			break;
		}
	}
	return( FALSE );													// ���b�Z�[�W�����������ꍇ�� 1 ���A�������Ȃ������ꍇ�� 0 ��Ԃ�
}

#	endif

