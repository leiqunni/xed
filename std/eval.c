/************************************************************************/
/*																		*/
/*	�����]��( Evaluation of Expressions )	by H.Watanabe				*/
/*	Ver1.00	2009/04/05 13:52:04	�쐬									*/
/*																		*/
/************************************************************************/

#	include	"eval.h"													/* �����]��( Evaluation of Expressions )*/

/************************************************************************/
/*	���[�J����`														*/
/************************************************************************/

	typedef struct{														// �\���e�[�u��
		int			cal;												// ���Z�^�C�v
		signed char pri;												// ���Z�v���C�I���e�B
		char		calnam[3];											// ���Z�q��
	}	stCODTAB;

/************************************************************************/
/*	�e�����ƃf�[�^�e�[�u��												*/
/************************************************************************/

	enum{
		CAL_ANDAND,														// &&:�_����
		CAL_OROR,														// ||:�_���a
		CAL_AND,														// & :�r�b�g���Ƃ� AND
		CAL_OR,															// | :�r�b�g���Ƃ� OR
		CAL_XOR,														// ^ :�r�b�g���Ƃ� XOR
		CAL_LSL,														// <<:���V�t�g
		CAL_LSR,														// >>:�E�V�t�g
		CAL_ADD,														// + :���Z
		CAL_SUB,														// - :���Z
		CAL_MUL,														// * :��Z
		CAL_DIV,														// / :���Z
		CAL_MOD,														// % :��]
		CAL_EQ,															// ==:������
		CAL_NE,															// !=:�������Ȃ�
		CAL_LE,															// <=:��������������
		CAL_LT,															// < :������
		CAL_GE,															// >=:�傫����������
		CAL_GT,															// > :�傫��
		CAL_N
	};

	static const stCODTAB CodTab[]={									// �v���C�I���e�B�͂b�Ɠ���
		{	CAL_ANDAND,	 5,		"&&"	},								// &&:�_����
		{	CAL_OROR,	 4,		"||"	},								// ||:�_���a
		{	CAL_AND,	 8,		"&"		},								// & :�r�b�g���Ƃ� AND
		{	CAL_OR,		 6,		"|"		},								// | :�r�b�g���Ƃ� OR
		{	CAL_XOR,	 7,		"^"		},								// ^ :�r�b�g���Ƃ� XOR
		{	CAL_LSL,	11,		"<<"	},								// <<:���V�t�g
		{	CAL_LSR,	11,		">>"	},								// >>:�E�V�t�g
		{	CAL_ADD,	12,		"+"		},								// + :���Z
		{	CAL_SUB,	12,		"-"		},								// - :���Z
		{	CAL_MUL,	13,		"*"		},								// * :��Z
		{	CAL_DIV,	13,		"/"		},								// / :���Z
		{	CAL_MOD,	13,		"%"		},								// % :��]
		{	CAL_EQ,		 9,		"=="	},								// ==:������
		{	CAL_NE,		 9,		"!="	},								// !=:�������Ȃ�
		{	CAL_LE,		10,		"<="	},								// <=:��������������
		{	CAL_LT,		10,		"<"		},								// < :������
		{	CAL_GE,		10,		">="	},								// >=:�傫����������
		{	CAL_GT,		10,		">"		},								// > :�傫��
		{	0,			-1,		""		}								// �e�[�u���̏I��
	};

/************************************************************************/
/*	������𐔒l�ɕϊ�����												*/
/************************************************************************/

static char	*s2ip( EVALTYP *valp, const char *txtp, int radix ){

	char	chr;														// �e�L�X�g����
	int		uni;														// �P��
	EVALTYP	val=0;														// ���l�\���l

	for(;;){
		chr=*txtp++;													// �e�L�X�g�|�C���^����P�����𓾂�
		if( chr<='9' ){													// '9'�ȉ�
			uni=(unsigned char)( chr-'0' );								// ��������'0'�������� unsigned char �ŃL���X�g
		}else{															// '9'�ȉ��ł͂Ȃ�
			uni=( (unsigned char)( (chr|0x20)-'a' ) )+10;				// ��������'a'�������� unsigned char �ŃL���X�g���A10�����Z����
		}
		if( uni>=radix ){												// ����傫��
			break;														// ���[�v�I��
		}
		val=val*radix+uni;												// ��{���Ēl�����Z����
	}
	if( valp ){															// �f�B�X�g�l�[�V�����|�C���^���w�肳��Ă���
		*valp=val;														// �l��������
	}
	return( (char*)( txtp-1 ) );										// �e�L�X�g�|�C���^�͎��̕����ɃV�[�N���Ă���̂Ł|�P�����l��Ԃ�
}

/************************************************************************/
/*	�P�����̕]��														*/
/************************************************************************/

static char	*evalUni( EVALTYP *dstp, const char *txtp, tpEVALCB *valprcp, void *usrp ){

	unsigned char	chr;												// �e�L�X�g����
	EVALTYP			val;												// ���l�]�����[�N
	const char		*bakp;												// �e�L�X�g�|�C���^�o�b�N�A�b�v

	for(;;){															// �X�L�b�v�X�y�[�X
		chr=*txtp++;													// �e�L�X�g�|�C���^����P�����𓾂�
		if( chr!='\t' && chr!=' ' && chr!='\n' && chr!='+' ){			// �^�u�E�X�y�[�X�E���s�R�[�h�E�{�ȊO
			break;														// ���[�v�I��
		}
	}
	switch( chr ){
		case '-':														// - �P�����Z�q
			txtp=evalUni( &val, txtp, valprcp, usrp );					// �ċA�R�[��
			*dstp=-val;													// NEG ����
			break;														// switch �I��
		case '~':														// ~ �P�����Z�q
			txtp=evalUni( &val, txtp, valprcp, usrp );					// �ċA�R�[��
			*dstp=~val;													// NOT ����
			break;														// switch �I��
		case '!':														// ~ �P�����Z�q
			txtp=evalUni( &val, txtp, valprcp, usrp );					// �ċA�R�[��
			*dstp=( val==0 );											// NOT ����
			break;														// switch �I��
		case '(':
			txtp=eval( dstp, txtp, valprcp, usrp );						// �ċA�R�[��
			if( !txtp ){												// �G���[���N������
				return( 0 );											// �G���[�I��
			}
			if( *txtp++!=')' ){											// ')'������
				return( 0 );											// �G���[�I��
			}
			break;														// switch �I��
		case '\'':														// �����萔
			val=0;
			while( ( chr=*txtp++ )!=0 ){
				if( chr=='\'' ){
					break;
				}
				val=(val<<8)+chr;
			}
			*dstp=val;
			break;
		case '0':
			if( *txtp=='x' || *txtp=='X' ){								// 0x �܂��� 0X �Ŏn�܂�P�U�i��
				txtp=s2ip( dstp, txtp+1, 16 );							// ������𐔒l�ɕϊ�����
			}else{														// �W�i���܂��͂P�O�i���̂O
				txtp=s2ip( dstp, txtp, 8 );								// ������𐔒l�ɕϊ�����
			}
			break;														// switch �I��
		case '1': case '2': case '3':
		case '4': case '5': case '6':
		case '7': case '8': case '9':									// �P�O�i��
			txtp=s2ip( dstp, txtp-1, 10 );								// ������𐔒l�ɕϊ�����
			break;														// switch �I��
		default:
			if( !valprcp ){												// ���[�U�[�w�菈�������̏ꍇ
				return( 0 );											// �G���[���^�[��
			}
			txtp=valprcp( dstp, bakp=txtp-1, usrp );					// ���[�U�[�w�菈���R�[��
			if( txtp<=bakp ){											// �]���s�\.
				return( 0 );											// �G���[���^�[��
			}
			break;														// switch �I��
	}
	return( (char*)txtp );												// ����I��
}

/************************************************************************/
/*	�����]��( Evaluation of Expressions )								*/
/************************************************************************/

char	*eval( EVALTYP *dstp, const char *txtp, tpEVALCB *valprcp, void *usrp ){

	struct{																// ���Z�v���C�I���e�B�����̂��߂̕]���\����
		signed char pri;												// �v���C�I���e�B
		int			cal;												// ���Z�^�C�v
		EVALTYP		val;												// ���Z����
	}				va[10+1],*vp;										// �]���z��ƕ]���X�^�b�N�|�C���^
	const stCODTAB	*codp;												// �\���e�[�u���|�C���^
	const char		*curp;												// ���Z�q�����̂��߂̃|�C���^
	const char		*namp;												// ���Z�q�\���|�C���^
	char			chr;												// �J�����g����
	EVALTYP			dst, sou;

	vp=va;																// �]���X�^�b�N�|�C���^������
	va[0].pri=-2;														// ���
	while( txtp ){														// �]�����[�v( VC5 �o�O�̂��� do while �̎g�p�֎~ )
		txtp=evalUni( &vp->val, txtp, valprcp, usrp );					// �P�����̕]��
		if( !txtp ){													// �G���[���N������
			return( 0 );												// �G���[�I��
		}
		for(;;){														// �X�L�b�v�X�y�[�X
			chr=*txtp++;												// �e�L�X�g�|�C���^����P�����𓾂�
			if( chr!='\t' && chr!=' ' ){								// �^�u�E�X�y�[�X�Ȃ�΃��[�v����
				break;
			}
		}
		curp=txtp-1;													// �e�L�X�g�|�C���^�͂P�i��ł���̂Ŗ߂�
		for( codp=CodTab; ; codp++ ){									// ���Z�q�T�[�`���[�v
			txtp=curp;													// �e�L�X�g�|�C���^���J�����g�ɃZ�b�g
			namp=codp->calnam;											// �e�[�u�����̖��O
			while( ( chr=*namp++ )!=0 ){
				if( chr!=*txtp++ ){										// �����s��v
					break;												// ���[�v�I��
				}
			}
			if( !chr ){													// ��������
				break;													// ���[�v�I��
			}
		}
		while( vp->pri>=codp->pri ){									// �]���X�^�b�N�̃v���C�I���e�B���J�����g�v���C�I���e�B�ȏ�̊ԃ��[�v
			dst	=(vp-1)->val;											// �f�B�X�g�l�[�V����
			sou	=vp->val;												// �\�[�X
			switch( vp->cal ){
				case CAL_AND:		dst=( dst & sou );	break;			// &
				case CAL_OR:		dst=( dst | sou );	break;			// |
				case CAL_XOR:		dst=( dst ^ sou );	break;			// ^
				case CAL_LSL:											// ���V�t�g
					if( sou<0 || sou>63 ){								// �V�t�g�͈͂𒴂��Ă���
						if( valprcp ){									// �R�[���o�b�N�֐����w�肳��Ă���
							valprcp( 0, EVALERR_LSL_OF, usrp );			// �G���[�ʒm�R�[���o�b�N
						}
						txtp=0;											// �G���[
					}
					dst=( dst<< sou );									// ���ʂ̏�������
					break;
				case CAL_LSR:											// �E�V�t�g
					if( sou<0 || sou>63 ){								// �V�t�g�͈͂𒴂��Ă���
						if( valprcp ){									// �R�[���o�b�N�֐����w�肳��Ă���
							valprcp( 0, EVALERR_LSR_OF, usrp );			// �G���[�ʒm�R�[���o�b�N
						}
						txtp=0;											// �G���[
					}
					dst=( dst>> sou );									// ���ʂ̏�������
					break;
				case CAL_ADD:		dst=( dst + sou );		break;		// ���Z
				case CAL_SUB:		dst=( dst - sou );		break;		// ���Z
				case CAL_MUL:		dst=( dst * sou );		break;		// ��Z
				case CAL_DIV:											// ���Z
					if( !sou ){											// �O���Z
						if( valprcp ){									// �R�[���o�b�N�֐����w�肳��Ă���
							valprcp( 0, EVALERR_DIV_0, usrp );			// �G���[�ʒm�R�[���o�b�N
						}
						txtp=0;											// �G���[
					}else{
						if( dst==0x80000000 && sou==-1 ){				// �\���ł��镉�̍ő吔���|�P�Ŋ���ƁA�Q�̕����ŕ\���������t�����̏ꍇ�̓I�[�o�[�t���[�ƂȂ邱�Ƃ�����B
							if( valprcp ){								// �R�[���o�b�N�֐����w�肳��Ă���
								valprcp( 0, EVALERR_DIV_OF, usrp );		// �G���[�ʒm�R�[���o�b�N
							}
							txtp=0;										// �G���[
						}else{
							dst=( dst / sou );							// ���ʂ̏�������
						}
					}
					break;
				case CAL_MOD:											// ��]
					if( !sou ){											// �O��]
						if( valprcp ){									// �R�[���o�b�N�֐����w�肳��Ă���
							valprcp( 0, EVALERR_MOD_0, usrp );			// �G���[�ʒm�R�[���o�b�N
						}
						txtp=0;											// �G���[
					}else{
						if( sou<0 ){									// ��]�͐������w�肷�ׂ�
							if( valprcp ){								// �R�[���o�b�N�֐����w�肳��Ă���
								valprcp( 0, EVALERR_MOD_OF, usrp );		// �G���[�ʒm�R�[���o�b�N
							}
							txtp=0;										// �G���[
						}else{
							dst=( dst % sou );							// ���ʂ̏�������
						}
					}
					break;
				case CAL_EQ:		dst=( dst == sou );	break;			// ������
				case CAL_NE:		dst=( dst != sou );	break;			// �������Ȃ�
				case CAL_LE:		dst=( dst <= sou );	break;			// �ȉ�
				case CAL_LT:		dst=( dst  < sou );	break;			// ����
				case CAL_GE:		dst=( dst >= sou );	break;			// �ȏ�
				case CAL_GT:		dst=( dst  > sou );	break;			// �傫��
				case CAL_ANDAND:	dst=( dst && sou );	break;			// &&
				case CAL_OROR:		dst=( dst || sou );	break;			// ||
			}
			(vp-1)->val=dst;											// ���Z���ʃX�g�A
			vp--;														// �]���X�^�b�N�|�C���^���Z
		}
		vp++;															// �]���X�^�b�N�|�C���^���Z
		if( (vp->pri=codp->pri)<0 ){									// �e�[�u���I�[�� �v���C�I���e�B=-1 ��������
			break;														// ���[�v�I��
		}
		vp->cal=codp->cal;												// ���Z�����A�h���X
	}
	*dstp=va[0].val;													// �v�Z���ʂ̏�������
	return( (char*)txtp );												// ����I��
}

