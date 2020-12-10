/************************************************************************/
/*																		*/
/*	�����]��( Evaluation of Expressions )	by H.Watanabe				*/
/*	Ver1.00	2009/04/05 13:52:04	�쐬									*/
/*																		*/
/************************************************************************/

#	ifndef	EVAL_H
#	define	EVAL_H
#	ifdef	__cplusplus
	extern "C"{
#	endif

#	ifndef	LLONG
#		ifdef	_WIN32
#			define	LLONG		__int64
#			define	LL			"I64"
#			define	LONGLONG(I)	(I##i64)
#		else
#			define	LLONG		long long
#			define	LL			"q"
#			define	LONGLONG(I)	(I##LL)
#		endif
#	endif

#	ifndef	EVALTYP
#		define	EVALTYP			LLONG									/* �^*/
#	endif

#	define	EVALERR_LSR_OF		"���V�t�g�l�͈͊O"
#	define	EVALERR_LSL_OF		"�E�V�t�g�l�͈͊O"
#	define	EVALERR_DIV_0		"�O���Z"
#	define	EVALERR_DIV_OF		"���Z�I�[�o�[�t���["
#	define	EVALERR_MOD_0		"�O��]"
#	define	EVALERR_MOD_OF		"��]�I�[�o�[�t���["

	typedef char *tpEVALCB( EVALTYP *dstp, const char *txtp, void *usrp );		// ���[�U�[�����֐�

//	dstp:	�������ʂ��i�[����A�h���X�ł��B( 0 �̏ꍇ�̓G���[���ł� )
//	txtp:	dstp ��0�ȊO�̏ꍇ�ɂ̓e�L�X�g���Z�b�g����Ă��܂��Bdstp �� 0 �̏ꍇ�ɂ̓G���[�����񂪃Z�b�g����Ă��܂��B
//	usrp:	���[�U�[���w�肷��|�C���^�ł��B
//	�A�l:	���������e�L�X�g�̎���Ԃ��Ă��������B�����s�\�������ꍇ��0��Ԃ��Ă��������B

	char	*eval( EVALTYP *dstp, const char *txtp, tpEVALCB *valprcp, void *usrp );

//	dstp:	�������ʂ��i�[����A�h���X�ł��B
//	txtp:	������������Z�b�g���܂��B
//	valprcp:�g�������֐��A�h���X�ł��B�K�v���Ȃ����0���Z�b�g���Ă��������B
//	usrp:	���[�U�[���w�肷��|�C���^�ł��B�K�v���Ȃ����0���Z�b�g���Ă��������B
//	�A�l:	���������e�L�X�g�̎����A��܂��B�G���[�̏ꍇ��0���A��܂��B

#	ifdef	__cplusplus
	}
#	endif
#	endif

