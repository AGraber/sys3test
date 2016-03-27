/*
	ALICE SOFT SYSTEM 3 for Win32

	[ NACT - crc32 ]
*/

#include "nact.h"
#include "crc32.h"
#include "../fileio.h"

extern _TCHAR g_root[_MAX_PATH];

uint32 NACT::calc_crc32()
{
	uint32 crc = 0;
	FILEIO* fio = new FILEIO();

	_TCHAR file_path[_MAX_PATH];
	_stprintf_s(file_path, _MAX_PATH, _T("%sADISK.DAT"), g_root);

	if(fio->Fopen(file_path, FILEIO_READ_BINARY)) {
		uint32 table[256];
		for(int i = 0; i < 256; i++) {
			uint32 c = i;
			for(int j = 0; j < 8; j++) {
				if(c & 1) {
					c = (c >> 1) ^ 0xedb88320;
				} else {
					c >>= 1;
				}
			}
			table[i] = c;
		}
		// ADISK.DAT�̐擪256bytes
		for(int i = 0; i < 256; i++) {
			int d = fio->Fgetc();
			uint32 c = ~crc;
			c = table[(c ^ d) & 0xff] ^ (c >> 8);
			crc = ~c;
		}
		fio->Fclose();
	}
#if defined(_SYSTEM2)
	if(crc == CRC32_SDPS) {
		// Super D.P.S�̏ꍇ��BDISK.DAT��CRC�����
		_stprintf_s(file_path, _MAX_PATH, _T("%sBDISK.DAT"), g_root);
		
		if(fio->Fopen(file_path, FILEIO_READ_BINARY)) {
			uint32 table[256];
			for(int i = 0; i < 256; i++) {
				uint32 c = i;
				for(int j = 0; j < 8; j++) {
					if(c & 1) {
						c = (c >> 1) ^ 0xedb88320;
					} else {
						c >>= 1;
					}
				}
				table[i] = c;
			}
			// BDISK.DAT�̐擪256bytes
			crc = 0;
			for(int i = 0; i < 256; i++) {
				int d = fio->Fgetc();
				uint32 c = ~crc;
				c = table[(c ^ d) & 0xff] ^ (c >> 8);
				crc = ~c;
			}
			fio->Fclose();
		}
	}
#endif
	delete fio;
#if 0
FILE* fp = fopen("crc32.txt", "w");
fprintf(fp, "%x\n", crc);
fclose(fp);
#endif
	return crc;
}

bool NACT::get_title(_TCHAR title[], int length)
{
#if defined(_SYSTEM1)

#if defined(_CRESCENT)
	_tcscpy_s(title, length, _T("�N���Z���g���[��������"));
	return true;
#elif defined(_DPS)
	_tcscpy_s(title, length, _T("D.P.S - Dream Program System"));
	return true;
#elif defined(_FUKEI)
	_tcscpy_s(title, length, _T("�w�x����u�w"));
	return true;
#elif defined(_INTRUDER)
	_tcscpy_s(title, length, _T("Intruder -�����~�̒T��-"));
	return true;
#elif defined(_TENGU)
	_tcscpy_s(title, length, _T("���ԂȂ��Ă񂮓`��"));
	return true;
#elif defined(_VAMPIRE)
	_tcscpy_s(title, length, _T("Little Vampire"));
	return true;
#else
	title[0] = _T('\0');
	return false;
#endif

#elif defined(_SYSTEM2)

	switch(crc32) {
		case CRC32_AYUMI_PROTO:
			_tcscpy_s(title, length, _T("����݂���񕨌� PROTO"));
			return true;
		case CRC32_SDPS_MARIA:
		case CRC32_SDPS_TONO:
		case CRC32_SDPS_KAIZOKU:
			_tcscpy_s(title, length, _T("Super D.P.S"));
			return true;
		case CRC32_PROSTUDENTG_FD:
			_tcscpy_s(title, length, _T("prostudent G"));
			return true;
	}
	title[0] = _T('\0');
	return false;

#else

	switch(crc32) {
		case CRC32_AMBIVALENZ_FD:
		case CRC32_AMBIVALENZ_CD:
			_tcscpy_s(title, length, _T("AmbivalenZ �|�񗥔w���|"));
			return true;
		case CRC32_DPSALL:
			_tcscpy_s(title, length, _T("D.P.S. �S��"));
			return true;
		case CRC32_FUNNYBEE_CD:
//		case CRC32_FUNNYBEE_PATCH:
		case CRC32_FUNNYBEE_FD:
			_tcscpy_s(title, length, _T("�F�������t�@�j�[Bee"));
			return true;
		case CRC32_ONLYYOU:
			_tcscpy_s(title, length, _T("Only You �|���I���̃W�����G�b�g�B�|"));
			return true;
		case CRC32_ONLYYOU_DEMO:
			_tcscpy_s(title, length, _T("Only You �|���I���̃W�����G�b�g�B�| �f����"));
			return true;
		case CRC32_PROSTUDENTG_CD:
			_tcscpy_s(title, length, _T("prostudent G"));
			return true;
		case CRC32_RANCE41:
			_tcscpy_s(title, length, _T("�����X 4.1 �`����H����~���I�`"));
			return true;
		case CRC32_RANCE42:
			_tcscpy_s(title, length, _T("�����X 4.2 �`�G���W�F���g�`"));
			return true;
		case CRC32_AYUMI_CD:
			_tcscpy_s(title, length, _T("����݂���񕨌�"));
			return true;
		case CRC32_AYUMI_JISSHA_256:
			_tcscpy_s(title, length, _T("����݂���񕨌� ���ʔ�"));
			return true;
		case CRC32_AYUMI_JISSHA_FULL:
			_tcscpy_s(title, length, _T("����݂���񕨌� �t���J���[���ʔ�"));
			return true;
		case CRC32_YAKATA3_CD:
		case CRC32_YAKATA3_FD:
			_tcscpy_s(title, length, _T("�A���X�̊�3"));
			return true;
		case CRC32_HASHIRIONNA2:
			_tcscpy_s(title, length, _T("���菗2"));
			return true;
		case CRC32_TOUSHIN2_SP:
			_tcscpy_s(title, length, _T("���_�s�s2 �����āA���ꂩ��c"));
			return true;
		case CRC32_OTOMESENKI:
			_tcscpy_s(title, length, _T("������L"));
			return true;
		case CRC32_NINGYO:
			_tcscpy_s(title, length, _T("�l�� -�f�q-"));
			return true;
		case CRC32_MUGENHOUYOU:
			_tcscpy_s(title, length, _T("�����A�e"));
			return true;
	}
	title[0] = _T('\0');
	return false;

#endif
}

