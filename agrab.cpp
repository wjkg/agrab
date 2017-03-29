#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/cursorfont.h>
#include <cairo.h>
#include <cairo-xlib.h>
#include <unistd.h>
#include <cstdio>

unsigned int width, height;

void get_screen_size(Display *display, unsigned int &w, unsigned int &h) {
	Screen *screen = XDefaultScreenOfDisplay(display);
	w = WidthOfScreen(screen);
	h = HeightOfScreen(screen);
}

cairo_surface_t *take_screenshot(Display *display) {
	cairo_surface_t *root_s = cairo_xlib_surface_create(
		display, DefaultRootWindow(display),
		DefaultVisual(display, DefaultScreen(display)),
		width, height
	);

	cairo_surface_t *screenshot = cairo_image_surface_create(CAIRO_FORMAT_RGB24, width, height);
	cairo_t *cscr = cairo_create(screenshot);
	cairo_set_source_surface(cscr, root_s, 0, 0);
	cairo_paint(cscr);
	cairo_destroy(cscr);
	cairo_surface_destroy(root_s);

	return screenshot;
}

Window create_overlay(Display *display) {
	Window window = XCreateWindow(
		display, DefaultRootWindow(display), 0, 0, 100, 100,
		0, CopyFromParent, CopyFromParent, CopyFromParent, 0, 0
	);

	Atom atoms[2] = { XInternAtom(display, "_NET_WM_STATE_FULLSCREEN", False), None };

	XChangeProperty(
		display, window,
		XInternAtom(display, "_NET_WM_STATE", False), XA_ATOM,
		32, PropModeReplace, (unsigned char *) atoms, 1
	);

	return window;
}

cairo_surface_t* overlay_surface(Display *dsp, Drawable &da) {
	int screen;
	cairo_surface_t *sfc;

	screen = DefaultScreen(dsp);

	sfc = cairo_xlib_surface_create(dsp, da, DefaultVisual(dsp, screen), width, height);
	cairo_xlib_surface_set_size(sfc, width, height);

	return sfc;
}

int _abs(int x) {
	return x < 0 ? -x : x;
}

int _min(int a, int b) {
	return a < b ? a : b;
}

int main() {
	Display *display = XOpenDisplay(0);

	if (!display) return 1;

	get_screen_size(display, width, height);
	cairo_surface_t *screenshot = take_screenshot(display);

	Window overlay = create_overlay(display);
	XDefineCursor(display, overlay, XCreateFontCursor(display, XC_crosshair));
	XSelectInput(display, overlay, ButtonPressMask | ButtonReleaseMask | ButtonMotionMask);

	cairo_surface_t *surface = overlay_surface(display, overlay);
	cairo_t *ctx = cairo_create(surface);
	cairo_set_line_width(ctx, 1);

	XMapWindow(display, overlay);

	unsigned int orgx, orgy;
	unsigned int x, y, w = 0, h = 0;
	XEvent event;
	bool loop = true;

	double ov_r = 0.0, ov_g = 0.7, ov_b = 1.0, ov_a = 0.3;

	while (loop) {
		XNextEvent(display, &event);

		switch (event.type) {
			case MotionNotify:
				cairo_push_group(ctx);

				cairo_set_source_surface(ctx, screenshot, 0.0, 0.0);
				cairo_rectangle(ctx, x, y, w, h);
				cairo_fill(ctx);

				w = event.xmotion.x - orgx;
				h = event.xmotion.y - orgy;
				x = _min(orgx, orgx + w);
				y = _min(orgy, orgy + h);
				w = _abs(w);
				h = _abs(h);

				cairo_rectangle(ctx, x, y, w, h);
				cairo_set_source_rgba(ctx, ov_r, ov_g, ov_b, ov_a);
				cairo_fill(ctx);
				cairo_rectangle(ctx, (double) x + 0.5, (double) y + 0.5, w - 1, h - 1);
				cairo_set_source_rgba(ctx, ov_r, ov_g, ov_b, 1);
				cairo_stroke(ctx);

				cairo_pop_group_to_source(ctx);
				cairo_paint(ctx);
				break;
			case ButtonPress:
				orgx = event.xbutton.x;
				orgy = event.xbutton.y;
				break;
			case ButtonRelease:
				loop = false;
				break;
		}
	}

	printf("-x %d -y %d -w %d -h %d\n", x, y, w, h);

	cairo_destroy(ctx);
	cairo_surface_destroy(surface);
	XCloseDisplay(display);
	return 0;
}
