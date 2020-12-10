/************************************************************************/
/*																		*/
/*	�e�L�X�g�G�f�B�^�̂��߂̃f�B�X�v���C���X�g�ϊ�	by H.Watanabe		*/
/*	Ver1.00	2009/04/05 13:52:04	�쐬									*/
/*																		*/
/************************************************************************/

#	ifndef	TXTDL_H
#	define	TXTDL_H
#	ifdef	__cplusplus
	extern "C"{
#	endif

#	include	"db2.h"														/* �v���O�������ėp�f�[�^�x�[�X�V�X�e��*/

	typedef struct{
		int					dspsiz;										// �\���T�C�Y
		int					rvs;										// ���]�t���O
		int					cur;										// �J�[�\���t���O
		int					col;										// �J���[( 0:�e�L�X�g, 1:[EOF]�� )
	}	stTXTDL;														// �e�L�X�g�f�B�X�v���C���X�g�\����

	stDB	*txtdl( const char *txtp, int curxx, int tabstp, int tabdsp, int eof, int eofdsp, int pgxx, int findsiz, int xxw );

#	ifdef	__cplusplus
	}
#	endif
#	endif

