/************************************************************************/
/*																		*/
/*	マルチキー管理				by H.Watanabe							*/
/*	Ver1.00	2009/04/05 13:52:04	作成									*/
/*																		*/
/************************************************************************/

#	ifndef	MKEY_H
#	define	MKEY_H
#	ifdef	__cplusplus
	extern "C"{
#	endif

#	define	MKEY_SHIFT		0x00010000
#	define	MKEY_CTRL		0x00020000
#	define	MKEY_ALT		0x00040000
#	define	MKEY_ALTSHIFT	(MKEY_ALT|MKEY_SHIFT)

#	define	MKEY_PGUP		0xff21
#	define	MKEY_PGDOWN		0xff22
#	define	MKEY_END		0xff23
#	define	MKEY_HOME		0xff24
#	define	MKEY_LEFT		0xff25
#	define	MKEY_UP			0xff26
#	define	MKEY_RIGHT		0xff27
#	define	MKEY_DOWN		0xff28
#	define	MKEY_INSERT		0xff2d
#	define	MKEY_DELETE		0xff2e
#	define	MKEY_HELP		0xff2f

#	define	MKEY_ESC_ESC	0xff1b
#	ifdef	CURSES
#	define	MKEY_ESC		MKEY_ESC_ESC
#	else
#	define	MKEY_ESC		0x1b
#	endif

#	define	MKEY_ESC_0		0xff30
#	define	MKEY_ESC_1		0xff31
#	define	MKEY_ESC_2		0xff32
#	define	MKEY_ESC_3		0xff33
#	define	MKEY_ESC_4		0xff34
#	define	MKEY_ESC_5		0xff35
#	define	MKEY_ESC_6		0xff36
#	define	MKEY_ESC_7		0xff37
#	define	MKEY_ESC_8		0xff38
#	define	MKEY_ESC_9		0xff39

#	define	MKEY_ESC_AT		0xff40
#	define	MKEY_ESC_A		0xff41
#	define	MKEY_ESC_B		0xff42
#	define	MKEY_ESC_C		0xff43
#	define	MKEY_ESC_D		0xff44
#	define	MKEY_ESC_E		0xff45
#	define	MKEY_ESC_F		0xff46
#	define	MKEY_ESC_G		0xff47
#	define	MKEY_ESC_H		0xff48
#	define	MKEY_ESC_I		0xff49
#	define	MKEY_ESC_J		0xff4a
#	define	MKEY_ESC_K		0xff4b
#	define	MKEY_ESC_L		0xff4c
#	define	MKEY_ESC_M		0xff4d
#	define	MKEY_ESC_N		0xff4e
#	define	MKEY_ESC_O		0xff4f
#	define	MKEY_ESC_P		0xff50
#	define	MKEY_ESC_Q		0xff51
#	define	MKEY_ESC_R		0xff52
#	define	MKEY_ESC_S		0xff53
#	define	MKEY_ESC_T		0xff54
#	define	MKEY_ESC_U		0xff55
#	define	MKEY_ESC_V		0xff56
#	define	MKEY_ESC_W		0xff57
#	define	MKEY_ESC_X		0xff58
#	define	MKEY_ESC_Y		0xff59
#	define	MKEY_ESC_Z		0xff5a
#	define	MKEY_ESC_LBO	0xff5b
#	define	MKEY_ESC_YEN	0xff5c
#	define	MKEY_ESC_LBC	0xff5d
#	define	MKEY_ESC_HAT	0xff5e
#	define	MKEY_ESC__		0xff5f

#	define	MKEY_ESC_a		0xff61
#	define	MKEY_ESC_b		0xff62
#	define	MKEY_ESC_c		0xff63
#	define	MKEY_ESC_d		0xff64
#	define	MKEY_ESC_e		0xff65
#	define	MKEY_ESC_f		0xff66
#	define	MKEY_ESC_g		0xff67
#	define	MKEY_ESC_h		0xff68
#	define	MKEY_ESC_i		0xff69
#	define	MKEY_ESC_j		0xff6a
#	define	MKEY_ESC_k		0xff6b
#	define	MKEY_ESC_l		0xff6c
#	define	MKEY_ESC_m		0xff6d
#	define	MKEY_ESC_n		0xff6e
#	define	MKEY_ESC_o		0xff6f
#	define	MKEY_ESC_p		0xff70
#	define	MKEY_ESC_q		0xff71
#	define	MKEY_ESC_r		0xff72
#	define	MKEY_ESC_s		0xff73
#	define	MKEY_ESC_t		0xff74
#	define	MKEY_ESC_u		0xff75
#	define	MKEY_ESC_v		0xff76
#	define	MKEY_ESC_w		0xff77
#	define	MKEY_ESC_x		0xff78
#	define	MKEY_ESC_y		0xff79
#	define	MKEY_ESC_z		0xff7a

#	define	MKEY_F1			0xff80
#	define	MKEY_F2			0xff81
#	define	MKEY_F3			0xff82
#	define	MKEY_F4			0xff83
#	define	MKEY_F5			0xff84
#	define	MKEY_F6			0xff85
#	define	MKEY_F7			0xff86
#	define	MKEY_F8			0xff87
#	define	MKEY_F9			0xff88
#	define	MKEY_F10		0xff89
#	define	MKEY_F11		0xff8a
#	define	MKEY_F12		0xff8b

	typedef struct tgMKEY	stMKEY;

	stMKEY	*mkeyOpen(void);
	void	mkeyClose( stMKEY *mkp );
	void	mkeyPut( stMKEY *mkp, int key );
	void	mkeyAlc( stMKEY *mkp, int mkey );
	int		mkeyGet( stMKEY *mkp );
	int		mkeyPeek( stMKEY *mkp );

#	ifdef	__cplusplus
	}
#	endif
#	endif

