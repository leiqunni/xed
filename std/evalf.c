/************************************************************************/
/*																		*/
/*	�����]��					by H.Watanabe							*/
/*	Ver1.00	2009/04/04 13:44:21	�쐬									*/
/*																		*/
/************************************************************************/

#	include	<math.h>													/* ���w�֐�*/
#	include	<stdlib.h>													/* �W�����C�u�����֐�*/
#	include	"evalf.h"													/* �����]��*/

/************************************************************************/
/*	���[�J����`														*/
/************************************************************************/

	typedef struct{
		FPTYP			(*prc)( FPTYP, FPTYP );							// �l�̕]��������R�[���o�b�N�֐�
		signed char		pri;											// ���Z�q�v���C�I���e�B
		char			calnam[3];										// ���Z�q��
	}	stCODTAB;

/************************************************************************/
/*	�e�����ƃf�[�^�e�[�u��												*/
/************************************************************************/

static FPTYP	evalfAnd(		FPTYP dst, FPTYP sou ){	return( (int )dst &(int )sou );	}
static FPTYP	evalfOr(		FPTYP dst, FPTYP sou ){	return( (int )dst |(int )sou );	}
static FPTYP	evalfUpa(		FPTYP dst, FPTYP sou ){	return( (int )dst ^(int )sou );	}
static FPTYP	evalfLsl(		FPTYP dst, FPTYP sou ){	return( (int )dst<<(int )sou );	}
static FPTYP	evalfLsr(		FPTYP dst, FPTYP sou ){	return( (int )dst>>(int )sou );	}
static FPTYP	evalfAdd(		FPTYP dst, FPTYP sou ){	return( dst +sou );	}
static FPTYP	evalfSub(		FPTYP dst, FPTYP sou ){	return( dst -sou );	}
static FPTYP	evalfPow(		FPTYP dst, FPTYP sou ){	return( pow( dst, sou ) );	}
static FPTYP	evalfMul(		FPTYP dst, FPTYP sou ){	return( dst *sou );	}
static FPTYP	evalfDiv(		FPTYP dst, FPTYP sou ){	return( dst /sou );	}
static FPTYP	evalfMod(		FPTYP dst, FPTYP sou ){	return( (int )dst %(int )sou );	}
static FPTYP	evalfEq(		FPTYP dst, FPTYP sou ){	return( dst==sou );	}
static FPTYP	evalfNe(		FPTYP dst, FPTYP sou ){	return( dst!=sou );	}
static FPTYP	evalfLe(		FPTYP dst, FPTYP sou ){	return( dst<=sou );	}
static FPTYP	evalfLt(		FPTYP dst, FPTYP sou ){	return( dst <sou );	}
static FPTYP	evalfGe(		FPTYP dst, FPTYP sou ){	return( dst>=sou );	}
static FPTYP	evalfGt(		FPTYP dst, FPTYP sou ){	return( dst >sou );	}
static FPTYP	evalfAndAnd(	FPTYP dst, FPTYP sou ){	return( dst&&sou );	}
static FPTYP	evalfOrOr(		FPTYP dst, FPTYP sou ){	return( dst||sou );	}

	static const stCODTAB	CodTab[]={
		{	 0,				-1, "//"	},								// �R�����g
		{	 0,				-1, "/*"	},								// �R�����g
		{	 evalfAndAnd,	 0, "&&"	},								// &&
		{	 evalfOrOr,		 0, "||"	},								// ||
		{	 evalfAnd,		 3, "&"		},								// &	and group
		{	 evalfOr,		 2, "|"		},								// |
		{	 evalfUpa,		 2, "^"		},								// ^
		{	 evalfLsl,		 5, "<<"	},								// <<	sft group
		{	 evalfLsr,		 5, ">>"	},								// >>
		{	 evalfAdd,		 4, "+"		},								// +	add group
		{	 evalfSub,		 4, "-"		},								// -
		{	 evalfPow,		 6, "**"	},								// �ׂ���
		{	 evalfMul,		 5, "*"		},								// *	mul grup
		{	 evalfDiv,		 5, "/"		},								// /
		{	 evalfMod,		 5, "%"		},								// %
		{	 evalfEq,		 1, "=="	},								// ==
		{	 evalfNe,		 1, "!="	},								// !=
		{	 evalfLe,		 1, "<="	},								// <=
		{	 evalfLt,		 1, "<"		},								// <
		{	 evalfGe,		 1, ">="	},								// >=
		{	 evalfGt,		 1, ">"		},								// >
		{	 0,				-1, ""		}								// end
	};

/************************************************************************/
/*	�P�����]��															*/
/************************************************************************/

char	*evalfUni( FPTYP *dstp, const char *txtp, tpEVALDVAL *valprcp, void *usrp ){

	unsigned char	chr,uni;											// �e�L�X�g����ǂݍ��񂾕���
	char			*nxtp;												// ���̃e�L�X�g�|�C���^
	FPTYP			val=0.0;											// �\���l

	for(;;){															// �X�L�b�v�X�y�[�X
		chr=*txtp++;													// �e�L�X�g�|�C���^����P�����𓾂�
		if( chr!='\t' && chr!=' ' && chr!='+' ) break;					// �^�u�E�X�y�[�X�E�{�̂Ȃ�΃��[�v����
	}
	switch( chr ){
		case '-':														// - �P�����Z�q
			txtp=evalfUni( &val, txtp, valprcp, usrp );					// �ċA�R�[��
			*dstp=-val;													// NEG ����
			break;
		case '~':														// ~ �P�����Z�q
			txtp=evalfUni( &val, txtp, valprcp, usrp );					// �ċA�R�[��
			*dstp=~(int )val;											// NOT ����
			break;
		case '(':
			txtp=evalf( dstp, txtp, valprcp, usrp );					// �ċA�R�[��
			if( !txtp ) return( 0 );									// �G���[�I��
			if( *txtp++!=')' ) return( 0 );								// �G���[�I��
			break;
		case '0':														// �P�U�i��,�W�i��,�P�O�i���̂O�̂����ꂩ
			if( (*txtp|0x20)=='x' ){									// 0x �܂��� 0X �Ŏn�܂�P�U�i��
				txtp++;													// 'x' �܂��� 'X' ���X�L�b�v
				for(;;){
					chr=*txtp++;										// �e�L�X�g�|�C���^����P�����𓾂�
					if( ( uni=(unsigned char)( chr-'0' ) )>=10 ){		// '0'�`'9' �͈̔͂ł͂Ȃ�
						if( ( uni=(unsigned char)( (chr|0x20)-'a' ) )>=6 ) break;				// 'a'�`'f' �͈̔͂ł͂Ȃ�
						uni+=10;										// 'a'�`'f' �͈̔͂Ȃ̂�10�����Z����
					}
					val=val*16.0+(FPTYP)uni;							// �P�U�{���Ēl�����Z����
				}
				txtp--;													// �e�L�X�g�|�C���^�͂P�i��ł���̂Ŗ߂�
			}else{														// ��������
				val=strtod( txtp-1, (char**)&txtp );
				if( (*txtp|0x20)=='f' ){								// ���l�I�[�� 'f' �܂��� 'F' �͖���
					txtp++;
				}
			}
			*dstp=val;
			break;
		case '1': case '2': case '3':
		case '4': case '5': case '6':
		case '7': case '8': case '9':									// �P�O�i��
			val=strtod( txtp-1, (char**)&txtp );
			if( (*txtp|0x20)=='f' ){									// ���l�I�[�� 'f' �܂��� 'F' �͖���
				txtp++;
			}
			*dstp=val;
			break;
		default:
			if( !valprcp ) return( 0 );									// ���[�U�[�w�菈�������̏ꍇ�̓G���[
			nxtp=valprcp( dstp, txtp-1, usrp );							// ���[�U�[�w�菈���R�[��
			if( nxtp<txtp ) return( 0 );								// �]���s�\.
			txtp=nxtp;													// �e�L�X�g�|�C���^�X�V
			break;
	}
	return( (char*)txtp );												// ����I��
}

/************************************************************************/
/*	�����]��( Evaluation of Expressions )								*/
/************************************************************************/

char	*evalf( FPTYP *dstp, const char *txtp, tpEVALDVAL *valprcp, void *usrp ){

	struct{																// ���Z�v���C�I���e�B�����̂��߂̕]���\����
		signed char pri;												// �v���C�I���e�B
		FPTYP		(*prc)( FPTYP, FPTYP );								// �����A�h���X
		FPTYP		val;												// ���Z����
	}				va[6+1],*vp;										// �]���z��ƕ]���X�^�b�N�|�C���^
	const stCODTAB	*codp;												// �\���e�[�u���|�C���^
	const char		*curp;												// ���Z�q�����̂��߂̃|�C���^
	const char		*namp;												// ���Z�q�\���|�C���^
	int				chr;												// char �ŗǂ��̂��� �������E�������ߖ�̂��� int �Ƃ���

	vp=va;																// �]���X�^�b�N�|�C���^������
	va[0].pri=-2;														// ���
	for(;;){															// �]�����[�v( VC5 �o�O�̂��� do while �̎g�p�֎~ )
		txtp=evalfUni( &vp->val, txtp, valprcp, usrp );					// �P�����̕]��
		if( !txtp ) return( 0 );										// �G���[�I��
		for(;;){														// �X�L�b�v�X�y�[�X
			chr=*txtp++;												// �e�L�X�g�|�C���^����P�����𓾂�
			if( chr!='\t' && chr!=' ' ) break;							// �^�u�E�X�y�[�X�Ȃ�΃��[�v����
		}
		curp=txtp-1;													// �e�L�X�g�|�C���^�͂P�i��ł���̂Ŗ߂�
		for( codp=CodTab; ; codp++ ){									// ���Z�q�T�[�`���[�v
			txtp=curp;													// �e�L�X�g�|�C���^���J�����g�ɃZ�b�g
			namp=codp->calnam;											// �e�[�u�����̖��O
			while( ( chr=*namp++ )!=0 ){
				if( chr!=*txtp++ ) break;								// �����s��v
			}
			if( !chr ) break;											// ��������
		}
		while( vp->pri>=codp->pri ){									// �]���X�^�b�N�̃v���C�I���e�B���J�����g�v���C�I���e�B�ȏ�̊ԃ��[�v
			(vp-1)->val=vp->prc( (vp-1)->val, vp->val );				// ���Z���s
			vp--;														// �]���X�^�b�N�|�C���^���Z
		}
		vp++;															// �]���X�^�b�N�|�C���^���Z
		if( (vp->pri=codp->pri)<0 ) break;								// �e�[�u���I�[�� �v���C�I���e�B=-1 ��������܂�
		vp->prc=codp->prc;												// ���Z�����A�h���X
	}
	*dstp=va[0].val;													// �v�Z���ʂ̏�������
	return( (char*)txtp );												// ����I��
}

