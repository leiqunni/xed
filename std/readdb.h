/************************************************************************/
/*																		*/
/*	�N���b�v�{�[�h������̍s�ϊ�	by H.Watanabe						*/
/*	Ver1.00	2009/04/05 13:52:04	�쐬									*/
/*																		*/
/************************************************************************/

#	ifndef	READDB_H
#	define	READDB_H
#	ifdef	__cplusplus
	extern "C"{
#	endif

#	include	"db2.h"														/* �v���O�������ėp�f�[�^�x�[�X�V�X�e��*/

	typedef struct tgREADDB	stREADDB;									// �Ǘ��\���̒�`

	stREADDB	*readdbOpen(void);										// �I�[�v��
	stDB	*readdbClose( stREADDB *rhp );								// �N���[�Y
	void	readdbAlc( stREADDB *rhp, const void *bufadr, int bufsiz );	// �o�^

#	ifdef	__cplusplus
	}
#	endif
#	endif

