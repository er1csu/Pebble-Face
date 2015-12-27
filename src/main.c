#include <pebble.h>

static Window* s_main_window; 
static TextLayer* s_time_layer; 
static GFont s_time_font; 

// For the bitmap image
static BitmapLayer *s_background_layer;
static GBitmap *s_background_bitmap;

static void update_time() {
    // Get a tm struct
    time_t temp = time(NULL); 
    struct tm* tick_time = localtime(&temp); 
    
    // Place the time into a buffer
    static char s_buffer[8];
    strftime(s_buffer, sizeof(s_buffer), clock_is_24h_style() ? "%H:%M" : "I:%M", tick_time);
    
    // The actual time to display
    text_layer_set_text(s_time_layer, s_buffer); 
    
}

static void tick_handler(struct tm* tick_time, TimeUnits units_changed) {
    update_time(); 
}


static void main_window_load(Window* window) {
    // Making the layer use the entire canvas
    Layer* root_window_layer = window_get_root_layer(window); 
    GRect bounds = layer_get_bounds(root_window_layer); 
    
    // Load and initialize the time font
    s_time_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_PERFECT_DOS_VGA_24));
    s_time_layer = text_layer_create(GRect(0, PBL_IF_ROUND_ELSE(58, 52), bounds.size.w, 50));  
    
     
    // Make the time layer background clear, the text black, set font, and align
    text_layer_set_background_color(s_time_layer, GColorClear);
    text_layer_set_text_color(s_time_layer, GColorBlack);
    text_layer_set_font(s_time_layer, s_time_font);
    text_layer_set_text_alignment(s_time_layer, GTextAlignmentLeft);
     
    // Set up the bitmap and the layer for the bitmap, Then put the bitmap onto the layer
    s_background_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_WATCH_DESIGN); 
    s_background_layer = bitmap_layer_create(bounds); 
    bitmap_layer_set_bitmap(s_background_layer, s_background_bitmap); 
    
     // Then, add the layer onto the root window layer
    layer_add_child(root_window_layer, bitmap_layer_get_layer(s_background_layer)); 
    layer_add_child(root_window_layer, text_layer_get_layer(s_time_layer)); 
}

static void main_window_unload(Window* window) {
    text_layer_destroy(s_time_layer); 
    fonts_unload_custom_font(s_time_font); 
    // Destroy GBitmap
    gbitmap_destroy(s_background_bitmap);

    // Destroy BitmapLayer
    bitmap_layer_destroy(s_background_layer);
}

static void init() {
    // Create the main Window element and set it to the pointer
    s_main_window = window_create(); 
        
    // Set handlers to manage the elements inside the Window
    window_set_window_handlers(s_main_window, (WindowHandlers) {
        .load = main_window_load,
        .unload = main_window_unload
     });
    
    // Set bg color of main window to match the image
    window_set_background_color(s_main_window, GColorBlack); 
    
    // Show the Window on the watch, with animated=true
    window_stack_push(s_main_window, true);
    update_time(); 
    
    // Subscribe to TickTimerService
    tick_timer_service_subscribe(MINUTE_UNIT, tick_handler); 
}

static void deinit() {
    window_destroy(s_main_window); 
}

int main(void) {
    init(); 
    app_event_loop(); 
    deinit();
}



