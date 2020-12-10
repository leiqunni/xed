/************************************************************************/
/*																		*/
/*	�I�v�V��������				by H.Watanabe							*/
/*	Ver1.00	2009/04/05 13:52:04	�쐬									*/
/*																		*/
/************************************************************************/

#	ifndef	OPT_H
#	define	OPT_H

#	include	"db2.h"														/* �v���O�������ėp�f�[�^�x�[�X�V�X�e��*/

#	ifndef	FPTYP														/* FPTYP ����`����Ă��Ȃ�*/
#		ifdef	_WIN32													/* Windows �̏ꍇ*/
#			define	FPTYP	double										/* �P���x���g�p����ƕs�v�c�Ȃ��Ƃ����낢��N����̂Ŕ{���x���g�p����*/
#			define	FPSIZ	8
#		else															/* Windows �ȊO�̏ꍇ*/
#			define	FPTYP	float										/* WIN32 �ȊO�ł̓������������d�����ĒP���x���g�p����*/
#			define	FPSIZ	4
#		endif
#	endif

#	ifndef	STDERR
#		ifdef	_WIN32													/* Windows �̏ꍇ*/
#			define	STDERR	stdout										/* �G���[�o�͕͂W���o��( �p�C�v���Ŗ�肪�������� )*/
#		else															/* ���̑�*/
#			define	STDERR	stderr										/* �G���[�o�͕͂W���G���[�o��*/
#		endif
#	endif

	typedef char	*tpOPTIONUSR( void *, char * );						// ���[�U�[�����֐�

	class	cOPT{
	public:
		stDB	*m_argdbp;												// ����DB
		stDB	*m_optdbp;												// -�I�v�V����DB
		stDB	*m_plsdbp;												// +�I�v�V����DB
		char	*m_titlep;												// �R�}���h�^�C�g��
		char	*m_verp;												// �o�[�W����
		char	*m_ownerp;												// �I�[�i�[
		char	*m_linep;												// ���C�����
		char	*m_extrap;												// �ǉ��I�v�V����
		stDB	*m_exedbp;												// ��v����Ύ��s���郏�C���h�J�[�h
		stDB	*m_igndbp;												// ��v����Ζ������郏�C���h�J�[�h
	public:
		cOPT();															// �R���X�g���N�^
		~cOPT();														// �f�X�g���N�^
		void	SetTitle( const char *strp );							// �^�C�g���Z�b�g
		void	SetVer( const char *strp );								// �o�[�W�����Z�b�g
		void	SetOwner( const char *strp );							// �I�[�i�[�Z�b�g
		void	SetLine( const char *strp );							// �������Z�b�g
		void	SetExtra( const char *strp );							// �g�����Z�b�g
		void	AllocInc( const char *idp, int *varp, const char *commentp );	// �C���N�������g�I�v�V�����o�^
		void	AllocInt( const char *idp, int *varp, const char *commentp );	// �C���g�I�v�V�����o�^
		void	AllocSet( const char *idp, int *varp, int var, const char *commentp );			// �Z�b�g�I�v�V�����o�^
		void	AllocFPTYP( const char *idp, FPTYP *varp, const char *commentp );				// ���������I�v�V�����o�^
		void	AllocStr( const char *idp, char *varp, int siz, const char *commentp );			// ������I�v�V�����o�^
		void	AllocDB( const char *idp, stDB **varp, const char *commentp );	// DB�I�v�V�����o�^
		void	AllocDef( const char *idp, stDB **varp, const char *commentp );	// ��`�I�v�V�����o�^
		void	AllocW( const char *idp, const char *idcp );			// -w �����o�^
		void	AllocUsr( const char *idp, tpOPTIONUSR *prcp, void *usrp, const char *commentp );				// ���[�U�[�I�v�V�����o�^
		void	Usage( const char *cmdp );								// �g�p�@�̕\��
		int		optwTst( const char *fnamp, int dirflg );				// -w �����e�X�g
		stDB	*PrcLin( char *txtp );									// �R�}���h���C������
		stDB	*PrcArg( int argc, char *argv[] );						// ��������
	};

#	endif

