/*
 * Copyright (c) 2015 Ted Unangst <tedu@openbsd.org>
 * Copyright (c) 2015-2020 Fabian Foerg
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include <glib.h>
#include <X11/extensions/Xrandr.h>

#include <math.h>

#include "xsct.h"

#define GAMMA_MULT          65535.0
// Approximation of the `redshift` table from
// https://github.com/jonls/redshift/blob/04760afe31bff5b26cf18fe51606e7bdeac15504/src/colorramp.c#L30-L273
// without limits:
// GAMMA = K0 + K1 * ln(T - T0)
// Red range (T0 = XSCT_TEMPERATURE_ZERO)
// Green color
#define GAMMA_K0GR          -1.47751309139817
#define GAMMA_K1GR          0.28590164772055
// Blue color
#define GAMMA_K0BR          -4.38321650114872
#define GAMMA_K1BR          0.6212158769447
// Blue range  (T0 = XSCT_TEMPERATURE_NORM - XSCT_TEMPERATURE_ZERO)
// Red color
#define GAMMA_K0RB          1.75390204039018
#define GAMMA_K1RB          -0.1150805671482
// Green color
#define GAMMA_K0GB          1.49221604915144
#define GAMMA_K1GB          -0.07513509588921

#define XSCT_TEMPERATURE_NORM    6500.0
#define XSCT_TEMPERATURE_ZERO    700.0

static double
DoubleTrim(double x, double a, double b)
{
    double buff[3] = {a, x, b};
    return buff[ (int)(x > a) + (int)(x > b) ];
}

static void
sct_for_screen (Display *dpy, int screen, double temp)
{
    double t = 0.0, g = 0.0, gammar, gammag, gammab;
    int n, c;
    Window root = RootWindow(dpy, screen);
    XRRScreenResources *res = XRRGetScreenResourcesCurrent(dpy, root);

    t = temp;
    if (temp < XSCT_TEMPERATURE_NORM)
    {
        gammar = 1.0;
        if (temp < XSCT_TEMPERATURE_ZERO)
        {
            gammag = 0.0;
            gammab = 0.0;
        }
        else
        {
            g = log(t - XSCT_TEMPERATURE_ZERO);
            gammag = DoubleTrim(GAMMA_K0GR + GAMMA_K1GR * g, 0.0, 1.0);
            gammab = DoubleTrim(GAMMA_K0BR + GAMMA_K1BR * g, 0.0, 1.0);
        }
    }
    else
    {
        g = log(t - (XSCT_TEMPERATURE_NORM - XSCT_TEMPERATURE_ZERO));
        gammar = DoubleTrim(GAMMA_K0RB + GAMMA_K1RB * g, 0.0, 1.0);
        gammag = DoubleTrim(GAMMA_K0GB + GAMMA_K1GB * g, 0.0, 1.0);
        gammab = 1.0;
    }

    g_debug ("Gamma: %f, %f, %f\n", gammar, gammag, gammab);

    n = res->ncrtc;
    for (c = 0; c < n; c++)
    {
        int size, i;
        RRCrtc crtcxid;
        XRRCrtcGamma *crtc_gamma;
        crtcxid = res->crtcs[c];
        size = XRRGetCrtcGammaSize(dpy, crtcxid);

        crtc_gamma = XRRAllocGamma(size);

        for (i = 0; i < size; i++)
        {
            g = GAMMA_MULT * (double)i / (double)size;
            crtc_gamma->red[i] = (unsigned short int)(g * gammar + 0.5);
            crtc_gamma->green[i] = (unsigned short int)(g * gammag + 0.5);
            crtc_gamma->blue[i] = (unsigned short int)(g * gammab + 0.5);
        }

        XRRSetCrtcGamma(dpy, crtcxid, crtc_gamma);
        XRRFreeGamma(crtc_gamma);
    }

    XFree(res);
}

int
x11_set_display_temperature (double temp)
{
    int screen = 0, screens;

    Display *dpy = XOpenDisplay(NULL);

    if (!dpy)
    {
        g_warning ("XOpenDisplay(NULL) failed. Make sure DISPLAY is set correctly.");
        return -1;
    }

    if (temp == 0)
    {
        temp = XSCT_TEMPERATURE_NORM;
    }
    else if (temp < XSCT_TEMPERATURE_ZERO)
    {
        g_warning ("Temperatures below %d cannot be displayed.", XSCT_TEMPERATURE_ZERO);
        temp = XSCT_TEMPERATURE_ZERO;
    }

    screens = XScreenCount(dpy);
    for (screen = 0; screen < screens; screen++)
    {
        sct_for_screen(dpy, screen, temp);
    }

    XCloseDisplay(dpy);

    return temp;
}

