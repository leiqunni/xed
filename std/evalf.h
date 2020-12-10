/************************************************************************/
/*																		*/
/*	�����]��					by H.Watanabe							*/
/*	Ver1.00	2009/04/04 13:44:21	�쐬									*/
/*																		*/
/************************************************************************/

#	ifndef	EVALF_H
#	define	EVALF_H
#	ifdef	__cplusplus
	extern "C"{
#	endif

#	ifndef	FPTYP														/* FPTYP ����`����Ă��Ȃ�*/
#		ifdef	_WIN32													/* Windows �̏ꍇ*/
#			define	FPTYP	double										/* �P���x���g�p����ƕs�v�c�Ȃ��Ƃ����낢��N����̂Ŕ{���x���g�p����*/
#			define	FPSIZ	8
#		else															/* Windows �ȊO�̏ꍇ*/
#			define	FPTYP	float										/* WIN32 �ȊO�ł̓������������d�����ĒP���x���g�p����*/
#			define	FPSIZ	4
#		endif
#	endif

	typedef char *tpEVALDVAL( FPTYP *, const char *, void * );			// �l�̕]��������R�[���o�b�N�֐�

	char	*evaldUni( FPTYP *dstp, const char *txtp, tpEVALDVAL *valprcp, void *usrp );
	char	*evalf( FPTYP *dstp, const char *txtp, tpEVALDVAL *valprcp, void *usrp );

#	ifdef	__cplusplus
	}
#	endif
#	endif

