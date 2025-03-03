//      icewm-menu - a dynamic menu for icewm
//      Copyright (C) 2010-15 Fabrice THIROUX <fabrice.thiroux@free.fr>
//
//      This program is free software; you can redistribute it and/or modify
//      it under the terms of the GNU General Public License as published by
//      the Free Software Foundation; version 3 of the License.
//
//      This program is distributed in the hope that it will be useful,
//      but WITHOUT ANY WARRANTY; without even the implied warranty of
//      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//      GNU General Public License for more details.
//
//      You should have received a copy of the GNU General Public License
//      along with this program; if not, write to the Free Software
//      Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
//      MA 02110-1301, USA.

#include <stdio.h>
#include <glib.h>
#include <glib/gi18n.h>
#include <signal.h>
#include <locale.h>
#include <stdlib.h>

#include "icewm-menu.h"

GMainLoop *loop = NULL;

/* from lxsession */
void sig_term_handler (int sig)
{
	g_warning ("Aborting");
	g_main_loop_quit (loop);
}



/****f* icewm-menu/check_application_menu
 * FUNCTION
 *   Test if menu file exists.
 *
 * PARAMETERS
 *   * menu, a string containing the filename of the menu
 *
 * RETURN VALUE
 *    FALSE if menu file is not found. TRUE otherwise.
 *
 * NOTES
 *    User custom menu file can be used if XDG_CONFIG_DIRS is set, i.g
 *    'export XDG_CONFIG_DIRS="$HOME/.config/:/etc/xdg/" to use
 *    menu file located in $HOME/menus or /etc/xdg/ directories.
 ****/
gboolean
check_application_menu (gchar *menu)
{
	const gchar * const *dir;
	gchar *menu_path;

	for (dir = g_get_system_config_dirs(); *dir ; dir++)
	{
		menu_path = g_build_filename (*dir, "menus", menu, NULL);
		if (g_file_test (menu_path, G_FILE_TEST_EXISTS))
		{
			g_free (menu_path);
			return TRUE;
		}

		g_free (menu_path);
	}

	return FALSE;
}

OB_Menu *
configure (int argc, char **argv)
{
	GError   *error = NULL;
	gboolean  comment = FALSE;
	gchar    *terminal_cmd = NULL;
	gboolean  persistent = FALSE;
	gboolean  show_gnome = FALSE;
	gboolean  show_kde = FALSE;
	gboolean  show_xfce = FALSE;
	gboolean  show_rox = FALSE;
	gboolean  show_unknown = FALSE;
	gboolean  no_icons = FALSE;
	gchar    *template = NULL;
	gboolean  sn = FALSE;
	gchar    *output = NULL;
	gchar   **app_menu = NULL;

	GOptionEntry entries[] = {
		{ "comment",   'c', 0, G_OPTION_ARG_NONE,   &comment,
		  "Show generic name instead of application name", NULL },
		{ "terminal",  't', 0, G_OPTION_ARG_STRING, &terminal_cmd,
		  "Terminal command (default xterm -e)", "cmd" },
		{ "gnome",     'g', 0, G_OPTION_ARG_NONE,   &show_gnome,
		  "Show GNOME entries", NULL },
		{ "kde",       'k', 0, G_OPTION_ARG_NONE,   &show_kde,
		  "Show KDE entries",   NULL },
		{ "xfce",      'x', 0, G_OPTION_ARG_NONE,   &show_xfce,
		  "Show XFCE entries",  NULL },
		{ "rox",       'r', 0, G_OPTION_ARG_NONE,   &show_rox,
		  "Show ROX entries",   NULL },
		{ "unknown",   'u', 0, G_OPTION_ARG_NONE,   &show_unknown,
		  "Show Unknown deskstop entries",   NULL },
		{ "persistent",'p', 0, G_OPTION_ARG_NONE,   &persistent,
		  "stay active",        NULL },
		{ "sn",        's', 0, G_OPTION_ARG_NONE,   &sn,
		  "Enable startup notification", NULL },
		{ "output",    'o', 0, G_OPTION_ARG_STRING, &output,
		  "file to write data to", NULL },
		{ "template",    'T', 0, G_OPTION_ARG_STRING, &template,
		  "Use filename as template for icewm-menu output", NULL },
		{ "noicons", 'i',   0, G_OPTION_ARG_NONE,   &no_icons,
		  "Don't display icons in menu", NULL },
		{ G_OPTION_REMAINING, 0, 0, G_OPTION_ARG_STRING_ARRAY, &app_menu,
		  NULL, "[file.menu]" },
		{NULL}
	};
	GOptionContext *help_context = NULL;

	help_context = g_option_context_new (" - IceWM menu generator " VERSION);
	g_option_context_set_help_enabled (help_context, TRUE);
	g_option_context_add_main_entries (help_context, entries, NULL);
	g_option_context_parse (help_context, &argc, &argv, &error);

	if (error)
	{
		g_warning ("%s\n", error->message);
		g_error_free (error);
		return NULL;
	}

	OB_Menu *context = context_new();

	if (output)
		context->output = g_build_filename (g_get_home_dir (), ".icewm", output, NULL);
	else
		context->output =  NULL;

	// We add extra desktop entries to display.
	// Our current desktop is set when menu_cache has loaded its own cache.
	// (likely in menu_display function).
	if (show_gnome)   context_add_desktop_flag(context, SHOW_IN_GNOME);
	if (show_kde)     context_add_desktop_flag(context, SHOW_IN_KDE);
	if (show_xfce)    context_add_desktop_flag(context, SHOW_IN_XFCE);
	if (show_rox)     context_add_desktop_flag(context, SHOW_IN_GNOME);
	if (show_unknown) context_add_desktop_flag(context, 1 << N_KNOWN_DESKTOPS);

	context_set_terminal_cmd (context, (terminal_cmd) ? terminal_cmd : TERMINAL_CMD);

	context_set_comment(context, comment);

	if (sn)
		context->sn = TRUE;

	if (no_icons)
		context->no_icons = TRUE;

	if (persistent)
		context->persistent = TRUE;

	if (!app_menu)
		context->menu_file = get_default_application_menu();
	else
		context->menu_file = strdup (*app_menu);

	if (template)
		context->template = template;

	g_option_context_free (help_context);

	return context;
}

guint
run (OB_Menu *context)
{
	gpointer reload_notify_id = NULL;
	MenuCache *menu_cache = NULL;

	g_unsetenv("XDG_MENU_PREFIX"); // For unknow reason, it doesn't work when it is set.

	if (context->persistent) /* persistent mode */
	{
		// No need to get sync lookup. The callback function will be called
		// when menu-cache is ready.
		menu_cache = menu_cache_lookup (context->menu_file);
		if (!menu_cache)
		{
			g_warning ("Cannot connect to menu-cache :/");
			return MENU_CACHE_ERROR;
		}

		// menucache used to reload the cache after a call to menu_cache_lookup* ()
		// It's not true anymore with version >= 0.4.0.
		reload_notify_id = menu_cache_add_reload_notify (menu_cache,
		                        (MenuCacheReloadNotify) menu_display,
		                        context);

		// install signals handler
		signal (SIGTERM, sig_term_handler);
		signal (SIGINT, sig_term_handler);

		// run main loop
		loop = g_main_loop_new (NULL, FALSE);
		g_main_loop_run (loop);
		g_main_loop_unref (loop);

		menu_cache_remove_reload_notify (menu_cache, reload_notify_id);
	}
	else
	{ /* single shot */
		// wait for the menu to get ready
		menu_cache = menu_cache_lookup_sync (context->menu_file);
		if (!menu_cache )
		{
			g_warning ("Cannot connect to menu-cache :/");
			return MENU_CACHE_ERROR;
		}

		// display the menu anyway
		menu_display (menu_cache, context);
	}

	menu_cache_unref (menu_cache);

	// return error code set in callback function.
	return context->code;
}


int
main (int argc, char **argv)
{
	OB_Menu *ob_context;

	setlocale (LC_ALL, "");

#ifdef WITH_ICONS
	gtk_init (&argc, &argv);
#endif

	if ((ob_context = configure (argc, argv)) == NULL)
		return CONFIG_ERROR;

	if (!check_application_menu (ob_context->menu_file))
	{
		g_print ("File $XDG_CONFIG_DIRS/%s doesn't exist. Can't create menu.\n", ob_context->menu_file);
		return LOOKUP_ERROR;
	}

	guint ret = run (ob_context);
	context_free (ob_context);
	return ret;
}
