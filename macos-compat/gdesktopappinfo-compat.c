/*
 * Minimal GDesktopAppInfo implementation for macOS.
 *
 * GLib on Darwin does not provide GDesktopAppInfo; desktop entries are a
 * freedesktop.org concept that has no native counterpart.  Anything that can
 * be honoured is translated into a GAppInfo created from the Exec line, so
 * that e.g. "archivers.list"/"terminals.list" style lookups keep working when
 * a desktop file happens to be present.
 */

#include <string.h>

#include <gio/gio.h>
#include "gio/gdesktopappinfo.h"

/* Remove %f %F %u %U %i %c %k field codes and turn %% into %. */
static char*
strip_field_codes(const char* exec)
{
    GString* out = g_string_sized_new(strlen(exec));
    for(const char* p = exec; *p; ++p) {
        if(*p == '%' && p[1] != '\0') {
            switch(p[1]) {
            case '%':
                g_string_append_c(out, '%');
                ++p;
                break;
            case 'f': case 'F': case 'u': case 'U':
            case 'i': case 'c': case 'k':
                ++p;
                break;
            default:
                g_string_append_c(out, *p);
                break;
            }
        }
        else {
            g_string_append_c(out, *p);
        }
    }
    return g_string_free(out, FALSE);
}

GDesktopAppInfo*
g_desktop_app_info_new_from_keyfile(GKeyFile* key_file)
{
    g_return_val_if_fail(key_file != NULL, NULL);

    gchar* type = g_key_file_get_string(key_file, G_KEY_FILE_DESKTOP_GROUP,
                                        G_KEY_FILE_DESKTOP_KEY_TYPE, NULL);
    if(!type || g_strcmp0(type, G_KEY_FILE_DESKTOP_TYPE_APPLICATION) != 0) {
        g_free(type);
        return NULL;
    }
    g_free(type);

    gchar* exec = g_key_file_get_string(key_file, G_KEY_FILE_DESKTOP_GROUP,
                                        G_KEY_FILE_DESKTOP_KEY_EXEC, NULL);
    if(!exec) {
        return NULL;
    }
    gchar* name = g_key_file_get_string(key_file, G_KEY_FILE_DESKTOP_GROUP,
                                        G_KEY_FILE_DESKTOP_KEY_NAME, NULL);
    gchar* command = strip_field_codes(exec);
    GAppInfo* app = g_app_info_create_from_commandline(command, name,
                                                       G_APP_INFO_CREATE_SUPPORTS_URIS,
                                                       NULL);
    g_free(command);
    g_free(name);
    g_free(exec);
    return (GDesktopAppInfo*)app;
}

GDesktopAppInfo*
g_desktop_app_info_new_from_filename(const char* filename)
{
    g_return_val_if_fail(filename != NULL, NULL);

    if(g_str_has_suffix(filename, ".desktop")) {
        GKeyFile* kf = g_key_file_new();
        GDesktopAppInfo* app = NULL;
        if(g_key_file_load_from_file(kf, filename, G_KEY_FILE_NONE, NULL)) {
            app = g_desktop_app_info_new_from_keyfile(kf);
        }
        g_key_file_free(kf);
        return app;
    }

    /* A macOS application bundle can be launched through /usr/bin/open. */
    if(g_str_has_suffix(filename, ".app")) {
        gchar* command = g_strdup_printf("/usr/bin/open -a \"%s\"", filename);
        GAppInfo* app = g_app_info_create_from_commandline(command, NULL,
                                                           G_APP_INFO_CREATE_SUPPORTS_URIS,
                                                           NULL);
        g_free(command);
        return (GDesktopAppInfo*)app;
    }

    return NULL;
}

GDesktopAppInfo*
g_desktop_app_info_new(const char* desktop_id)
{
    g_return_val_if_fail(desktop_id != NULL, NULL);

    if(g_path_is_absolute(desktop_id)) {
        return g_desktop_app_info_new_from_filename(desktop_id);
    }
    /* There is no desktop-entry database on macOS. */
    return NULL;
}

char*
g_desktop_app_info_get_filename(GDesktopAppInfo* info)
{
    (void)info;
    return NULL;
}

gboolean
g_desktop_app_info_get_nodisplay(GDesktopAppInfo* info)
{
    (void)info;
    return FALSE;
}

gboolean
g_desktop_app_info_get_is_hidden(GDesktopAppInfo* info)
{
    (void)info;
    return FALSE;
}

const char* const*
g_desktop_app_info_get_categories(GDesktopAppInfo* info)
{
    (void)info;
    return NULL;
}
