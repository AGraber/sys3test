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

NACT::NACT(const char* game_id, const char* font_file, const char* playlist)
{
	mouse_x = mouse_y = 0;

	// �f�o�b�O�R���\�[���N��
	initialize_console();

	// SYSTEM3 ������

	crc32 = calc_crc32(game_id);
	fatal_error = post_quit = false;

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
#if defined(_PROG_OMAKE)
	strcpy_s(adisk, 16, "AGAME.DAT");
#else
	strcpy_s(adisk, 16, "ADISK.DAT");
#endif

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

#ifdef _SYSTEM1
	// SYETEM1 ������
	menu_max = 6;
	switch (crc32) {
	case CRC32_DPS:
		text_refresh = false;
		strcpy_s(tvar[0], 22, "�J�X�^��");
		strcpy_s(tvar[1], 22, "���[�i�X");
		strcpy_s(tvar[2], 22, "����");
		strcpy_s(tvar[3], 22, "�R���q");
		strcpy_s(tvar[4], 22, "����");
		strcpy_s(tvar[5], 22, "�ЂƂ�");
		strcpy_s(tvar[6], 22, "�^���q");
		break;
	case CRC32_INTRUDER:
		menu_max = 11;
		paint_x = paint_y = map_page = 0;
		break;
	}
#endif

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
		execute();
		// 512�R�}���h���s����Sleep(10)
		if(!(sleep_cnt = (sleep_cnt + 1) & 0x1ff)) {
			SDL_Delay(10);
		}
	}
}

// �R�}���h�p�[�T

void NACT::execute()
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

#if defined(_SYSTEM1)
	if(scenario_page == 0 && scenario_addr == 2) {
		opening();
	}
#endif

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
		cmd_open_verb();
		return;
	}

	switch(cmd) {
		case '!':
			cmd_calc();
			break;
		case '{':
			cmd_branch();
			break;
		case '}':
			break;
		case '@':
			cmd_label_jump();
			break;
		case '\\':
			cmd_label_call();
			break;
		case '&':
			cmd_page_jump();
			break;
		case '%':
			cmd_page_call();
			break;
		case '$':
			cmd_set_menu();
			break;
		case '[':
			cmd_set_verbobj();
			break;
		case ':':
			cmd_set_verbobj2();
			break;
		case ']':
			cmd_open_menu();
			break;
		case 'A':
			cmd_a();
			break;
		case 'B':
			cmd_b();
			break;
		case 'D':
			cmd_d();
			break;
		case 'E':
			cmd_e();
			break;
		case 'F':
			cmd_f();
			break;
		case 'G':
			cmd_g();
			break;
		case 'H':
			cmd_h();
			break;
		case 'I':
			cmd_i();
			break;
		case 'J':
			cmd_j();
			break;
		case 'K':
			cmd_k();
			break;
		case 'L':
			cmd_l();
			break;
		case 'M':
			cmd_m();
			break;
		case 'N':
			cmd_n();
			break;
		case 'O':
			cmd_o();
			break;
		case 'P':
			cmd_p();
			break;
		case 'Q':
			cmd_q();
			break;
		case 'R':
			cmd_r();
			break;
		case 'S':
			cmd_s();
			break;
		case 'T':
			cmd_t();
			break;
		case 'U':
			cmd_u();
			break;
		case 'V':
			cmd_v();
			break;
		case 'W':
			cmd_w();
			break;
		case 'X':
			cmd_x();
			break;
		case 'Y':
			cmd_y();
			break;
		case 'Z':
			cmd_z();
			break;
		default:
			if(cmd == 0x20 || (0xa1 <= cmd && cmd <= 0xdd)) {
				// message (1 byte)
				char string[2];
				string[0] = cmd;
				string[1] = '\0';
				ags->draw_text(string);
#if defined(_SYSTEM1)
				if(crc32 == CRC32_DPS && !ags->draw_menu) {
					text_refresh = false;
				}
#endif
				
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
#if defined(_SYSTEM1)
				if(crc32 == CRC32_DPS && !ags->draw_menu) {
					text_refresh = false;
				}
#endif
				
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
