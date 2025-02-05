// Display images inside a terminal
// Copyright (C) 2023  JustKidding
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

#ifndef X11_CANVAS_H
#define X11_CANVAS_H

#include "canvas.hpp"
#include "image.hpp"
#include "window.hpp"
#include "dimensions.hpp"
#include "util/x11.hpp"

#include <memory>
#include <unordered_map>
#include <unordered_set>
#include <thread>
#include <mutex>

#include <xcb/xcb.h>
#include <spdlog/fwd.h>

#ifdef ENABLE_XCB_ERRORS
#   include <xcb/xcb_errors.h>
#endif

#ifdef ENABLE_OPENGL
#   include "util/egl.hpp"
#endif

class Flags;

class X11Canvas : public Canvas
{
public:
    explicit X11Canvas();
    ~X11Canvas() override;

    void add_image(const std::string& identifier, std::unique_ptr<Image> new_image) override;
    void remove_image(const std::string& identifier) override;
    void hide() override;
    void show() override;

private:
    xcb_connection_t *connection;
    xcb_screen_t *screen;

#ifdef ENABLE_XCB_ERRORS
    xcb_errors_context_t *err_ctx;
#endif

    std::unique_ptr<X11Util> xutil;

    // map for event handler
    std::unordered_map<xcb_window_t, std::shared_ptr<Window>> windows;

    // windows per image
    std::unordered_map<std::string,
        std::unordered_map<xcb_window_t, std::shared_ptr<Window>>> image_windows;

    std::unordered_map<std::string, std::shared_ptr<Image>> images;
    std::unordered_map<std::string, std::jthread> draw_threads;

    std::thread event_handler;
    std::mutex windows_mutex;

    std::shared_ptr<spdlog::logger> logger;
    std::shared_ptr<Flags> flags;

#ifdef ENABLE_OPENGL
    std::unique_ptr<EGLUtil<xcb_connection_t, xcb_window_t>> egl;
    bool egl_available = true;
#endif

    void draw(const std::string& identifier);
    void handle_events();
    void get_tmux_window_ids(std::unordered_set<xcb_window_t>& windows);
    void print_xcb_error(const xcb_generic_error_t* err);
};

#endif
