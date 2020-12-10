/************************************************************************/
/*																		*/
/*	�p�X���֘A					by H.Watanabe							*/
/*	Ver1.00	2009/03/29 10:03:05	�쐬									*/
/*																		*/
/************************************************************************/

#	include	<string.h>													/* �W��������֐�*/
#	include	"path2.h"													/* �p�X���֘A*/

/************************************************************************/
/*	���[�J����`														*/
/************************************************************************/

#	define	RNGTSTB(C,L,H)	((unsigned char)((C)-(L))<=(unsigned char)((H)-(L)))				/* ���������W�r�b�g�Ń����W�`�F�b�N������*/
#	if	defined(_WIN32) || defined(__CYGWIN__)							/* SJIS*/
#		define	KANJI(C)	(RNGTSTB((C),0x81,0x9f)||RNGTSTB((C),0xe0,0xfc))
#	else																/* EUC*/
#		define	KANJI(C)	(((C)&0x80)>>7)
#	endif

/************************************************************************/
/*	�m�[�h���܂ŃV�[�N����												*/
/************************************************************************/

char	*pathSeekNam( char *pathp, const char *txtadr ){

	const char		*txtp=txtadr, *sepp=txtadr;
	unsigned char	chr;

	while( (chr=*txtp++)!=0 ){
		if( chr==':' || chr=='/' || chr=='\\' ){						// �p�X�Z�p���[�^��������
			sepp=txtp;
		}
		txtp+=KANJI( chr );												// �����Ȃ��+1
	}
	if( pathp ){														// �f�B�X�g�l�[�V�������w�肳��Ă���
		memcpy( pathp, txtadr, sepp-txtadr );							// �p�X������R�s�[
		pathp[sepp-txtadr]=0;											// �O�^�[�~�l�[�g
	}
	return( (char*)sepp );
}

/************************************************************************/
/*	�g���q���܂ŃV�[�N����												*/
/************************************************************************/

char	*pathSeekExt( char *pathp, char *namp, const char *txtadr ){

	const char	*txtp, *sepp;

	txtp=pathSeekNam( pathp, txtadr );									// �m�[�h���܂ŃV�[�N����
	sepp=txtp+strspn( txtp, "." );										// �擪�ɑ��� '.' ���X�L�b�v
	sepp=strrchr( sepp, '.' );											// �Ō��'.'����
	if( !sepp ){														// ������Ȃ�����
		sepp=txtp+strlen( txtp );										// �I�[�փV�[�N
	}
	if( namp ){															// �f�B�X�g�l�[�V�������w�肳��Ă���
		memcpy( namp, txtp, sepp-txtp );								// �m�[�h������R�s�[
		namp[sepp-txtp]=0;												// �O�^�[�~�l�[�g
	}
	return( (char*)sepp );
}

/************************************************************************/
/*	�p�X���ƃt�@�C������A������										*/
/************************************************************************/

void	pathCat( char *dstp, const char *pathp, const char *namp ){

	unsigned char	chr='/';											// �p�X������""�̏ꍇ�̃f�t�H���g

	while( *pathp ){
		chr=( *dstp++=*pathp++ );										// 1�o�C�g�R�s�[
		if( KANJI( chr ) ){												// 2�o�C�g�ڂ� '\' �̏ꍇ�����邽��
			*dstp++=*pathp++;											// 1�o�C�g�R�s�[
		}
	}
	if( chr!='/' && chr!='\\' && chr!=':' ){							// �f�B���N�g���Z�p���[�^�ł͂Ȃ�
		*dstp++='/';													// �f�B���N�g���Z�p���[�^�t��
	}
	strcpy( dstp, namp );
}

/************************************************************************/
/*	�g���q���ύX														*/
/************************************************************************/

void	pathRepExt( char *dstp, const char *soup, const char *extp ){

	int		len;														// �p�X�{���O�̃T�C�Y

	len=pathSeekExt( 0, 0, soup )-soup;									// �p�X�{���O�̃T�C�Y
	memcpy( dstp, soup, len );											// �p�X�{���O���R�s�[
	strcpy( dstp+len, extp );											// �g���q�����R�s�[
}

