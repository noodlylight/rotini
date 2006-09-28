//pixmap engine
#include <emerald.h>
#include <engine.h>
#include <pixmap_icon.h>
#define SECT "pixmap_settings"
#define TEXTURE_FROM_PNG(surface, png) \
        surface = (cairo_surface_t*) cairo_image_surface_create_from_png(png);


static gchar * p_types[]=
{
    "top",
    "top_left",
    "top_right",
    "left",
    "right",
    "bottom",
    "bottom_left",
    "bottom_right",
    "title",
    "title_left",
    "title_right"
};
static gchar * names[]={
    "Top",
    "Top Left",
    "Top Right",
    "Left",
    "Right", 
    "Bottom", 
    "Bottom Left", 
    "Bottom Right",
    "Title",
    "Title Left",
    "Title Right"
};
typedef struct _pixmap_data
{
    cairo_surface_t* surface;
    gboolean use_scaled;
    gboolean use_width;
    gboolean use_height;
    double width;
    double height;
} pixmap_data;
typedef struct _private_fs
{
    alpha_color inner;
    alpha_color outer;
    alpha_color title_inner;
    alpha_color title_outer;
    pixmap_data pixmaps[11];
} private_fs;
typedef struct _private_ws
{
    gboolean round_top_left;
    gboolean round_top_right;
    gboolean round_bottom_left;
    gboolean round_bottom_right;
    gboolean inactive_use_active_pixmaps;
    double	top_corner_radius;
    double	bottom_corner_radius;
} private_ws;

void get_meta_info (EngineMetaInfo * emi)
{
    emi->version = g_strdup("0.2");
    emi->description = g_strdup("Everything done with customizable pixmaps!");
    emi->last_compat = g_strdup("0.0"); // old themes marked still compatible
    emi->icon = gdk_pixbuf_new_from_inline(-1,my_pixbuf,TRUE,NULL);
}


void
fill_rounded_rectangle_pixmap_blend (cairo_t       *cr,
        double        x, double        y,
        double        w, double        h,
        int           corner,
	alpha_color * c0,
	alpha_color * c1,
	int           gravity,
        pixmap_data * pix,
        window_settings * ws,
        double    radius,
	gboolean blend_only_if_pixmaps_available)
{
    cairo_pattern_t * pattern;
    gboolean blend = TRUE;
    int iw, ih;
    if(cairo_surface_status(pix->surface) == CAIRO_STATUS_SUCCESS) {
        iw = cairo_image_surface_get_width(pix->surface);
        ih = cairo_image_surface_get_height(pix->surface);

        cairo_set_operator (cr, CAIRO_OPERATOR_SOURCE);
        cairo_set_line_width (cr, 0.0);

        // While using scaled pixmaps
        if(pix->use_scaled) {
           cairo_matrix_t matrix;
           cairo_matrix_init_scale(&matrix, iw/w, ih/h);
           cairo_matrix_translate(&matrix, -x, -y);
           pattern = cairo_pattern_create_for_surface(pix->surface);
           cairo_pattern_set_matrix(pattern, &matrix);
           cairo_set_source (cr, pattern);
           cairo_pattern_set_extend(pattern, CAIRO_EXTEND_REPEAT);
        } else {
        // While using tiled pixmaps
           cairo_set_source_surface (cr, pix->surface, x, y);
           pattern = cairo_pattern_reference(cairo_get_source(cr));
           cairo_pattern_set_extend(pattern, CAIRO_EXTEND_REPEAT);
        }

        rounded_rectangle (cr, x, y, w, h, corner,ws,radius);
        cairo_fill (cr);
        cairo_pattern_destroy (pattern);
    }
    else if(blend_only_if_pixmaps_available) blend = FALSE;

    cairo_set_operator (cr, CAIRO_OPERATOR_OVER);
    if(w > 0 && blend) {
       // Now blend in the gradients
       fill_rounded_rectangle (cr,x,y,w,h,corner,c0,c1,gravity,ws,radius);
    }
}
static gint get_real_pos(window_settings * ws, gint tobj, decor_t * d)
{
    gint width = d->width;
    gint base=ws->left_space;
    switch(d->tobj_item_state[tobj])
    {
        case 1:
            base=(width-ws->left_space-ws->right_space-d->tobj_size[0]-d->tobj_size[2])/2-
                (d->tobj_size[1]/2)+ws->left_space+d->tobj_size[0];
            break;
        case 2:
            base=width-ws->right_space-d->tobj_size[2];
            break;
        case 3:
            return -1;
        default:
            break;
    }
    return base+d->tobj_item_pos[tobj];
}
void engine_draw_frame (decor_t * d, cairo_t * cr)
{
    double        x1, y1, x2, y2, h;
    double        top_left_width, top_right_width;
    double        top_left_height, top_right_height;
    double        left_width, right_width;
    double        bottom_left_width, bottom_right_width;
    double        bottom_left_height, bottom_right_height;
    int		  top, title_width, title_pos;
    int           title_left_width, title_right_width;
    frame_settings * fs = d->fs;
    private_fs * pfs = fs->engine_fs;
    window_settings * ws = fs->ws;
    private_ws * pws = ws->engine_ws;

    top = ws->win_extents.top + ws->titlebar_height;
    x1 = ws->left_space - ws->win_extents.left;
    y1 = ws->top_space - ws->win_extents.top;
    x2 = d->width - ws->right_space + ws->win_extents.right;
    y2 = d->height - ws->bottom_space + ws->win_extents.bottom;

    h = d->height - ws->top_space - ws->titlebar_height - ws->bottom_space;

    int corners = 
        ((pws->round_top_left)?CORNER_TOPLEFT:0) |
        ((pws->round_top_right)?CORNER_TOPRIGHT:0) |
        ((pws->round_bottom_left)?CORNER_BOTTOMLEFT:0) |
        ((pws->round_bottom_right)?CORNER_BOTTOMRIGHT:0);
    if (d->state & (WNCK_WINDOW_STATE_MAXIMIZED_HORIZONTALLY |
                WNCK_WINDOW_STATE_MAXIMIZED_VERTICALLY))
        corners = 0;

    left_width = top_left_width = bottom_left_width = ws->win_extents.left;
    right_width = top_right_width = bottom_right_width = ws->win_extents.right;
    title_width = title_left_width = title_right_width = 0;

    if(cairo_surface_status(pfs->pixmaps[9].surface) == CAIRO_STATUS_SUCCESS) 
       title_left_width = cairo_image_surface_get_width(pfs->pixmaps[9].surface);
    if(cairo_surface_status(pfs->pixmaps[10].surface) == CAIRO_STATUS_SUCCESS) 
       title_right_width = cairo_image_surface_get_width(pfs->pixmaps[10].surface);

    top_left_height = top_right_height = top;
    bottom_left_height = bottom_right_height = ws->win_extents.bottom;

    // Adjustments of the (top/bottom)-(left/right) bar 
    // if the radius is bigger than left/right extents
    if((ws->win_extents.left < pws->top_corner_radius) &&
        (CORNER_TOPLEFT & corners)) {
	    top_left_width = pws->top_corner_radius;
    }
    if((ws->win_extents.left < pws->bottom_corner_radius) &&
        (CORNER_BOTTOMLEFT & corners)) {
	    bottom_left_width = pws->bottom_corner_radius;
    }
    if((ws->win_extents.right < pws->top_corner_radius) &&
        (CORNER_TOPRIGHT & corners)) {
	    top_right_width = pws->top_corner_radius;
    }
    if((ws->win_extents.right < pws->bottom_corner_radius) &&
        (CORNER_BOTTOMRIGHT & corners)) {
	    bottom_right_width = pws->bottom_corner_radius;
    }

    // Manual Width overrides
    if(pfs->pixmaps[1].use_width) top_left_width = pfs->pixmaps[1].width;
    if(pfs->pixmaps[2].use_width) top_right_width = pfs->pixmaps[2].width;
    if(pfs->pixmaps[3].use_width) left_width = pfs->pixmaps[3].width;
    if(pfs->pixmaps[4].use_width) right_width = pfs->pixmaps[4].width;
    if(pfs->pixmaps[6].use_width) bottom_left_width = pfs->pixmaps[6].width;
    if(pfs->pixmaps[7].use_width) bottom_right_width = pfs->pixmaps[7].width;

    if(pfs->pixmaps[9].use_width) title_left_width = pfs->pixmaps[9].width;
    if(pfs->pixmaps[10].use_width) title_right_width = pfs->pixmaps[10].width;

    if(pfs->pixmaps[1].use_height) top_left_height = pfs->pixmaps[1].height;
    if(pfs->pixmaps[2].use_height) top_right_height = pfs->pixmaps[2].height;
    if(pfs->pixmaps[6].use_height) bottom_left_height = pfs->pixmaps[6].height;
    if(pfs->pixmaps[7].use_height) bottom_right_height = pfs->pixmaps[7].height;

    // Top Left Bar
    fill_rounded_rectangle_pixmap_blend (cr,
            x1,
            y1,
            top_left_width,
            top_left_height+1,
            CORNER_TOPLEFT & corners,
            &pfs->title_inner,&pfs->title_outer,
            SHADE_TOP | SHADE_LEFT, &pfs->pixmaps[1], ws,
            pws->top_corner_radius, TRUE);

    // Top Bar
    fill_rounded_rectangle_pixmap_blend (cr,
            x1 + top_left_width,
            y1,
            x2 - x1 - top_left_width - top_right_width,
            top+1,
            0,
            &pfs->title_inner,&pfs->title_outer,
            SHADE_TOP, &pfs->pixmaps[0], ws,
            pws->top_corner_radius, TRUE);

    // Top Right Bar
    fill_rounded_rectangle_pixmap_blend (cr,
            x2 - top_right_width,
            y1,
            top_right_width,
            top_right_height+1,
            CORNER_TOPRIGHT & corners,
            &pfs->title_inner,&pfs->title_outer,
            SHADE_TOP | SHADE_RIGHT, &pfs->pixmaps[2], ws,
            pws->top_corner_radius, TRUE);

    // Left Bar
    //if(ws->win_extents.left > 0)
       fill_rounded_rectangle_pixmap_blend (cr,
            x1 + ws->win_extents.left - left_width, 
	    y1 + top_left_height - 1,
            left_width, 
	    h+1 - (top_left_height - top),
	    0,
            &pfs->inner,&pfs->outer,
            SHADE_LEFT, &pfs->pixmaps[3], ws,
            pws->top_corner_radius, FALSE);

    // Right Bar
    //if(ws->win_extents.right > 0)
       fill_rounded_rectangle_pixmap_blend (cr,
            x2 - ws->win_extents.right, 
	    y1 + top_right_height - 1, 
	    right_width, 
	    h+1 - (top_right_height - top),
	    0,
            &pfs->inner,&pfs->outer,
            SHADE_RIGHT, &pfs->pixmaps[4], ws,
            pws->top_corner_radius, FALSE);

    // Bottom Left Bar
    fill_rounded_rectangle_pixmap_blend (cr,
            x1,
            y2 - bottom_left_height,
            bottom_left_width,
            bottom_left_height,
            CORNER_BOTTOMLEFT & corners,
            &pfs->inner,&pfs->outer,
            SHADE_BOTTOM | SHADE_LEFT, &pfs->pixmaps[6], ws,
            pws->bottom_corner_radius, FALSE);

    // Bottom Bar
    fill_rounded_rectangle_pixmap_blend (cr,
            x1 + bottom_left_width,
            y2 - ws->win_extents.bottom,
            x2 - x1 - bottom_left_width - bottom_right_width,
            ws->win_extents.bottom, 0,
            &pfs->inner,&pfs->outer,
            SHADE_BOTTOM, &pfs->pixmaps[5], ws,
            pws->bottom_corner_radius, FALSE);

    // Bottom Right Bar
    fill_rounded_rectangle_pixmap_blend (cr,
            x2 - bottom_right_width,
            y2 - bottom_right_height,
            bottom_right_width,
            bottom_right_height,
            CORNER_BOTTOMRIGHT & corners,
            &pfs->inner,&pfs->outer,
            SHADE_BOTTOM | SHADE_RIGHT,&pfs->pixmaps[7], ws,
            pws->bottom_corner_radius, FALSE);

    cairo_set_operator (cr, CAIRO_OPERATOR_OVER);
    //cairo_set_operator (cr, CAIRO_OPERATOR_ADD);

    // Draw Title pixmaps

    if(PANGO_IS_LAYOUT(d->layout))
       pango_layout_get_pixel_size(d->layout, &title_width, NULL);
    title_pos = get_real_pos(ws, TBT_TITLE, d);

    // Check that it doesn't overflow
    if((title_width + title_left_width + title_right_width) > 
       (x2 - x1 - top_left_width - top_right_width - 10)) {
       double scaledown = (x2 - x1 - top_left_width - top_right_width - 10) /
                          (title_width + title_left_width + title_right_width);
       title_width = scaledown*title_width;
       title_left_width = scaledown*title_left_width - 1;
       title_right_width = scaledown*title_right_width;
    }

    // Title Left
    fill_rounded_rectangle_pixmap_blend (cr,
            title_pos - title_left_width - 1,
            y1,
            title_left_width + 1,
            top,
            0,
            &pfs->title_inner,&pfs->title_outer,
            SHADE_TOP, &pfs->pixmaps[9], ws,
            pws->top_corner_radius, TRUE);

    // Title
    fill_rounded_rectangle_pixmap_blend (cr,
            title_pos - 0.5,
            y1,
            title_width + 0.5,
            top,
            0,
            &pfs->title_inner,&pfs->title_outer,
            SHADE_TOP, &pfs->pixmaps[8], ws,
            pws->top_corner_radius, TRUE);

    // Title Right
    fill_rounded_rectangle_pixmap_blend (cr,
            title_pos + title_width - 1,
            y1,
            title_right_width + 1,
            top,
            0,
            &pfs->title_inner,&pfs->title_outer,
            SHADE_TOP, &pfs->pixmaps[10], ws,
            pws->top_corner_radius, TRUE);

    cairo_stroke (cr);
}
void load_engine_settings(GKeyFile * f, window_settings * ws)
{
    private_ws * pws = ws->engine_ws;
    int i;
    char *pre = "active";
    char *junk;
    PFACS(outer);
    PFACS(inner);
    PFACS(title_outer);
    PFACS(title_inner);
    load_bool_setting(f,&pws->round_top_left,"round_top_left",SECT);
    load_bool_setting(f,&pws->round_top_right,"round_top_right",SECT);
    load_bool_setting(f,&pws->round_bottom_left,"round_bottom_left",SECT);
    load_bool_setting(f,&pws->round_bottom_right,"round_bottom_right",SECT);
    load_bool_setting(f,&pws->inactive_use_active_pixmaps,"inactive_use_active_pixmaps",SECT);
    load_float_setting(f,&pws->top_corner_radius,"top_radius",SECT);
    load_float_setting(f,&pws->bottom_corner_radius,"bottom_radius",SECT);

    // Active window
    private_fs * pfs = ws->fs_act->engine_fs;
    for(i=0; i<11; i++) {
	junk = g_strdup_printf("%s_%s", pre, p_types[i]);
	//printf("%s\n", junk);
	TEXTURE_FROM_PNG(pfs->pixmaps[i].surface, make_filename("pixmaps", junk, "png"));
	//printf("%d\n", cairo_surface_status(pfs->pixmaps[i].surface));
        load_bool_setting(f,&pfs->pixmaps[i].use_scaled, 
		g_strdup_printf("%s_%s_use_scaled", pre, p_types[i]),SECT);
        load_bool_setting(f,&pfs->pixmaps[i].use_width, 
		g_strdup_printf("%s_%s_use_width", pre, p_types[i]),SECT);
        load_float_setting(f,&pfs->pixmaps[i].width, 
		g_strdup_printf("%s_%s_width", pre, p_types[i]),SECT);
        load_bool_setting(f,&pfs->pixmaps[i].use_height, 
		g_strdup_printf("%s_%s_use_height", pre, p_types[i]),SECT);
        load_float_setting(f,&pfs->pixmaps[i].height, 
		g_strdup_printf("%s_%s_height", pre, p_types[i]),SECT);
    }

    // Inactive window
    pfs = ws->fs_inact->engine_fs;
    if(!pws->inactive_use_active_pixmaps) pre = "inactive";
    for(i=0; i<11; i++) {
	junk = g_strdup_printf("%s_%s", pre, p_types[i]);
	//printf("%s\n", junk);
	TEXTURE_FROM_PNG(pfs->pixmaps[i].surface, make_filename("pixmaps", junk, "png"));
	//printf("%d\n", cairo_surface_status(pfs->pixmaps[i].surface));
        load_bool_setting(f,&pfs->pixmaps[i].use_scaled, 
		g_strdup_printf("%s_%s_use_scaled", pre, p_types[i]),SECT);
        load_bool_setting(f,&pfs->pixmaps[i].use_width, 
		g_strdup_printf("%s_%s_use_width", pre, p_types[i]),SECT);
        load_float_setting(f,&pfs->pixmaps[i].width, 
		g_strdup_printf("%s_%s_width", pre, p_types[i]),SECT);
        load_bool_setting(f,&pfs->pixmaps[i].use_height, 
		g_strdup_printf("%s_%s_use_height", pre, p_types[i]),SECT);
        load_float_setting(f,&pfs->pixmaps[i].height, 
		g_strdup_printf("%s_%s_height", pre, p_types[i]),SECT);
    }
    printf("Engine settings loaded\n");
}
#define ACOLOR(idn,zr,zg,zb,za) \
    pfs->idn.color.r = (zr);\
    pfs->idn.color.g = (zg);\
    pfs->idn.color.b = (zb);\
    pfs->idn.alpha   = (za);
void init_engine(window_settings * ws)
{
    private_fs * pfs;
    private_ws * pws;

    pws = malloc(sizeof(private_ws));
    ws->engine_ws = pws;
    bzero(pws,sizeof(private_ws));
    pws->round_top_left=TRUE;
    pws->round_top_right=TRUE;
    pws->round_bottom_left=TRUE;
    pws->round_bottom_right=TRUE;
    pws->top_corner_radius=5.0;
    pws->bottom_corner_radius=5.0;

    pfs = malloc(sizeof(private_fs));
    ws->fs_act->engine_fs = pfs;
    bzero(pfs,sizeof(private_fs));
    ACOLOR(inner,0.8,0.8,0.8,0.5);
    ACOLOR(outer,0.8,0.8,0.8,0.5);
    ACOLOR(title_inner,0.8,0.8,0.8,0.8);
    ACOLOR(title_outer,0.8,0.8,0.8,0.8);

    pfs = malloc(sizeof(private_fs));
    bzero(pfs,sizeof(private_fs));
    ws->fs_inact->engine_fs = pfs;
    ACOLOR(inner,0.8,0.8,0.8,0.3);
    ACOLOR(outer,0.8,0.8,0.8,0.3);
    ACOLOR(title_inner,0.8,0.8,0.8,0.6);
    ACOLOR(title_outer,0.8,0.8,0.8,0.6);
}
void fini_engine(window_settings * ws)
{
    free(ws->fs_act->engine_fs);
    free(ws->fs_inact->engine_fs);
}
void layout_corners_frame(GtkWidget * vbox)
{
    GtkWidget * hbox;
    GtkWidget * junk;

    junk = gtk_check_button_new_with_label("Round Top Left Corner");
    gtk_box_pack_startC(vbox,junk,FALSE,FALSE,0);
    register_setting(junk,ST_BOOL,SECT,"round_top_left");

    junk = gtk_check_button_new_with_label("Round Top Right Corner");
    gtk_box_pack_startC(vbox,junk,FALSE,FALSE,0);
    register_setting(junk,ST_BOOL,SECT,"round_top_right");

    junk = gtk_check_button_new_with_label("Round Bottom Left Corner");
    gtk_box_pack_startC(vbox,junk,FALSE,FALSE,0);
    register_setting(junk,ST_BOOL,SECT,"round_bottom_left");

    junk = gtk_check_button_new_with_label("Round Bottom Right Corner");
    gtk_box_pack_startC(vbox,junk,FALSE,FALSE,0);
    register_setting(junk,ST_BOOL,SECT,"round_bottom_right");

    hbox = gtk_hbox_new(FALSE,2);
    gtk_box_pack_startC(vbox,hbox,FALSE,FALSE,0);
    gtk_box_pack_startC(hbox,gtk_label_new("Top Rounding Radius"),FALSE,FALSE,0);
    junk = scaler_new(0,20,0.5);
    gtk_box_pack_startC(hbox,junk,TRUE,TRUE,0);
    register_setting(junk,ST_FLOAT,SECT,"top_radius");

    hbox = gtk_hbox_new(FALSE,2);
    gtk_box_pack_startC(vbox,hbox,FALSE,FALSE,0);
    gtk_box_pack_startC(hbox,gtk_label_new("Bottom Rounding Radius"),FALSE,FALSE,0);
    junk = scaler_new(0,20,0.5);
    gtk_box_pack_startC(hbox,junk,TRUE,TRUE,0);
    register_setting(junk,ST_FLOAT,SECT,"bottom_radius");
}
void my_engine_settings(GtkWidget * hbox, gboolean active)
{
    GtkWidget * vbox;
    GtkWidget * scroller;
    vbox = gtk_vbox_new(FALSE,2);
    gtk_box_pack_startC(hbox,vbox,TRUE,TRUE,0);
    gtk_box_pack_startC(vbox,gtk_label_new(active?"Active Window":"Inactive Window"),FALSE,FALSE,0);
    gtk_box_pack_startC(vbox,gtk_hseparator_new(),FALSE,FALSE,0);
    scroller = gtk_scrolled_window_new(NULL,NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scroller),
            GTK_POLICY_NEVER,GTK_POLICY_AUTOMATIC);
    gtk_box_pack_startC(vbox,scroller,TRUE,TRUE,0);
    
    table_new(3,FALSE,FALSE);

    gtk_scrolled_window_add_with_viewport(GTK_SCROLLED_WINDOW(scroller),GTK_WIDGET(get_current_table()));
    
    make_labels("Colors");
    table_append_separator();
    ACAV("Outer Frame Blend","outer",SECT);
    ACAV("Inner Frame Blend","inner",SECT);
    table_append_separator();
    ACAV("Outer Titlebar Blend","title_outer",SECT);
    ACAV("Inner Titlebar Blend","title_inner",SECT);
    table_append_separator();
    ACAV("Titlebar Separator","separator_line",SECT);
}
void layout_engine_colors(GtkWidget * vbox)
{
    GtkWidget * hbox;
    hbox = gtk_hbox_new(FALSE,2);
    gtk_box_pack_startC(vbox,hbox,TRUE,TRUE,0);
    my_engine_settings(hbox,TRUE);
    gtk_box_pack_startC(hbox,gtk_vseparator_new(),FALSE,FALSE,0);
    my_engine_settings(hbox,FALSE);
}
static void layout_pixmap_box(GtkWidget * vbox, gint b_t, gboolean active)
{
    GtkWidget * filesel;
    GtkWidget * scroller;
    GtkFileFilter * imgfilter;
    GtkWidget * clearer;
    GtkWidget * use_scaled;
    GtkWidget * width;
    GtkWidget * use_my_width;
    GtkWidget * height;
    GtkWidget * use_my_height;
    GtkWidget * image;
    GtkWidget * tbox;
    GtkWidget * ttbox;
    SettingItem * item;
    char * pre = "active";
    if(!active) pre = "inactive";

    table_append(gtk_label_new(names[b_t]),FALSE);
    
    filesel=gtk_file_chooser_button_new(g_strdup_printf("%s Pixmap",names[b_t]),
            GTK_FILE_CHOOSER_ACTION_OPEN);
    table_append(filesel,FALSE);
    imgfilter = gtk_file_filter_new();
    gtk_file_filter_set_name(imgfilter,"Images");
    gtk_file_filter_add_pixbuf_formats(imgfilter);
    gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(filesel),imgfilter);
    
    scroller=gtk_scrolled_window_new(NULL,NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scroller),
            GTK_POLICY_AUTOMATIC,GTK_POLICY_AUTOMATIC);
    gtk_widget_set_size_request(scroller, 150,50);

    image = gtk_image_new();
    item = register_img_file_setting(filesel,"pixmaps", g_strdup_printf("%s_%s", pre, p_types[b_t]), (GtkImage *)image);
    gtk_scrolled_window_add_with_viewport(
            GTK_SCROLLED_WINDOW(scroller),GTK_WIDGET(image));
    table_append(scroller,TRUE);

    clearer = gtk_button_new_from_stock(GTK_STOCK_CLEAR);
    g_signal_connect(clearer,"clicked",G_CALLBACK(cb_clear_file),item);
    table_append(clearer,FALSE);

    // Style : Use Tiled or Scaled pixmaps
    use_scaled = gtk_check_button_new_with_label("Scaled");
    register_setting(use_scaled, ST_BOOL, SECT, g_strdup_printf("%s_%s_use_scaled", pre, p_types[b_t]));
    table_append(use_scaled, FALSE);

    // Width : Checkbox (Use my width) + Number (0-500)
    if(b_t == 0 || b_t == 5 || b_t == 8) {
        table_append(gtk_label_new("Not adjustable"), FALSE);
    } else {
        width = gtk_spin_button_new_with_range(0,500,1);
        register_setting(width,
	      ST_INT, SECT, g_strdup_printf("%s_%s_width", pre, p_types[b_t]));

        use_my_width = gtk_check_button_new_with_label("");
        register_setting(use_my_width, ST_BOOL,SECT, g_strdup_printf("%s_%s_use_width", pre, p_types[b_t]));

        tbox = gtk_hbox_new(FALSE,2);
        gtk_box_pack_startC(tbox, width, FALSE, FALSE, 0);
        gtk_box_pack_startC(tbox, use_my_width, FALSE, FALSE, 0);
        table_append(tbox, FALSE);
    }

    // Height : Checkbox (Use my width) + Number (0-500)
    if(b_t == 1 || b_t == 2 || b_t == 6 || b_t == 7) {
        height = gtk_spin_button_new_with_range(0,500,1);
        register_setting(height,
	      ST_INT, SECT, g_strdup_printf("%s_%s_height", pre, p_types[b_t]));

        use_my_height = gtk_check_button_new_with_label("");
        register_setting(use_my_height, ST_BOOL,SECT, g_strdup_printf("%s_%s_use_height", pre, p_types[b_t]));

        ttbox = gtk_hbox_new(FALSE,2);
        gtk_box_pack_startC(ttbox, height, FALSE, FALSE, 0);
        gtk_box_pack_startC(ttbox, use_my_height, FALSE, FALSE, 0);
        table_append(ttbox, FALSE);
    } else {
        table_append(gtk_label_new("Not adjustable"), FALSE);
    }

}
void layout_engine_pixmaps(GtkWidget * vbox, gboolean active)
{
    GtkWidget * scroller;
    GtkWidget * hbox;
    GtkWidget * junk;
    gint i;
   
    hbox = gtk_hbox_new(TRUE,2);
    gtk_box_pack_startC(vbox,hbox,FALSE,FALSE,0);

    if(!active) {
       junk = gtk_check_button_new_with_label("Same as Active");
       gtk_box_pack_startC(hbox,junk,TRUE,TRUE,0);
       register_setting(junk,ST_BOOL,SECT,"inactive_use_active_pixmaps");
    }

    scroller=gtk_scrolled_window_new(NULL,NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scroller),
            GTK_POLICY_AUTOMATIC,GTK_POLICY_AUTOMATIC);
    gtk_box_pack_startC(vbox,scroller,TRUE,TRUE,0);
    
    table_new(7,FALSE,FALSE);
    gtk_scrolled_window_add_with_viewport(
            GTK_SCROLLED_WINDOW(scroller),GTK_WIDGET(get_current_table()));
    
    table_append(gtk_label_new("Pixmap"),FALSE);
    table_append(gtk_label_new("File"),FALSE);
    table_append(gtk_label_new("Preview"),FALSE);
    table_append(gtk_label_new("Clear"),FALSE);
    table_append(gtk_label_new("Tiled/Scaled"),FALSE);
    table_append(gtk_label_new("Width Override"),FALSE);
    table_append(gtk_label_new("Height Override"),FALSE);
    
    for(i=0;i<11;i++)
    {
        layout_pixmap_box(vbox,i, active);
    }
}
void layout_engine_settings(GtkWidget * vbox)
{
    GtkWidget * note;
    note = gtk_notebook_new();
    gtk_box_pack_startC(vbox,note,TRUE,TRUE,0);
    layout_engine_pixmaps(build_notebook_page("Pixmaps (Active)",note), TRUE);
    layout_engine_pixmaps(build_notebook_page("Pixmaps (Inactive)",note), FALSE);
    layout_engine_colors(build_notebook_page("Colors",note));
    layout_corners_frame(build_notebook_page("Frame",note));
}