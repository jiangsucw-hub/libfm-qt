/*
 * macOS compatibility shim for <gio/gdesktopappinfo.h>.
 *
 * GLib does not build gdesktopappinfo.c on Darwin: there, GAppInfo is
 * implemented on top of NSWorkspace (gio/gosxappinfo.m) and the
 * GDesktopAppInfo header is not installed at all.  libfm-qt only needs a
 * handful of GDesktopAppInfo entry points, so this header maps the type onto
 * GAppInfo and the functions are implemented in gdesktopappinfo-compat.c.
 */

#ifndef __G_DESKTOP_APP_INFO_H__
#define __G_DESKTOP_APP_INFO_H__

#include <gio/gio.h>

G_BEGIN_DECLS

typedef GAppInfo GDesktopAppInfo;

#define G_DESKTOP_APP_INFO(o)    ((GDesktopAppInfo*)(o))
#define G_IS_DESKTOP_APP_INFO(o) (FALSE)

GDesktopAppInfo* g_desktop_app_info_new          (const char *desktop_id);
GDesktopAppInfo* g_desktop_app_info_new_from_filename (const char *filename);
GDesktopAppInfo* g_desktop_app_info_new_from_keyfile  (GKeyFile   *key_file);

char*            g_desktop_app_info_get_filename (GDesktopAppInfo *info);
gboolean         g_desktop_app_info_get_nodisplay (GDesktopAppInfo *info);
gboolean         g_desktop_app_info_get_is_hidden (GDesktopAppInfo *info);
const char*      g_desktop_app_info_get_categories (GDesktopAppInfo *info);

G_END_DECLS

#endif /* __G_DESKTOP_APP_INFO_H__ */
