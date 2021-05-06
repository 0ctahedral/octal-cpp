#pragma once
#include "platform/platform.h"
#include <X11/Xlib-xcb.h>
#include <X11/Xlib.h>
#include <xcb/xcb.h>

namespace octal {
  struct LinuxState {
    /// Display
    Display *display;
    /// Pointer to the xcb connection
    xcb_connection_t* connection;
    /// Window we are drawing on
    xcb_window_t window;
    /// Screen the window is in
    xcb_screen_t* screen;
    /// Atom to store our protocol list
    xcb_atom_t wm_protocols;
    /// Atom to notify us when the window is deleted
    xcb_atom_t wm_delete_win;
  };

}
