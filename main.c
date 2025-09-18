#include <gtk/gtk.h>

static cairo_surface_t *surface = NULL;

static void clear_surface(void)
{
    cairo_t *cr;
    cr = cairo_create(surface);
    cairo_set_source_rgb(cr,1,1,1);
    cairo_paint(cr);
    cairo_destroy(cr);
}

//caret a surface to appropriate size to store our scribbles
static void resize_cb(GtkWidget *widget, int width,int height, gpointer data)
{
    if(surface)
    {
        cairo_surface_destroy(surface);
        surface = NULL;
    }
    if(gtk_native_get_surface(gtk_widget_get_native(widget)))
    {
        surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32,gtk_widget_get_width(widget),gtk_widget_get_height(widget));
        clear_surface();
    }
}
static void draw_cb(GtkDrawingArea *drawArea,cairo_t *cr, int width, int height,gpointer data)
{
    cairo_set_source_surface(cr,surface,0,0);
    cairo_paint(cr);
}

static void draw_brush(GtkWidget *widget, double x, double y)
{

    cairo_t *cr;
    //paint to surface
    cr = cairo_create(surface);
    cairo_rectangle(cr,x-3,y-3, 6,6);
    cairo_fill(cr);
    cairo_destroy(cr);

    gtk_widget_queue_draw(widget);
}

static double start_x;
static double start_y;

static void drag_begin(GtkGestureDrag *gesture,double x, double y, GtkWidget *area)
{
    start_x = x;
    start_y = y;

    draw_brush(area,x,y);
}

static void drag_update(GtkGestureDrag *gesture,double x,double y,GtkWidget *area)
{
     draw_brush(area,start_x+x, start_y+y);
}

static void drag_end(GtkGestureDrag *gesture, double x, double y, GtkWidget *area)
{

    draw_brush(area,start_x+x,start_y+y);
}
static void pressed(GtkGestureClick *gesture,int n_press, double x, double y, GtkWidget *area)
{
    clear_surface();
    gtk_widget_queue_draw(area);
}
static void close_window(void)
{
    if(surface)
    {
        cairo_surface_destroy(surface);
    }
}

static void activate(GtkApplication *app, gpointer user_data)
{
    GtkWidget *window;
    GtkWidget *frame;
    GtkWidget *draw_area;
    GtkGesture *drag;
    GtkGesture *press;

    window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window),"lemon");
    g_signal_connect(window,"destroy",G_CALLBACK(close_window),NULL);

    frame = gtk_frame_new(NULL);
    gtk_window_set_child(GTK_WINDOW(window),frame);
    draw_area = gtk_drawing_area_new();
    gtk_widget_set_size_request(draw_area,100,100);
    gtk_frame_set_child(GTK_FRAME(frame),draw_area);

    gtk_drawing_area_set_draw_func(GTK_DRAWING_AREA(draw_area),draw_cb,NULL,NULL);
    g_signal_connect_after(draw_area,"resize",G_CALLBACK(resize_cb),NULL);

    drag = gtk_gesture_drag_new();
    gtk_gesture_single_set_button(GTK_GESTURE_SINGLE(drag),GDK_BUTTON_PRIMARY);
    gtk_widget_add_controller(draw_area,GTK_EVENT_CONTROLLER(drag));
    g_signal_connect(drag,"drag-being",G_CALLBACK(drag_begin),draw_area);
    g_signal_connect(drag,"drag-update",G_CALLBACK(drag_update),draw_area);
    g_signal_connect(drag,"drag-end",G_CALLBACK(drag_end),draw_area);

    press = gtk_gesture_click_new();
    gtk_gesture_single_set_button(GTK_GESTURE_SINGLE(press), GDK_BUTTON_SECONDARY);
    gtk_widget_add_controller(draw_area,GTK_EVENT_CONTROLLER(press));
    g_signal_connect(press,"pressed",G_CALLBACK(pressed),draw_area);

    gtk_window_present(GTK_WINDOW(window));
}

int main (int    argc,char **argv)
{
  GtkApplication *app;
  int status;

  app = gtk_application_new ("org.gtk.example", G_APPLICATION_DEFAULT_FLAGS);
  g_signal_connect (app, "activate", G_CALLBACK (activate), NULL);
  status = g_application_run (G_APPLICATION (app), argc, argv);
  g_object_unref (app);

  return status;
}
