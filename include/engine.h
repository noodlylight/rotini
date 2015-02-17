#ifndef ROTINI_ENGINE_H
#define ROTINI_ENGINE_H

#include <rotini.h>
#include <libengine.h>

typedef void (*init_engine_proc) (window_settings *);

typedef void (*load_settings_proc) (GKeyFile        *f,
                                    window_settings *ws);

typedef void (*fini_engine_proc) (window_settings *);

typedef void (*draw_frame_proc) (decor_t *d,
                                 cairo_t *cr);

typedef void (*layout_settings_proc) (GtkWidget *vbox);

typedef void (*get_meta_info_proc) (EngineMetaInfo *d);

#endif
