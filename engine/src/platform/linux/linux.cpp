#include "octal/core/logger.h"
#include "platform/platform.h"
#include "platform/linux/linux.h"
#include <thread>
#include <chrono>
#include <cstdlib>
#include <cstring>
#include <cstdio>

namespace octal {
  // Initialize statue to null
  void* Platform::s_State = nullptr;

  bool Platform::Init(std::string& title, i16 x, i16 y, i16 w, i16 h) {
    //TODO: setup new and free to use our platform

    // intialize state
    s_State = new LinuxState{};
    // make things a little easier to do
    LinuxState* state = (LinuxState*) s_State;

    state->display = XOpenDisplay(nullptr);
    XAutoRepeatOff(state->display);

    state->connection = XGetXCBConnection(state->display);
    // check for connection error
    if (xcb_connection_has_error(state->connection)) {
      FATAL("Cannot connect to x server with xcb");
      return false;
    }
    

    // Get the setup for the screen
    const struct xcb_setup_t* setup = xcb_get_setup(state->connection);

    // Loop through screens using iterator
    xcb_screen_iterator_t it = xcb_setup_roots_iterator(setup);
    int screen_p = 0;
    for (i32 s = screen_p; s > 0; s--) {
        xcb_screen_next(&it);
    }

    // Use the last screen
    state->screen = it.data;

    // Allocate an id for our window
    state->window = xcb_generate_id(state->connection);

    // We are setting the background pixel color and the event mask
    u32 mask = XCB_CW_BACK_PIXEL | XCB_CW_EVENT_MASK;

    // Request key and mouse events
    //u32 events = ;

    // background color and events to request
    u32 values[] = {state->screen->black_pixel, XCB_EVENT_MASK_BUTTON_PRESS | XCB_EVENT_MASK_BUTTON_RELEASE |
                       XCB_EVENT_MASK_KEY_PRESS | XCB_EVENT_MASK_KEY_RELEASE |
                       XCB_EVENT_MASK_EXPOSURE | XCB_EVENT_MASK_POINTER_MOTION |
                       XCB_EVENT_MASK_STRUCTURE_NOTIFY};

    // TODO: set these through api
    char app_name[] = "Octal";

    // Create the window
    xcb_void_cookie_t cookie = xcb_create_window(
        state->connection,
        XCB_COPY_FROM_PARENT,
        state->window,
        state->screen->root,
        x,
        y,
        w,
        h,
        0,
        XCB_WINDOW_CLASS_INPUT_OUTPUT,
        state->screen->root_visual,
        mask,
        values);

    // Change the title
    xcb_change_property(
        state->connection,
        XCB_PROP_MODE_REPLACE,
        state->window,
        XCB_ATOM_WM_NAME,
        XCB_ATOM_STRING,
        8,  // data should be viewed 8 bits at a time
        strlen(app_name),
        app_name);

    // Notify us when the window manager wants to delete the window
    xcb_intern_atom_reply_t* wm_delete_reply = xcb_intern_atom_reply(
        state->connection,
        xcb_intern_atom(
          state->connection,
          0,
          strlen("WM_DELETE_WINDOW"),
          "WM_DELETE_WINDOW"),
        nullptr);

    xcb_intern_atom_reply_t* wm_protocols_reply = xcb_intern_atom_reply(
        state->connection,
        xcb_intern_atom(
          state->connection,
          0,
          strlen("WM_PROTOCOLS"),
          "WM_PROTOCOLS"),
        nullptr);

    // store the atoms
    state->wm_delete_win = wm_delete_reply->atom;
    state->wm_protocols = wm_protocols_reply->atom;

    // ask the sever to actually set the atom
    xcb_change_property(
        state->connection,
        XCB_PROP_MODE_REPLACE,
        state->window,
        wm_protocols_reply->atom,
        4,
        32,
        1,
        &wm_delete_reply->atom);

    // Map the window to the screen
    xcb_map_window(state->connection, state->window);

    // flush pending actions to the server
    if (xcb_flush(state->connection) <= 0) {
        FATAL("An error occurred when flusing the stream");
        return false;
    }

    return true;
  }

  bool Platform::Shutdown() {
    LinuxState* state = (LinuxState*) s_State;
    // turn autorepeat back on
    XAutoRepeatOn(state->display);
    // delete our window
    xcb_destroy_window(state->connection, state->window);
    return true;
  }

  bool Platform::Flush() {
    bool should_quit = false;
    LinuxState* state = (LinuxState*)s_State;

    xcb_generic_event_t* event;
    xcb_client_message_event_t* cm;

    // Poll for events until null is returned.
    while (true) {
        event = xcb_poll_for_event(state->connection);
        if (event == 0) {
            break;
        }

        // Input events
        switch (event->response_type & ~0x80) {
            case XCB_KEY_PRESS:
            case XCB_KEY_RELEASE: {
                // TODO: Key presses and releases
                INFO("keypress");
            } break;
            case XCB_BUTTON_PRESS:
            case XCB_BUTTON_RELEASE: {
                // TODO: Mouse button presses and releases
                INFO("button press");
            } break;
            case XCB_MOTION_NOTIFY: {
                // TODO: mouse movement
                INFO("mouse moved");
            } break;
            case XCB_CONFIGURE_NOTIFY: {
                // TODO: Resizing
                INFO("window resized");
            } break;

            case XCB_CLIENT_MESSAGE: {
                cm = (xcb_client_message_event_t*)event;
                // Window close
                if (cm->data.data32[0] == state->wm_delete_win) {
                    should_quit = true;
                }
            } break;
            default:
                // Something else
                break;
        }

      xcb_flush(state->connection);
        free(event);
    }
    return !should_quit;
  }

  void* Platform::Allocate(u64 size, bool aligned) {
    return malloc(size);
  }

	void Platform::Free(void* block) {
	  free(block);
	}

  void* Platform::MemZero(void* block, u64 size) {
    return memset(block, 0, size);
  }

  void* Platform::MemSet(void* block, i32 value, u64 size) {
    return memset(block, value, size);
  }

  void* Platform::MemCopy(void* dest, const void* src, u64 size) {
    return memcpy(dest, src, size);
  }


  void Platform::Write(const char* msg, u8 color) {
    const char* colour_strings[] = {"0;41", "1;31", "1;33", "1;32", "1;34", "1;35"};
    fprintf(stdout, "\033[%sm%s\033[0m", colour_strings[color], msg);
  }

  void Platform::WriteError(const char* msg, u8 color) {
    const char* colour_strings[] = {"0;41", "1;31", "1;33", "1;32", "1;34", "1;35"};
    fprintf(stderr, "\033[%sm%s\033[0m", colour_strings[color], msg);
  }

  f64 Platform::AbsoluteTime() {
    return 0.0;
  }

  void Platform::Sleep(u64 ms) {
    std::this_thread::sleep_for(std::chrono::milliseconds(ms));
  }
}
