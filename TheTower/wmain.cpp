#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <fstream>
#include <allegro5/allegro.h>
#include <allegro5/allegro_native_dialog.h>
#include <allegro5/allegro_windows.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_acodec.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_font.h>

#include "Tower.h"

#define WIDTH	800
#define HEIGHT	600
//#define FULLSCREEN

static ALLEGRO_DISPLAY* window;
static ALLEGRO_EVENT_QUEUE* Events;
static ALLEGRO_TIMER* gameTimer;
ALLEGRO_TRANSFORM screenTransform;

ALLEGRO_KEYBOARD_STATE keyboard;
ALLEGRO_MOUSE_STATE mouse;

class Tower* G_pTower;

static bool CloseApplication = false;

double timeStepDelay = 1.0 / 60.0;
double timeStepTimer;

struct KeyValuePair
{
	KeyValuePair(int key, std::string value)
	{
		Key = key;
		Value = value;
	};
	int Key;
	std::string Value;
};

std::vector<KeyValuePair*> EventMap =
{
	new KeyValuePair(ALLEGRO_EVENT_JOYSTICK_AXIS, "ALLEGRO_EVENT_JOYSTICK_AXIS"),
	new KeyValuePair(ALLEGRO_EVENT_JOYSTICK_BUTTON_DOWN, "ALLEGRO_EVENT_JOYSTICK_BUTTON_DOWN"),
	new KeyValuePair(ALLEGRO_EVENT_JOYSTICK_BUTTON_UP, "ALLEGRO_EVENT_JOYSTICK_BUTTON_UP"),
	new KeyValuePair(ALLEGRO_EVENT_JOYSTICK_CONFIGURATION, "ALLEGRO_EVENT_JOYSTICK_CONFIGURATION"),
	new KeyValuePair(ALLEGRO_EVENT_KEY_DOWN,  "ALLEGRO_EVENT_KEY_DOWN"),
	new KeyValuePair(ALLEGRO_EVENT_KEY_CHAR,  "ALLEGRO_EVENT_KEY_CHAR"),
	new KeyValuePair(ALLEGRO_EVENT_KEY_UP,  "ALLEGRO_EVENT_KEY_UP"),
	new KeyValuePair(ALLEGRO_EVENT_MOUSE_AXES,  "ALLEGRO_EVENT_MOUSE_AXES"),
	new KeyValuePair(ALLEGRO_EVENT_MOUSE_BUTTON_DOWN,  "ALLEGRO_EVENT_MOUSE_BUTTON_DOWN"),
	new KeyValuePair(ALLEGRO_EVENT_MOUSE_BUTTON_UP,  "ALLEGRO_EVENT_MOUSE_BUTTON_UP"),
	new KeyValuePair(ALLEGRO_EVENT_MOUSE_ENTER_DISPLAY,  "ALLEGRO_EVENT_MOUSE_ENTER_DISPLAY"),
	new KeyValuePair(ALLEGRO_EVENT_MOUSE_LEAVE_DISPLAY,  "ALLEGRO_EVENT_MOUSE_LEAVE_DISPLAY"),
	new KeyValuePair(ALLEGRO_EVENT_MOUSE_WARPED,  "ALLEGRO_EVENT_MOUSE_WARPED"),
	new KeyValuePair(ALLEGRO_EVENT_TIMER,  "ALLEGRO_EVENT_TIMER"),
	new KeyValuePair(ALLEGRO_EVENT_DISPLAY_EXPOSE,  "ALLEGRO_EVENT_DISPLAY_EXPOSE"),
	new KeyValuePair(ALLEGRO_EVENT_DISPLAY_RESIZE,  "ALLEGRO_EVENT_DISPLAY_RESIZE"),
	new KeyValuePair(ALLEGRO_EVENT_DISPLAY_CLOSE,  "ALLEGRO_EVENT_DISPLAY_CLOSE"),
	new KeyValuePair(ALLEGRO_EVENT_DISPLAY_LOST,  "ALLEGRO_EVENT_DISPLAY_LOST"),
	new KeyValuePair(ALLEGRO_EVENT_DISPLAY_FOUND,  "ALLEGRO_EVENT_DISPLAY_FOUND"),
	new KeyValuePair(ALLEGRO_EVENT_DISPLAY_SWITCH_IN,  "ALLEGRO_EVENT_DISPLAY_SWITCH_IN"),
	new KeyValuePair(ALLEGRO_EVENT_DISPLAY_SWITCH_OUT,  "ALLEGRO_EVENT_DISPLAY_SWITCH_OUT"),
	new KeyValuePair(ALLEGRO_EVENT_DISPLAY_ORIENTATION,  "ALLEGRO_EVENT_DISPLAY_ORIENTATION"),
	new KeyValuePair(ALLEGRO_EVENT_DISPLAY_HALT_DRAWING,  "ALLEGRO_EVENT_DISPLAY_HALT_DRAWING"),
	new KeyValuePair(ALLEGRO_EVENT_DISPLAY_RESUME_DRAWING,  "ALLEGRO_EVENT_DISPLAY_RESUME_DRAWING"),
	new KeyValuePair(ALLEGRO_EVENT_TOUCH_BEGIN,  "ALLEGRO_EVENT_TOUCH_BEGIN"),
	new KeyValuePair(ALLEGRO_EVENT_TOUCH_END,  "ALLEGRO_EVENT_TOUCH_END"),
	new KeyValuePair(ALLEGRO_EVENT_TOUCH_MOVE,  "ALLEGRO_EVENT_TOUCH_MOVE"),
	new KeyValuePair(ALLEGRO_EVENT_TOUCH_CANCEL, "ALLEGRO_EVENT_TOUCH_CANCEL"),
	new KeyValuePair(ALLEGRO_EVENT_DISPLAY_CONNECTED, "ALLEGRO_EVENT_DISPLAY_CONNECTED"),
	new KeyValuePair(ALLEGRO_EVENT_DISPLAY_DISCONNECTED, "ALLEGRO_EVENT_DISPLAY_DISCONNECTED")
};

bool WinCallback(ALLEGRO_DISPLAY* display, UINT message, WPARAM wparam, LPARAM lparam, LRESULT* result, void* userdata)
{
	return false;
}

void EventHandle()
{
	do
	{
		ALLEGRO_EVENT event;
		al_wait_for_event(Events, &event);

		std::string eventName = "";
		for (auto& e : EventMap) if (e->Key == event.type) eventName = e->Value;
		if (eventName != "ALLEGRO_EVENT_TIMER" && eventName != "ALLEGRO_EVENT_MOUSE_AXES")
		{
			printf(eventName.c_str());
		}

		switch (event.type)
		{
		case ALLEGRO_EVENT_DISPLAY_CLOSE:
			CloseApplication = true;
			break;
		case ALLEGRO_EVENT_TIMER:
			if (al_get_time() - timeStepTimer >= timeStepDelay)
			{
				G_pTower->UpdateScreen();
				timeStepTimer = al_get_time();
			}
			G_pTower->DrawScreen();
			break;
		case ALLEGRO_EVENT_KEY_DOWN:
			if (event.keyboard.keycode == ALLEGRO_KEY_ESCAPE)
			{
				CloseApplication = true;
			}
			al_get_keyboard_state(&keyboard);
			break;
		case ALLEGRO_EVENT_KEY_UP:
			al_get_keyboard_state(&keyboard);
			G_pTower->OnKeyUp(event.keyboard.keycode);
			break;
		case ALLEGRO_EVENT_KEY_CHAR:
			printf(" - %c", event.keyboard.unichar);
			break;
		case ALLEGRO_EVENT_MOUSE_AXES:
			G_pTower->UpdateMouseAxis(event.mouse.x, event.mouse.y);
			break;
		case ALLEGRO_EVENT_MOUSE_BUTTON_DOWN:
			G_pTower->UpdateMouseDownState(event.mouse.button, event.mouse.x, event.mouse.y);
			break;
		case ALLEGRO_EVENT_MOUSE_BUTTON_UP:
			G_pTower->UpdateMouseUpState(event.mouse.button, event.mouse.x, event.mouse.y);
			break;
		}

		if (eventName != "ALLEGRO_EVENT_TIMER" && eventName != "ALLEGRO_EVENT_MOUSE_AXES")
		{
			printf("\n");
		}
	} while (!al_is_event_queue_empty(Events));
}

int main()
{
	if (!al_init())
	{
		al_show_native_message_box(nullptr, "Error!", nullptr, "Could not initialize Allegro5", nullptr, 0);
		return -1;
	}
	if (!al_install_keyboard())
	{
		al_show_native_message_box(nullptr, "Error!", nullptr, "Could not initialize Allegro5 Keyboard hook", nullptr, 0);
		return -1;
	}
	if (!al_install_mouse())
	{
		al_show_native_message_box(nullptr, "Error!", nullptr, "Could not initialize Allegro5 Mouse hook", nullptr, 0);
		return -1;
	}
	if (!al_install_audio() || !al_init_acodec_addon())
	{
		al_show_native_message_box(nullptr, "Error!", nullptr, "Could not initialize Allegro5 audio hook", nullptr, 0);
		return -1;
	}
	if (!al_init_image_addon())
	{
		al_show_native_message_box(nullptr, "Error!", nullptr, "Could not initialize Allegro5 image hook", nullptr, 0);
		return -1;
	}
	if (!al_init_ttf_addon())
	{
		al_show_native_message_box(nullptr, "Error!", nullptr, "Could not initialize Allegro5 ttf font hook", nullptr, 0);
		return -1;
	}
	if (al_init_primitives_addon() == false)
	{
		al_show_native_message_box(nullptr, "Error!", nullptr, "Could not initialize Allegro5 primitive shapes.", nullptr, 0);
		return -1;
	}

	al_set_new_display_option(ALLEGRO_SAMPLE_BUFFERS, 1, ALLEGRO_REQUIRE);
	al_set_new_display_option(ALLEGRO_SAMPLES, 8, ALLEGRO_SUGGEST);
	al_set_new_display_option(ALLEGRO_SINGLE_BUFFER, 1, ALLEGRO_REQUIRE);
	al_set_new_display_option(ALLEGRO_DEPTH_SIZE, 32, ALLEGRO_REQUIRE);
	al_set_new_display_option(ALLEGRO_STENCIL_SIZE, 8, ALLEGRO_REQUIRE);
	int flags = ALLEGRO_DIRECT3D_INTERNAL;
	int windowFactor = 0;
#ifdef FULLSCREEN
	flags |= ALLEGRO_FULLSCREEN;
#else
	flags |= ALLEGRO_WINDOWED;
	windowFactor = 200;
#endif
	al_set_new_display_flags(flags);
	window = al_create_display(WIDTH, HEIGHT + windowFactor);
	if (window == nullptr)
	{
		al_show_native_message_box(nullptr, "Error!", nullptr, "Could not create Allegro5 Window", nullptr, 0);
		return -2;
	}
	al_set_new_bitmap_flags(ALLEGRO_VIDEO_BITMAP);
	al_grab_mouse(window);

	//const float scale_factor_x = ((float)al_get_display_width(window)) / 1024;
	//const float scale_factor_y = ((float)al_get_display_height(window)) / 768;

	//al_identity_transform(&screenTransform);
	//al_scale_transform(&screenTransform, scale_factor_x, scale_factor_y);
	//al_use_transform(&screenTransform);

	Events = al_create_event_queue();
	gameTimer = al_create_timer(1.0 / 244.0f);
	al_register_event_source(Events, al_get_timer_event_source(gameTimer));
	al_register_event_source(Events, al_get_keyboard_event_source());
	al_register_event_source(Events, al_get_mouse_event_source());
	al_register_event_source(Events, al_get_display_event_source(window));

	al_win_add_window_callback(window, WinCallback, 0);

	al_start_timer(gameTimer);

	G_pTower = new class Tower;
	if (G_pTower->bInit(window) == false)
	{
		al_show_native_message_box(nullptr, "Error!", nullptr, "Could not initialize the game data.", nullptr, 0);
		return -2;
	}

	al_set_display_flag(window, ALLEGRO_VIDEO_BITMAP, true);
	while (CloseApplication == false)
	{
		EventHandle();
	}

	al_destroy_display(window);
	al_destroy_timer(gameTimer);
	al_destroy_event_queue(Events);
	delete G_pTower;

	return 0;
}