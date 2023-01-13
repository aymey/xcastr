#include <X11/Xlib.h>
#include <X11/Xutil.h>

#include "WindowFunctions.c"
#include "config.h"

#include <stdlib.h>

#include <unistd.h>

int main(int argc, char *argv[]) {

    Display *display = XOpenDisplay(NULL);
    int screen = DefaultScreen(display);
    Window root = XDefaultRootWindow(display), window;

    if(width == -1 || height == -1) {
        width = DisplayWidth(display, screen) / 9;
        height = DisplayHeight(display, screen) / 13;
    }


    // create window
    window = CreateWindow(display, root, screen, background, width, height);

    int diameter = radius * 2;
    XWindowAttributes windowAttributes;
    if(radius != -1) {
        // window attribuets for size (for shape)
        XGetWindowAttributes(display, window, &windowAttributes);
        ShapeWindow(display, window, windowAttributes, radius, diameter);
    }

    if(!interactable)
        WindowIntractable(display, window);

    // make window transparent
    TransparentWindow(display, window, transparency);

    // TODO: not Interactable should be able to move and resizew the window
    // TODO: something like an ini file to keep window position and user variables
    // TODO: render text with default font but optionally custom
    // TODO: remove decorations? (done with override redirect but isn tractable)
    // TODO: Check if focus window asks for password and if it does then replace the characters with "*"
    // TODO: dynamically change window size by text, add maximum width variable
    // TODO: fade window when silent after a while (from 255/200 alpha to 0)
    // TODO: detect key presses from root window
    // TODO: if the window isnt round then the update interval can be alot larger (how to do this in a clean way)
    // TODO: only include headers that are corelate with user config (e.g. no need for lxfixes if window isnt intractable) orfind a way to do it without extension

    GC gc = XCreateGC(display, window, 0, 0);
    XSetBackground(display, gc, WhitePixel(display, 0));
    XSetForeground(display, gc, BlackPixel(display, 0));

    // Display window
    XMapWindow(display, window);
    XSync(display, 0);

    // allow key events to be reported (from root)
    XSelectInput(display, window, KeyPressMask);
    XEvent event;

    // keep window open until manually closed
    // TODO: Why does it take longer to remove window depending on how big it is? (lower sleep less time but it shouldnt be effected by size?).
    // TODO: find a better way to update the window other than while loop or is that he best? ANSWER: yes there is, add an event loop instead
    while(!WindowClosed(display, window)) {
        // usleep is depricated use nanosleep?
        usleep(update);

        if(radius != -1) {
            XGetWindowAttributes(display, window, &windowAttributes);
            ShapeWindow(display, window, windowAttributes, radius, diameter);
        }

        if(XPending(display)) {
            XNextEvent(display, &event);

            if(event.type == KeyPress) {
                // printf("Key: %d\n", event.xkey.keycode);
                WindowText(display, window, font, event.xkey.keycode - 8, gc);
            }
        }
    }

    XUnmapWindow(display, window);
    XDestroyWindow(display, window);
    // XFlush(display);
    XCloseDisplay(display);

    return 0;
}

