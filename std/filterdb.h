/************************************************************************/
/*																		*/
/*	�t�B���^�[����				by H.Watanabe							*/
/*	Ver1.00	2009/04/05 13:52:04	�쐬									*/
/*																		*/
/************************************************************************/

#	ifndef	FILTERDB_H
#	define	FILTERDB_H
#	ifdef	__cplusplus
	extern "C"{
#	endif

#	include	"db2.h"														/* �v���O�������ėp�f�[�^�x�[�X�V�X�e��*/

	stDB	*filterdb( const char *cmdlin, const stDB *indbp );

#	ifdef	__cplusplus
	}
#	endif
#	endif

