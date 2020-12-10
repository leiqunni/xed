/************************************************************************/
/*																		*/
/*	�Z���N�V�����Ǘ�			by H.Watanabe							*/
/*	Ver1.00	2009/04/05 13:52:04	�쐬									*/
/*																		*/
/************************************************************************/

#	ifdef	unix

#	ifndef	XSEL_H
#	define	XSEL_H
#	ifdef	__cplusplus
	extern "C"{
#	endif

#	include	<X11/Xlib.h>
#	include	"db2.h"														/* �v���O�������ėp�f�[�^�x�[�X�V�X�e��*/

	typedef struct tgXSEL	stXSEL;

	stXSEL	*xselOpen( Display *dpyp, Window win );
	void	xselClose( stXSEL *xselp );
	int		xselOwner( stXSEL *xselp, unsigned long tim2 );
	void	xselClear( stXSEL *xselp );
	void	xselDemand( stXSEL *xselp, unsigned long tim2 );
	void	xselRequest( stXSEL *xselp, const XEvent *eventp, const stDB *cbdbp );
	stDB	*xselNotify( stXSEL *xselp, const XEvent *eventp );

#	ifdef	__cplusplus
	}
#	endif
#	endif

#	endif

