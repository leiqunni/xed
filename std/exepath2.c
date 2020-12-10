/************************************************************************/
/*																		*/
/*	���s�t�@�C���p�X�𓾂�		by H.Watanabe							*/
/*	Ver1.00	2009/04/05 13:52:04	�쐬									*/
/*																		*/
/************************************************************************/

#	include	<stdio.h>													/* �W�����o�͊֐�*/
#	include	<stdlib.h>													/* �W�����C�u�����֐�*/
#	include	<string.h>													/* �W��������֐�*/
#	include	<sys/stat.h>												/* �t�@�C���X�e�[�^�X*/
#	include	"exepath2.h"												/* ���s�t�@�C���p�X����*/

/************************************************************************/
/*	���[�J����`														*/
/************************************************************************/

#	define	RNGTSTB(C,L,H)	((unsigned char)((C)-(L))<=(unsigned char)((H)-(L)))				/* ���������W�r�b�g�Ń����W�`�F�b�N������*/
#	if	defined(_WIN32) || defined(__CYGWIN__)							/* SJIS*/
#		define	KANJI(C)	(RNGTSTB((C),0x81,0x9f)||RNGTSTB((C),0xe0,0xfc))
#		define	ENVSEP		';'
#	else																/* EUC*/
#		define	KANJI(C)	(((C)&0x80)>>7)
#		define	ENVSEP		':'
#	endif

/************************************************************************/
/*	���s�t�@�C�����݃`�F�b�N											*/
/************************************************************************/

static int	exechk( char *pacnamp, const char *pathp, const char *exenamp, const char **extpp ){

	int			res=0;
	struct stat	st;														// stat �\����
	const char	*extp;
	const char	*sepp="";												// �f�B���N�g�����Z�p���[�^
	int			len;
	int			i;

	len=strlen( pathp );												// �p�X��������
	if( len ){															// �p�X�������݂���
		if( pathp[len-1]!='\\' && pathp[len-1]!='/' ){					// �p�X���̏I�[���p�X���Z�p���[�^�ŏI����Ă��Ȃ�
			sepp="/";													// �p�X���Z�p���[�^������Z�b�g
		}
	}
	for( i=0; ( extp=extpp[i] )!=0; i++ ){
		sprintf( pacnamp, "%s%s%s%s", pathp, sepp, exenamp, extp );
		res=!stat( pacnamp, &st );										// �t�@�C�����݃`�F�b�N
#	ifdef	unix
		if( res ){
			res=( ((S_IXUSR|S_IXGRP|S_IXOTH)&( st.st_mode ))!=0 );		// ���s�A�g���r���[�g�������Ă���
		}
#	endif
		if( res ){														// �t�@�C�������݂���
			break;														// ���[�v�I��
		}
	}
	return( res );
}

/************************************************************************/
/*	���s�t�@�C���p�X�𓾂�												*/
/************************************************************************/

int		exepath2( char *pacnamp, const char *exenamp, const char **extpp ){

	static const char	*extpa[]={
		"",
		0
	};
	int				res;												// ���U���g�X�e�[�^�X
	const char		*txtp;												// �e�L�X�g�|�C���^
	const char		*nxtp;												// �����|�C���^
	const char		*extp=0;											// �g���q���|�C���^
	unsigned char	chr;
	char			path[4096];											// �p�X��

	txtp=exenamp;
	while( ( chr=*txtp++ )!=0 ){
		if( chr==':' || chr=='/' || chr=='\\' ){						// �p�X�Z�p���[�^��������
			extp=0;														// �g���q���|�C���^���Z�b�g
		}else if( chr=='.' ){											// �J���}��������
			extp="";													// �g���q���|�C���^�Z�b�g
		}
		txtp+=KANJI( chr );												// �����Ȃ��+1
	}
	if( extp ){															// �g���q�̎w�肪������
		extpp=extpa;
	}else{																// �g���q�̎w��͖���
		if( !extpp ){
#	ifdef	unix
			extpp=extpa;
#	else
			static const char	*winextpa[]={
				".exe",
				".com",
				".bat",
				0
			};
			extpp=winextpa;
#	endif
		}
	}
	res=exechk( pacnamp, "", exenamp, extpp );							// ���s�t�@�C�����݃`�F�b�N
	if( !res ){															// ������Ȃ�
		txtp=getenv( "PATH" );											// ���ϐ��𓾂�
		if( txtp ){
			while( *txtp ){												// �e�L�X�g�|�C���^�I�[�܂Ń��[�v
				nxtp=strchr( txtp, ENVSEP );							// �p�X�Z�p���[�^����
				if( nxtp ){												// �p�X�Z�p���[�^����������
					memcpy( path, txtp, nxtp-txtp );					// �p�X�Z�p���[�^�܂ł̕�������p�X���ɃR�s�[
					path[nxtp-txtp]=0;									// �O�^�[�~�l�[�g
					txtp=nxtp+1;										// �e�L�X�g�|�C���^���p�X�Z�p���[�^�̎��ɃV�[�N
				}else{													// �p�X�Z�p���[�^�͌�����Ȃ�
					strcpy( path, txtp );								// �I�[�܂ŃR�s�[
					txtp+=strlen( txtp );								// �e�L�X�g�|�C���^���I�[�ɃV�[�N
				}
				res=exechk( pacnamp, path, exenamp, extpp );			// ���s�t�@�C�����݃`�F�b�N
				if( res ){												// �t�@�C�������݂���
					break;												// ���[�v�I��
				}
			}
		}
	}
	return( res );														// �A��l: 0=������Ȃ�, 1:��������
}

