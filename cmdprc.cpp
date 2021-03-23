/************************************************************************/
/*																		*/
/*	�R�}���h����				by H.Watanabe							*/
/*	Ver1.00	2009/04/04 13:49:05	�쐬�J�n								*/
/*																		*/
/************************************************************************/

#	include	<stdio.h>													/* �W�����o�͊֐�*/
#	include	<stdlib.h>													/* �W�����C�u�����֐�*/
#	include	<string.h>													/* �W��������֐�*/
#	include	"std/filedb.h"												/* �f�[�^�x�[�X�t�@�C���o�^*/
#	include	"std/clpbrd.h"												/* �N���b�v�{�[�h�Ǘ�*/
#	include	"std/filterdb.h"											/* �t�B���^�[����*/
#	include	"std/tagparse.h"											/* �^�O���*/
#	include	"std/envcnv3.h"												/* ���ϐ��ϊ�*/
#	include	"std/eval.h"												/* �����]��( Evaluation of Expressions )*/
#	include	"std/evalf.h"												/* �����]��*/
#	include	"main.h"													/* ���C���C���N���[�h�t�@�C��*/

/************************************************************************/
/*	�J�[�\�����W�Z�b�g( �}�E�X���{�^��������������Ă΂�� )			*/
/************************************************************************/

void	curset( int musx, int musy ){

	int			xx, yy;													// �L�����N�^���W
	int			seekn;													// ���ۂɃV�[�N������
	stLIN		*linp;													// �s�|�C���^
	stLM		lm;														// �s�}�b�v

	xx=musx/g_fxw-g_txxc;												// �L�����N�^X���W
	yy=musy/g_fyw-g_tyyc;												// �L�����N�^Y���W
	linp=(stLIN*)dbSeek( &seekn, PGP, yy );								// ���̃y�[�W�擪�܂ŃV�[�N���Ă݂�
	if( seekn==yy ){													// �V�[�N�ł���
		lmMak( &lm, (char*)dbKEYADR( linp ), g_tabstp, 0 );				// �s�}�b�v�쐬
		if( xx+PGXX<=lm.siz ){											// �L���͈͓�
			CURP=linp;													// �J�[�\���|�C���^
			CURYY=yy;													// �J�[�\��Y���W
			CURXX=PGXX+xx;												// �J�[�\��X���W
			invalidateAll();											// �S�čĕ`��
		}
	}
}

/************************************************************************/
/*	�I�����[�h�őI��擪����̍s�ō�Ƃ�������I�����[�h����������	*/
/************************************************************************/

static void	cutchk(void){
	if( CUTCURP ){
		if( PGYY+CURYY<=CUTPGYY+CUTCURYY ){
			CUTCURP=0;													// �͈͎w��I��
			invalidateAll();											// �S�čĕ`��
		}
	}
}

/************************************************************************/
/*	�s�ύX�Z�b�g														*/
/************************************************************************/

void	linchg( stLIN *linp ){
	linp->chgflg=1;														// �s�ύX�t���O�Z�b�g
	FILCHG=1;															// �t�@�C���ύX�t���O�Z�b�g
}

/************************************************************************/
/*	�s�ύX�Z�b�g�ƍĕ\��												*/
/************************************************************************/

static void	linchgdsp( stLIN *linp, int yy ){
	if( !linp->chgflg ){												// �s�ύX�t���O�͗����Ă��Ȃ�
		linp->chgflg=1;													// �s�ύX�t���O�Z�b�g
		invalidateLno( yy );											// �s�ԍ��ĕ\��
	}
	if( !FILCHG ){														// �t�@�C���ύX�t���O�͗����Ă��Ȃ�
		FILCHG=1;														// �t�@�C���ύX�t���O�Z�b�g
		invalidateFil();												// �t�@�C���ĕ\��
	}
}

/************************************************************************/
/*	�e�L�X�g�X�V														*/
/************************************************************************/

stLIN	*linflush( stLIN *orgp, const char *txtp ){

	stLIN		*linp;													// �s�|�C���^
	int			orgsiz, newsiz;											// �s�T�C�Y

	orgsiz=dbKEYSIZ( orgp );											// ���̍s�T�C�Y
	newsiz=linAlcSizStr( txtp );										// �V�����s�T�C�Y
	if( orgsiz==newsiz ){												// �s�T�C�Y�͓���
		strcpy( (char*)dbKEYADR( orgp ), txtp );						// ���̂܂܃R�s�[
		linp=orgp;														// �s�|�C���^�͌��̍s�|�C���^
	}else{																// �s�T�C�Y�͈Ⴄ
		linp=linAlcStr( LINDBP, txtp );									// �s�o�^
		dbMov( LINDBP, linp, orgp );									// ���̍s�̑O�Ɉړ�
		if( CUTPGP==orgp ){												// �J�b�g�y�[�W�|�C���^�̏ꍇ
			CUTPGP=linp;												// �J�b�g�y�[�W�|�C���^�X�V
		}
		if( CUTCURP==orgp ){											// �J�b�g�J�����g�|�C���^�̏ꍇ
			CUTCURP=linp;												// �J�b�g�J�����g�|�C���^�X�V
		}
		if( PGP==orgp ){												// �y�[�W�|�C���^�̏ꍇ
			PGP=linp;													// �y�[�W�|�C���^�X�V
		}
		if( CURP==orgp ){												// �J�����g�|�C���^�̏ꍇ
			CURP=linp;													// �J�����g�|�C���^�X�V
		}
		dbFre( LINDBP, orgp );											// ���̋ƊE��
	}
	return( linp );														// �s�|�C���^�������ă��^�[��
}

/************************************************************************/
/*	�J�����g�e�L�X�g�X�V												*/
/************************************************************************/

stLIN	*renewtxt( stLIN *orgp, int yy, const char *txtp ){

	stLIN	*linp;														// �s�|�C���^

	linp=linflush( orgp, txtp );										// �e�L�X�g�X�V
	linchgdsp( linp, yy );												// �s�ύX�Z�b�g�ƍĕ\��
	return( linp );
}

/************************************************************************/
/*	�R���e�L�X�g�ύX													*/
/************************************************************************/

static void	ctxchg( int ofs ){

	stCTX	*ctxp;														// �R���e�L�X�g�|�C���^

	if( ofs<0 ){														// ���
		ctxp=(stCTX*)dbBak( g_ctxp );									// �O
		if( !ctxp ){													// �O�͂Ȃ�
			ctxp=(stCTX*)dbEnd( g_ctxdbp );								// �擪
		}
	}else{																// �O��
		ctxp=(stCTX*)dbNxt( g_ctxp );									// ��
		if( !ctxp ){													// ���͂Ȃ�
			ctxp=(stCTX*)dbTop( g_ctxdbp );								// �I�[
		}
	}
	if( g_ctxp != ctxp ){												// �ύX��������
		g_ctxp=ctxp;													// �J�����g�R���e�L�X�g�ύX
		invalidateAll();												// �S�čĕ`��
	}
}

/************************************************************************/
/*	�s�ԍ��Z�b�g														*/
/************************************************************************/

void	lnoset( int lno ){

	int			seekn;													// ���ۂɃV�[�N������
	int			pgseekn;												// �y�[�W�V�[�N��
	int			linn;													// ���s��

	if( lno>=PGYY && lno<PGYY+g_tyyd ){									// �\���͈͓�
		CURP=(stLIN*)dbSeek( &seekn, PGP, lno-PGYY );					// ���̃y�[�W�擪�܂ŃV�[�N���Ă݂�
		CURYY=seekn;
	}else{																// �\���͈͊O
		pgseekn=(lno/g_tyyd)*g_tyyd;									// �y�[�W�V�[�N���v�Z
		linn=dbAlcN( LINDBP );											// ���s��
		if( pgseekn>linn-g_tyyd ){										// �y�[�W�V�[�N�������s���|��ʍs�����傫��
			pgseekn=linn-g_tyyd;										// �N�����v
		}
		PGP=(stLIN*)dbSeek( &seekn, dbTop( LINDBP ), pgseekn );			// �擪����y�[�W�V�[�N
		PGYY=seekn;														// �y�[�W�擪�s�ԍ��Z�b�g
		CURP=(stLIN*)dbSeek( &seekn, PGP, lno-seekn );					// �J�����g�|�C���^�V�[�N
		CURYY=seekn;													// �J�����g�x�Z�b�g
		invalidateAll();												// �S�čĕ`��
	}
}

/************************************************************************/
/*	�y�[�W�X�N���[��													*/
/************************************************************************/

void	pgscroll( int ofs ){

	int			seekn;													// ���ۂɃV�[�N������
	int			pgseekn;												// �y�[�W�V�[�N��
	int			linseekn=ofs;											// ���C���V�[�N��
	int			dspflg=0;												// �\���t���O

	if( ofs<0 ){														// ���
		PGP=(stLIN*)dbSeek( &seekn, PGP, linseekn );					// ���̃y�[�W�擪�܂ŃV�[�N���Ă݂�
		PGYY+=seekn;													// �y�[�WY�X�V
		CURP=(stLIN*)dbSeek( 0, CURP, seekn );							// �J�����g�|�C���^�X�V
		linseekn-=seekn;												// ���C���V�[�N�����Z
		dspflg=( seekn!=0 );											// �\���t���O�Z�b�g
		CURP=(stLIN*)dbSeek( &seekn, CURP, linseekn );					// ���̃y�[�W�擪�܂ŃV�[�N���Ă݂�
		CURYY+=seekn;													// �J�����gY�X�V
		dspflg|=( seekn!=0 );											// �\���t���O�Z�b�g
	}else if( ofs ){													// �O��
		dbSeek( &seekn, PGP, ofs+g_tyyd-1 );							// ���̃y�[�W�擪�܂ŃV�[�N���Ă݂�
		pgseekn=seekn-(g_tyyd-1);										// �y�[�W�V�[�N��
		if( pgseekn>0 ){												// �y�[�W�V�[�N�\.
			PGP=(stLIN*)dbSeek( 0, PGP, pgseekn );						// ���̃y�[�W�擪�܂ŃV�[�N���Ă݂�
			PGYY+=pgseekn;												// �y�[�WY�X�V
			CURP=(stLIN*)dbSeek( 0, CURP, pgseekn );					// �J�����g�|�C���^�X�V
			linseekn-=pgseekn;											// ���C���V�[�N�����Z
			dspflg=1;													// �\���t���O�Z�b�g
		}
		CURP=(stLIN*)dbSeek( &seekn, CURP, linseekn );					// �J�����g�|�C���^�V�[�N
		CURYY+=seekn;													// �J�����gY�X�V
		dspflg|=( seekn!=0 );											// �\���t���O�Z�b�g
	}
	if( dspflg ){														// �\���t���O�������Ă���
		invalidateAll();												// �S�čĕ`��
	}
}

/************************************************************************/
/*	���C���X�N���[��													*/
/************************************************************************/

static void	linscroll( int ofs ){

	int			seekn;													// ���ۂɃV�[�N������

	if( ofs<0 ){														// ���
		if( dbBak( PGP ) ){												// �擪�s�ł͂Ȃ�
			PGP=(stLIN*)dbBak( PGP );									// �O��
			if( CURYY==g_tyyd-1 ){										// �ŉ��s
				CURP=(stLIN*)dbBak( CURP );								// �J�����g�|�C���^���O��
			}else{														// �ŉ��s�ł͂Ȃ�
				CURYY++;												// �J�[�\���x���W�C���N�������g
			}
			PGYY--;														// �y�[�W�擪�s�ԍ��f�N�������g
			invalidateAll();											// �S�čĕ`��
		}
	}else{																// �O��
		dbSeek( &seekn, PGP, g_tyyd );									// ���̃y�[�W�擪�܂ŃV�[�N���Ă݂�
		if( seekn==g_tyyd ){											// �V�[�N����
			PGP=(stLIN*)dbNxt( PGP );									// ����
			if( !CURYY ){												// �y�[�W�擪�s
				CURP=PGP;												// �J�����g�|�C���^�ݒ�
			}else{														// �y�[�W�擪�s�ł͂Ȃ�
				CURYY--;												// �J�[�\���x���W�f�N�������g
			}
			PGYY++;														// �y�[�W�擪�s�ԍ��C���N�������g
			invalidateAll();											// �S�čĕ`��
		}
	}
}

/************************************************************************/
/*	���[�Ɉړ�															*/
/************************************************************************/

static void	curtop(void){
	CURXX=0;															// �J�[�\�������[�ɃZ�b�g
	if( linAdjScr() ){													// ���X�N���[���̐��K��
		invalidateAll();												// �S�čĕ`��
	}
}

/************************************************************************/
/*	�E�[�Ɉړ�															*/
/************************************************************************/

static void	curend(void){

	stLM		lm;														// �s�}�b�v

	lmMak( &lm, CURTXTP, g_tabstp, 0 );									// �s�}�b�v�쐬
	CURXX=lm.siz;														// �J�[�\�����E�[�ɃZ�b�g
	if( linAdjScr() ){													// ���X�N���[���̐��K��
		invalidateAll();												// �S�čĕ`��
	}
}

/************************************************************************/
/*	�J�[�\����															*/
/************************************************************************/

static void	curup(void){
	if( dbBak( CURP ) ){												// �擪�s�ł͂Ȃ�
		if( !CURYY ){													// �y�[�W�擪�s
			linscroll( -1 );											// �X�N���[��
		}
		CURP=(stLIN*)dbBak( CURP );										// �ҏW�s�ύX
		CURYY--;														// ���[�J���s�ԍ��C���N�������g
		if( CUTCURP ){													// �͈͎w�蒆
			invalidateLin( CURYY+1 );									// ���s�ĕ\��
		}
		invalidateLin( CURYY+2 );										// ���s�ĕ\��
	}
}

/************************************************************************/
/*	�J�[�\����															*/
/************************************************************************/

static void	curdown(void){
	if( dbNxt( CURP ) ){												// �ŏI�s�ł͂Ȃ�
		if( CURYY==g_tyyd-1 ){											// �y�[�W�ŏI�s
			linscroll( 1 );												// �X�N���[��
		}
		CURP=(stLIN*)dbNxt( CURP );										// �ҏW�s�ύX
		CURYY++;														// ���[�J���s�ԍ��C���N�������g
		if( CUTCURP ){													// �͈͎w�蒆
			invalidateLin( CURYY-1 );									// ���s�ĕ\��
		}
	}
}

/************************************************************************/
/*	�J�[�\����															*/
/************************************************************************/

static int	curleft( int wordflg ){

	int			res=0;													// 0:�ړ��s��, 1:�ړ��\.
	stLM		lm;														// �s�}�b�v

	lmMak( &lm, CURTXTP, g_tabstp, 0 );									// �s�}�b�v�쐬
	CURXX=lmPosAdj( &lm, CURXX, 0 );									// �J�[�\���ʒu���K��
	if( CURXX ){														// �s�擪�ł͂Ȃ�
		lmMak( &lm, CURTXTP, g_tabstp, wordflg );						// �s�}�b�v�쐬
		CURXX=lmPosAdj( &lm, CURXX, -1 );								// �J�[�\�����ړ�
		res=1;															// �ړ��\.
	}else{																// �s�擪
		if( dbBak( CURP ) ){											// �擪�s�ł͂Ȃ�
			curup();													// �J�[�\����
			curend();													// �E�[�Ɉړ�
			res=1;														// �ړ��\.
		}
	}
	if( linAdjScr() ){													// ���X�N���[���̐��K��
		invalidateAll();												// �S�čĕ`��
	}
	return( res );
}

/************************************************************************/
/*	�J�[�\����															*/
/************************************************************************/

static int	curright( int wordflg ){

	int			res=0;													// 0:�ړ��s��, 1:�ړ��\.
	stLM		lm;														// �s�}�b�v

	lmMak( &lm, CURTXTP, g_tabstp, 0 );									// �s�}�b�v�쐬
	CURXX=lmPosAdj( &lm, CURXX, 0 );									// �J�[�\���ʒu���K��
	if( CURXX!=lm.siz ){												// �s�I�[�ł͂Ȃ�
		lmMak( &lm, CURTXTP, g_tabstp, wordflg );						// �s�}�b�v�쐬
		CURXX=lmPosAdj( &lm, CURXX, 1 );								// �J�[�\���E�ړ�
		res=1;															// �ړ��\.
	}else{																// �s�擪
		if( dbNxt( CURP ) ){											// �I�[�s�ł͂Ȃ�
			curdown();													// �J�[�\����
			curtop();													// ���[�Ɉړ�
			res=1;														// �ړ��\.
		}
	}
	if( linAdjScr() ){													// ���X�N���[���̐��K��
		invalidateAll();												// �S�čĕ`��
	}
	return( res );
}

/************************************************************************/
/*	�s�}��																*/
/************************************************************************/

static void	inslin( const char *txtp ){

	stLIN		*linp;													// �J�����g�s�|�C���^

	linp=linAlcStr( LINDBP, txtp );										// �o�b�t�@�𕡎�
	dbMov( LINDBP, linp, CURP );										// �O��
	if( PGP==CURP ){													// �y�[�W�s�擪
		PGP=linp;														// �y�[�W�擪�ύX
	}
	CURP=linp;															// �J�����g�|�C���^�ύX
	linchg( linp );														// �s�ύX�Z�b�g
	invalidateAll();													// �S�čĕ`��
}

/************************************************************************/
/*	���s																*/
/************************************************************************/

static void	cr(void){

	stLM		lm;														// �s�}�b�v
	int			curxx;													// �J�����gX���W
	int			cutofs;													// �J�����g�I�t�Z�b�g
	stLIN		*curp;													// �J�����g�s�|�C���^
	stLIN		*nxtp;													// �l�N�X�g�s�|�C���^

	cutchk();															// �I�����[�h�őI��擪����̍s�ō�Ƃ�������I�����[�h����������
	lmMak( &lm, CURTXTP, g_tabstp, 0 );									// �s�}�b�v�쐬
	curxx=lmPosAdj( &lm, CURXX, 0 );									// �J�[�\���ʒu���K��
	cutofs=lm.txtofsa[curxx];											// �J�b�g�I�t�Z�b�g
	curp=linAlc( LINDBP, CURTXTP, cutofs );								// ���s�ȑO��o�^
	nxtp=linAlcStr( LINDBP, CURTXTP+cutofs );							// ���s�ȍ~��o�^
	dbMov( LINDBP, curp, CURP );										// �O��
	dbMov( LINDBP, nxtp, CURP );										// �㔼
	if( PGP==CURP ){													// �y�[�W�s�擪
		PGP=curp;														// �y�[�W�擪�ύX
	}
	dbFre( LINDBP, CURP );												// ���J�����g�s�j��
	CURP=nxtp;															// �J�����g�|�C���^�ύX
	CURXX=0;															// �J�[�\���͍��[��
	CURYY++;															// �J�����gY���W���Z
	if( CURYY>=g_tyyd ){												// �X�N���[���͈͊O�ƂȂ���
		linscroll( 1 );													// �X�N���[��
	}
	linchg( curp );														// �s�ύX�Z�b�g
	linchg( CURP );														// �s�ύX�Z�b�g
	invalidateAll();													// �S�čĕ`��
}

/************************************************************************/
/*	�폜																*/
/************************************************************************/

static void	del( int wordflg ){

	stLM		lm;														// �s�}�b�v
	int			souxx;													// �\�[�X���W
	stLIN		*nxtp;													// ���̍s�|�C���^
	int			cursiz, nxtsiz;											// �J�����g�T�C�Y�A�l�N�X�g�T�C�Y
	char		tmpa[0x10000];											// �e���|�����o�b�t�@
	stLIN		*linp;													// �s�|�C���^
	char		*dstp;													// �f�B�X�g�l�[�V�����|�C���^

	cutchk();															// �I�����[�h�őI��擪����̍s�ō�Ƃ�������I�����[�h����������
	lmMak( &lm, CURTXTP, g_tabstp, 0 );									// �s�}�b�v�쐬
	CURXX=lmPosAdj( &lm, CURXX, 0 );									// �J�[�\���ʒu���K��
	if( CURXX!=lm.siz ){												// �s�I�[�ł͂Ȃ�
		dstp=CURTXTP+lm.txtofsa[CURXX];									// �f�B�X�g�l�[�V�����|�C���^�Z�b�g
		lmMak( &lm, CURTXTP, g_tabstp, wordflg );						// �s�}�b�v�쐬
		souxx=lmPosAdj( &lm, CURXX, 1 );								// �\�[�X���W
		strcpy( dstp, CURTXTP+lm.txtofsa[souxx] );						// �f�B�X�g�l�[�V�����|�C���^�ɃR�s�[
		renewtxt( CURP, CURYY, CURTXTP );								// �J�����g�e�L�X�g�X�V
	}else{																// �s�擪
		if( !wordflg ){													// ���[�h���[�h�ł͂Ȃ�
			nxtp=(stLIN*)dbNxt( CURP );									// ���̃|�C���^�𓾂�
			if( nxtp ){													// �I�[�s�ł͂Ȃ�
				cursiz=strlen( CURTXTP );								// �J�����g�T�C�Y
				nxtsiz=strlen( (char*)dbKEYADR( nxtp ) );				// �l�N�X�g�T�C�Y
				if( cursiz+nxtsiz<(int)sizeof(tmpa)-1 ){				// ���v�T�C�YOK
					strcpy( tmpa, CURTXTP );							// �J�����g�e�L�X�g�R�s�[
					strcat( tmpa, (char*)dbKEYADR( nxtp ) );			// ���̍s���W���C���g
					linp=linAlcStr( LINDBP, tmpa );						// �s�o�^
					dbMov( LINDBP, linp, CURP );						// �J�����g�|�C���^�̑O�Ɉړ�
					dbFre( LINDBP, nxtp );								// ���̍s���폜
					dbFre( LINDBP, CURP );								// �J�����g�s�폜
					CURP=linp;											// �J�����g�|�C���^�X�V
					if( !CURYY ){										// �y�[�W�擪�s
						PGP=linp;										// �y�[�W�|�C���^�X�V
					}
					linchg( CURP );										// �s�ύX�Z�b�g
					invalidateAll();									// �S�čĕ`��
				}else{													// �����񂪒�������
					msgSet( "Line Too Long" );							// �����񂪒�������
				}
			}
		}
	}
}

/************************************************************************/
/*	�ꕶ���Z�b�g														*/
/************************************************************************/

void	setchr( int xedk ){

	char		txta[0x10000];											// �e�L�X�g�o�b�t�@
	stLIN		*linp;													// �s�|�C���^
	int			toplno, curlno;											// �擪�s�ԍ�, �J�����g�s�ԍ�
	int			lno;													// �s�ԍ�
	int			curxx=CURXX;											// �J�[�\���w

	toplno=CUTPGYY+CUTCURYY;											// �擪�s�ԍ�
	curlno=PGYY+CURYY;													// �J�����g�s�ԍ�
	if( CUTCURP && !CUTTYP && curlno>toplno ){							// �I�����[�h���ł���
		linp=CUTCURP;													// �s�|�C���^�͑I��擪
		for( lno=toplno; lno<curlno; lno++ ){							// �s�ԍ����[�v
			strcpy( txta, (char*)dbKEYADR( linp ) );					// ��U�e�L�X�g�o�b�t�@�ɃR�s�[
			curxx=lmSetChr( txta, CURXX, xedk, g_tabstp, g_flgins );	// �ꕶ���Z�b�g
			linp=linflush( linp, txta );								// �e�L�X�g�X�V
			linchg( linp );												// �s�ύX�Z�b�g
			linp=(stLIN*)dbNxt( linp );									// �s�|�C���^�X�V
		}
		CURXX=curxx;													// �J�[�\���w�ύX
		linAdjScr();													// ���X�N���[���̐��K��
		invalidateAll();												// �S�čĕ`��
	}else{
		strcpy( txta, CURTXTP );										// ��U�e�L�X�g�o�b�t�@�ɃR�s�[
		CURXX=lmSetChr( txta, CURXX, xedk, g_tabstp, g_flgins );		// �J�[�\���ʒu���K��
		renewtxt( CURP, CURYY, txta );									// �J�����g�e�L�X�g�X�V
		if( linAdjScr() ){												// ���X�N���[���̐��K��
			invalidateAll();											// �S�čĕ`��
		}
	}
}

/************************************************************************/
/*	�J�����g���[�h�𓾂�												*/
/************************************************************************/

void	getcurword( char *dstp ){

	stLM		lm;														// �s�}�b�v
	int			xx;														// �w���W
	int			mapval;													// �s�}�b�v�l
	char		*txtp, *nxtp;											// �e�L�X�g�|�C���^

	*dstp=0;															// �Ƃ肠�����f�B�X�g�l�[�V�����N���A
	lmMak( &lm, CURTXTP, g_tabstp, 0 );									// �s�}�b�v�쐬
	xx=lmPosAdj( &lm, CURXX, 0 );										// �J�[�\���ʒu���K��
	if( xx!=lm.siz ){													// �s�I�[�ł͂Ȃ�
		txtp=CURTXTP+lm.txtofsa[xx];									// �擪�e�L�X�g�|�C���^
		lmMak( &lm, CURTXTP, g_tabstp, 1 );								// ���[�h���[�h�ōs�}�b�v�쐬
		mapval=lm.txtofsa[xx];											// �s�}�b�v�l�𓾂�
		while( lm.txtofsa[xx]==mapval ){								// �s�}�b�v�l�������̊ԃ��[�v
			xx++;														// �J�[�\���ʒu�C���N�������g
		}
		nxtp=CURTXTP+lm.txtofsa[xx];									// ���̃e�L�X�g�|�C���^
		memcpy( dstp, txtp, nxtp-txtp );								// �擪���玟�̃e�L�X�g�|�C���^�܂ł��R�s�[
		dstp[nxtp-txtp]=0;												// �O�^�[�~�l�[�g
	}
}

/************************************************************************/
/*	����������𓾂�													*/
/************************************************************************/

static int	scdbtrget( char *dstp ){

	int		res=0;														// ���U���g
	stDB	*cbdbp;														// �N���b�v�{�[�hDB
	char	*cbp;														// �N���b�v�{�[�h�|�C���^

	*dstp=0;															// �Ƃ肠�����O�^�[�~�l�[�g
	cbdbp=clpbrdGet( CLPBRDP, CB_SEARCH );								// �N���b�v�{�[�h�擾
	if( cbdbp ){														// �N���b�v�{�[�h�擾����
		cbp=(char*)dbTop( cbdbp );										// �擪�e�L�X�g
		if( cbp ){														// �擪�e�L�X�g�����݂���
			strcpy( dstp, cbp );										// �f�B�X�g�l�[�V�����փR�s�[
			res=1;														// ���U���g�Z�b�g
		}
		dbClose( cbdbp );												// �N���b�v�{�[�hDB�N���[�Y
//	}else{																// �N���b�v�{�[�h�擾���s
//		printf( "�N���b�v�{�[�h�G���[\n" );
	}
	return( res );														// 0:���s, 1:����
}

/************************************************************************/
/*	������������N���b�v�{�[�h�ɏ�������								*/
/************************************************************************/

static void	scdbtrput( const char *soup ){

	stDB	*cbdbp;														// �N���b�v�{�[�hDB

	cbdbp=dbOpen( 0, DBSIZ_STR, 0, 0 );									// �N���b�v�{�[�hDB�I�[�v��
	dbAlc( cbdbp, soup );												// ������o�^
	clpbrdPut( CLPBRDP, CB_SEARCH, cbdbp, 0, 0 );						// �N���b�v�{�[�h��������
	dbClose( cbdbp );													// �N���b�v�{�[�hDB�N���[�Y
}

/************************************************************************/
/*	�N���b�v�{�[�h�Ƀe�L�X�g����������									*/
/************************************************************************/

static void	cbtxtput( const stDB *cbdbp, int rctmod ){

	stDB	*schdbp;													// ����DB

	schdbp=clpbrdGet( CLPBRDP, CB_SEARCH );								// ����������𓾂�
	clpbrdPut( CLPBRDP, CB_TXT, cbdbp, rctmod, 1 );						// �N���b�v�{�[�h�v�b�g
	clpbrdPut( CLPBRDP, CB_SEARCH, schdbp, 0, 0 );						// �N���b�v�{�[�h�v�b�g
	dbClose( schdbp );													// ����DB�N���[�Y
#	ifndef	_WIN32
#		ifndef	CURSES
	xselOwner( g_xselp, g_tim );										// �Z���N�V�����I�[�i�[�錾
#		endif
#	endif
}

/************************************************************************/
/*	�V�F��																*/
/************************************************************************/

static void	shell( const char txta[] ){

	stDBLNK			*bakpgp, *bakcurp;									// DB�����N�|�C���^
	stLIN			*linp;												// �s�|�C���^
	char			*outp;												// �J�b�g�|�C���^
	stDB			*indbp, *outdbp;									// ���o��DB
	char			tmpa[4096];											// ���ϐ��ϊ��p�e���|�����o�b�t�@

	cutchk();															// �I�����[�h�őI��擪����̍s�ō�Ƃ�������I�����[�h����������
	envcnv( tmpa, txta );												// ���ϐ��ϊ�
	indbp=clpbrdGet( CLPBRDP, CB_TXT );									// �N���b�v�{�[�h�Q�b�g
	outdbp=filterdb( tmpa, indbp );										// �t�B���^�[
	if( outdbp ){														// �t�B���^�[����
		bakpgp=dbLNKP( PGP )->bakp;										// �y�[�W�擪�̑O�̃����N�|�C���^
		bakcurp=dbLNKP( CURP )->bakp;									// �J�����g�̑O�̃����N�|�C���^
		dbFOR( outdbp, outp, char* ){									// �J�b�g�o�b�t�@������
			linp=linAlcStr( LINDBP, outp );								// �V�s�o�^
			linchg( linp );												// �s�ύX�Z�b�g
			dbMov( LINDBP, linp, CURP );								// �J�����g�|�C���^�̑O�Ɉړ�
		}
		PGP=(stLIN*)(bakpgp->nxtp+1);									// �y�[�W�擪�|�C���^
		CURP=(stLIN*)(bakcurp->nxtp+1);									// �J�����g�|�C���^
		invalidateAll();												// �S�čĕ`��
		dbClose( outdbp );												// �o��DB�N���[�Y
	}else{																// �t�B���^�[�G���[
		msgSet( "FILTERDB ERROR!" );
	}
	dbClose( indbp );													// ����DB�N���[�Y
}

/************************************************************************/
/*	ESC+ESC �R�}���h													*/
/************************************************************************/

void	esc_esc(void){
#	ifdef	CURSES
	clear();
#	endif
	invalidateAll();													// �S�čĕ`��
}

/************************************************************************/
/*	ESC+-	�I�v�V����													*/
/************************************************************************/

void	esc_minus(void){
}

/************************************************************************/
/*	ESC+NUM	�s�ړ�/�^�u�ݒ�/���x���F��									*/
/************************************************************************/

static void	esc_num( int keysym ){

	char	txta[256];													// �e�L�X�g�o�b�t�@
	char	*txtp;														// �e�L�X�g�|�C���^
	int		val;														// �]���l
	int		lno;														// �s�ԍ�

	txta[0]=(char)keysym;												// ��s�ҏW�o�b�t�@
	txta[1]=0;															// ��s�ҏW�o�b�t�@
	if( linedt( txta, "LINE NO or TAB(t),LBL(s),WILD[w],C2L[l],NUM[n],HEX[h]" ) ){
		val=strtol( txta, &txtp, 10 );									// �P�O�i�����񂩂�l�ɕϊ�
		if( txtp>txta ){												// �P�O�i������]������
			switch( *txtp ){
				case 0:													// �s�ԍ�
					lno=val-1;
					if( lno<0 ){										// �����ȍs�ԍ�
						lno=0;											// �s�ԍ��͐擪
					}
					lnoset( lno );										// �s�ԍ��Z�b�g
					break;
				case 't':												// �^�u�ݒ�
					if( val==1 || val==2 || val==4 || val==8 ){			// �^�u��1,2,4,8�̂����ꂩ
						g_tabstp=val;									// �^�u�ݒ�
						invalidateAll();								// �S�čĕ`��
					}else{
						msgSet( "Invalid Tab Step" );
					}
					break;
				case 's':												// ���x���F��
					g_flglbl=val;										// �����A�u�����̃��x���F��
					break;
				case 'n':												// �s�ԍ��T�C�Y
					g_lnosiz=val;										// �s�ԍ��T�C�Y
					dspConfig( g_xw, g_yw );
					invalidateAll();									// �S�čĕ`��
					break;
				case 'l':												// �������̑啶�����������
					g_flgc2l=val;										// �������̑啶����������ʃt���O
					break;
				case 'w':												// ���C���h�J�[�h
					g_flgwld=val;										// �����A�u�����̃��C���h�J�[�h�L���t���O
					break;
				case 'h':												// �s�ԍ��P�U�i�\��
					g_flgh=val;											// �s�ԍ��P�U�i�\��
					invalidateAll();									// �S�čĕ`��
					break;
				default:												// ���̑��̏ꍇ
					msgSet( "Syntax Error" );							// �V���^�b�N�X�G���[
					break;
			}
		}
	}
}

void	esc_0(void){
	esc_num( '0' );
}
void	esc_1(void){
	esc_num( '1' );
}
void	esc_2(void){
	esc_num( '2' );
}
void	esc_3(void){
	esc_num( '3' );
}
void	esc_4(void){
	esc_num( '4' );
}
void	esc_5(void){
	esc_num( '5' );
}
void	esc_6(void){
	esc_num( '6' );
}
void	esc_7(void){
	esc_num( '7' );
}
void	esc_8(void){
	esc_num( '8' );
}
void	esc_9(void){
	esc_num( '9' );
}

/************************************************************************/
/*	ESC+A	�e�L�X�g�̏����؂�ւ�										*/
/************************************************************************/

void	esc_a(void){
	ctxchg( 1 );														// �R���e�L�X�g�ύX
}

/************************************************************************/
/*	ESC+B	�擪�ֈړ�													*/
/************************************************************************/

void	esc_b(void){
	PGP=CURP=(stLIN*)dbTop( LINDBP );									// �擪�s�ɃZ�b�g
	PGYY=CURYY=0;														// �J�[�\�����W�N���A
	invalidateAll();													// �S�čĕ`��
}

/************************************************************************/
/*	ESC+C	�t�B���^�[													*/
/************************************************************************/

void	esc_c(void){

	static char		txta[256]={ 0 };									// �e�L�X�g�o�b�t�@

	if( linedt( txta, "SHELL" ) ){										// ��s�ҏW
		shell( txta );													// �V�F��
	}
}

/************************************************************************/
/*	ESC+D	�t�@�B���̍~���؂�ւ�										*/
/************************************************************************/

void	esc_d(void){
	ctxchg( -1 );														// �R���e�L�X�g�ύX
}

/************************************************************************/
/*	ESC+E	�ҏW����													*/
/************************************************************************/

void	esc_e(void){

	stCTX	*ctxp;														// �R���e�L�X�g�|�C���^
	int		res;														// �t�@�C���Z�[�u�̃��U���g
	int		yn=-1;														// �C�G�X�A�m�[���U���g

	dbFOR( g_ctxdbp, ctxp, stCTX* ){									// �R���e�L�X�g�����胋�[�v
		if( ctxp->chgflg ){												// �ύX����Ă���
			res=ctxSave( ctxp );										// �t�@�C���Z�[�u
			if( !res ){													// �t�@�C���Z�[�u���s
				if( yn<0 ){												// �ŏ��̏ꍇ
					yn=YorN( "File Save Error Exit?(Y/N)" );			// �{�C�ł����H�ƕ���
				}
				if( yn!=YN_YES ){										// �{�C�ł͂Ȃ��ꍇ
					break;												// ���[�v�I��
				}
			}
		}
	}
	g_flgend=( ctxp==0 );												// �I���t���O�̓R���e�L�X�g�����胋�[�v�𖞑������ꍇ
}

/************************************************************************/
/*	ESC+F	�V�����e�L�X�g�̕ҏW										*/
/************************************************************************/

void	esc_f(void){

	static char	fnam[256]={ 0 };										// �t�@�C����
	char		enva[256];												// ���ϐ��ϊ��o�b�t�@
	stDB		*filedbp;												// �t�@�C��DB
	stFILE		*filep;													// �t�@�C���\���̃|�C���^
	stCTX		*ctxp;													// �R���e�L�X�g�|�C���^
	int			first=1;												// �ŏ��t���O

	if( linedt( fnam, "NEW FILE" ) ){									// ��s�ҏW
		envcnv( enva, fnam );											// ���ϐ��ϊ�
		filedbp=filedbAlc( 0, enva, FILEATR_REG|FILEATR_DOT|FILEATR_NON );		// �t�@�C�����o�^
		dbFOR( filedbp, filep, stFILE* ){								// �o�^���ꂽ�t�@�C���������胋�[�v
			ctxp=(stCTX*)dbSch( g_ctxdbp, dbKEYADR( filep ) );			// �t�@�C��������
			if( !ctxp ){												// ������Ȃ�
				ctxp=ctxOpen( (char*)dbKEYADR( filep ) );				// �R���e�L�X�g�I�[�v��
			}
			if( ctxp && first ){										// �R���e�L�X�g�̓I�[�v���ł��Ă��ŏ��̏ꍇ
				first=0;												// �ŏ��t���O�N���A
				g_ctxp=ctxp;											// �J�����g�R���e�L�X�g�|�C���^�Z�b�g
			}
		}
		dbClose( filedbp );												// �t�@�C��DB�N���[�Y
		invalidateAll();												// �S�čĕ`��
	}
}

/************************************************************************/
/*	ESC+H	���e�L�X�g�̃Z�[�u											*/
/************************************************************************/

void	esc_h(void){

	stLIN	*linp;														// �s�|�C���^

	if( FILCHG ){														// �J�����g�t�@�C���͕ύX����Ă���
		if( ctxSave( g_ctxp ) ){										// �J�����g�t�@�C���Z�[�u����
			dbFOR( LINDBP, linp, stLIN* ){								// �s������
				linp->chgflg=0;											// �ύX�t���O�N���A
			}
			FILCHG=0;													// �t�@�C���ύX�t���O�N���A
			invalidateAll();											// �S�čĕ`��
		}else{															// �J�����g�t�@�C���Z�[�u���s
			msgSet( "File Save Error" );								// �t�@�C���Z�[�u�G���[
		}
	}
}

/************************************************************************/
/*	ESC+I	�^�u�����̕\���E��\���؂�ւ�								*/
/************************************************************************/

void	esc_i(void){
	g_tabdsp=!g_tabdsp;													// �^�u�����\���t���O���]
	invalidateAll();													// �S�čĕ`��
}

/************************************************************************/
/*	ESC+J	�O���m�F�u��												*/
/************************************************************************/

void	esc_j(void){

	char	reporg[256]={ 0 }, repnew[256]={ 0 };						// �u��������
	int		yn;															// �C�G�X�m�[
	int		chkflg;														// �u���m�F�t���O

	if( linedt2( reporg, "REPLACE FORWORD", repnew, "NEW STRINGS" ) ){
		yn=YorN( "KAKUNIN ? (Y/N)" );									// �m�F���邩�H
		if( yn!=YN_CANCEL ){											// �L�����Z���łȂ��ꍇ
			chkflg=( yn==YN_YES );										// �C�G�X�̏ꍇ�u���m�F�t���O�Z�b�g
			replace( reporg, repnew, 0, chkflg, !chkflg );				// �O���m�F�u��
		}
	}
}

/************************************************************************/
/*	ESC+K	���e�L�X�g�̔j��											*/
/************************************************************************/

void	esc_k(void){

	int		doflg=0;													// ���s�t���O
	stCTX	*nxtp;														// �R���e�L�X�g�|�C���^

	if( FILCHG ){														// �J�����g�t�@�C���͕ύX����Ă���
		if( YorN( "Are You Sure(Y/N)" )==YN_YES ){						// �{�C�ł����H�ŃC�G�X�̏ꍇ
			doflg=1;													// ���s�t���O�𗧂Ă�
		}
	}else{																// �ύX����Ă��Ȃ�
		doflg=1;														// ���s�t���O�𗧂Ă�
	}
	if( doflg ){														// ���s�t���O�������Ă���
		nxtp=(stCTX*)dbNxt( g_ctxp );									// ���̃R���e�L�X�g�|�C���^���Ƃ��Ă���
		ctxClose( g_ctxp );												// �J�����g�R���e�L�X�g�N���[�Y
		if( !nxtp ){													// ���̃R���e�L�X�g�|�C���^�͖���
			nxtp=(stCTX*)dbTop( g_ctxdbp );								// ���̃R���e�L�X�g�|�C���^�͐擪
		}
		g_ctxp=nxtp;													// �J�����g�R���e�L�X�g�|�C���^�X�V
		if( nxtp ){														// �܂��ҏW���̃t�@�C���͑��݂���
			invalidateAll();											// �S�čĕ`��
		}else{															// �ҏW���̃t�@�C���͑��݂��Ȃ�
			g_flgend=1;													// �I���t���O�𗧂Ă�
		}
	}
}

/************************************************************************/
/*	ESC+L	����m�F�u��												*/
/************************************************************************/

void	esc_l(void){
	if( linedt2( g_reporg, "REPLACE BACK", g_repnew, "NEW STRINGS" ) ){
		replace( g_reporg, g_repnew, 1, 1, 0 );							// ����m�F�u��
	}
}

/************************************************************************/
/*	ESC+M	���s�����̕\���^��\��										*/
/************************************************************************/

void	esc_m(void){
	g_flgcr=!g_flgcr;													// ���s�\���t���O���]
	invalidateAll();													// �S�čĕ`��
}

/************************************************************************/
/*	ESC+N	�O������													*/
/************************************************************************/

void	esc_n(void){

	char	scdbtr[256];												// ����������

	scdbtrget( scdbtr );												// ����������𓾂�
	if( linedt( scdbtr, "SEARCH2 FORWORD" ) ){							// ��s�ҏW
		if( scdbtr[0] ){												// �L���ȕ�����
			scdbtrput( scdbtr );										// ������������N���b�v�{�[�h�ɏ�������
			replace( scdbtr, 0, 0, 0, 0 );								// �����܂��͒u��
		}
	}
}

/************************************************************************/
/*	ESC+O	���e�L�X�g�̍ă��[�h										*/
/************************************************************************/

void	esc_o(void){

	stCTX	*orgp;														// �R���e�L�X�g�|�C���^

	if( FILCHG ){														// �J�����g�t�@�C���͕ύX����Ă���
		if( YorN( "Are You Sure(Y/N)" )==YN_YES ){						// �{�C�ł����H�ŃC�G�X�̏ꍇ
			orgp=g_ctxp;												// �J�����g�R���e�L�X�g�|�C���^�o�b�N�A�b�v
			g_ctxp=ctxOpen( FNAMP );									// �R���e�L�X�g�I�[�v��
			dbMov( g_ctxdbp, g_ctxp, orgp );							// �J�����g�Ɉړ�
			ctxClose( orgp );											// �����e�L�X�g�j��
			invalidateAll();											// �S�čĕ`��
		}
	}
}

/************************************************************************/
/*	ESC+Q	�S�e�L�X�g��j�����I��										*/
/************************************************************************/

void	esc_q(void){

	int		chgflg=0;													// �ύX�t���O�_���a
	int		flgend=1;													// �I���t���O
	stCTX	*ctxp;														// �R���e�L�X�g�|�C���^

	dbFOR( g_ctxdbp, ctxp, stCTX* ){									// �R���e�L�X�g�����胋�[�v
		chgflg|=ctxp->chgflg;											// �ύX�t���O�_���a���Ƃ�
	}
	if( chgflg ){														// �ύX���ꂽ���̂�����
		if( YorN( "Are You Sure(Y/N)" )!=YN_YES ){						// �{�C�ł����H�ŃC�G�X�ł͂Ȃ��ꍇ
			flgend=0;													// �I���t���O�N���A
		}
	}
	g_flgend=flgend;													// �I���t���O�Z�b�g
}

/************************************************************************/
/*	ESC+R	�O���u��													*/
/************************************************************************/

void	esc_r(void){
	if( linedt2( g_reporg, "REPLACE FORWORD", g_repnew, "NEW STRINGS" ) ){
		replace( g_reporg, g_repnew, 0, 0, 0 );							// �O���u��
	}
}

/************************************************************************/
/*	ESC+S	�������													*/
/************************************************************************/

void	esc_s(void){

	char	scdbtr[256];												// ����������

	scdbtrget( scdbtr );												// ����������𓾂�
	if( linedt( scdbtr, "SEARCH2 BACK" ) ){								// ��s�ҏW
		if( scdbtr[0] ){												// �L���ȕ�����
			scdbtrput( scdbtr );										// ������������N���b�v�{�[�h�ɏ�������
			replace( scdbtr, 0, 1, 0, 0 );								// �����܂��͒u��
		}
	}
}

/************************************************************************/
/*	ESC+T	�t�@�C�����ύX												*/
/************************************************************************/

void	esc_t(void){

	char	fnam[256];													// �t�@�C����
	stCTX	*ctxp;														// �R���e�L�X�g�|�C���^

	strcpy( fnam, FNAMP );												// �I���W�i���t�@�C�����Z�b�g
	if( linedt( fnam, "NEW NAME" ) ){									// ��s�ҏW
		ctxp=(stCTX*)dbSch( g_ctxdbp, fnam );							// ����
		if( !ctxp ){													// ��v������̂͂Ȃ�
			g_ctxp=ctxRename( g_ctxp, fnam );							// �R���e�L�X�g���l�[��
			invalidateFil();											// �t�@�C�����ĕ\��
		}else{															// ��v������̂�������
			msgSet( "Same File Exist!" );								// �����t�@�C��������
		}
	}
}

/************************************************************************/
/*	ESC+U	����u��													*/
/************************************************************************/

void	esc_u(void){
	if( linedt2( g_reporg, "REPLACE BACK", g_repnew, "NEW STRINGS" ) ){
		replace( g_reporg, g_repnew, 1, 0, 0 );							// ����u��
	}
}

/************************************************************************/
/*	CYGWIN �h���C�u�\�L���� DOS �\�L�ɕύX����							*/
/************************************************************************/

#	ifdef	_WIN32

static char	*cyg2dos( char *dstadr, const char *txtadr ){

	const char	*txtp;
	char		*dstp;
	char		chr;													// �e�L�X�g����ǂݍ��񂾕���
	char		drvchr;													// �h���C�u�L�����N�^

	txtp=txtadr;
	if( *txtp++=='/' ){													// �A�u�\�����[�g�p�X
		dstp=dstadr;													// �f�B�X�g�l�[�V�����o�b�t�@���e���|�����Ƃ��Ďg�p����
		while( ( chr=*txtp++ )!=0 ){
			*dstp++=chr;
			if( chr=='/' ){
				break;
			}
		}
		*dstp=0;
		if( strcmp( dstadr, "cygdrive/" ) ){							// CYGWIN ���[�g
			drvchr='c';
			dstp=dstadr+sprintf( dstadr, "%c:\\cygwin", drvchr );
			txtp=txtadr;
		}else{															// cygdrive/
			drvchr=*txtp++;
			dstp=dstadr+sprintf( dstadr, "%c:", drvchr );
		}
	}else{																// ���΃p�X
		txtp=txtadr;
		dstp=dstadr;
	}
	while( ( chr=*txtp++ )!=0 ){
//		if( chr=='/' ){
//			chr='\\';
//		}
		*dstp++=chr;
	}
	*dstp=0;
	return( dstp );
}

#	endif

/************************************************************************/
/*	ESC+V	�^�O�W�����v												*/
/************************************************************************/

void	esc_v(void){

	char	tmpa[256];													// �t�@�C����
	char	fnam[256];													// �t�@�C����
	int		lno=1;														// �s�ԍ�
	stCTX	*ctxp;														// �R���e�L�X�g�|�C���^

	if( tagparse( &lno, tmpa, CURTXTP, FNAMP ) ){						// �^�O�����
#	ifdef	_WIN32
		cyg2dos( fnam, tmpa );											// CYGWIN �h���C�u�\�L���� DOS �\�L�ɕύX����
#	else
		strcpy( fnam, tmpa );
#	endif
		if( lno<=0 ){													// �s�ԍ��͖���
			lno=0;														// �擪�Z�b�g
		}else{															// �s�ԍ��͗L��
			lno--;														// �����`���͂O���擪�̂��߃f�N�������g
		}
		ctxp=(stCTX*)dbSch( g_ctxdbp, fnam );							// �t�@�C��������
		if( !ctxp ){													// ������Ȃ�
			ctxp=ctxOpen( fnam );										// �R���e�L�X�g�I�[�v��
		}
		if( ctxp ){														// �R���e�L�X�g�|�C���^�͗L��
			g_ctxp=ctxp;												// �O���[�o���R���e�L�X�g�|�C���^�Z�b�g
			lnoset( lno );												// �s�ԍ��Z�b�g
			invalidateAll();											// �S�čĕ`��
		}else{															// �t�@�C���I�[�v���G���[
			msgSet( "Can't Open File" );
		}
	}else{																// �^�O�t�H�[�}�b�g�G���[
		msgSet( "TAG Format Error" );
	}
}

/************************************************************************/
/*	ESC+X	���e�L�X�g�̊���											*/
/************************************************************************/

void	esc_x(void){

	stCTX	*nxtp;														// �R���e�L�X�g�|�C���^
	int		res=1;														// ���U���g

	if( FILCHG ){														// �J�����g�t�@�C���͕ύX����Ă���
		res=ctxSave( g_ctxp );											// �t�@�C���Z�[�u
		if( !res ){														// �t�@�C���Z�[�u���s
			msgSet( "File Save Error" );
		}
	}
	if( res ){															// �t�@�C���Z�[�u�������ύX����Ă��Ȃ�����
		nxtp=(stCTX*)dbNxt( g_ctxp );									// ���̃R���e�L�X�g�|�C���^
		ctxClose( g_ctxp );												// �J�����g�R���e�L�X�g�N���[�Y
		if( !nxtp ){													// ���̃R���e�L�X�g�|�C���^�͖���
			nxtp=(stCTX*)dbTop( g_ctxdbp );								// �擪�R���e�L�X�g�|�C���^
		}
		g_ctxp=nxtp;													// �J�����g�R���e�L�X�g�|�C���^�Z�b�g
		if( nxtp ){														// �܂��ҏW���̃t�@�C���͑��݂���
			invalidateAll();											// �S�čĕ`��
		}else{															// �ҏW���̃t�@�C���͑��݂��Ȃ�
			g_flgend=1;													// �I���t���O�𗧂Ă�
		}
	}
}

/************************************************************************/
/*	ESC+Y	�t�@�B���̓ǂݍ���											*/
/************************************************************************/

void	esc_y(void){

	stDBLNK		*bakpgp, *bakcurp;										// DB�����N�|�C���^
	char		fnam[256]={ 0 };										// �t�@�C����
	char		enva[256];												// ���ϐ��ϊ��o�b�t�@
	stDB		*filedbp;												// �t�@�C��DB
	stFILE		*filep;													// �t�@�C���\���̃|�C���^
	char		*fnamp;													// �t�@�C�����|�C���^
	FILE		*fp;													// �t�@�C���|�C���^
	stLIN		*linp;													// �s�|�C���^
	char		txta[0x10000], *txtp;									// �e�L�X�g�o�b�t�@�A�|�C���^

	if( linedt( fnam, "INSERT FILE" ) ){								// ��s�ҏW
		envcnv( enva, fnam );											// ���ϐ��ϊ�
		bakpgp=dbLNKP( PGP )->bakp;										// �y�[�W�擪�̑O�̃����N�|�C���^
		bakcurp=dbLNKP( CURP )->bakp;									// �J�����g�̑O�̃����N�|�C���^
		filedbp=filedbAlc( 0, enva, FILEATR_REG|FILEATR_DOT );			// �t�@�C���o�^
		dbFOR( filedbp, filep, stFILE* ){								// �o�^���ꂽ�t�@�C���������胋�[�v
			fnamp=(char*)dbKEYADR( filep );								// �t�@�C�����|�C���^
			fp=fopen( fnamp, "r" );										// �t�@�C���I�[�v��
			if( fp ){													// �t�@�C���I�[�v������
				while( !feof( fp ) ){									// �t�@�C���I�[�ɒB���Ȃ��ԃ��[�v
					if( !fgets( txta, sizeof(txta), fp ) ){				// �o�b�t�@�͖���
						continue;
					}
					txtp=strpbrk( txta, "\r\n\x1a" );					// ���s�R�[�h����
					if( txtp ){											// ��������
						*txtp=0;										// ���s�R�[�h�N���A
					}
					linp=linAlcStr( LINDBP, txta );
					linchg( linp );										// �s�ύX�Z�b�g
					dbMov( LINDBP, linp, CURP );						// �J�[�\���|�C���^�̑O�ɑ}��
				}
				fclose( fp );											// �t�@�C���N���[�Y
			}
		}
		dbClose( filedbp );												// �t�@�C��DB�N���[�Y
		PGP=(stLIN*)(bakpgp->nxtp+1);									// �y�[�W�擪�|�C���^
		CURP=(stLIN*)(bakcurp->nxtp+1);									// �J�����g�|�C���^
		invalidateAll();												// �S�čĕ`��
	}
}

/************************************************************************/
/*	ESC+Z	�����ֈړ�													*/
/************************************************************************/

void	esc_z(void){

	int		seekn;														// ���ۂɃV�[�N������
	stLIN	*endp;														// �I�[�|�C���^
	stLIN	*pgp;														// �y�[�W�|�C���^

	endp=(stLIN*)dbEnd( LINDBP );										// �I�[�|�C���^�𓾂�
	pgp=(stLIN*)dbSeek( &seekn, endp, 1-g_tyyd );						// �I�[�|�C���^����\���s������ɃV�[�N���Ă݂�
	if( seekn==1-g_tyyd ){												// �V�[�N�ł���
		PGP=pgp;														// �y�[�W�|�C���^�Z�b�g
		PGYY=dbAlcN( LINDBP )-1+seekn;									// �y�[�W�x�Z�b�g
		CURYY=-seekn;													// �J�[�\���x�Z�b�g
	}else{																// �V�[�N�ł��Ȃ�
		PGP=(stLIN*)dbTop( LINDBP );									// �y�[�W�|�C���^�͐擪
		PGYY=0;															// �y�[�W�x�Z�b�g
		CURYY=dbAlcN( LINDBP )-1;										// �J�[�\���x�͏I�[
	}
	CURP=endp;															// �J�����g�|�C���^�Z�b�g
	invalidateAll();													// �S�čĕ`��
}

/************************************************************************/
/*	ESC+[	�J�����g���[�h�O���u��										*/
/************************************************************************/

void	esc_lbo(void){
	getcurword( g_reporg );												// �J�����g���[�h�𓾂�
	if( linedt2( g_reporg, "REPLACE FORWORD", g_repnew, "NEW STRINGS" ) ){
		replace( g_reporg, g_repnew, 0, 0, 1 );							// �J�����g���[�h�O���u��
	}
}

/************************************************************************/
/*	ESC+\	�J�����g���[�h����u��										*/
/************************************************************************/

void	esc_yen(void){
	getcurword( g_reporg );												// �J�����g���[�h�𓾂�
	if( linedt2( g_reporg, "REPLACE BACK", g_repnew, "NEW STRINGS" ) ){
		replace( g_reporg, g_repnew, 1, 0, 0 );							// �J�����g���[�h����u��
	}
}

/************************************************************************/
/*	ESC+]	�啶���E�������ϊ��ؑ�										*/
/************************************************************************/

void	esc_lbc(void){
	g_flgl2c=!g_flgl2c;													// ����������啶���ϊ��t���O
	invalidateMod();													// �S�čĕ`��
}

/************************************************************************/
/*	ESC+^	�O���u��(�\���Ȃ�)											*/
/************************************************************************/

void	esc_hat(void){
	if( linedt2( g_reporg, "REPLACE FORWORD NO-DISP", g_repnew, "NEW STRINGS" ) ){
		replace( g_reporg, g_repnew, 0, 0, 1 );							// �O���u��(�\���Ȃ�)
	}
}

/************************************************************************/
/*	ESC+_	����u��(�\���Ȃ�)											*/
/************************************************************************/

void	esc__(void){
	if( linedt2( g_reporg, "REPLACE BACK NO-DISP", g_repnew, "NEW STRINGS" ) ){
		replace( g_reporg, g_repnew, 1, 0, 1 );							// ����u��(�\���Ȃ�)
	}
}

/************************************************************************/
/*	CTRL+[	ESC �R�}���h												*/
/************************************************************************/

void	esc(void){

	int		keysym;
	int		curbak;

	msgSet( "[ESC]" );													// ���b�Z�[�W�\��
	curbak=curSw( 1 );													// �J�[�\���I��
	keysym=getKey()&0xffff;												// �L�[����
	curSw( curbak );													// �J�[�\�����̏�Ԃ�
//	if( keysym>='a' && keysym<='z' ){									// �������̏ꍇ
//		keysym&=~0x20;													// ����������啶���ϊ�
//	}
	msgSet( "" );														// ���b�Z�[�W�N���A
	switch( keysym ){
		case '-' :														// �I�v�V����
			esc_minus();
			break;
		case '0' :
		case '1' :
		case '2' :
		case '3' :
		case '4' :
		case '5' :
		case '6' :
		case '7' :
		case '8' :
		case '9' :														// ESC+ 0-9
			esc_num( keysym );											// �s�ԍ��Z�b�g
			break;
		case '@' :														// �L�[�{�[�h�}�N���̒�`
			f11();
			break;
		case 'a' :														// �e�L�X�g�̏����؂�ւ�
			esc_a();
			break;
		case 'b' :														// �擪�ֈړ�
			esc_b();
			break;
		case 'c' :														// �q�v���Z�X�̎��s
			esc_c();
			break;
		case 'd' :														// �t�@�B���̍~���؂�ւ�
			esc_d();
			break;
		case 'e' :														// �ҏW����
			esc_e();
			break;
		case 'f' :														// �V�����e�L�X�g�̕ҏW
			esc_f();
			break;
		case 'h' :														// ���e�L�X�g�̃Z�[�u
			esc_h();
			break;
		case 'i' :														// �^�u�����̕\���؂�ւ�
			g_tabdsp=!g_tabdsp;											// �^�u�����\���t���O���]
			invalidateAll();											// �S�čĕ`��
			break;
		case 'j' :														// �O���m�F�u��
			esc_j();
			break;
		case 'k' :														// ���e�L�X�g�̔j��
			esc_k();
			break;
		case 'l' :														// ����m�F�u��
			esc_l();
			break;
		case 'm' :														// ���s�����̕\���^��\��
			g_flgcr=!g_flgcr;											// ���s�\���t���O���]
			invalidateAll();											// �S�čĕ`��
			break;
		case 'n' :														// �O������
			esc_n();
			break;
		case 'o' :														// ���e�L�X�g�̍ă��[�h
			esc_o();													// ESC+O	���e�L�X�g�̍ă��[�h
			break;
		case 'q' :														// �S�e�L�X�g��j�����I��
			esc_q();													// ESC+Q	�S�e�L�X�g��j�����I��
			break;
		case 'r' :														// �O���u��
			esc_r();
			break;
		case 's' :														// �������
			esc_s();
			break;
		case 't' :														// �t�@�C�����ύX
			esc_t();
			break;
		case 'u' :														// ����u��
			esc_u();
			break;
		case 'v' :														// �^�O�W�����v
			esc_v();
			break;
		case 'w' :														// �q�v���Z�X�̎��s
			esc_c();
			break;
		case 'x' :														// ���e�L�X�g�̊���
			esc_x();
			break;
		case 'y' :														// �t�@�B���̓ǂݍ���
			esc_y();
			break;
		case 'z' :														// �����ֈړ�
			esc_z();
			break;
		case '[' :														// �J�����g���[�h�O���u��
			esc_lbo();
			break;
		case '\\':														// �J�����g���[�h����u��
			esc_yen();
			break;
		case ']' :														// �啶���E�������ϊ��ؑ�
			g_flgl2c=!g_flgl2c;											// ����������啶���ϊ��t���O
			invalidateMod();											// �S�čĕ`��
			break;
		case '^' :														// �O���u��(�\���Ȃ�)
			esc_hat();
			break;
		case '_' :														// ����u��(�\���Ȃ�)
			esc__();
			break;
		default:
#	ifdef	CURSES
			clear();
#	endif
			invalidateAll();											// �S�čĕ`��
			break;
	}
}

/************************************************************************/
/*	CTRL+A	�P�����Ɉړ�												*/
/************************************************************************/

void	ctrl_a(void){
	curleft( 1 );														// �J�[�\����
}

/************************************************************************/
/*	CTRL+B	���s�[�Ɉړ�												*/
/************************************************************************/

void	ctrl_b(void){
	if( CURXX ){														// �J�[�\���͍��[�ł͂Ȃ�
		curtop();														// ���[�Ɉړ�
	}else{																// �J�[�\���͍��[
		curend();														// �E�[�Ɉړ�
	}
}

/************************************************************************/
/*	CTRL+C	��ʂ����[���A�b�v											*/
/************************************************************************/

void	ctrl_c(void){
	pgscroll( g_tyyd );													// �X�N���[��
}

/************************************************************************/
/*	CTRL+D	�P�����O���Ɉړ�											*/
/************************************************************************/

void	ctrl_d(void){
	curright( 0 );														// �J�[�\����
}

/************************************************************************/
/*	CTRL+E	�P�s��Ɉړ�												*/
/************************************************************************/

void	ctrl_e(void){
	curup();															// �J�[�\����
}

/************************************************************************/
/*	CTRL+F	�P��O���Ɉړ�												*/
/************************************************************************/

void	ctrl_f(void){
	curright( 1 );														// �J�[�\����
}

/************************************************************************/
/*	CTRL+G	�P�����폜													*/
/************************************************************************/

void	ctrl_g(void){
	del( 0 );															// �폜
}

/************************************************************************/
/*	CTRL+H	�o�b�N�X�y�[�X												*/
/************************************************************************/

void	ctrl_h(void){
	if( curleft( 0 ) ){													// �J�[�\����
		del( 0 );														// �폜
	}
}

/************************************************************************/
/*	CTRL+J	�w���v��ʂ̕\��											*/
/************************************************************************/

void	ctrl_j(void){
	help();																// �w���v����
}

/************************************************************************/
/*	CTRL+K	�s���܂ō폜												*/
/************************************************************************/

void	ctrl_k(void){

	stLM		lm;														// �s�}�b�v
	char		txta[0x10000];											// �e�L�X�g�o�b�t�@
	int			txtsiz;													// �e�L�X�g�T�C�Y

	lmMak( &lm, CURTXTP, g_tabstp, 0 );									// �s�}�b�v�쐬
	CURXX=lmPosAdj( &lm, CURXX, 0 );									// �J�[�\���ʒu���K��
	txtsiz=lm.txtofsa[CURXX];											// �V�����e�L�X�g�T�C�Y
	memcpy( txta, CURTXTP, txtsiz );									// �e�L�X�g�o�b�t�@�ɐV�����T�C�Y�ŃR�s�[
	txta[txtsiz]=0;														// �O�^�[�~�l�[�g
	renewtxt( CURP, CURYY, txta );										// �J�����g�e�L�X�g�X�V
}

/************************************************************************/
/*	CTRL+L	�폜�o�b�t�@����}��										*/
/************************************************************************/

void	ctrl_l(void){
	inslin( g_cutlin );													// �s�}��
}

/************************************************************************/
/*	CTRL+M	���s(�s����)												*/
/************************************************************************/

void	ctrl_m(void){
	cr();																// ���s
}

/************************************************************************/
/*	CTRL+N	�P�s�}��													*/
/************************************************************************/

void	ctrl_n(void){
	inslin( "" );														// ��s�}��
}

/************************************************************************/
/*	CTRL+O	�}�����[�h�؂�ւ�											*/
/************************************************************************/

void	ctrl_o(void){
	g_flgins=!g_flgins;													// �}�����[�h�t���O�_�����]
	invalidateAll();													// �S�čĕ`��
}

/************************************************************************/
/*	CTRL+P	�E�[�Ɉړ�													*/
/************************************************************************/

void	ctrl_p(void){
	curend();															// �E�[�Ɉړ�
}

/************************************************************************/
/*	CTRL+Q	���[�Ɉړ�													*/
/************************************************************************/

void	ctrl_q(void){
	curtop();															// ���[�Ɉړ�
}

/************************************************************************/
/*	CTRL+R	��ʂ����[���_�E��											*/
/************************************************************************/

void	ctrl_r(void){
	pgscroll( -g_tyyd );												// �X�N���[��
}

/************************************************************************/
/*	CTRL+S	�P��������Ɉړ�											*/
/************************************************************************/

void	ctrl_s(void){
	curleft( 0 );														// �J�[�\����
}

/************************************************************************/
/*	CTRL+T	�P��폜													*/
/************************************************************************/

void	ctrl_t(void){
	del( 1 );															// �폜
}

/************************************************************************/
/*	CTRL+U	�s������폜												*/
/************************************************************************/

void	ctrl_u(void){

	stLM		lm;														// �s�}�b�v

	lmMak( &lm, CURTXTP, g_tabstp, 0 );									// �s�}�b�v�쐬
	CURXX=lmPosAdj( &lm, CURXX, 0 );									// �J�[�\���ʒu���K��
	renewtxt( CURP, CURYY, CURTXTP+lm.txtofsa[CURXX] );					// �J�����g�e�L�X�g�X�V
	CURXX=0;															// �J�[�\�������[�Ɉړ�
}

/************************************************************************/
/*	CTRL+V	���ڃR�[�h����												*/
/************************************************************************/

void	ctrl_v(void){

	int		keysym;
	int		curbak;

	msgSet( "CODE ?" );													// ���b�Z�[�W�\��
	curbak=curSw( 1 );													// �J�[�\���I��
	keysym=getKey()&0xffff;												// �L�[����
	curSw( curbak );													// �J�[�\�����̏�Ԃ�
	setchr( keysym );													// �ꕶ���Z�b�g
}

/************************************************************************/
/*	CTRL+W	��ʂ��P�s���[���_�E��										*/
/************************************************************************/

void	ctrl_w(void){
	linscroll( -1 );													// �X�N���[��
}

/************************************************************************/
/*	CTRL+X	�P�s���Ɉړ�												*/
/************************************************************************/

void	ctrl_x(void){
	curdown();															// �J�[�\����
}

/************************************************************************/
/*	CTRL+Y	�P�s�폜													*/
/************************************************************************/

void	ctrl_y(void){

	stLIN		*linp;													// �s�|�C���^

	linp=(stLIN*)dbNxt( CURP );											// ���̍s
	if( linp ){															// �ŏI�s�ł͂Ȃ�
		strcpy( g_cutlin, CURTXTP );									// �J�b�g�o�b�t�@�ɃR�s�[
		if( PGP==CURP ){												// �y�[�W�擪
			PGP=linp;													// �y�[�W�|�C���^�X�V
		}
		if( CUTCURP==CURP ){
			CUTCURP=0;
		}
		dbFre( LINDBP, CURP );											// �J�����g�s�j��
		CURP=linp;														// �J�����g�s�X�V
		FILCHG=1;														// �t�@�C���ύX�Z�b�g
		invalidateAll();												// �S�čĕ`��
	}
}

/************************************************************************/
/*	CTRL+Z	�P�s���[���A�b�v											*/
/************************************************************************/

void	ctrl_z(void){
	linscroll( 1 );														// �X�N���[��
}

/************************************************************************/
/*	CTRL+\	�J�����g���[�h�������										*/
/************************************************************************/

void	ctrl_yen(void){

	char	scdbtr[256];												// ����������

	getcurword( scdbtr );												// �J�����g���[�h�𓾂�
	if( linedt( scdbtr, "SEARCH2 BACK" ) ){								// ��s�ҏW
		if( scdbtr[0] ){												// �L���ȕ�����
			scdbtrput( scdbtr );										// ������������N���b�v�{�[�h�ɏ�������
			replace( scdbtr, 0, 1, 0, 0 );								// �����܂��͒u��
		}
	}
}

/************************************************************************/
/*	CTRL+]	�啶���E�������ϊ�											*/
/************************************************************************/

void	ctrl_lbktc(void){

	stLM		lm;														// �s�}�b�v
	char		*txtp;													// �e�L�X�g�|�C���^
	int			chr;													// ����

	lmMak( &lm, CURTXTP, g_tabstp, 0 );									// �s�}�b�v�쐬
	CURXX=lmPosAdj( &lm, CURXX, 0 );									// �J�[�\���w���K��
	txtp=CURTXTP+lm.txtofsa[CURXX];										// ���݂̃e�L�X�g�|�C���^
	chr=*txtp;															// ���݂̃e�L�X�g�|�C���^����ꕶ���𓾂�
	if( !g_flgl2c ){													// �啶�����珬�����ϊ�
		if( chr>='A' && chr<='Z' ){										// �啶��
			chr |= 0x20;												// ��������
		}
	}else{																// ����������啶���ϊ�
		if( chr>='a' && chr<='z' ){										// ������
			chr &= 0xdf;												// �啶����
		}
	}
	if( chr!=*txtp ){													// �������ύX���ꂽ
		*txtp=(char)chr;												// �����Z�b�g
		linchg( CURP );													// �s�ύX�Z�b�g
	}
	curright( 0 );														// �J�[�\����
	if( linAdjScr() ){													// ���X�N���[���̐��K��
		invalidateAll();												// �S�čĕ`��
	}
}

/************************************************************************/
/*	CTRL+^	�J�����g���[�h�O������										*/
/************************************************************************/

void	ctrl_hat(void){

	char	scdbtr[256];												// ����������

	getcurword( scdbtr );												// �J�����g���[�h�𓾂�
	if( linedt( scdbtr, "SEARCH2 BACK" ) ){								// ��s�ҏW
		if( scdbtr[0] ){												// �L���ȕ�����
			scdbtrput( scdbtr );										// ������������N���b�v�{�[�h�ɏ�������
			replace( scdbtr, 0, 0, 0, 0 );								// �����܂��͒u��
		}
	}
}

/************************************************************************/
/*	CTRL+_	�t�@�C���I�[�L��											*/
/************************************************************************/

void	ctrl__(void){
	g_flgeof=!g_flgeof;													// [EOF]�\���t���O
	invalidateAll();													// �S�čĕ`��
}

/************************************************************************/
/*	F5		�p���O������												*/
/************************************************************************/

void	f5(void){

	char	scdbtr[256];												// ����������

	if( scdbtrget( scdbtr ) ){											// ����������𓾂�
		replace( scdbtr, 0, 0, 0, 0 );									// �����܂��͒u��
	}
}

/************************************************************************/
/*	�͈͎w��J�n														*/
/************************************************************************/

static void	cutstart( int cuttyp ){
	if( !CUTCURP ){														// �͈͎w�蒆�ł͂Ȃ�
		CUTTYP=cuttyp;													// �͈͎w��^�C�v
		CUTPGP=PGP;														// �͈͎w��J�n�y�[�W�|�C���^
		CUTCURP=CURP;													// �͈͎w��J�n�J�[�\���|�C���^
		CUTPGYY=PGXX;													// �͈͎w��J�n�y�[�W�w
		CUTPGYY=PGYY;													// �͈͎w��J�n�y�[�W�x
		CUTCURXX=CURXX;													// �͈͎w��J�n�J�[�\���w
		CUTCURYY=CURYY;													// �͈͎w��J�n�J�[�\���x
	}else{																// �͈͎w�蒆
		CUTCURP=0;														// �͈͎w��I��
		invalidateAll();												// �S�čĕ`��
	}
}

/************************************************************************/
/*	�s���[�h�J�b�g�o�b�t�@�ɃR�s�[										*/
/************************************************************************/

static void	cutcpylin( int delflg ){

	stDBLNK		*bakpgp, *bakcurp;										// DB�����N�|�C���^
	stLIN		*linp, *nxtp;											// �s�|�C���^
	int			toplno, curlno;											// �擪�s�ԍ�, �J�����g�s�ԍ�
	int			lno;													// �s�ԍ�
	stDB		*cbdbp;													// �N���b�v�{�[�hDB

	if( CUTCURP ){														// �I�����[�h���ł���
		toplno=CUTPGYY+CUTCURYY;										// �擪�s�ԍ�
		curlno=PGYY+CURYY;												// �J�����g�s�ԍ�
		if( curlno>toplno ){											// �J�b�g�擪������ɂ���
			bakpgp=dbLNKP( CUTPGP )->bakp;								// �y�[�W�擪�̑O�̃����N�|�C���^
			bakcurp=dbLNKP( CUTCURP )->bakp;							// �J�����g�̑O�̃����N�|�C���^
			cbdbp=dbOpen( 0, DBSIZ_STR, 0, 0x4000 );					// �N���b�v�{�[�hDB�I�[�v��
			linp=CUTCURP;												// �I��擪�s
			for( lno=toplno; lno<curlno && linp; lno++ ){				// �s�ԍ����[�v
				dbAlc( cbdbp, dbKEYADR( linp ) );						// �o�^
				nxtp=(stLIN*)dbNxt( linp );								// ���̍s
				if( delflg ){											// �폜
					dbFre( LINDBP, linp );								// �s�J��
				}
				linp=nxtp;												// �s�|�C���^�X�V
			}
			if( delflg ){												// �폜
				PGP=(stLIN*)(bakpgp->nxtp+1);							// �y�[�W�擪�|�C���^
				CURP=(stLIN*)(bakcurp->nxtp+1);							// �J�����g�|�C���^
				PGYY=CUTPGYY;											// �y�[�W�s�ԍ�
				CURYY=CUTCURYY;											// �J�[�\���x���W
				FILCHG=1;												// �t�@�C���ύX�Z�b�g
			}
			CUTCURP=0;													// �I�����[�h����
			cbtxtput( cbdbp, 0 );										// �N���b�v�{�[�h�Ƀe�L�X�g����������
			dbClose( cbdbp );											// �N���b�v�{�[�hDB�N���[�Y
			invalidateAll();											// �S�čĕ`��
		}
	}
}

/************************************************************************/
/*	�e�L�X�g�͈͍폜													*/
/************************************************************************/

static void	txtcut( char *cutp, char *jntp, const char *txtp, int xx1, int xx2 ){

	stLM		lm;														// �s�}�b�v
	int			axx1, axx2;												// ���K�����ꂽ�J�[�\���ʒu
	int			o1, o2, o3;												// �I�t�Z�b�g

	lmMak( &lm, txtp, g_tabstp, 0 );									// �s�}�b�v�쐬
	axx1=lmPosAdj( &lm, xx1, 0 );										// �J�[�\���ʒu���K��
	axx2=lmPosAdj( &lm, xx2, 0 );										// �J�[�\���ʒu���K��
	o1=lm.txtofsa[axx1];												// �I�t�Z�b�g�P
	o2=lm.txtofsa[axx2];												// �I�t�Z�b�g�Q
	o3=strlen( txtp );													// �I�t�Z�b�g�R
	memcpy( cutp, txtp+o1, o2-o1 );										// �J�b�g�o�b�t�@�ɃR�s�[
	cutp[o2-o1]=0;														// �J�b�g�o�b�t�@�O�^�[�~�l�[�g
	if( jntp ){															// �c����W���C���g����
		memcpy( jntp, txtp, o1 );										// �e�L�X�g�擪����I�t�Z�b�g�P�T�C�Y�R�s�[
		memcpy( jntp+o1, txtp+o2, o3-o2 );								// �I�t�Z�b�g�Q����c��S�Ă��R�s�[
		jntp[o1+o3-o2]=0;												// �O�^�[�~�l�[�g
	}
}

/************************************************************************/
/*	��`���[�h�J�b�g�o�b�t�@�ɃR�s�[									*/
/************************************************************************/

static void	cutcpybox( int delflg ){

	stDBLNK		*bakpgp, *bakcurp;										// DB�����N�|�C���^
	int			xx1, yy1, xx2, yy2;										// ��`�I���G���A
	stLIN		*linp;													// �s�|�C���^
	int			lno;													// �s�ԍ�
	char		cuta[0x10000];											// �J�b�g�o�b�t�@
	char		jnta[0x10000];											// �W���C���g�o�b�t�@
	stDB		*cbdbp;													// �N���b�v�{�[�hDB

	if( CUTCURP ){														// �I�����[�h���ł���
		xx1=CUTCURXX;													// ��`�I���G���A
		yy1=CUTPGYY+CUTCURYY;											// ��`�I���G���A
		xx2=CURXX;														// ��`�I���G���A
		yy2=CURYY+PGYY;													// ��`�I���G���A
		if( xx1<=xx2 && yy1<=yy2 ){										// �����`�G���A
			cbdbp=dbOpen( 0, DBSIZ_STR, 0, 0x4000 );					// �N���b�v�{�[�hDB�I�[�v��
			bakpgp=dbLNKP( CUTPGP )->bakp;								// �y�[�W�擪�̑O�̃����N�|�C���^
			bakcurp=dbLNKP( CUTCURP )->bakp;							// �J�����g�̑O�̃����N�|�C���^
			linp=CUTCURP;												// �s�|�C���^�͑I��擪
			for( lno=yy1; lno<=yy2 && linp; lno++ ){					// �s�ԍ����[�v
				txtcut( cuta, jnta, (char*)dbKEYADR( linp ), xx1, xx2 );// �e�L�X�g�͈͍폜
				dbAlc( cbdbp, cuta );									// �J�b�g�o�b�t�@�o�^
				if( delflg ){											// �폜
					linp=linflush( linp, jnta );						// �e�L�X�g�X�V
					linchg( linp );										// �s�ύX�Z�b�g
				}
				linp=(stLIN*)dbNxt( linp );								// ��ƍs�X�V
			}
			if( delflg ){												// �폜
				PGP=(stLIN*)(bakpgp->nxtp+1);							// �y�[�W�擪�|�C���^
				CURP=(stLIN*)(bakcurp->nxtp+1);							// �J�����g�|�C���^
				PGXX=CUTPGXX;											// �y�[�W�w
				PGYY=CUTPGYY;											// �y�[�W�x
				CURXX=CUTCURXX;											// �J�[�\���w
				CURYY=CUTCURYY;											// �J�[�\���x
				linAdjScr();											// ���X�N���[���̐��K��
			}
			CUTCURP=0;													// �͈͎w�胂�[�h�I��
			cbtxtput( cbdbp, 1 );										// �N���b�v�{�[�h�Ƀe�L�X�g����������
			dbClose( cbdbp );											// �N���b�v�{�[�hDB�N���[�Y
			invalidateAll();											// �S�čĕ`��
		}
	}
}

/************************************************************************/
/*	�J�b�g�o�b�t�@�ɃR�s�[												*/
/************************************************************************/

static void	cutcpy( int delflg ){
	if( !CUTTYP ){														// �s�I�����[�h
		cutcpylin( delflg );											// �s���[�h�J�b�g�o�b�t�@�ɃR�s�[
	}else{																// ��`�I�����[�h
		cutcpybox( delflg );											// ��`���[�h�J�b�g�o�b�t�@�ɃR�s�[
	}
}

/************************************************************************/
/*	F3	�J�����g���[�h�O���u��											*/
/************************************************************************/

void	f3(void){

	int		yn;															// �C�G�X�m�[
	int		chkflg;														// �u���m�F�t���O

	getcurword( g_reporg );												// �J�����g���[�h�𓾂�
	if( linedt2( g_reporg, "REPLACE FORWORD", g_repnew, "NEW STRINGS" ) ){
		yn=YorN( "KAKUNIN ? (Y/N)" );									// �m�F���邩�H
		if( yn!=YN_CANCEL ){											// �L�����Z���łȂ��ꍇ
			chkflg=( yn==YN_YES );										// �C�G�X�̏ꍇ�u���m�F�t���O�Z�b�g
			replace( g_reporg, g_repnew, 0, chkflg, !chkflg );			// �O���m�F�u��
		}
	}
}

/************************************************************************/
/*	F6		�͈͎w��J�n												*/
/************************************************************************/

void	f6(void){
	cutstart( 0 );														// �͈͎w��J�n
}

/************************************************************************/
/*	F7		�J�b�g�o�b�t�@�ֈړ�										*/
/************************************************************************/

void	f7(void){
	cutcpy( 1 );														// �J�b�g�o�b�t�@�ɃR�s�[
}

/************************************************************************/
/*	F8		�J�b�g�o�b�t�@�֕���										*/
/************************************************************************/

void	f8(void){
	cutcpy( 0 );														// �J�b�g�o�b�t�@�ɃR�s�[
}

/************************************************************************/
/*	F9		�J�b�g�o�b�t�@����}��										*/
/************************************************************************/

void	f9(void){

	stDBLNK		*bakpgp, *bakcurp;										// DB�����N�|�C���^
	stLIN		*linp;													// �s�|�C���^
	char		*cutp;													// �J�b�g�|�C���^
	stDB		*cbdbp;													// �N���b�v�{�[�hDB

	cbdbp=clpbrdGet( CLPBRDP, CB_TXT );									// �N���b�v�{�[�h�Q�b�g
	if( cbdbp ){														// �N���b�v�{�[�h�Q�b�g����
		bakpgp=dbLNKP( PGP )->bakp;										// �y�[�W�擪�̑O�̃����N�|�C���^
		bakcurp=dbLNKP( CURP )->bakp;									// �J�����g�̑O�̃����N�|�C���^
		dbFOR( cbdbp, cutp, char* ){									// �J�b�g�o�b�t�@������
			linp=linAlcStr( LINDBP, cutp );								// �V�s�o�^
			linchg( linp );												// �s�ύX�Z�b�g
			dbMov( LINDBP, linp, CURP );								// �J�����g�|�C���^�̑O�Ɉړ�
		}
		PGP=(stLIN*)(bakpgp->nxtp+1);									// �y�[�W�擪�|�C���^
		CURP=(stLIN*)(bakcurp->nxtp+1);									// �J�����g�|�C���^
		invalidateAll();												// �S�čĕ`��
		dbClose( cbdbp );												// �N���b�v�{�[�hDB�N���[�Y
	}
}

/************************************************************************/
/*	F10		�s����														*/
/************************************************************************/

void	f10(void){

	stDBLNK		*bakpgp;												// DB�����N�|�C���^
	stLIN		*newp;													// �s�|�C���^

	bakpgp=dbLNKP( PGP )->bakp;											// �y�[�W�擪�̑O�̃����N�|�C���^
	newp=linAlcStr( LINDBP, CURTXTP );									// �s�쐬
	linchg( newp );														// �s�ύX�Z�b�g
	dbMov( LINDBP, newp, CURP );										// �J�����g�s�̑O�Ɉړ�
	PGP=(stLIN*)(bakpgp->nxtp+1);										// �y�[�W�擪�|�C���^
	CURP=newp;															// �J�����g�|�C���^��V�����s�ɃZ�b�g
	invalidateAll();													// �S�čĕ`��
}

/************************************************************************/
/*	F11		�L�[�{�[�h�}�N���̒�`										*/
/************************************************************************/

void	f11(void){

	int		*keyp;														// �L�[�|�C���^

	if( !g_macalcdbp ){													// �}�N���o�^���ł͂Ȃ�
		g_macalcdbp=dbOpen( 0, sizeof(int), 0, 4096 );					// �}�N���o�^�J�n
		msgSet( "MACRO SET START" );
	}else{																// �}�N���o�^��
		keyp=(int*)dbEnd( g_macalcdbp );								// �ŏI�L�[�𓾂�
		if( *keyp==MKEY_F11 ){											// �}�N����`�I���L�[
			dbFre( g_macalcdbp, keyp );									// F11 �폜
		}else if( *keyp=='@' ){											// �}�N����`�I���L�[
			dbFre( g_macalcdbp, keyp );									// @ �폜
			keyp=(int*)dbEnd( g_macalcdbp );							// �ŏI�L�[�𓾂�
			dbFre( g_macalcdbp, keyp );									// [ESC] �폜
		}
		dbClose( g_macrsvdbp );											// �}�N���m��DB�N���[�Y
		g_macrsvdbp=g_macalcdbp;										// �}�N���m��DB�ݒ�
		g_macalcdbp=0;													// �}�N���o�^DB�N���A
		msgSet( "MACRO SET END" );										// �}�N���o�^�I��
	}
}

/************************************************************************/
/*	UNDO		�}�N�����s												*/
/************************************************************************/

void	f12(void){
	dbClose( g_macexedbp );												// �}�N�����sDB�N���[�Y
	g_macexedbp=dbDup( g_macrsvdbp );									// �}�N���m��DB����}�N�����sDB�ɃR�s�[
	msgSet( "MACRO EXECUTE" );											// �}�N�����s
}

/************************************************************************/
/*	SHIFT+F5	�p���������											*/
/************************************************************************/

void	shift_f5(void){

	char	scdbtr[256];												// ����������

	if( scdbtrget( scdbtr ) ){											// ����������𓾂�
		replace( scdbtr, 0, 1, 0, 0 );									// �����܂��͒u��
	}
}

/************************************************************************/
/*	SHIFT+F6	��`���[�h�͈͎w��J�n									*/
/************************************************************************/

void	shift_f6(void){
	cutstart( 1 );														// �͈͎w��J�n
	msgSet( "��`�I��" );
}

/************************************************************************/
/*	SHIFT+F7	��`���[�h�J�b�g�o�b�t�@�ֈړ�							*/
/************************************************************************/

void	shift_f7(void){
	cutcpy( 1 );														// �J�b�g�o�b�t�@�ɃR�s�[
}

/************************************************************************/
/*	SHIFT+F8	��`���[�h�J�b�g�o�b�t�@�֕���							*/
/************************************************************************/

void	shift_f8(void){
	cutcpy( 0 );														// �J�b�g�o�b�t�@�ɃR�s�[
}

/************************************************************************/
/*	SHIFT+F9	��`���[�h�J�b�g�o�b�t�@����}��						*/
/************************************************************************/

void	shift_f9(void){

	stLIN		*linp, *newp;											// �s�|�C���^
	char		tmpa[0x10000];											// �e���|�����o�b�t�@
	char		*cutp;													// �J�b�g�|�C���^
	char		*txtp;													// �e�L�X�g�|�C���^
	stDB		*cbdbp;													// �N���b�v�{�[�hDB

	cbdbp=clpbrdGet( CLPBRDP, CB_TXT );									// �N���b�v�{�[�h�Q�b�g
	if( cbdbp ){														// �N���b�v�{�[�h�Q�b�g����
		linp=CURP;														// �ύX�̓J�����g����
		dbFOR( cbdbp, cutp, char* ){									// �J�b�g�o�b�t�@������
			if( linp ){													// �L���s�����݂���
				if( strlen( cutp ) ){									// �}�����ׂ�������͑��݂���
					txtp=(char*)dbKEYADR( linp );						// �e�L�X�g�|�C���^
					if( strlen( txtp )+strlen( cutp )+1<sizeof(tmpa) ){	// �}�����ׂ��L���ȕ����񂪑��݂���
						strcpy( tmpa, txtp );							// �e���|�����o�b�t�@�Ɉ�U�R�s�[
						lmSetStr( tmpa, CURXX, cutp, g_tabstp, g_flgins );		// ������쐬
						linp=linflush( linp, tmpa );					// �e�L�X�g�X�V
						linchg( linp );									// �s�ύX�Z�b�g
					}else{												// �e�L�X�g����������
						msgSet( "Line Too Long" );
					}
				}
				linp=(stLIN*)dbNxt( linp );								// �����s�X�V
			}else{														// �L���s�͑��݂��Ȃ�
				newp=linAlcStr( LINDBP, cutp );							// �����̍s�o�^
				linchg( newp );											// �s�ύX�Z�b�g
			}
		}
		invalidateAll();												// �S�čĕ`��
		dbClose( cbdbp );												// �N���b�v�{�[�hDB�N���[�Y
	}
}

/************************************************************************/
/*	CTRL+TAB	�����^�u												*/
/************************************************************************/

void	ctrl_tab(void){

	stLIN		*linp;													// �s�|�C���^
	int			toplno, curlno;											// �擪�s�ԍ�, �J�����g�s�ԍ�
	int			lno;													// �s�ԍ�
	int			curxx;													// �J�[�\���w���W
	stLM		lm;														// �s�}�b�v
	char		*txtp;													// �s�e�L�X�g�|�C���^
	char		*a0;													// �폜�|�C���^

	toplno=CUTPGYY+CUTCURYY;											// �I��擪�s�ԍ�
	curlno=PGYY+CURYY;													// �J�����g�s�ԍ�
	if( CUTCURP && !CUTTYP && curlno>toplno ){							// �I�����[�h���ł���
		linp=CUTCURP;													// �s�|�C���^�͑I��擪
		for( lno=toplno; lno<curlno; lno++ ){							// �s�ԍ����[�v
			txtp=(char*)dbKEYADR( linp );								// �s�e�L�X�g�|�C���^
			lmMak( &lm, txtp, g_tabstp, 0 );							// �s�}�b�v�쐬
			curxx=lmPosAdj( &lm, CURXX, 0 );							// �J�[�\���w���W���K��
			a0=txtp+lm.txtofsa[curxx];									// �폜�|�C���^�Z�b�g
			if( *a0=='\t' || *a0==' ' ){								// �^�u���̓X�y�[�X�̏ꍇ
				strcpy( a0, a0+1 );										// �^�u���̓X�y�[�X�폜
				linp=linflush( linp, txtp );							// �e�L�X�g�X�V
				linchg( linp );											// �s�ύX�Z�b�g
			}
			linp=(stLIN*)dbNxt( linp );									// �s�|�C���^�X�V
		}
		invalidateAll();												// �S�čĕ`��
	}
}

/************************************************************************/
/*	�E�B���h�E���̃T�C�Y�ɖ߂�											*/
/************************************************************************/

void	alt_f5(void){
#	ifdef	_WIN32
	ShowWindow( g_hwnd, SW_RESTORE );									// �E�B���h�E���̃T�C�Y�ɖ߂�
#	endif
}

/************************************************************************/
/*	�E�B���h�E�ŏ���													*/
/************************************************************************/

void	alt_f9(void){
#	ifdef	_WIN32
	ShowWindow( g_hwnd, SW_SHOWMAXIMIZED );								// �E�B���h�E�ŏ���
#	endif
}

/************************************************************************/
/*	�E�B���h�E�ő剻													*/
/************************************************************************/

void	alt_f10(void){
#	ifdef	_WIN32
	ShowWindow( g_hwnd, SW_SHOWMAXIMIZED );								// �E�B���h�E�ő剻
#	endif
}

/************************************************************************/
/*	�d��																*/
/************************************************************************/

void	alt_slash(void){

	stLIN		*linp;													// �s�|�C���^
	const char	*txtp;													// �e�L�X�g�|�C���^
	LLONG		val;
	char		srca[LM_MAX];
	char		tmpa[LM_MAX];

	txtp=SKPSP( CURTXTP );												// �e�L�X�g�|�C���^�̓J�����g�e�L�X�g�|�C���^
	if( txtp[0]=='/' && txtp[1]=='/' ){									// �擪�̓R�����g
		txtp=SKPSP( txtp+2 );											// �R�����g�X�L�b�v
	}
	txtp=eval( &val, txtp, 0, 0 );
	if( txtp ){
		memcpy( srca, CURTXTP, txtp-CURTXTP );
		srca[txtp-CURTXTP]=0;
		sprintf( tmpa, "%s	%lld 0x%I64x", srca, val, val );
		linp=linflush( CURP, tmpa );									// �e�L�X�g�X�V
		linchg( linp );													// �s�ύX�Z�b�g
		invalidateAll();												// �S�čĕ`��
	}
}

/************************************************************************/
/*	���������d��														*/
/************************************************************************/

void	alt_ast(void){

	stLIN		*linp;													// �s�|�C���^
	const char	*txtp;													// �e�L�X�g�|�C���^
	FPTYP		val;
	char		srca[LM_MAX];
	char		tmpa[LM_MAX];

	txtp=SKPSP( CURTXTP );												// �e�L�X�g�|�C���^�̓J�����g�e�L�X�g�|�C���^
	if( txtp[0]=='/' && txtp[1]=='/' ){									// �擪�̓R�����g
		txtp=SKPSP( txtp+2 );											// �R�����g�X�L�b�v
	}
	txtp=evalf( &val, txtp, 0, 0 );
	if( txtp ){
		memcpy( srca, CURTXTP, txtp-CURTXTP );
		srca[txtp-CURTXTP]=0;
		sprintf( tmpa, "%s	%f 0x%08x", srca, val, (int)val );
		linp=linflush( CURP, tmpa );									// �e�L�X�g�X�V
		linchg( linp );													// �s�ύX�Z�b�g
		invalidateAll();												// �S�čĕ`��
	}
}

/************************************************************************/
/*	�V�F��																*/
/************************************************************************/

void	alt_plus(void){

	const char	*txtp;													// �e�L�X�g�|�C���^

	txtp=SKPSP( CURTXTP );												// �e�L�X�g�|�C���^�̓J�����g�e�L�X�g�|�C���^
	if( txtp[0]=='/' && txtp[1]=='/' ){									// �擪�̓R�����g
		txtp=SKPSP( txtp+2 );											// �R�����g�X�L�b�v
	}
	shell( txtp );														// �V�F��
	invalidateAll();													// �S�čĕ`��
}

/************************************************************************/
/*	SHIFT+SPACE	FEP�I�[�v��												*/
/************************************************************************/

void	shift_space(void){
#	ifndef	_WIN32
#		ifndef	CURSES
	fepopen();															// FEP�I�[�v��
#		endif
#	endif
}

/************************************************************************/
/*	CTRL+F9		�Z���N�V�����ǂݍ���									*/
/************************************************************************/

void	ctrl_f9calbak( stDB *cbdbp ){

	stDBLNK		*bakpgp, *bakcurp;										// DB�����N�|�C���^
	stLIN		*linp;													// �s�|�C���^
	char		*cutp;													// �J�b�g�|�C���^

	bakpgp=dbLNKP( PGP )->bakp;											// �y�[�W�擪�̑O�̃����N�|�C���^
	bakcurp=dbLNKP( CURP )->bakp;										// �J�����g�̑O�̃����N�|�C���^
	dbFOR( cbdbp, cutp, char* ){										// �J�b�g�o�b�t�@������
		linp=linAlcStr( LINDBP, cutp );									// �V�s�o�^
		linchg( linp );													// �s�ύX�Z�b�g
		dbMov( LINDBP, linp, CURP );									// �J�����g�|�C���^�̑O�Ɉړ�
	}
	PGP=(stLIN*)(bakpgp->nxtp+1);										// �y�[�W�擪�|�C���^
	CURP=(stLIN*)(bakcurp->nxtp+1);										// �J�����g�|�C���^
	invalidateAll();													// �S�čĕ`��
}

void	ctrl_f9(void){
#	ifndef	_WIN32
#		ifndef	CURSES
	xselDemand( g_xselp, g_tim );										// �Z���N�V�����v��
#		endif
#	endif
}

