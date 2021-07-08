/*
 * xsct - X11 set color temperature
 *
 * Original code published by Ted Unangst:
 * http://www.tedunangst.com/flak/post/sct-set-color-temperature
 *
 * Modified by Fabian Foerg in order to:
 * - compile on Ubuntu 14.04
 * - iterate over all screens of the default display and change the color
 *   temperature
 * - fix memleaks
 * - clean up code
 * - return EXIT_SUCCESS
 *
 * Public domain, do as you wish.
 *
 * Compile the code using the following command:
 * gcc -Wall -Wextra -Werror -pedantic -std=c99 -O2 -I /usr/X11R6/include xsct.c -o xsct -L /usr/X11R6/lib -lX11 -lXrandr -lm -s
 *
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
// Red range (T0 = TEMPERATURE_ZERO)
// Green color
#define GAMMA_K0GR          -1.47751309139817
#define GAMMA_K1GR          0.28590164772055
// Blue color
#define GAMMA_K0BR          -4.38321650114872
#define GAMMA_K1BR          0.6212158769447
// Blue range  (T0 = TEMPERATURE_NORM - TEMPERATURE_ZERO)
// Red color
#define GAMMA_K0RB          1.75390204039018
#define GAMMA_K1RB          -0.1150805671482
// Green color
#define GAMMA_K0GB          1.49221604915144
#define GAMMA_K1GB          -0.07513509588921

#define TEMPERATURE_NORM    6500
#define TEMPERATURE_ZERO    700

static double
DoubleTrim(double x, double a, double b)
{
    double buff[3] = {a, x, b};
    return buff[ (int)(x > a) + (int)(x > b) ];
}

static int
get_sct_for_screen(Display *dpy, int screen, int fdebug)
{
    Window root = RootWindow(dpy, screen);
    XRRScreenResources *res = XRRGetScreenResourcesCurrent(dpy, root);

    int temp = 0, n, c;
    double t = 0.0;
    double gammar = 0.0, gammag = 0.0, gammab = 0.0, gammam = 1.0, gammad = 0.0;

    n = res->ncrtc;
    for (c = 0; c < n; c++)
    {
        RRCrtc crtcxid;
        int size;
        XRRCrtcGamma *crtc_gamma;
        crtcxid = res->crtcs[c];
        crtc_gamma = XRRGetCrtcGamma(dpy, crtcxid);
        size = crtc_gamma->size;
        gammar += crtc_gamma->red[size - 1];
        gammag += crtc_gamma->green[size - 1];
        gammab += crtc_gamma->blue[size - 1];

        XRRFreeGamma(crtc_gamma);
    }
    XFree(res);
    gammam = (gammar > gammag) ? gammar : gammag;
    gammam = (gammab > gammam) ? gammab : gammam;
    if (gammam > 0.0)
    {
        gammar /= gammam;
        gammag /= gammam;
        gammab /= gammam;
        g_debug ("Gamma: %f, %f, %f\n", gammar, gammag, gammab);
        gammad = gammab - gammar;
        if (gammad < 0.0)
        {
            if (gammab > 0.0)
            {
                t = exp((gammag + 1.0 + gammad - (GAMMA_K0GR + GAMMA_K0BR)) / (GAMMA_K1GR + GAMMA_K1BR)) + TEMPERATURE_ZERO;
            }
            else
            {
                t = (gammag > 0.0) ? (exp((gammag - GAMMA_K0GR) / GAMMA_K1GR) + TEMPERATURE_ZERO) : TEMPERATURE_ZERO;
            }
        }
        else
        {
            t = exp((gammag + 1.0 - gammad - (GAMMA_K0GB + GAMMA_K0RB)) / (GAMMA_K1GB + GAMMA_K1RB)) + (TEMPERATURE_NORM - TEMPERATURE_ZERO);
        }
    }

    temp = (int)(t + 0.5);

    return temp;
}

static void
sct_for_screen(Display *dpy, int screen, int temp, int fdebug)
{
    double t = 0.0, g = 0.0, gammar, gammag, gammab;
    int n, c;
    Window root = RootWindow(dpy, screen);
    XRRScreenResources *res = XRRGetScreenResourcesCurrent(dpy, root);

    t = (double)temp;
    if (temp < TEMPERATURE_NORM)
    {
        gammar = 1.0;
        if (temp < TEMPERATURE_ZERO)
        {
            gammag = 0.0;
            gammab = 0.0;
        }
        else
        {
            g = log(t - TEMPERATURE_ZERO);
            gammag = DoubleTrim(GAMMA_K0GR + GAMMA_K1GR * g, 0.0, 1.0);
            gammab = DoubleTrim(GAMMA_K0BR + GAMMA_K1BR * g, 0.0, 1.0);
        }
    }
    else
    {
        g = log(t - (TEMPERATURE_NORM - TEMPERATURE_ZERO));
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
x11_get_display_temperature()
{
    int i, screen, screens, temp;
    int fdebug = 0, fdelta = 0, fhelp = 0;

    Display *dpy = XOpenDisplay(NULL);

    if (!dpy)
    {
        g_warning ("XOpenDisplay(NULL) failed. Make sure DISPLAY is set correctly.");
        return -1;
    }

    screens = XScreenCount(dpy);

    // No arguments, so print estimated temperature for each screen
    for (screen = 0; screen < screens; screen++) {
        temp += get_sct_for_screen(dpy, screen, fdebug);
    }

    XCloseDisplay(dpy);

    return (int) temp/screens;
}

int
x11_set_display_temperature(int temp)
{
    int i, screen, screens;
    int fdebug = 0;

    Display *dpy = XOpenDisplay(NULL);

    if (!dpy)
    {
        g_warning ("XOpenDisplay(NULL) failed. Make sure DISPLAY is set correctly.");
        return EXIT_FAILURE;
    }
    screens = XScreenCount(dpy);
    if (temp == 0)
    {
        temp = TEMPERATURE_NORM;
    }
    else if (temp < TEMPERATURE_ZERO)
    {
        g_warning ("Temperatures below %d cannot be displayed.", TEMPERATURE_ZERO);
        temp = TEMPERATURE_ZERO;
    }

    for (screen = 0; screen < screens; screen++)
    {
        sct_for_screen(dpy, screen, temp, fdebug);
    }

    XCloseDisplay(dpy);

    return EXIT_SUCCESS;
}

int
x11_reset_display_temperature()
{
	return x11_set_display_temperature (TEMPERATURE_NORM);
}

