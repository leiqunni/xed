/************************************************************************/
/*																		*/
/*	���ϐ��ϊ�				by H.Watanabe							*/
/*	Ver1.00	2009/04/05 13:52:04	�쐬									*/
/*																		*/
/************************************************************************/

#	include	<stdlib.h>													/* �W�����C�u�����֐�*/
#	include	<string.h>													/* �W��������֐�*/
#	include	"envcnv3.h"													/* ���ϐ��ϊ�*/

/************************************************************************/
/*	���[�J����`														*/
/************************************************************************/

#	define	RNGCHK(C,L,H)	((unsigned)((C)-(L))<=(unsigned)((H)-(L)))
#	define	LBLTOPCHR(C)	(RNGCHK(((C)|0x20),'a','z')||((C)=='_'))
#	define	LBLNXTCHR(C)	(RNGCHK((C),'0','9')||LBLTOPCHR(C))

/************************************************************************/
/*	���ϐ��ϊ�														*/
/************************************************************************/

void	envcnv( char *dstp, const char *txtp ){

	char			*envp;
	char			stra[4096], *strp;
	char			tmpa[4096];
	unsigned int	chr;

	strcpy( tmpa, txtp );
	txtp=tmpa;
	while( ( chr=*txtp++ )!=0 ){										// �����|�C���^����P�����𓾂�
		if( chr=='$' ){													// ���ϐ��w�莯�ʎq
			strp=stra+1;												// �L�����x���擪�� str+1
			stra[0]=(char)chr;											// �㏈���̂��ߐ擪��'$'���Z�b�g
			if( LBLTOPCHR( *txtp ) ){									// ���x���擪�Ɏg�p�ł��镶��
				for(;;){												// ���x���𓾂郋�[�v
					*strp++=*txtp++;									// �����R�s�[
					if( !LBLNXTCHR( *txtp ) ){							// ���x�����Ŏg�p�ł��Ȃ����������ꂽ
						break;
					}
				}
			}
			*strp=0;													// ���x���I�[���O�^�[�~�l�[�g
			envp=getenv( stra+1 );										// ���ϐ��̃|�C���^�𓾂�
			if( !envp ){												// ������Ȃ�����
				envp=stra;												// �u��������̓I���W�i��������
			}
			strcpy( dstp, envp );
			dstp+=strlen( dstp );
		}else{
			*dstp++=(char)chr;
		}
	}
	*dstp=0;															// �O�^�[�~�l�[�g
}

