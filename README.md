icewm-menu is a pipemenu for the IceWM window manager. It provides a dynamic menu listing installed applications. Most of the work is done by the LXDE library menu-cache.

This is just a modification of openbox-menu, so beware the documentation. Most of it has just had "openbox" replace with "icewm" for now.

# Compilation.

Type `make` then, with superuser privileges, `make install` to install it. icewm-menu is installed in /usr/local/bin directory by default.type `make install DESTDIR=/usr` to install it to /usr/bin/directory.

## Compile icewm-menu without icons ##

icewm-menu shows icons before categories name and applications name (in fact, the menu and the icons are displayed by icewm; icewm-menu only outputs text content). You can remove icon support by editing the Makefile and commenting the following line (add a # in the begining)

>  CFLAGS+=-DWITH_ICONS

If errors occur while compiling, remove gtk+-2.0 from lines above the one previously commented.

## SVG support

icewm can display SVG icons since version 3.5.1. SVG support in icewm-menu has to be activated in Makefile by uncommenting the following line

>  CFLAGS+=-DWITH_SVG

SVG suport will be activated by default in the future.

# Settings

## Default menu file

If no menu file specified in the command line, icewm-menu will use "applications.menu" as default menu filename. If $XDG_MENU_PREFIX is set, icewm-menu uses its content to prefix "applications.menu".

## User menu file

icewm-menu looks up for a valid menu file in /etc/xdg/menus directory. In order to make it looks up somewhere else, set up the $XDG_CONFIG_DIRS variable (don't suffix pathnames with "menus", it's automatically added).

For example, if $XDG_CONFIG_DIRS is set like this:

>  export XDG_CONFIG_DIRS="$HOME/.config:/etc/xdg"

And icewm-menu is called this way:

>  icewm-menu my.menu

icewm-menu will check for "my.menu" (or "my.menu" prefixed by the value of $XDG_MENU_PREFIX) in "$HOME/.config/menus/" directory and, if the file doesn't exist here, in "/etc/xdg/menus/" directory.

## Custom XML header and footer.

icewm-menu is not only a pipe-menu, it can also be used as an XML generator by setting the output file with the -o parameter (output file will be written in `$HOME/.cache/` directory).

In case you want to embed icewm-menu output in another menu, it is possible to define the header and the footer for the XML output generated by icewm-menu with the -H and -F parameters. icewm-menu can even create the equivalent of icewm's menu.xml file if proper header and footer content are passed.

icewm-menu uses, as default, a `<icewm_pipe_menu>` tag for header, and a `</icewm_pipe_menu>` tag for footer.

# Examples

For the folliwing examples, we consider that the proper menu file is automatically found by icewm-menu.


## Memory and CPU load when called, manual refesh.

We won't use icewm-menu as a pipe-menu, we will use a file containing the pipe-menu XML output. User should launch the next command every time he wants the menu to be refreshed.

>   icewm-menu -o menu.xml

This command creates a file menu.xml located in ~/.cache. Its content is used as a pipe-menu with the `cat ~/.cache/menu.xml` command.

## Memory and CPU load when called, automatic refresh.

This time we simply use the command `icewm-menu`; icewm-menu is used as every pipe-menu.

## Low memory, small CPU load, automatic refresh.

First, we launch icewm-menu at icewm startup with `icewm-menu -p -o menu.xml &`. It makes icewm-menu to stay in memory and to output the menu content in the ~/.cache/menu.xml` file. As in the first example, we simply use `cat ~/.cache/menu.xml` to display the menu as a pipe-menu.
