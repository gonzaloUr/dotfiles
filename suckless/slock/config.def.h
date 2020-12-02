/* user and group to drop privileges to */
static const char *user  = "nobody";
static const char *group = "nogroup";

static const char *colorname[NUMCOLS] = {
	[INIT] =   "#212121",     /* after initialization */
	[INPUT] =  "#f1f1f1",   /* during input */
	[FAILED] = "#c30771",   /* wrong password */
};

/* treat a cleared input like a wrong password (color) */
static const int failonclear = 1;
