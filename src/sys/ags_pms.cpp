/*
	ALICE SOFT SYSTEM 3 for Win32

	[ AGS - pms loader ]
*/

#include "ags.h"
#include "crc32.h"

void AGS::load_pms(uint8* data, int page, int transparent)
{
	// �w�b�_�擾
	int sx = data[0x0] | (data[0x1] << 8);
	int sy = data[0x2] | (data[0x3] << 8);
	int ex = data[0x4] | (data[0x5] << 8);
	int ey = data[0x6] | (data[0x7] << 8);
	int width = ex - sx + 1;
	int height = ey - sy + 1;
	uint8 mask = data[0xa];

	// J�R�}���h�̏���
	if(set_cg_dest) {
		sx = cg_dest_x;
		sy = cg_dest_y;
		ex = sx + width - 1;
		ey = sy + height - 1;
		set_cg_dest = false;
	}

	// �p���b�g�擾
	uint32 tmp_palette[256];

	if(get_palette) {
		int p = 0x20;
		for(int i = 0; i < 16; i++) {
			for(int j = 0; j < 16; j++) {
				uint32 r = data[p++];
				uint32 g = data[p++];
				uint32 b = data[p++];
				tmp_palette[i * 16 + j] = SETPALETTE256(r, g, b);
			}
		}
#if defined(_SYSTEM3)
		if(nact->crc32 == CRC32_RANCE41 || nact->crc32 == CRC32_RANCE42 || nact->crc32 == CRC32_HASHIRIONNA2) {
			// �㉺16�F�͎擾���Ȃ�
			for(int i = 1; i < 15; i++) {
				for(int j = 0; j < 16; j++) {
					if(!(mask & (0x01 << i))) {
						program_palette[i * 16 + j] = tmp_palette[i * 16 + j];
					}
				}
			}
		} else if(nact->crc32 == CRC32_MUGENHOUYOU && transparent != -1) {
			// U�R�}���h�ł͏㉺32�F�͎擾���Ȃ�
			for(int i = 2; i < 14; i++) {
				for(int j = 0; j < 16; j++) {
					if(!(mask & (0x01 << i))) {
						program_palette[i * 16 + j] = tmp_palette[i * 16 + j];
					}
				}
			}
		} else {
#endif
			for(int i = 0; i < 16; i++) {
				for(int j = 0; j < 16; j++) {
					if(!(mask & (0x01 << i))) {
						program_palette[i * 16 + j] = tmp_palette[i * 16 + j];
					}
				}
			}
#if defined(_SYSTEM3)
		}
#endif
	}

	// �p���b�g�W�J
#if defined(_SYSTEM3)
	if((extract_palette && extract_palette_cg[page]) || nact->crc32 == CRC32_FUNNYBEE_CD) {
		if(nact->crc32 == CRC32_RANCE41 || nact->crc32 == CRC32_RANCE42 || nact->crc32 == CRC32_HASHIRIONNA2) {
			// �㉺16�F�͓W�J���Ȃ�
			for(int i = 1; i < 15; i++) {
				for(int j = 0; j < 16; j++) {
					if(!(mask & (0x01 << i))) {
						screen_palette[i * 16 + j] = program_palette[i * 16 + j];
					}
				}
			}
		} else if(nact->crc32 == CRC32_MUGENHOUYOU && transparent != -1) {
			// U�R�}���h�ł͏㉺32�F�͓W�J���Ȃ�
			for(int i = 2; i < 14; i++) {
				for(int j = 0; j < 16; j++) {
					if(!(mask & (0x01 << i))) {
						screen_palette[i * 16 + j] = program_palette[i * 16 + j];
					}
				}
			}
		} else {
			for(int i = 0; i < 16; i++) {
				for(int j = 0; j < 16; j++) {
					if(!(mask & (0x01 << i))) {
						screen_palette[i * 16 + j] = program_palette[i * 16 + j];
					}
				}
			}
		}
	}
#else
	if(extract_palette && extract_palette_cg[page]) {
		for(int i = 0; i < 16; i++) {
			for(int j = 0; j < 16; j++) {
				if(!(mask & (0x01 << i))) {
					screen_palette[i * 16 + j] = program_palette[i * 16 + j];
				}
			}
		}
	}
#endif

	// PMS�W�J
	uint8 cgdata[3][640];
	int p = 0x320;
	memset(cgdata, 0, sizeof(cgdata));

	for(int y = 0; y < height; y++) {
		int x = 0;
		while(x < width) {
			uint8 d1 = data[p++];
			if(d1 == 0xff) {
				int length = data[p++] + 3;
				memcpy(cgdata[0] + x, cgdata[1] + x, length);
				x += length;
			} else if(d1 == 0xfe) {
				int length = data[p++] + 3;
				memcpy(cgdata[0] + x, cgdata[2] + x, length);
				x += length;
			} else if(d1 == 0xfd) {
				int length = data[p++] + 4;
				uint8 d2 = data[p++];
				memset(cgdata[0] + x, d2, length);
				x += length;
			} else if(d1 == 0xfc) {
				int length = data[p++] + 3;
				uint8 d2 = data[p++];
				uint8 d3 = data[p++];
				for(int i = 0; i < length; i++) {
					cgdata[0][x++] = d2;
					cgdata[0][x++] = d3;
				}
			} else if(d1 == 0xfb || d1 == 0xfa || d1 == 0xf9 || d1 == 0xf8) {
				cgdata[0][x++] = data[p++];
			} else {
				cgdata[0][x++] = d1;
			}
		}

		// VRAM�ɓ]��
		if(extract_cg) {
			uint32* dest = &vram[dest_screen][y + sy][sx];
			if(transparent == -1) {
				for(int x = 0; x < width; x++) {
					cgdata[2][x] = cgdata[1][x];
					cgdata[1][x] = cgdata[0][x];
					dest[x] = cgdata[0][x];
				}
			} else {
				for(int x = 0; x < width; x++) {
					cgdata[2][x] = cgdata[1][x];
					cgdata[1][x] = cgdata[0][x];
					if(cgdata[0][x] != transparent) {
						dest[x] = cgdata[0][x];
					}
				}
			}
		}
	}

	// ��ʍX�V
	if(dest_screen == 0 && extract_cg) {
		draw_screen(sx, sy, width, height);
	}
}