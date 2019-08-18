/*
	ALICE SOFT SYSTEM 3 for Win32

	[ crc32 database ]
*/

#ifndef _CRC32_H_
#define _CRC32_H_

// ADISK.DAT�̐擪256bytes
#if defined(_SYSTEM1)

#define CRC32_CRESCENT	0x42351f2c	// �N���Z���g���[��������
#define CRC32_DPS		0x69ea4865	// D.P.S. - Dream Program System
#define CRC32_FUKEI		0x026de326	// �w�x����u�w (ALICE�̊�3 UNITBASE/Y_SYUREN.LZH)
#define CRC32_INTRUDER	0xa7520fb2	// Intruder -�����~�̒T��-
#define CRC32_TENGU		0xc942ff58	// ���ԂȂ��Ă񂮓`��
#define CRC32_VAMPIRE	0x957bcfbf	// Little Vampire
#define CRC32_BUNKASAI	0			// ���ԂȂ������ՑO��? (���m�F)

#elif defined(_SYSTEM2)

#define CRC32_AYUMI_PROTO	0x4e2f5678	// ����݂���񕨌� PROTO
#define CRC32_SDPS		0xc7a20cdf	// Super D.P.S
#define CRC32_SDPS_MARIA	0x80d4eaca	//	�}���A�ƃJ���p��
#define CRC32_SDPS_TONO		0xbb1edff1	//	����̐X
#define CRC32_SDPS_KAIZOKU	0xf81829e3	//	�C���Ƌ�
#define CRC32_PROSTUDENTG_FD	0x5ffbfee7	// Prostudent -G- (FD)

#else

#define CRC32_AMBIVALENZ_FD	0xa6b48dfe	// AmbivalenZ (FD)
#define CRC32_AMBIVALENZ_CD	0x4b10db69	// AmbivalenZ (CD)
#define CRC32_DPSALL		0xd48b4ec6	// DPS�S��
#define CRC32_FUNNYBEE_CD	0xe14e3971	// Funny Bee (CD)
#define CRC32_FUNNYBEE_PATCH	0xe14e3971	// Funny Bee (CD + Patch)
#define CRC32_FUNNYBEE_FD	0x731267fa	// Funny Bee (FD)
#define CRC32_ONLYYOU		0x832aeb97	// Only You
#define CRC32_ONLYYOU_DEMO	0xc1d13e44	// Only You (DEMO)
#define CRC32_PROSTUDENTG_CD	0xfb0e4a63	// Prostudent -G- (CD)
#define CRC32_RANCE41		0xa43fb4b6	// Rance 4.1
#define CRC32_RANCE42		0x04d24d1e	// Rance 4.2
#define CRC32_AYUMI_CD		0xd2bed9ee	// ����݂���񕨌� (CD)
#define CRC32_AYUMI_JISSHA_256	0x00d15a2b	// ����݂���񕨌� ���ʔ�
#define CRC32_AYUMI_JISSHA_FULL	0x5f66ff1d	// ����݂���񕨌� �t���J���[���ʔ�
#define CRC32_YAKATA3_CD	0x7f8f5e2a	// �A���X�̊قR (CD)
#define CRC32_YAKATA3_FD	0x58ebcc99	// �A���X�̊قR (FD)
#define CRC32_HASHIRIONNA2	0x09f47cbd	// ���菗�Q (Rance 4.x �q���g�f�B�X�N)
#define CRC32_TOUSHIN2_SP	0x2172c7b2	// ���_�s�s�Q �`�����āA���ꂩ��`
#define CRC32_OTOMESENKI	0x49a4db15	// ������L
#define CRC32_NINGYO		0xd491e7ab	// �l�� -�f�q-
#define CRC32_MUGENHOUYOU	0xbb27d1ba	// �����A�e

#endif

#endif
