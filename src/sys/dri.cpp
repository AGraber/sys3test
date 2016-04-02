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

uint8* DRI::load_mda(uint32 crc32, int page, int* size)
{
#if defined(_SYSTEM1)
	return NULL;
#elif defined(_SYSTEM2)
	return NULL;
#else
	// �f�[�^�擾
	const char* fname = NULL;

	switch(crc32) {
		case CRC32_AMBIVALENZ_FD:	// AmbivalenZ (FD)
		case CRC32_AMBIVALENZ_CD:	// AmbivalenZ (CD)
			fname = "AMUS_AMB.MDA";
			break;
		case CRC32_DPSALL:		// DPS�S��
			fname = "AMUS_ALL.MDA";
			break;
		case CRC32_FUNNYBEE_CD:		// Funny Bee (CD)
//		case CRC32_FUNNYBEE_PATCH:	// Funny Bee (CD + Patch)
		case CRC32_FUNNYBEE_FD:		// Funny Bee (FD)
			fname = "AMUS_BEE.MDA";
			break;
		case CRC32_ONLYYOU:		// Only You
		case CRC32_ONLYYOU_DEMO:	// Only You (DEMO)
			fname = "AMUS_OY.MDA";
			break;
		case CRC32_PROSTUDENTG_CD:	// Prostudent -G- (CD)
			fname = "AMUS_PSG.MDA";
			break;
		case CRC32_RANCE41:		// Rance 4.1
			fname = "AMUS_R41.MDA";
			break;
		case CRC32_RANCE42:		// Rance 4.2
			fname = "AMUS_R42.MDA";
			break;
		case CRC32_AYUMI_CD:		// ����݂���񕨌� (CD)
		case CRC32_AYUMI_JISSHA_256:	// ����݂���񕨌� ���ʔ�
		case CRC32_AYUMI_JISSHA_FULL:	// ����݂���񕨌� �t���J���[���ʔ�
			fname = "AMUS_AYM.MDA";
			break;
		case CRC32_YAKATA3_CD:		// �A���X�̊قR (CD)
		case CRC32_YAKATA3_FD:		// �A���X�̊قR (FD)
			fname = "AMUS_AL3.MDA";
			break;
		case CRC32_HASHIRIONNA2:	// ���菗�Q (Rance 4.x �q���g�f�B�X�N)
			fname = "AMUS_RG2.MDA";
			break;
		case CRC32_OTOMESENKI:		// ������L
			fname = "AMUS_OTM.MDA";
			break;
		case CRC32_MUGENHOUYOU:		// �����A�e
			fname = "AMUS_MGN.MDA";
			break;
	}

	if(fname == NULL) {
		return NULL;
	}

	FILE* fp = fopen(fname, "rb");
	if (!fp)
		return NULL;
	uint8 buf[4];

	// �y�[�W�̈ʒu���擾
	fread(buf, 4, 1, fp);
	int link_sector = buf[0] | (buf[1] << 8);
	int data_sector = buf[2] | (buf[3] << 8);

	if(page > (data_sector - link_sector) * 128 - 1) {
		// �y�[�W�ԍ��s��
		return NULL;
	}

	fseek(fp, (link_sector - 1) * 256 + (page - 1) * 2, SEEK_SET);
	fread(buf, 2, 1, fp);
	int disk_index = buf[0];
	int link_index = buf[1];

	if(disk_index == 0 || disk_index == 0x1a) {
		// ����
		return NULL;
	}

	// �f�[�^�擾
	fseek(fp, link_index * 2, SEEK_SET);
	fread(buf, 4, 1, fp);
	int start_sector = buf[0] | (buf[1] << 8);
	int end_sector = buf[2] | (buf[3] << 8);

	int buffer_size = (end_sector - start_sector) * 256;
	uint8* buffer = (uint8*)malloc(buffer_size);
	fseek(fp, (start_sector - 1) * 256, SEEK_SET);
	fread(buffer, buffer_size, 1, fp);

	fclose(fp);

	*size = buffer_size;
	return buffer;
#endif
}

