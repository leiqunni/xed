/************************************************************************/
/*																		*/
/*	�t�H�[�}�b�g�t����������	by H.Watanabe							*/
/*	Ver1.00	2009/04/05 13:52:04	�쐬									*/
/*																		*/
/************************************************************************/

#	include	<stdarg.h>													/* �ψ���������`*/
#	include	<stdlib.h>													/* �W�����C�u�����֐�*/
#	include	<string.h>													/* �W��������֐�*/
#	include	"db2.h"														/* �v���O�������ėp�f�[�^�x�[�X�V�X�e��*/
#	include	"getfmt2.h"													/* �t�H�[�}�b�g�t����������*/

/************************************************************************/
/*	�t�H�[�}�b�g�t����������											*/
/************************************************************************/

char	*getfmt( const char *txtp, const char *format, ... ){

    va_list			ap;													// �ψ������[�N
	unsigned char	cmd;												// �t�H�[�}�b�g�R�}���h
	unsigned char	uni;												// ���l�]�����[�N
	const char		*bakp;												// �e�L�X�g�|�C���^�o�b�N�A�b�v
	int				bufsiz;												// �o�b�t�@�T�C�Y

	va_start( ap, format );												// �ψ����I�[�v��
	while( txtp ){														// �G���[���N����΃u���[�N
		cmd=*format++;													// �t�H�[�}�b�g�����񂩂�R�}���h�𓾂�
		if( !cmd ){														// �t�H�[�}�b�g������I�[
			break;														// ���[�v�I��
		}
		switch( cmd ){													// �t�H�[�}�b�g�R�}���h������ɂ�镪��
			case ' ':													// �X�L�b�v�X�y�[�X
				txtp+=strspn( txtp, "\t\r\n " );						// �������Ŏw�肵�������Q�ɂȂ����������������ɍŏ��Ɍ��ꂽ�ʒu�̃C���f�b�N�X��Ԃ��܂��B
				break;													// switch �I��
			case '%':
				bufsiz=0;												// �o�b�t�@�T�C�Y�N���A
				while( ( uni=(unsigned char)( ( cmd=*format++ )-'0' ) )<=9 ){	// �t�B�[���h�T�C�Y�ǂݍ���
					bufsiz=bufsiz*10+uni;								// �P�O�{���ĉ��Z����
				}
				if( cmd=='*' ){											// �t�B�[���h�T�C�Y�͉ψ������瓾��
					bufsiz=va_arg( ap, int );							// �ψ������� int �l��ǂݍ���
					cmd=*format++;										// ���̐��䕶���𓾂�
				}
				if( !bufsiz ){											// �o�b�t�@�T�C�Y�͎w�肳��Ă��Ȃ�
					bufsiz=256;											// �f�t�H���g�T�C�Y
				}
				switch( cmd ){
					case 'x':											// �P�U�i��
					case 'd':{											// �P�O�i��

						int		val;									// ���l�]�����[�N
						int		*ip;									// �f�X�g�l�[�V�������l�|�C���^

						val=strtol( bakp=txtp, (char**)&txtp, (cmd=='x')?16:10 );				// ������𐔒l�ɕϊ�����
						if( txtp==bakp ){								// �t�H�[�}�b�g�G���[
							txtp=0;										// �G���[�Z�b�g
							break;										// switch �I��
						}
						if( ( ip=va_arg( ap, int * ) )!=0 ){			// �ψ����𓾂�( �f�B�X�g�l�[�V�����|�C���^ )
							*ip=val;
						}
						break;											// switch �I��
					}
					case 'f':{											// ��������

						FPTYP	val;
						FPTYP	*fp;

						val	=strtod( bakp=txtp, (char**)&txtp );
						if( txtp<=bakp ){
							txtp=0;										// �G���[�Z�b�g
							break;										// switch �I��
						}
						if( ( fp=va_arg( ap, FPTYP * ) )!=0 ){			// �ψ����𓾂�( �f�B�X�g�l�[�V�����|�C���^ )
							*fp=val;
						}
						break;											// switch �I��
					}
					case 's':{											// ������

						int		len;									// ������
						char	*cp;									// �f�X�g�l�[�V����������|�C���^

						bakp=txtp;										// �e�L�X�g�|�C���^�o�b�N�A�b�v�Z�b�g
						if( *format==' ' ){								// �X�y�[�X
							txtp=strpbrk( txtp, "\t\r\n " );			// �^�u�A���s�A�X�y�[�X����������
						}else{											// �X�y�[�X�ȊO
							txtp=strchr( txtp, *format );				// ���������� ���������ŏ��Ɍ��ꂽ�ʒu�ւ̃|�C���^��Ԃ��܂��B������������Ȃ��� NULL ��Ԃ��܂��B
						}
						if( !txtp ){									// ������Ȃ��ꍇ
							txtp=bakp+strlen( bakp );					// ������I�[�ɃV�[�N����
						}
						len=txtp-bakp;
						if( len>bufsiz-1 ){
							len=bufsiz-1;
						}
						if( ( cp=va_arg( ap, char * ) )!=0 ){			// �ψ����𓾂�( �f�B�X�g�l�[�V�����|�C���^ )
							memcpy( cp, bakp, len );					// ��������R�s�[
							cp[len]=0;									// �O�^�[�~�l�[�g
						}
						break;											// switch �I��
					}
					case 'l':{											// ���x��

						int		len;									// ���x����
						char	*cp;									// �f�X�g�l�[�V����������|�C���^

						len=dbLblLen( txtp, bufsiz );					// �������x���������߂�
						if( ( cp=va_arg( ap, char * ) )!=0 ){			// �ψ����𓾂�( �f�B�X�g�l�[�V�����|�C���^ )
							memcpy( cp, txtp, len );					// ��������R�s�[
							cp[len]=0;									// �O�^�[�~�l�[�g
						}
						txtp+=len;
						break;											// switch �I��
					}
					case 'q':{											// �_�u���R�[�e�[�V����������

						char	*cp;									// �f�X�g�l�[�V����������|�C���^

						cp=va_arg( ap, char * );						// �ψ����𓾂�( �f�B�X�g�l�[�V�����|�C���^ )
						txtp=dbGetDQ( cp, bufsiz, txtp );				// �_�u���N�H�[�e�[�V�����ň͂܂ꂽ������𓾂�
						break;											// switch �I��
					}
					default:{											// �F���ł��Ȃ��L�����N�^( �V�X�e���G���[ )
						if( *txtp++=='%' ){								// �R�}���h�ȊO
							format--;									// �ꕶ�����߂�
						}else{											// ��v���Ȃ�
							txtp=0;										// �G���[�Z�b�g
						}
						break;											// switch �I��
					}
				}
				break;													// switch �I��
			default:
				if( cmd!=*txtp++ ){										// �Ⴄ����
					txtp=0;												// �G���[�Z�b�g
				}
				break;													// switch �I��
		}
	}
	va_end( ap );														// �ψ����N���[�Y
	return( (char*)( txtp ) );											// �e�L�X�g�|�C���^�������ă��^�[��
}

