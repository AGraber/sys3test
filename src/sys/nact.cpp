/*
	ALICE SOFT SYSTEM 3 for Win32

	[ NACT ]
*/

#include <stdarg.h>
#include "nact.h"
#include "ags.h"
#include "mako.h"
#include "dri.h"
#include "../fileio.h"
#include "crc32.h"

extern SDL_Window* g_window;

// ������

NACT::NACT(int sys_ver, uint32 crc32, const char* font_file, const char* playlist)
	: sys_ver(sys_ver), crc32(crc32)
{
	// �f�o�b�O�R���\�[���N��
	initialize_console();

	// AG00.DAT�ǂݍ���
	FILEIO* fio = new FILEIO();
	if(fio->Fopen("AG00.DAT", FILEIO_READ_BINARY)) {
		int d0, d1, d2, d3;
		char string[MAX_CAPTION];
		fio->Fgets(string);
		sscanf_s(string, "%d,%d,%d,%d", &d0, &d1, &d2, &d3);
		for(int i = 0; i < d1; i++) {
			// �����̓ǂݍ���
			fio->Fgets(string);
			memcpy(caption_verb[i], string, sizeof(string));
		}
		for(int i = 0; i < d2; i++) {
			// �ړI��̓ǂݍ���
			fio->Fgets(string);
			memcpy(caption_obj[i], string, sizeof(string));
		}
		fio->Fclose();
	}
	delete fio;

	// ADISK.DAT
	if (crc32 == CRC32_PROG_OMAKE)
		strcpy_s(adisk, 16, "AGAME.DAT");
	else
		strcpy_s(adisk, 16, "ADISK.DAT");

	// �V�i���I�Ǘ�
	scenario_data = NULL;
	load_scenario(0);
	scenario_page = 0;
	scenario_addr = 2;
	label_depth = page_depth = 0;

	// �ϐ�������
	memset(var, 0, sizeof(var));
	memset(var_stack, 0, sizeof(var_stack));
	memset(tvar, 0, sizeof(tvar));
	memset(tvar_stack, 0, sizeof(tvar_stack));

	// �R�}���h������
	column = true;		// ���W���[�h
	wait_keydown = true;	// �E�F�C�g���̃L�[��t
	seed = SDL_GetTicks();	// �����̎�
	text_wait_time = 100;	// ���b�Z�[�W�\���̃E�F�C�g
	text_wait_enb = false;
	text_skip_enb = false;
	mouse_sence = 16;	// �}�E�X�ړ����m

	menu_window = text_window = 1;
	menu_index = 0;
	show_push = true;
	clear_text = true;

	verb_obj = false;
	set_palette = false;

	tvar_index = 0;

	pcm_index = 0;
	memset(pcm, 0, sizeof(pcm));

	// �e��N���X����
	ags = new AGS(this, font_file);
	mako = new MAKO(this, playlist);

#ifdef USE_JOY
	// ���͏�����
	joy_num = joyGetNumDevs();
	if(joy_num) {
		joyGetDevCaps(JOYSTICKID1, &joycaps, sizeof(JOYCAPS));
	}
#endif

	terminate = false;
}

NACT::~NACT()
{
	// �e��N���X�J��
	if(ags) {
		delete ags;
	}
	if(mako) {
		delete mako;
	}

	// �V�i���I�J��
	if(scenario_data) {
		free(scenario_data);
	}

	// �f�o�b�O�R���\�[���J��
	release_console();
}

void NACT::mainloop()
{
	int sleep_cnt = 0;

	while(!terminate) {
		switch (sys_ver) {
		case 1:
			execute(static_cast<NACT_Sys1*>(this));
			break;
		case 2:
			execute(static_cast<NACT_Sys2*>(this));
			break;
		case 3:
			execute(static_cast<NACT_Sys3*>(this));
			break;
		}
		// 512�R�}���h���s����Sleep(10)
		if(!(sleep_cnt = (sleep_cnt + 1) & 0x1ff)) {
			SDL_Delay(10);
		}
	}
}

// �R�}���h�p�[�T

template <class T>
void NACT::execute(T* impl)
{
	// �A�h���X�̊m�F
	if(scenario_addr < 2 || scenario_addr >= scenario_size) {
		fatal("Scenario error");
	}

	// �v���I�ȃG���[���� or ����I��
	if(fatal_error) {
		if(!post_quit) {
			terminate = true;
		}
		post_quit = true;
		return;
	}

	if(sys_ver == 1 && scenario_page == 0 && scenario_addr == 2) {
		impl->opening();
	}

	// �P�R�}���h���s
	uint8 cmd = getd();

	if(set_palette && cmd != 'P') {
		// �p���b�g�ݒ肪�I�����
		ags->flush_screen(true);
		set_palette = false;
	}
	if(verb_obj && cmd != '[' && cmd != ':') {
		// ����-�ړI�ꃁ�j���[�̕\��
		scenario_addr--;
		impl->cmd_open_verb();
		return;
	}

	switch(cmd) {
		case '!':
			impl->cmd_calc();
			break;
		case '{':
			impl->cmd_branch();
			break;
		case '}':
			break;
		case '@':
			impl->cmd_label_jump();
			break;
		case '\\':
			impl->cmd_label_call();
			break;
		case '&':
			impl->cmd_page_jump();
			break;
		case '%':
			impl->cmd_page_call();
			break;
		case '$':
			impl->cmd_set_menu();
			break;
		case '[':
			impl->cmd_set_verbobj();
			break;
		case ':':
			impl->cmd_set_verbobj2();
			break;
		case ']':
			impl->cmd_open_menu();
			break;
		case 'A':
			impl->cmd_a();
			break;
		case 'B':
			impl->cmd_b();
			break;
		case 'D':
			impl->cmd_d();
			break;
		case 'E':
			impl->cmd_e();
			break;
		case 'F':
			impl->cmd_f();
			break;
		case 'G':
			impl->cmd_g();
			break;
		case 'H':
			impl->cmd_h();
			break;
		case 'I':
			impl->cmd_i();
			break;
		case 'J':
			impl->cmd_j();
			break;
		case 'K':
			impl->cmd_k();
			break;
		case 'L':
			impl->cmd_l();
			break;
		case 'M':
			impl->cmd_m();
			break;
		case 'N':
			impl->cmd_n();
			break;
		case 'O':
			impl->cmd_o();
			break;
		case 'P':
			impl->cmd_p();
			break;
		case 'Q':
			impl->cmd_q();
			break;
		case 'R':
			impl->cmd_r();
			break;
		case 'S':
			impl->cmd_s();
			break;
		case 'T':
			impl->cmd_t();
			break;
		case 'U':
			impl->cmd_u();
			break;
		case 'V':
			impl->cmd_v();
			break;
		case 'W':
			impl->cmd_w();
			break;
		case 'X':
			impl->cmd_x();
			break;
		case 'Y':
			impl->cmd_y();
			break;
		case 'Z':
			impl->cmd_z();
			break;
		default:
			if(cmd == 0x20 || (0xa1 <= cmd && cmd <= 0xdd)) {
				// message (1 byte)
				char string[2];
				string[0] = cmd;
				string[1] = '\0';
				ags->draw_text(string);
				if(crc32 == CRC32_DPS && !ags->draw_menu) {
					text_refresh = false;
				}
				
				if(!ags->draw_menu && text_wait_enb && cmd != 0x20) {
					Uint32 dwTime = SDL_GetTicks() + text_wait_time;
					for(;;) {
						if(terminate) {
							return;
						}
						RND = get_key();
						if(RND && wait_keydown) {
							break;
						}
						if(dwTime <= SDL_GetTicks()) {
							break;
						}
						SDL_Delay(16);
					}
				}

#if defined(_DEBUG_CONSOLE)
				output_console(string);
#endif
			} else if((0x81 <= cmd && cmd <= 0x9f) || 0xe0 <= cmd) {
				// message (2 bytes)
				char string[3];
				string[0] = cmd;
				string[1] = getd();
				string[2] = '\0';
				ags->draw_text(string);
				if(crc32 == CRC32_DPS && !ags->draw_menu) {
					text_refresh = false;
				}
				
				if(!ags->draw_menu && text_wait_enb) {
					Uint32 dwTime = SDL_GetTicks() + text_wait_time;
					for(;;) {
						if(terminate) {
							return;
						}
						RND = get_key();
						if(RND && wait_keydown) {
							break;
						}
						if(dwTime <= SDL_GetTicks()) {
							break;
						}
						SDL_Delay(16);
					}
				}

#if defined(_DEBUG_CONSOLE)
				output_console(string);
#endif
			} else {
				fatal("Unknown command %2x at %d", cmd, scenario_addr);
			}
			break;
	}
}

void NACT::load_scenario(int page)
{
	if(scenario_data) {
		free(scenario_data);
	}
	DRI* dri = new DRI();
	if((scenario_data = dri->load(adisk, page + 1, &scenario_size)) == NULL) {
		fatal("Cannot load scenario %d", page);
	}
	delete dri;
}

// ���ʊ֐�

uint16 NACT::random(uint16 range)
{
	// xorhift32
	seed = seed ^ (seed << 13);
	seed = seed ^ (seed >> 17);
	seed = seed ^ (seed << 15);
	return (uint16)(((uint32)range * (seed & 0xffff)) >> 16) + 1;
}

// WinMain�Ƃ̃C���^�[�t�F�[�X

int NACT::get_screen_height()
{
	return ags->screen_height;
}

void NACT::select_cursor()
{
	ags->select_cursor();
}

void NACT::select_sound(int dev)
{
	// �����I�ɉ�����ύX����
	int page = mako->current_music;
	int old_dev = (1 <= page && page <= 99 && mako->cd_track[page]) ? 1 : 0;

	for(int i = 1; i <= 99; i++) {
		mako->cd_track[i] = dev ? i : 0;
	}

	// �f�o�C�X���ύX���ꂽ�ꍇ�͍ĉ��t����
	if(dev != old_dev && page) {
		mako->stop_music();
		mako->play_music(page);
	}
}

void NACT::fatal(const char* format, ...) {
	char buf[512];
	va_list args;
	va_start(args, format);
	vsnprintf(buf, sizeof buf, format, args);
	SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "system3", buf, g_window);
	fatal_error = true;
}

NACT* NACT::create(const char* game_id, const char* font_file, const char* playlist) {
	uint32 crc32 = NACT::calc_crc32(game_id);
	int sys_ver = NACT::get_sys_ver(crc32);
	switch (sys_ver) {
	case 1:
		return new NACT_Sys1(crc32, font_file, playlist);
	case 2:
		return new NACT_Sys2(crc32, font_file, playlist);
	default:
		return new NACT_Sys3(crc32, font_file, playlist);
	}
}
