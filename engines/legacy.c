//legacy engine
#include <emerald.h>
#include <engine.h>
#include <legacy_icon.h>
#define SECT "legacy_settings"
typedef struct _private_fs
{
    alpha_color inner;
    alpha_color outer;
    alpha_color title_inner;
    alpha_color title_outer;
    alpha_color window_halo;
    alpha_color window_highlight;
    alpha_color window_shadow;
    alpha_color separator_line;
    alpha_color contents_highlight;
    alpha_color contents_shadow;
    alpha_color contents_halo;
} private_fs;
typedef struct _private_ws
{
    gboolean round_top_left;
    gboolean round_top_right;
    gboolean round_bottom_left;
    gboolean round_bottom_right;
    double	corner_radius;
} private_ws;
void get_meta_info (EngineMetaInfo * emi)
{
    emi->version = g_strdup("0.1");
    emi->description = g_strdup(_("Based on original gnome-window-decorator"));
    emi->last_compat = g_strdup("0.0"); // old themes still compatible
    emi->icon = gdk_pixbuf_new_from_inline(-1,my_pixbuf,TRUE,NULL);
}
void engine_draw_frame (decor_t * d, cairo_t * cr)
{
    double        x1, y1, x2, y2, h;
    int		  top;
    frame_settings * fs = d->fs;
    private_fs * pfs = fs->engine_fs;
    window_settings * ws = fs->ws;
    private_ws * pws = ws->engine_ws;
    gdouble pleft;
    gdouble ptop;
    gdouble pwidth;
    gdouble pheight;
    top = ws->win_extents.top + ws->titlebar_height;

    x1 = ws->left_space - ws->win_extents.left;
    y1 = ws->top_space - ws->win_extents.top;
    x2 = d->width - ws->right_space + ws->win_extents.right;
    y2 = d->height - ws->bottom_space + ws->win_extents.bottom;
    pleft=x1+ws->win_extents.left-0.5;
    ptop=y1+top-0.5;
    pwidth=x2-x1-ws->win_extents.left-ws->win_extents.right+1;
    pheight=y2-y1-top-ws->win_extents.bottom+1;

    h = d->height - ws->top_space - ws->titlebar_height - ws->bottom_space;

    int corners = 
        ((pws->round_top_left)?CORNER_TOPLEFT:0) |
        ((pws->round_top_right)?CORNER_TOPRIGHT:0) |
        ((pws->round_bottom_left)?CORNER_BOTTOMLEFT:0) |
        ((pws->round_bottom_right)?CORNER_BOTTOMRIGHT:0);
    if (d->state & (WNCK_WINDOW_STATE_MAXIMIZED_HORIZONTALLY |
                WNCK_WINDOW_STATE_MAXIMIZED_VERTICALLY))
        corners = 0;
    cairo_set_operator (cr, CAIRO_OPERATOR_SOURCE);

    cairo_set_line_width (cr, 1.0);

    fill_rounded_rectangle (cr,
            x1 + 0.5,
            y1 + 0.5,
            ws->win_extents.left - 0.5,
            top - 0.5,
            CORNER_TOPLEFT & corners,
            &pfs->title_inner,&pfs->title_outer,
            SHADE_TOP | SHADE_LEFT, ws,
            pws->corner_radius);

    fill_rounded_rectangle (cr,
            x1 + ws->win_extents.left,
            y1 + 0.5,
            x2 - x1 - ws->win_extents.left -
            ws->win_extents.right,
            top - 0.5,
            0,
            &pfs->title_inner,&pfs->title_outer,
            SHADE_TOP, ws,
            pws->corner_radius);

    fill_rounded_rectangle (cr,
            x2 - ws->win_extents.right,
            y1 + 0.5,
            ws->win_extents.right - 0.5,
            top - 0.5,
            CORNER_TOPRIGHT & corners,
            &pfs->title_inner,&pfs->title_outer,
            SHADE_TOP | SHADE_RIGHT, ws,
            pws->corner_radius);

    fill_rounded_rectangle (cr,
            x1 + 0.5,
            y1 + top,
            ws->win_extents.left - 0.5,
            h,
            0,
            &pfs->inner,&pfs->outer,
            SHADE_LEFT, ws,
            pws->corner_radius);

    fill_rounded_rectangle (cr,
            x2 - ws->win_extents.right,
            y1 + top,
            ws->win_extents.right - 0.5,
            h,
            0,
            &pfs->inner,&pfs->outer,
            SHADE_RIGHT, ws,
            pws->corner_radius);


    fill_rounded_rectangle (cr,
            x1 + 0.5,
            y2 - ws->win_extents.bottom,
            ws->win_extents.left - 0.5,
            ws->win_extents.bottom - 0.5,
            CORNER_BOTTOMLEFT & corners,
            &pfs->inner,&pfs->outer,
            SHADE_BOTTOM | SHADE_LEFT, ws,
            pws->corner_radius);

    fill_rounded_rectangle (cr,
            x1 + ws->win_extents.left,
            y2 - ws->win_extents.bottom,
            x2 - x1 - ws->win_extents.left -
            ws->win_extents.right,
            ws->win_extents.bottom - 0.5,
            0,
            &pfs->inner,&pfs->outer,
            SHADE_BOTTOM,ws,
            pws->corner_radius);

    fill_rounded_rectangle (cr,
            x2 - ws->win_extents.right,
            y2 - ws->win_extents.bottom,
            ws->win_extents.right - 0.5,
            ws->win_extents.bottom - 0.5,
            CORNER_BOTTOMRIGHT & corners,
            &pfs->inner,&pfs->outer,
            SHADE_BOTTOM | SHADE_RIGHT,ws,
            pws->corner_radius);

    cairo_set_operator (cr, CAIRO_OPERATOR_OVER);
    /* =====================titlebar separator line */

    cairo_set_source_alpha_color(cr,&pfs->separator_line);
    cairo_move_to (cr, x1 + 0.5, y1 + top - 0.5);
    cairo_rel_line_to (cr, x2 - x1 - 1.0, 0.0);

    cairo_stroke (cr);

    rounded_rectangle (cr,
            x1 + 0.5, y1 + 0.5,
            x2 - x1 - 1.0, y2 - y1 - 1.0,
            (CORNER_TOPLEFT | CORNER_TOPRIGHT | CORNER_BOTTOMLEFT |
             CORNER_BOTTOMRIGHT) & corners,ws,
            pws->corner_radius);

    cairo_clip (cr);

    cairo_translate (cr, 1.0, 1.0);

    rounded_rectangle (cr,
            x1 + 0.5, y1 + 0.5,
            x2 - x1 - 1.0, y2 - y1 - 1.0,
            (CORNER_TOPLEFT | CORNER_TOPRIGHT | CORNER_BOTTOMLEFT |
             CORNER_BOTTOMRIGHT) & corners,ws,
            pws->corner_radius);

    //HIGHLIGHT HERE
    cairo_set_source_alpha_color(cr,&pfs->window_highlight);
    cairo_stroke(cr);

    cairo_translate (cr, -2.0, -2.0);

    rounded_rectangle (cr,
            x1 + 0.5, y1 + 0.5,
            x2 - x1 - 1.0, y2 - y1 - 1.0,
            (CORNER_TOPLEFT | CORNER_TOPRIGHT | CORNER_BOTTOMLEFT |
             CORNER_BOTTOMRIGHT) & corners, ws,
            pws->corner_radius);

    //SHADOW HERE
    cairo_set_source_alpha_color(cr,&pfs->window_shadow);
    cairo_stroke(cr);

    cairo_translate (cr, 1.0, 1.0);

    cairo_reset_clip (cr);

    rounded_rectangle (cr,
            x1 + 0.5, y1 + 0.5,
            x2 - x1 - 1.0, y2 - y1 - 1.0,
            (CORNER_TOPLEFT | CORNER_TOPRIGHT | CORNER_BOTTOMLEFT |
             CORNER_BOTTOMRIGHT) & corners, ws,
            pws->corner_radius);

    cairo_set_source_alpha_color(cr,&pfs->window_halo);
    cairo_stroke (cr);

    //INNER STUFF

    //TODO - make this a bit more pixel-perfect...but it works for now

    cairo_set_line_width(cr,1.0);

    cairo_move_to(cr,pleft+pwidth+1.5,ptop-1);
    cairo_rel_line_to(cr,-pwidth-2.5,0);
    cairo_rel_line_to(cr,0,pheight+2.5);
    cairo_set_source_alpha_color(cr,&pfs->contents_shadow);
    cairo_stroke(cr);

    cairo_move_to(cr,pleft+pwidth+1,ptop-1.5);
    cairo_rel_line_to(cr,0,pheight+2.5);
    cairo_rel_line_to(cr,-pwidth-2.5,0);
    cairo_set_source_alpha_color(cr,&pfs->contents_highlight);
    cairo_stroke(cr);

    cairo_move_to(cr,pleft,ptop);
    cairo_rel_line_to(cr,pwidth,0);
    cairo_rel_line_to(cr,0,pheight);
    cairo_rel_line_to(cr,-pwidth,0);
    cairo_rel_line_to(cr,0,-pheight);
    cairo_set_source_alpha_color(cr,&pfs->contents_halo);
    cairo_stroke(cr);
}
void load_engine_settings(GKeyFile * f, window_settings * ws)
{
    private_ws * pws = ws->engine_ws;
    PFACS(outer);
    PFACS(inner);
    PFACS(title_outer);
    PFACS(title_inner);
    PFACS(window_halo);
    PFACS(window_highlight);
    PFACS(window_shadow);
    PFACS(separator_line);
    PFACS(contents_shadow);
    PFACS(contents_highlight);
    PFACS(contents_halo);
    load_bool_setting(f,&pws->round_top_left,"round_top_left",SECT);
    load_bool_setting(f,&pws->round_top_right,"round_top_right",SECT);
    load_bool_setting(f,&pws->round_bottom_left,"round_bottom_left",SECT);
    load_bool_setting(f,&pws->round_bottom_right,"round_bottom_right",SECT);
    load_float_setting(f,&pws->corner_radius,"radius",SECT);

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
    pws->corner_radius=5.0;

    pfs = malloc(sizeof(private_fs));
    ws->fs_act->engine_fs = pfs;
    bzero(pfs,sizeof(private_fs));
    ACOLOR(inner,0.8,0.8,0.8,0.5);
    ACOLOR(outer,0.8,0.8,0.8,0.5);
    ACOLOR(title_inner,0.8,0.8,0.8,0.8);
    ACOLOR(title_outer,0.8,0.8,0.8,0.8);
    ACOLOR(window_highlight,1.0,1.0,1.0,0.8);
    ACOLOR(window_shadow,0.6,0.6,0.6,0.8);
    ACOLOR(window_halo,0.8,0.8,0.8,0.8);
    ACOLOR(separator_line,0.0,0.0,0.0,0.0);
    ACOLOR(contents_highlight,1.0,1.0,1.0,0.8);
    ACOLOR(contents_shadow,0.6,0.6,0.6,0.8);
    ACOLOR(contents_halo,0.8,0.8,0.8,0.8);

    pfs = malloc(sizeof(private_fs));
    bzero(pfs,sizeof(private_fs));
    ws->fs_inact->engine_fs = pfs;
    ACOLOR(inner,0.8,0.8,0.8,0.3);
    ACOLOR(outer,0.8,0.8,0.8,0.3);
    ACOLOR(title_inner,0.8,0.8,0.8,0.6);
    ACOLOR(title_outer,0.8,0.8,0.8,0.6);
    ACOLOR(window_highlight,1.0,1.0,1.0,0.7);
    ACOLOR(window_shadow,0.6,0.6,0.6,0.7);
    ACOLOR(window_halo,0.8,0.8,0.8,0.7);
    ACOLOR(separator_line,0.0,0.0,0.0,0.0);
    ACOLOR(contents_highlight,1.0,1.0,1.0,0.8);
    ACOLOR(contents_shadow,0.6,0.6,0.6,0.8);
    ACOLOR(contents_halo,0.8,0.8,0.8,0.8);
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

    junk = gtk_check_button_new_with_label(_("Round Top Left Corner"));
    gtk_box_pack_startC(vbox,junk,FALSE,FALSE,0);
    register_setting(junk,ST_BOOL,SECT,"round_top_left");

    junk = gtk_check_button_new_with_label(_("Round Top Right Corner"));
    gtk_box_pack_startC(vbox,junk,FALSE,FALSE,0);
    register_setting(junk,ST_BOOL,SECT,"round_top_right");

    junk = gtk_check_button_new_with_label(_("Round Bottom Left Corner"));
    gtk_box_pack_startC(vbox,junk,FALSE,FALSE,0);
    register_setting(junk,ST_BOOL,SECT,"round_bottom_left");

    junk = gtk_check_button_new_with_label(_("Round Bottom Right Corner"));
    gtk_box_pack_startC(vbox,junk,FALSE,FALSE,0);
    register_setting(junk,ST_BOOL,SECT,"round_bottom_right");

    hbox = gtk_hbox_new(FALSE,2);
    gtk_box_pack_startC(vbox,hbox,FALSE,FALSE,0);
    
    gtk_box_pack_startC(hbox,gtk_label_new(_("Rounding Radius")),FALSE,FALSE,0);

    junk = scaler_new(0,20,0.5);
    gtk_box_pack_startC(hbox,junk,TRUE,TRUE,0);
    register_setting(junk,ST_FLOAT,SECT,"radius");

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
    
    make_labels(_("Colors"));
    table_append_separator();
    ACAV(_("Outer Frame Blend"),"outer",SECT);
    ACAV(_("Inner Frame Blend"),"inner",SECT);
    table_append_separator();
    ACAV(_("Outer Titlebar Blend"),"title_outer",SECT);
    ACAV(_("Inner Titlebar Blend"),"title_inner",SECT);
    table_append_separator();
    ACAV(_("Titlebar Separator"),"separator_line",SECT);
    table_append_separator();
    ACAV(_("Frame Outline"),"window_halo",SECT);
    ACAV(_("Frame Highlight"),"window_highlight",SECT);
    ACAV(_("Frame Shadow"),"window_shadow",SECT);
    table_append_separator();
    ACAV(_("Contents Outline"),"contents_halo",SECT);
    ACAV(_("Contents Highlight"),"contents_highlight",SECT);
    ACAV(_("Contents Shadow"),"contents_shadow",SECT);
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
void layout_engine_settings(GtkWidget * vbox)
{
    GtkWidget * note;
    note = gtk_notebook_new();
    gtk_box_pack_startC(vbox,note,TRUE,TRUE,0);
    layout_engine_colors(build_notebook_page(_("Colors"),note));
    layout_corners_frame(build_notebook_page(_("Frame"),note));
}