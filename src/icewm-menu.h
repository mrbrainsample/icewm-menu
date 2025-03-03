/*
 *      icewm-menu.h - this file is part of icewm-menu
 *      Copyright (C) 2010-15 Fabrice THIROUX <fabrice.thiroux@free.fr>
 *
 *      This program is free software; you can redistribute it and/or modify
 *      it under the terms of the GNU Lesser General Public License as published
 *      by the Free Software Foundation; version 3.0 only.
 *
 *      This program is distributed in the hope that it will be useful,
 *      but WITHOUT ANY WARRANTY; without even the implied warranty of
 *      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *      GNU General Public License for more details.
 *
 *      You should have received a copy of the GNU General Public License
 *      along with this program; if not, write to the Free Software
 *      Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 *      MA 02110-1301, USA.
 */

#ifndef __ICEWMMENU_APP__
#define __ICEWMMENU_APP__

#include <menu-cache.h>

#ifdef WITH_ICONS
	#include <gtk/gtk.h>
#endif

#define VERSION "0.8.3"
#define APPMENU_SIZE 30
#define TERMINAL_CMD "xterm -e"

#ifndef __VERSION_MINOR // since menu-cache 0.5.0.
#warning "If you are running a 0.3.x version of libmenu-cache, you need to compile the 3.6.7 version of icewm-menu"
#endif

typedef enum {
	NO_ERROR = 0,
	CONFIG_ERROR,
	MENU_DIR_ERROR,
	MENU_EMPTY_ERROR,
	LOOKUP_ERROR,
	MENU_CACHE_ERROR,
} OBM_Error;


typedef struct {
	/* Configuration */
	gchar    *output;
	guint32   show_flag;
	GString  *builder;      /* */
	gchar    *terminal_cmd; /* command to launch program in a terminal */
	gboolean  comment;      /* display description instead of name */
	gboolean  sn;           /* startup notification */
	gboolean  no_icons;     /* icons disabled */
	gboolean  persistent;
	gchar    *menu_file;
	gchar    *template;
	guint     code;
} OB_Menu;


gchar *get_default_application_menu ();
guint app_is_visible (MenuCacheApp *, guint32);
gchar *clean_exec (MenuCacheApp *);
gchar *item_icon_path (MenuCacheItem*);
guint32 get_current_desktop_flag ();
void add_current_desktop_to_context (MenuCache *, OB_Menu *);

void menu_display (MenuCache *, OB_Menu *);

OB_Menu *context_new();
void   context_set_terminal_cmd (OB_Menu*, gchar*);
gchar *context_get_terminal_cmd (OB_Menu*);
void   context_set_desktop_flag (OB_Menu*, int);
void   context_add_desktop_flag (OB_Menu*, int);
int    context_get_desktop_flag (OB_Menu*);
void   context_set_persistent (OB_Menu*, gboolean);
void   context_set_persistent (OB_Menu*, gboolean);
void   context_set_comment (OB_Menu*, gboolean);
gboolean context_get_persistent (OB_Menu*);

void context_free(OB_Menu *);

#endif // __ICEWMMENU_APP__
