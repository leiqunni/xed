/************************************************************************/
/*																		*/
/*	�e�L�X�g�G�f�B�^�̂��߂̃f�B�X�v���C���X�g�ϊ�	by H.Watanabe		*/
/*	Ver1.00	2009/04/05 13:52:04	�쐬									*/
/*																		*/
/************************************************************************/

#	include	<stdio.h>													/* �W�����o�͊֐�*/
#	include	<stdlib.h>													/* �W�����C�u�����֐�*/
#	include	<string.h>													/* �W��������֐�*/
#	include	"txtdl.h"													/* �e�L�X�g�G�f�B�^�̂��߂̃f�B�X�v���C���X�g�ϊ�*/

/************************************************************************/
/*	���[�J����`														*/
/************************************************************************/

#	ifndef	EUC
#		if	defined(_WIN32) || defined(__CYGWIN__)						/* SJIS*/
#			define	EUC			0
#		else															/* EUC*/
#			define	EUC			1
#		endif
#	endif

#	define	RNGTSTB(C,L,H)		((unsigned char)((C)-(L))<=(unsigned char)((H)-(L)))			/* ���������W�r�b�g�Ń����W�`�F�b�N������*/

/************************************************************************/
/*	�e�L�X�g�C���t�H���[�V����											*/
/************************************************************************/

static void	txtinf( int *dspsizp, int *txtsizp, int chr, int xx, int tabstp ){

	if( chr&0x80 ){														// ����
#	if	!EUC															/* Windows �̏ꍇ*/
		if( RNGTSTB( chr, 0xa0, 0xdf ) ){								// ���p����
			*dspsizp=1;													// �\���T�C�Y
			*txtsizp=1;													// �e�L�X�g�T�C�Y
		}else{															// �S�p����
			*dspsizp=2;													// �\���T�C�Y
			*txtsizp=2;													// �e�L�X�g�T�C�Y
		}
#	else																/* ���̑�*/
		if( chr==0x8e ){												// ���p����
			*dspsizp=1;													// �\���T�C�Y
		}else{															// �S�p����
			*dspsizp=2;													// �\���T�C�Y
		}
		*txtsizp=2;														// �e�L�X�g�T�C�Y
#	endif
	}else{																// ASCII
		if( chr=='\t' ){												// �^�u
			*dspsizp=((xx+tabstp)&-tabstp)-xx;							// �\���T�C�Y
		}else{															// �ʏ핶��
			*dspsizp=1;													// �\���T�C�Y
		}
		*txtsizp=1;														// �e�L�X�g�T�C�Y
	}
}

/************************************************************************/
/*	�t���b�V��															*/
/************************************************************************/

static void	flush( stDB *dldbp, stDB *tmpdbp ){

	stTXTDL		*soup, *nxtp;											// �\�[�X�|�C���^, ���|�C���^
	stTXTDL		*tmpp;													// �e���|�����|�C���^
	stTXTDL		*dlp;													// �f�B�X�v���C���X�g�|�C���^
	char		*keyp;													// �L�[�|�C���^
	int			keysiz;													// �L�[�T�C�Y

	if( tmpdbp ){														// �e���|����DB�͗L��
		soup=(stTXTDL*)dbTop( tmpdbp );									// �擪
		while( soup ){
			keysiz=0;													// �L�[�T�C�Y�N���A
			for( tmpp=soup; tmpp; tmpp=(stTXTDL*)dbNxt( tmpp ) ){		// �����A�g���r���[�g�ł���ԃ��[�v
				if( tmpp->rvs!=soup->rvs || tmpp->col!=soup->col || tmpp->cur!=soup->cur ){		// �A�g���r���[�g���Ⴄ
					break;
				}
				keysiz+=dbKEYSIZ( tmpp );								// �L�[�T�C�Y���Z
			}
			nxtp=tmpp;													// �����L�^
			dlp=(stTXTDL*)dbAlc( dldbp, 0, keysiz, 0 );					// �f�B�X�v���C���X�g�o�^
			dlp->rvs=soup->rvs;											// ���]�t���O
			dlp->col=soup->col;											// �J���[( 0:�e�L�X�g, 1:[EOF]�� )
			dlp->cur=soup->cur;											// �J�[�\���t���O
			keyp=(char*)dbKEYADR( dlp );								// �L�[�|�C���^�𓾂�
			for( tmpp=soup; tmpp!=nxtp; tmpp=(stTXTDL*)dbNxt( tmpp ) ){	// ���ɓ������Ȃ��ԃ��[�v
				memcpy( keyp, dbKEYADR( tmpp ), dbKEYSIZ( tmpp ) );		// �L�[���R�s�[
				keyp+=dbKEYSIZ( tmpp );									// �L�[�|�C���^�V�[�N
				dlp->dspsiz+=tmpp->dspsiz;								// �\���T�C�Y���Z
			}
			soup=nxtp;													// ���ɃV�[�N
		}
		dbClose( tmpdbp );												// �e���|����DB�j��
	}
}

/************************************************************************/
/*	�f�B�X�v���C���X�g�o�^												*/
/************************************************************************/

static stTXTDL	*dlalc( stDB *dldbp, const char *keyadr, int keysiz, int dspsiz, int rvs, int cur, int col ){

	stTXTDL		*dlp;													// �f�B�X�v���C���X�g�|�C���^

	dlp=(stTXTDL*)dbAlc( dldbp, keyadr, keysiz, 0 );					// DB�o�^
	dlp->dspsiz=dspsiz;													// �\���T�C�Y
	dlp->rvs=rvs;														// ���]�t���O
	dlp->col=col;														// �J���[( 0:�e�L�X�g, 1:[EOF]�� )
	dlp->cur=cur;														// �J�[�\���t���O
	return( dlp );														// �f�B�X�v���C���X�g�|�C���^�������ă��^�[��
}

/************************************************************************/
/*	�e�L�X�g�G�f�B�^�̂��߂̃f�B�X�v���C���X�g�ϊ�						*/
/************************************************************************/

//	txtp:	�e�L�X�g�A�h���X
//	curxx:	�J�[�\�����W
//	tabstp:	�^�u�X�e�b�v
//	tabdsp:	�^�u�\���t���O( 0:��\��, 1:�\�� )
//	eof:	�G���h�I�u�t�@�C���t���O( 0:�s�[, 1:�t�@�C���I�[ )
//	eofdsp:	�I�[�L���\��( 0:��\��, 1:�\�� )
//	pgxx:	���W�X�N���[���l
//	findsiz:�������ʂ̃T�C�Y( 0:���� )
//	xxw:	��ʕ\����

stDB	*txtdl( const char *txtp, int curxx, int tabstp, int tabdsp, int eof, int eofdsp, int pgxx, int findsiz, int xxw ){

	static const char	*eofpa[2]={ ".", "[EOF]" };						// �I�[������
	stDB		*dldbp;													// �f�B�X�v���C���X�gDB
	stDB		*tmpdbp=0;												// �e���|�����f�B�X�v���C���X�gDB
	stTXTDL		*tabdlp=0;												// �^�u�f�B�X�v���C���X�g�|�C���^
	const char	*findp=0;												// �����|�C���^
	int			chr;													// �J�����g����
	int			dspsiz, txtsiz;											// �\���T�C�Y�A�e�L�X�g�T�C�Y
	int			xx=0;													// �\�����W
	const char	*eofp;													// "." �܂��� "[EOF]" �ւ̃|�C���^
	int			rvs;													// ���]�t���O
	int			cur;													// �J�[�\���t���O
	int			find;													// �����t���O
	int			col;													// �J���[( 0:�e�L�X�g, 1:[EOF]�� )

	dldbp=dbOpen( DBSIZ_VAR, sizeof(stTXTDL), 0, 4096 );				// �f�B�X�v���C���X�gDB�I�[�v��
	for(;;){
		chr=(unsigned char)*txtp;										// �ꕶ������
		if( !chr || xx>=pgxx+xxw ){										// �e�L�X�g�I�����͈͊O
			break;
		}
		txtinf( &dspsiz, &txtsiz, chr, xx, tabstp );					// �e�L�X�g�C���t�H���[�V����
		cur=( xx==curxx );												// �J�[�\���t���O
		if( cur ){														// �J�[�\���|�W�V����
			findp=txtp;													// �����|�C���^�擪�Z�b�g
		}
		col=find=( txtp>=findp && txtp<findp+findsiz );					// �����t���O�Z�b�g
		rvs=( find || cur );											// ���]�t���O
		if( xx==pgxx || ( xx==pgxx-1 && dspsiz==2 ) ){					// ���W�͉�ʕ\���͈͂ɓ�����
			flush( dldbp, tmpdbp );										// �t���b�V��
			tmpdbp=0;													// �e���|����DB�N���A
			tabdlp=0;													// �^�u�I��
		}
		if( chr=='\t' ){												// �^�u
			flush( dldbp, tmpdbp );										// �t���b�V��
			tmpdbp=0;													// �e���|����DB�N���A
			if( !tabdsp ){												// �^�u��\��
				if( cur ){												// �J�[�\���ʒu�ł���
					tabdlp=0;											// �^�u�I��
					dlalc( dldbp, " ", 1, 1, rvs, cur, col );			// �J�[�\���\��
					dlalc( dldbp, 0, 0, dspsiz-1, find, 0, col );		// �c��̃X�y�[�X�\��
				}else{
					if( !tabdlp ){
						tabdlp=dlalc( dldbp, 0, 0, 0, rvs, cur, col );	// �f�B�X�v���C���X�g�o�^
					}
					tabdlp->dspsiz+=dspsiz;								// �^�u���Z
				}
			}else{														// �^�u�\��
				tabdlp=0;												// �^�u�I��
				dlalc( dldbp, ">", 1, 1, rvs, cur, 1 );					// �f�B�X�v���C���X�g�o�^
				dlalc( dldbp, "---------------", dspsiz-1, dspsiz-1, 0, cur, 1 );				// �f�B�X�v���C���X�g�o�^
			}
		}else{															// �^�u�ȊO�̕���
			tabdlp=0;													// �^�u�I��
			if( !tmpdbp ){												// �e���|����DB�͖���
				tmpdbp=dbOpen( DBSIZ_VAR, sizeof(stTXTDL), 0, 4096 );	// �e���|����DB�I�[�v��
			}
			dlalc( tmpdbp, txtp, txtsiz, dspsiz, rvs, cur, col );		// �e���|����DB�ɒʏ핶���o�^
		}
		txtp+=txtsiz;													// �e�L�X�g�|�C���^�V�[�N
		xx+=dspsiz;														// ���W�V�[�N
	}
	flush( dldbp, tmpdbp );												// �t���b�V��
	if( xx<pgxx+xxw ){													// ��ʔ͈͓�
		cur=rvs=( xx==curxx );											// �J�[�\���t���O
		if( eofdsp ){													// [EOF]�\����ON
			eofp=eofpa[ eof!=0 ];										// "." �܂��� "[EOF]" �ւ̃|�C���^
			dspsiz=strlen( eofp );
		}else{															// [EOF]�\����OFF
			eofp="";
			dspsiz=1;													// �J�[�\���\��
		}
		dlalc( dldbp, eofp, strlen( eofp ), dspsiz, rvs, cur, 1 );		// �f�B�X�v���C���X�g�o�^
		xx+=dspsiz;														// ���W�V�[�N
		if( xx<pgxx && curxx>=0 ){										// �y�[�W���X�N���[�����Ă��ĉ�ʖ����ŏI����Ă���
			dlalc( dldbp, 0, 0, pgxx-xx, 0, 0, 0 );						// �\���ʒu���V�[�N���邽�߂̃_�~�[
			dlalc( dldbp, 0, 0, 1, 1, 1, 1 );							// �J�[�\���\��
		}
	}
	return( dldbp );
}

