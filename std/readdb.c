/************************************************************************/
/*																		*/
/*	�N���b�v�{�[�h������̍s�ϊ�	by H.Watanabe						*/
/*	Ver1.00	2009/04/05 13:52:04	�쐬									*/
/*																		*/
/************************************************************************/

#	include	<stdlib.h>													/* �W�����C�u�����֐�*/
#	include	<string.h>													/* �W��������֐�*/
#	include	"readdb.h"													/* �N���b�v�{�[�h������̍s�ϊ�*/

/************************************************************************/
/*	���[�J����`														*/
/************************************************************************/

	struct	tgREADDB{
		stDB				*dbp;										// �sDB
		char				*leftp;										// �c�蕶����|�C���^
	};

/************************************************************************/
/*	�I�[�v��															*/
/************************************************************************/

stREADDB	*readdbOpen(void){

	stREADDB	*rhp;													// �Ǘ��\����

	rhp=(stREADDB*)malloc( sizeof(stREADDB) );							// �Ǘ��\���̃������m��
	memset( rhp, 0, sizeof(stREADDB) );									// �Ǘ��\���̃N���A
	rhp->dbp=dbOpen( 0, DBSIZ_STR, 0, 4096 );							// �sDB�I�[�v��
	return( rhp );														// �Ǘ��\���̂������ă��^�[��
}

/************************************************************************/
/*	�N���[�Y															*/
/************************************************************************/

stDB	*readdbClose( stREADDB *rhp ){

	stDB	*dbp=rhp->dbp;												// �sDB

	if( rhp->leftp ){													// �c�蕶���񂪂���
		dbAlc( dbp, rhp->leftp );										// �sDB�ɓo�^
		dbMemFre( rhp->leftp );											// �c�蕶����J��
	}
	free( rhp );														// �Ǘ��\���̃������J��
	return( dbp );														// �sDB�������ă��^�[��
}

/************************************************************************/
/*	�o�^																*/
/************************************************************************/

void	readdbAlc( stREADDB *rhp, const void *bufadr, int bufsiz ){

	int			leftsiz=0;												// �c��T�C�Y
	char		*txtalc, *txtp, *nxtp;									// �e�L�X�g�|�C���^
	char		chr;													// �o�b�N�A�b�v����
	int			i;														// ���[�v�J�E���^

	if( bufadr && bufsiz ){
		if( rhp->leftp ){												// �O�̎c�肪���݂���
			leftsiz=strlen( rhp->leftp );								// �c��T�C�Y
		}
		txtalc=(char*)malloc( leftsiz+bufsiz+1 );						// �e�L�X�g�������m��
		memcpy( txtalc, rhp->leftp, leftsiz );							// �c�蕶������R�s�[
		if( rhp->leftp ){
			dbMemFre( rhp->leftp );										// �c�蕶����J��
			rhp->leftp=0;												// �c�蕶����|�C���^�N���A
		}
		txtp=txtalc+leftsiz;											// �e�L�X�g�|�C���^�Z�b�g
		for( i=0; i<bufsiz; i++ ){										// �o�b�t�@�T�C�Y�����[�v
			chr=((char*)bufadr)[i];										// �ꕶ���𓾂�
			if( chr!='\r' && chr!='\x1a' ){								// DOS�̗]�v�ȃR�[�h�͖���
				*txtp++=chr;											// �ꕶ���Z�b�g
			}
		}
		*txtp=0;														// �O�^�[�~�l�[�g
		txtp=txtalc;													// �e�L�X�g�|�C���^�Z�b�g
		while( *txtp ){
			nxtp=strchr( txtp, '\n' );									// ������𑖍����A�w�肳�ꂽ�����Z�b�g�Ɋ܂܂�镶���������܂�
			if( !nxtp ){												// ������Ȃ�
				rhp->leftp=(char*)dbStrAlc( rhp->dbp, txtp );			// �c�蕶����o�^
				break;													// ���[�v�I��
			}
			*nxtp=0;													// �O�^�[�~�l�[�g
			dbAlc( rhp->dbp, txtp );									// �s�o�^
			txtp=nxtp+1;												// �e�L�X�g�|�C���^�X�V
		}
		free( txtalc );													// �e�L�X�g�������J��
	}
}

