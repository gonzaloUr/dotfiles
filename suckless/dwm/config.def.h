/* See LICENSE file for copyright and license details. */

/*
 * Required patches:
 * https://dwm.suckless.org/patches/vanitygaps/dwm-vanitygaps-6.2.diff
 * https://dwm.suckless.org/patches/push/dwm-push-20201112-61bb8b2.diff
 *
 * Other patches:
 * https://dwm.suckless.org/patches/alpha/dwm-fixborders-6.2.diff
 * https://dwm.suckless.org/patches/hide_vacant_tags/dwm-hide_vacant_tags-6.2.diff
 * https://dwm.suckless.org/patches/statusallmons/dwm-statusallmons-6.2.diff
 */

/* appearance */
static const unsigned int borderpx  = 1;        /* border pixel of windows */
static const unsigned int snap      = 5;        /* snap pixel */
static const unsigned int gappih    = 10;       /* horiz inner gap between windows */
static const unsigned int gappiv    = 10;       /* vert inner gap between windows */
static const unsigned int gappoh    = 10;       /* horiz outer gap between windows and screen edge */
static const unsigned int gappov    = 10;       /* vert outer gap between windows and screen edge */
static       int smartgaps          = 0;        /* 1 means no outer gap when there is only one window */
static const int showbar            = 1;        /* 0 means no bar */
static const int topbar             = 1;        /* 0 means bottom bar */
static const char *fonts[]          = { "monospace:size=10" };
static const char dmenufont[]       = "monospace:size=10";
static const char col_gray1[]       = "#222222";
static const char col_gray2[]       = "#444444";
static const char col_gray3[]       = "#bbbbbb";
static const char col_gray4[]       = "#eeeeee";
static const char col_cyan[]        = "#005577";
static const char col_border[]      = "#FF0000";
static const char *colors[][3]      = {
	/*               fg         bg         border     */
	[SchemeNorm] = { col_gray3, col_gray1, col_gray2   },
	[SchemeSel]  = { col_gray4, col_cyan,  col_border  },
};

/* tagging */
static const char *tags[] = { "1", "2", "3", "4", "5", "6", "7", "8", "9" };

static const Rule rules[] = {
	/* xprop(1):
	 *	WM_CLASS(STRING) = instance, class
	 *	WM_NAME(STRING) = title
	 */
	/* class      instance    title       tags mask     isfloating   monitor */
	{ "Gimp",     NULL,       NULL,       0,            1,           -1 },
	{ "Firefox",  NULL,       NULL,       1 << 8,       0,           -1 },
	{ "firefox",  NULL,       NULL,       1 << 8,       0,           -1 },
};

/* layout(s) */
static const float mfact     = 0.55; /* factor of master area size [0.05..0.95] */
static const int nmaster     = 1;    /* number of clients in master area */
static const int resizehints = 1;    /* 1 means respect size hints in tiled resizals */

#define FORCE_VSPLIT 1  /* nrowgrid layout: force two clients to always split vertically */
#include "vanitygaps.c"

static const Layout layouts[] = {
	/* symbol     arrange function */
	{ "[]=",      tile },    /* first entry is default */
	{ "><>",      NULL },    /* no layout function means floating behavior */
	{ "[M]",      monocle },
	{ "[@]",      spiral },
	{ "[\\]",     dwindle },
	{ "H[]",      deck },
	{ "TTT",      bstack },
	{ "===",      bstackhoriz },
	{ "HHH",      grid },
	{ "###",      nrowgrid },
	{ "---",      horizgrid },
	{ ":::",      gaplessgrid },
	{ "|M|",      centeredmaster },
	{ ">M>",      centeredfloatingmaster },
	{ NULL,       NULL },
};

/* key definitions */
#define MODKEY Mod4Mask

/* https://cgit.freedesktop.org/xorg/proto/x11proto/tree/XF86keysym.h */
#define XF86MonBrightnessDown 0x1008ff03
#define XF86MonBrightnessUp   0x1008ff02
#define XF86AudioMute         0x1008ff12
#define XF86AudioLowerVolume  0x1008ff11
#define XF86AudioRaiseVolume  0x1008ff13
#define XF86AudioMicMute      0x1008FFB2

#define TAGKEYS(KEY,TAG) \
	{ MODKEY,                       KEY,      view,           {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask,           KEY,      toggleview,     {.ui = 1 << TAG} }, \
	{ MODKEY|ShiftMask,             KEY,      tag,            {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask|ShiftMask, KEY,      toggletag,      {.ui = 1 << TAG} },

/* helper for spawning shell commands in the pre dwm-5.0 fashion */
#define SHCMD(cmd) { .v = (const char*[]){ "/bin/sh", "-c", cmd, NULL } }

/* commands */
static char dmenumon[2] = "0"; /* component of dmenucmd, manipulated in spawn() */
static const char *dmenucmd[]   = { "dmenu_run", "-m", dmenumon, "-fn", dmenufont, "-nb", col_gray1, "-nf", col_gray3, "-sb", col_cyan, "-sf", col_gray4, NULL };
static const char *termcmd[]    = { "st", NULL };

static const char *brigdown[] = { "xbacklight", "-dec", "10", NULL };
static const char *brigup[]   = { "xbacklight", "-inc", "10", NULL };
static const char *volmute[]  = { "pactl", "set-sink-mute", "@DEFAULT_SINK@", "toggle", NULL };
static const char *volup[]    = { "pactl", "set-sink-volume", "@DEFAULT_SINK@", "+10%", NULL };
static const char *voldown[]  = { "pactl", "set-sink-volume", "@DEFAULT_SINK@", "-10%", NULL };
static const char *micmute[]  = { "pactl", "set-source-mute", "@DEFAULT_SOURCE@", "toggle", NULL };
static const char *prtscr[]   = { "ptrscr", NULL };
static const char *clipmgr[]  = { "clipmanager", NULL };

static Key keys[] = {
	/* modifier                     key        function        argument */
	{ MODKEY,                       XK_p,      spawn,          {.v = dmenucmd } },
	{ MODKEY|ShiftMask,             XK_Return, spawn,          {.v = termcmd } },
	{ 0,                            XK_Print,  spawn,          {.v = prtscr } },
	{ MODKEY,                       XK_a,      spawn,          {.v = clipmgr } },

	{ 0,                            XF86MonBrightnessDown, spawn, {.v = brigdown } },
	{ 0,                            XF86MonBrightnessUp,   spawn, {.v = brigup } },
	{ 0,                            XF86AudioMute,         spawn, {.v = volmute } },
	{ 0,                            XF86AudioRaiseVolume,  spawn, {.v = volup } },
	{ 0,                            XF86AudioLowerVolume,  spawn, {.v = voldown } },
	{ 0,                            XF86AudioMicMute,      spawn, {.v = micmute } },

	{ MODKEY,                       XK_b,      togglebar,      {0} },
	{ MODKEY,                       XK_j,      focusstack,     {.i = +1 } },
	{ MODKEY,                       XK_k,      focusstack,     {.i = -1 } },
	{ MODKEY,                       XK_i,      incnmaster,     {.i = +1 } },
	{ MODKEY,                       XK_d,      incnmaster,     {.i = -1 } },
	{ MODKEY,                       XK_h,      setmfact,       {.f = -0.05} },
	{ MODKEY,                       XK_l,      setmfact,       {.f = +0.05} },
	{ MODKEY,                       XK_Return, zoom,           {0} },

	{ MODKEY,                       XK_Up,     incrgaps,       {.i = +1 } },
	{ MODKEY,                       XK_Down,   incrgaps,       {.i = -1 } },
	{ MODKEY,                       XK_Right,  incrigaps,      {.i = +1 } },
	{ MODKEY,                       XK_Left,   incrigaps,      {.i = -1 } },
	{ MODKEY|ShiftMask,             XK_Up,     incrogaps,      {.i = +1 } },
	{ MODKEY|ShiftMask,             XK_Down,   incrogaps,      {.i = -1 } },
	{ MODKEY|ShiftMask,             XK_Right,  incrihgaps,     {.i = +1 } },
	{ MODKEY|ShiftMask,             XK_Left,   incrihgaps,     {.i = -1 } },
	{ MODKEY|ControlMask,           XK_Up,     incrivgaps,     {.i = +1 } },
	{ MODKEY|ControlMask,           XK_Down,   incrivgaps,     {.i = -1 } },
	{ MODKEY|ControlMask,           XK_Right,  incrohgaps,     {.i = +1 } },
	{ MODKEY|ControlMask,           XK_Left,   incrohgaps,     {.i = -1 } },
	{ MODKEY|ControlMask|ShiftMask, XK_Up,     incrovgaps,     {.i = +1 } },
	{ MODKEY|ControlMask|ShiftMask, XK_Down,   incrovgaps,     {.i = -1 } },
	{ MODKEY,                       XK_minus,  togglegaps,     {0} },
	{ MODKEY,                       XK_equal,  defaultgaps,    {0} },

	{ MODKEY|ShiftMask,             XK_j,      pushdown,       {0} },
	{ MODKEY|ShiftMask,             XK_k,      pushup,         {0} },
	{ MODKEY,                       XK_Tab,    view,           {0} },
	{ MODKEY|ShiftMask,             XK_c,      killclient,     {0} },

	{ MODKEY,                       XK_t,      setlayout,      {.v = &layouts[0]} },
	{ MODKEY,                       XK_f,      setlayout,      {.v = &layouts[1]} },
	{ MODKEY,                       XK_m,      setlayout,      {.v = &layouts[2]} },

	{ MODKEY,                       XK_F1,     setlayout,      {.v = &layouts[3]} },
	{ MODKEY,                       XK_F2,     setlayout,      {.v = &layouts[4]} },
	{ MODKEY,                       XK_F3,     setlayout,      {.v = &layouts[5]} },
	{ MODKEY,                       XK_F4,     setlayout,      {.v = &layouts[6]} },
	{ MODKEY,                       XK_F5,     setlayout,      {.v = &layouts[7]} },
	{ MODKEY,                       XK_F6,     setlayout,      {.v = &layouts[8]} },
	{ MODKEY,                       XK_F7,     setlayout,      {.v = &layouts[9]} },
	{ MODKEY,                       XK_F8,     setlayout,      {.v = &layouts[10]} },
	{ MODKEY,                       XK_F9,     setlayout,      {.v = &layouts[11]} },
	{ MODKEY,                       XK_F10,    setlayout,      {.v = &layouts[12]} },
	{ MODKEY,                       XK_F11,    setlayout,      {.v = &layouts[13]} },

	{ MODKEY,                       XK_space,  setlayout,      {0} },
	{ MODKEY|ShiftMask,             XK_space,  togglefloating, {0} },
	{ MODKEY,                       XK_0,      view,           {.ui = ~0 } },
	{ MODKEY|ShiftMask,             XK_0,      tag,            {.ui = ~0 } },
	{ MODKEY,                       XK_comma,  focusmon,       {.i = -1 } },
	{ MODKEY,                       XK_period, focusmon,       {.i = +1 } },
	{ MODKEY|ShiftMask,             XK_comma,  tagmon,         {.i = -1 } },
	{ MODKEY|ShiftMask,             XK_period, tagmon,         {.i = +1 } },
	TAGKEYS(                        XK_1,                      0)
	TAGKEYS(                        XK_2,                      1)
	TAGKEYS(                        XK_3,                      2)
	TAGKEYS(                        XK_4,                      3)
	TAGKEYS(                        XK_5,                      4)
	TAGKEYS(                        XK_6,                      5)
	TAGKEYS(                        XK_7,                      6)
	TAGKEYS(                        XK_8,                      7)
	TAGKEYS(                        XK_9,                      8)
	{ MODKEY|ShiftMask,             XK_q,      quit,           {0} },
};

/* button definitions */
/* click can be ClkTagBar, ClkLtSymbol, ClkStatusText, ClkWinTitle, ClkClientWin, or ClkRootWin */
static Button buttons[] = {
	/* click                event mask      button          function        argument */
	{ ClkLtSymbol,          0,              Button1,        setlayout,      {0} },
	{ ClkLtSymbol,          0,              Button3,        setlayout,      {.v = &layouts[2]} },
	{ ClkWinTitle,          0,              Button2,        zoom,           {0} },
	{ ClkStatusText,        0,              Button2,        spawn,          {.v = termcmd } },
	{ ClkClientWin,         MODKEY,         Button1,        movemouse,      {0} },
	{ ClkClientWin,         MODKEY,         Button2,        togglefloating, {0} },
	{ ClkClientWin,         MODKEY,         Button3,        resizemouse,    {0} },
	{ ClkTagBar,            0,              Button1,        view,           {0} },
	{ ClkTagBar,            0,              Button3,        toggleview,     {0} },
	{ ClkTagBar,            MODKEY,         Button1,        tag,            {0} },
	{ ClkTagBar,            MODKEY,         Button3,        toggletag,      {0} },
};

