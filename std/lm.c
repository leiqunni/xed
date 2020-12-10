/************************************************************************/
/*																		*/
/*	���C���}�b�v				by H.Watanabe							*/
/*	Ver1.00	2009/04/05 13:23:49	�쐬									*/
/*																		*/
/************************************************************************/

#	include	<string.h>													/* �W��������֐�*/
#	include	"lm.h"														/* ���C���}�b�v*/

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

#	define	RNGTSTB(C,L,H)	((unsigned char)((C)-(L))<=(unsigned char)((H)-(L)))				/* ���������W�r�b�g�Ń����W�`�F�b�N������*/

	typedef unsigned char	uchar;

/************************************************************************/
/*	���C���}�b�v�쐬													*/
/************************************************************************/

int		lmMak( stLM *lmp, const char *txtadr, int tab, int wordflg ){

	enum{
		NO,																// ����
		CD,																// �L��
		SP,																// �X�y�[�X�܂��̓^�u
		LB,																// ���x��
		KJ																// ����
	};
	static const char	typa[]={										// ���x���LB�����NO�^�e�[�u��
		CD,CD,CD,CD,CD,CD,CD,CD,CD,SP,CD,CD,CD,CD,CD,CD,				// 0x00
		CD,CD,CD,CD,CD,CD,CD,CD,CD,CD,CD,CD,CD,CD,CD,CD,				// 0x10
		SP,CD,CD,CD,CD,CD,CD,CD,CD,CD,CD,CD,CD,CD,CD,CD,				// 0x00
		LB,LB,LB,LB,LB,LB,LB,LB,LB,LB,CD,CD,CD,CD,CD,CD,				// 0x00
		CD,LB,LB,LB,LB,LB,LB,LB,LB,LB,LB,LB,LB,LB,LB,LB,				// 0x40
		LB,LB,LB,LB,LB,LB,LB,LB,LB,LB,LB,CD,CD,CD,CD,LB,				// 0x50
		CD,LB,LB,LB,LB,LB,LB,LB,LB,LB,LB,LB,LB,LB,LB,LB,				// 0x60
		LB,LB,LB,LB,LB,LB,LB,LB,LB,LB,LB,CD,CD,CD,CD,CD,				// 0x70
	};
	uchar		*txtp;
	int			chr;
	tpLM	txtofs=0;
	int			typbak=NO;
	int			typ;
	int			pos=0;
	int			siz;
	int			i;

	txtp=(uchar*)txtadr;
	while( ( chr=*txtp++ )!=0 ){
		if( pos>=LM_MAX ){												// �s�o�b�t�@�I�[�o�[�t���[
			break;
		}
		if( !wordflg ){													// ���[�h�t���O�͂����Ă��Ȃ�
			txtofs=(tpLM)( (txtp-1)-(uchar*)txtadr );					// �e�L�X�g�I�t�Z�b�g�v�Z
		}else{															// ���[�h�t���O�������Ă���
			if( chr&0x80 ){												// ����
				typ=KJ;													// �J�����g�^�C�v�͊���
			}else{														// ASCII
				typ=typa[chr];											// �J�����g�^�C�v�̓R�[�h�܂��̓X�y�[�X�܂��̓��x��
			}
			if( typbak!=typ || typ==CD ){								// �O�^�CD�v�ƈႤ���܂��̓R�[�h
				typbak=typ;												// �O�^�CD�v�X�V
				txtofs=(tpLM)( (txtp-1)-(uchar*)txtadr );				// �e�L�X�g�I�t�Z�b�g�v�Z
			}
		}
		siz=1;															// �f�t�H���g�T�C�Y�Z�b�g
		if( chr=='\t' ){												// TAB
			siz=((pos+tab)&-tab)-pos;
		}else if( chr&0x80 ){											// ���p�܂��͑S�p�R�[�h
#	if	!EUC															// SJIS
			if( RNGTSTB(chr,0x81,0x9f)|RNGTSTB(chr,0xe0,0xfc) ){		// WIN32:SJIS
				siz=2;
				txtp++;
			}
#	else																// EUC
			if( chr!=0x8e ){											// ���p���Ȃł͂Ȃ�
				siz=2;
			}
			txtp++;
#	endif
		}
		for( i=0; i<siz; i++ ){
			lmp->txtofsa[pos++]=txtofs;
		}
	}
	lmp->txtofsa[pos]=(tpLM)( (txtp-1)-(uchar*)txtadr );
	lmp->siz=pos;
	return( pos );
}

/************************************************************************/
/*	���C���}�b�v�ɂ��J�[�\���|�W�V�����̐��K��						*/
/************************************************************************/

int		lmPosAdj( stLM *lmp, int pos, int ofs ){

	int		txtofs;

	if( pos>=lmp->siz ){
		pos=lmp->siz;
	}
	txtofs=lmp->txtofsa[pos];
	while( pos>0 && txtofs==lmp->txtofsa[pos-1] ){
		pos--;
	}
	if( ofs<0 ){														// ���ړ�
		if( pos ){
			pos--;
			txtofs=lmp->txtofsa[pos];
			while( pos>0 && txtofs==lmp->txtofsa[pos-1] ){
				pos--;
			}
		}
	}else if( ofs>0 ){													// �E�ړ�
		while( pos<lmp->siz && txtofs==lmp->txtofsa[pos] ){
			pos++;
		}
	}
	return( pos );
}

/************************************************************************/
/*	�\�[�X�̃I�t�Z�b�g����J�[�\���|�W�V�����𓾂�						*/
/************************************************************************/

int		lmOfs2Pos( stLM *lmp, int txtofs ){

	int		i;

	for( i=0; i<lmp->siz; i++ ){
		if( lmp->txtofsa[i]>=txtofs ){
			break;
		}
	}
	return( i );
}

/************************************************************************/
/*	�J�[�\���|�W�V��������\�[�X�̃I�t�Z�b�g�𓾂�						*/
/************************************************************************/

int		lmPos2Ofs( stLM *lmp, int pos ){

	if( pos>lmp->siz ) pos=lmp->siz;
	return( lmp->txtofsa[pos] );
}

/************************************************************************/
/*	������Z�b�g														*/
/************************************************************************/

int		lmSetStr( char *txtadr, int curxx, const char *strp, int tabstp, int flgins ){

	stLM	lm;															// �s�}�b�v
	int			nxtxx;
	int			curofs;													// �J�����g�I�t�Z�b�g
	int			nxtofs;													// �J�����g�I�t�Z�b�g
	char		txta[LM_MAX+4];
	char		*txtp;

	lmMak( &lm, txtadr, tabstp, 0 );									// �s�}�b�v�쐬
	curxx=lmPosAdj( &lm, curxx, 0 );									// �J�[�\���ʒu���K��
	curofs=lm.txtofsa[curxx];											// �J�b�g�I�t�Z�b�g
	nxtxx=lmPosAdj( &lm, curxx, flgins==0 );
	nxtofs=lm.txtofsa[nxtxx];
	txtp=txta;
	memcpy( txtp, txtadr, curofs );
	txtp+=curofs;
	strcpy( txtp, strp );
	txtp+=strlen( txtp );
	strcpy( txtp, txtadr+nxtofs );
	strcpy( txtadr, txta );
	lmMak( &lm, txtadr, tabstp, 0 );									// �s�}�b�v�쐬
	curxx=lmPosAdj( &lm, curxx, 1 );									// �J�[�\���ʒu���K��
	return( curxx );
}

/************************************************************************/
/*	�ꕶ���Z�b�g														*/
/************************************************************************/

int		lmSetChr( char *txtadr, int curxx, int xedk, int tabstp, int flgins ){

	char	tmpa[4];
	char	*tmpp=tmpa;

	if( xedk&0xff00 ){
		*tmpp++=(char)(xedk>>8);
	}
	*tmpp++=(char)xedk;
	*tmpp=0;
	return( lmSetStr( txtadr, curxx, tmpa, tabstp, flgins ) );
}

