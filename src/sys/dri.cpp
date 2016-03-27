/*
	ALICE SOFT SYSTEM 3 for Win32

	[ DRI ]
*/

#include "dri.h"
#include "crc32.h"
#include "../fileio.h"
#if defined(_SYSTEM1)
#include "../res1/resource.h"
#elif defined(_SYSTEM2)
#include "../res2/resource.h"
#else
#include "../res3/resource.h"
#endif

extern _TCHAR g_root[_MAX_PATH];

uint8* DRI::load(_TCHAR path[], int page, int* size)
{
	_TCHAR base_path[_MAX_PATH], file_path[_MAX_PATH];
	_tcscpy_s(base_path, _MAX_PATH, path);

	FILEIO* fio = new FILEIO();

	// �y�[�W�̈ʒu���擾
	base_path[0] = _T('A');
	_stprintf_s(file_path, _MAX_PATH, _T("%s%s"), g_root, base_path);

	if(!fio->Fopen(file_path, FILEIO_READ_BINARY)) {
		delete fio;
		return NULL;
	}

	int link_sector = fio->Fgetc();
	link_sector |= fio->Fgetc() << 8;
	int data_sector = fio->Fgetc();
	data_sector |= fio->Fgetc() << 8;

	if(page > (data_sector - link_sector) * 128 - 1) {
		// �y�[�W�ԍ��s��
		fio->Fclose();
		delete fio;
		return NULL;
	}

	fio->Fseek((link_sector - 1) * 256 + (page - 1) * 2, FILEIO_SEEK_SET);
	int disk_index = fio->Fgetc();
	int link_index = fio->Fgetc();

	if(disk_index == 0 || disk_index == 0x1a) {
		// ����
		fio->Fclose();
		delete fio;
		return NULL;
	}

	// A??.DAT�ȊO�Ƀ����N����Ă���ꍇ�̓t�@�C�����J������
	if(disk_index != 1) {
		base_path[0] = _T('A') + disk_index - 1;
		_stprintf_s(file_path, _MAX_PATH, _T("%s%s"), g_root, base_path);
		fio->Fclose();
		if(!fio->Fopen(file_path, FILEIO_READ_BINARY)) {
			delete fio;
			return NULL;
		}
	}

	// �f�[�^�擾
	fio->Fseek(link_index * 2, FILEIO_SEEK_SET);
	int start_sector = fio->Fgetc();
	start_sector |= fio->Fgetc() << 8;
	int end_sector = fio->Fgetc();
	end_sector |= fio->Fgetc() << 8;

	int buffer_size = (end_sector - start_sector) * 256;
	uint8* buffer = (uint8*)malloc(buffer_size);
	fio->Fseek((start_sector - 1) * 256, FILEIO_SEEK_SET);
	fio->Fread(buffer, 256, end_sector - start_sector);

	fio->Fclose();
	delete fio;

	*size = buffer_size;
	return buffer;
}

uint8* DRI::load_mda(HINSTANCE hInst, uint32 crc32, int page, int* size)
{
#if defined(_SYSTEM1)
	return NULL;
#elif defined(_SYSTEM2)
	return NULL;
#else
	// �f�[�^�擾
	HGLOBAL hGlobal = NULL;

	switch(crc32) {
		case CRC32_AMBIVALENZ_FD:	// AmbivalenZ (FD)
		case CRC32_AMBIVALENZ_CD:	// AmbivalenZ (CD)
			hGlobal = LoadResource(hInst, FindResource(hInst, MAKEINTRESOURCE(IDR_AMB), _T("mda")));
			break;
		case CRC32_DPSALL:		// DPS�S��
			hGlobal = LoadResource(hInst, FindResource(hInst, MAKEINTRESOURCE(IDR_ALL), _T("mda")));
			break;
		case CRC32_FUNNYBEE_CD:		// Funny Bee (CD)
//		case CRC32_FUNNYBEE_PATCH:	// Funny Bee (CD + Patch)
		case CRC32_FUNNYBEE_FD:		// Funny Bee (FD)
			hGlobal = LoadResource(hInst, FindResource(hInst, MAKEINTRESOURCE(IDR_BEE), _T("mda")));
			break;
		case CRC32_ONLYYOU:		// Only You
		case CRC32_ONLYYOU_DEMO:	// Only You (DEMO)
			hGlobal = LoadResource(hInst, FindResource(hInst, MAKEINTRESOURCE(IDR_OY), _T("mda")));
			break;
		case CRC32_PROSTUDENTG_CD:	// Prostudent -G- (CD)
			hGlobal = LoadResource(hInst, FindResource(hInst, MAKEINTRESOURCE(IDR_PSG), _T("mda")));
			break;
		case CRC32_RANCE41:		// Rance 4.1
			hGlobal = LoadResource(hInst, FindResource(hInst, MAKEINTRESOURCE(IDR_R41), _T("mda")));
			break;
		case CRC32_RANCE42:		// Rance 4.2
			hGlobal = LoadResource(hInst, FindResource(hInst, MAKEINTRESOURCE(IDR_R42), _T("mda")));
			break;
		case CRC32_AYUMI_CD:		// ����݂���񕨌� (CD)
		case CRC32_AYUMI_JISSHA_256:	// ����݂���񕨌� ���ʔ�
		case CRC32_AYUMI_JISSHA_FULL:	// ����݂���񕨌� �t���J���[���ʔ�
			hGlobal = LoadResource(hInst, FindResource(hInst, MAKEINTRESOURCE(IDR_AYM), _T("mda")));
			break;
		case CRC32_YAKATA3_CD:		// �A���X�̊قR (CD)
		case CRC32_YAKATA3_FD:		// �A���X�̊قR (FD)
			hGlobal = LoadResource(hInst, FindResource(hInst, MAKEINTRESOURCE(IDR_AL3), _T("mda")));
			break;
		case CRC32_HASHIRIONNA2:	// ���菗�Q (Rance 4.x �q���g�f�B�X�N)
			hGlobal = LoadResource(hInst, FindResource(hInst, MAKEINTRESOURCE(IDR_RG2), _T("mda")));
			break;
		case CRC32_OTOMESENKI:		// ������L
			hGlobal = LoadResource(hInst, FindResource(hInst, MAKEINTRESOURCE(IDR_OTM), _T("mda")));
			break;
		case CRC32_MUGENHOUYOU:		// �����A�e
			hGlobal = LoadResource(hInst, FindResource(hInst, MAKEINTRESOURCE(IDR_MGN), _T("mda")));
			break;
	}

	if(hGlobal == NULL) {
		return NULL;
	}

	uint8* data = (uint8*)LockResource(hGlobal);

	// �y�[�W�̈ʒu���擾
	int link_sector = data[0] | (data[1] << 8);
	int data_sector = data[2] | (data[3] << 8);

	if(page > (data_sector - link_sector) * 128 - 1) {
		// �y�[�W�ԍ��s��
		return NULL;
	}

	int disk_index = data[(link_sector - 1) * 256 + (page - 1) * 2 + 0];
	int link_index = data[(link_sector - 1) * 256 + (page - 1) * 2 + 1];

	if(disk_index == 0 || disk_index == 0x1a) {
		// ����
		return NULL;
	}

	// �f�[�^�擾
	int start_sector = data[link_index * 2 + 0] | (data[link_index * 2 + 1] << 8);
	int end_sector = data[link_index * 2 + 2] | (data[link_index * 2 + 3] << 8);

	int buffer_size = (end_sector - start_sector) * 256;
	uint8* buffer = (uint8*)malloc(buffer_size);
	memcpy(buffer, &data[(start_sector - 1) * 256], buffer_size);

	*size = buffer_size;
	return buffer;
#endif
}

