#include "pebble.h"
#include "main.h" // Used for drawing hands
#ifdef PBL_PLATFORM_BASALT // Only use this for 3.0+
  #include "gcolor_definitions.h" // Allows the use of colors
#endif
  
static Window *s_main_window; // Main window
static Layer *s_solid_bg_layer, *s_hands_layer, *s_date_layer; // Solid color background, hands, and date layer
static BitmapLayer *s_background_layer; // Face layer
static GBitmap *s_background_bitmap; // Face bitmap
static TextLayer *s_day_label, *s_month_label, *s_num_label; // Day, month, and date number labels

static GFont *s_date_font, *s_num_font; // Fonts

static char s_day_buffer[] = "XXXXXXXXX"; // Day buffer
static char s_num_buffer[] ="00"; // Date number buffer
static char s_month_buffer[] = "XXXXXXXXX"; // Month buffer

static GPath *s_minute_arrow, *s_hour_arrow, *s_second_arrow; // Path for hands
static GPath *s_minute_tip, *s_hour_tip, *s_second_tip; // Path for tips of hands
static GPath *s_center_outer, *s_center_inner; // Path for centers

// Create uppercase text when called
char *upcase(char *str)
{
    for (int i = 0; str[i] != 0; i++) {
        if (str[i] >= 'a' && str[i] <= 'z') {
            str[i] -= 0x20;
        }
    }

    return str;
}

// Update background when called
static void bg_update_proc(Layer *layer, GContext *ctx) {
  graphics_context_set_fill_color(ctx, GColorBlack); // Create fill color
  graphics_fill_rect(ctx, layer_get_bounds(layer), 0, GCornerNone); // Apply it
}

// Update date when called
static void date_update_proc(Layer *layer, GContext *ctx) {
  time_t now = time(NULL); // Get time
  struct tm *t = localtime(&now); // Create time structure

  strftime(s_day_buffer, sizeof(s_day_buffer), "%A", t); // Set buffer to day
  strftime(s_num_buffer, sizeof(s_num_buffer), "%d", t); // Set buffer to date number
  strftime(s_month_buffer, sizeof(s_month_buffer), "%B", t); // Set buffer to month
  
  upcase(s_day_buffer); // Set day to uppercase
  upcase(s_num_buffer); // Set date number to uppercase
  upcase(s_month_buffer); // Set month to uppercase
  
  text_layer_set_text(s_day_label, s_day_buffer); // Set day label to day
  text_layer_set_text(s_num_label, s_num_buffer); // Set date number label to date number
  text_layer_set_text(s_month_label, s_month_buffer); // Set the month label to the month
}

// Update hands when called
static void hands_update_proc(Layer *layer, GContext *ctx) {
  time_t now = time(NULL); // Get time
  struct tm *t = localtime(&now); // Create time structure
  
  // Hour hand
  #ifdef PBL_COLOR // For color screens
    graphics_context_set_fill_color(ctx, GColorLightGray); // Set fill color
  #else // For black and white screen
    graphics_context_set_fill_color(ctx, GColorWhite); // Set fill color
    graphics_context_set_stroke_color(ctx, GColorBlack); // Set outline color
  #endif
  gpath_rotate_to(s_hour_arrow, (TRIG_MAX_ANGLE * (((t->tm_hour % 12) * 6) + (t->tm_min / 10))) / (12 * 6)); // Define the path that the hour hand will follow
  gpath_draw_filled(ctx, s_hour_arrow); // Fill the hand with the above color
  #ifdef PBL_COLOR
  #else 
    gpath_draw_outline(ctx, s_hour_arrow); // Draw outline if black and white
  #endif
    
  // Hour hand tip
  gpath_rotate_to(s_hour_tip, (TRIG_MAX_ANGLE * (((t->tm_hour % 12) * 6) + (t->tm_min / 10))) / (12 * 6)); // Define the path that the hour hand will follow
  graphics_context_set_fill_color(ctx, GColorWhite); // Set fill color
  gpath_draw_filled(ctx, s_hour_tip); // Fill the tip with the above color

  // Minute hand
  #ifdef PBL_COLOR // For color screens
    graphics_context_set_fill_color(ctx, GColorLightGray); // Set fill color
  #endif // No need to redefine black and white
  gpath_rotate_to(s_minute_arrow, TRIG_MAX_ANGLE * t->tm_min / 60); // Define the path that the minute hand will follow
  gpath_draw_filled(ctx, s_minute_arrow); // Fill the hand with the above color
  #ifdef PBL_COLOR
  #else 
    gpath_draw_outline(ctx, s_minute_arrow); // Draw outline if black and white
  #endif
    
  // Minute hand tip
  gpath_rotate_to(s_minute_tip, TRIG_MAX_ANGLE * t->tm_min / 60); // Define the path that the minute hand tip will follow
  graphics_context_set_fill_color(ctx, GColorWhite); // Set fill color
  gpath_draw_filled(ctx, s_minute_tip); // Fill the tip with the above color
  
  //Second hand
  #ifdef PBL_COLOR // For color
    graphics_context_set_fill_color(ctx, GColorArmyGreen); // Set fill color
  #endif // No need to redefine black and white
  gpath_rotate_to(s_second_arrow, (TRIG_MAX_ANGLE * t->tm_sec / 60)); // Define the path that the second hand will follow
  gpath_draw_filled(ctx, s_second_arrow); // Fill the hand with the above color

  // Second hand tip
  #ifdef PBL_COLOR
    graphics_context_set_fill_color(ctx, GColorYellow); // Set fill color
  #endif
  gpath_rotate_to(s_second_tip, (TRIG_MAX_ANGLE * t->tm_sec / 60)); // Define the path the second hand tip will follow
  gpath_draw_filled(ctx, s_second_tip); // Fill the tip with the above color
  
  gpath_draw_filled(ctx, s_center_outer); // Fill the outer center with the above color
  
  graphics_context_set_fill_color(ctx, GColorBlack); // Set fill color
  gpath_draw_filled(ctx, s_center_inner); // Fill the inner center with the above color
}

// Update hands every second when called
static void handle_second_tick(struct tm *tick_time, TimeUnits units_changed) {
  layer_mark_dirty(window_get_root_layer(s_main_window));
}

// Loads the layers onto the main window
static void window_load(Window *s_main_window) {
  Layer *window_layer = window_get_root_layer(s_main_window); // Creates main layer
  GRect bounds = layer_get_bounds(window_layer); // Sets the bounds of the main layer to the full screen
  
  s_date_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_OPEN_SANS_12));
  s_num_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_OPEN_SANS_16));

  s_background_bitmap = gbitmap_create_with_resource(RESOURCE_ID_BACKGROUND); // Set the background face bitmap resource
  
  s_solid_bg_layer = layer_create(bounds); // Create solid background color layer
  s_background_layer = bitmap_layer_create(bounds); // Create face layer
  s_date_layer = layer_create(bounds); // Create date layer
  s_hands_layer = layer_create(bounds); // Create hands layer 
  s_day_label = text_layer_create(GRect(0, 30, 144, 30)); // Create day label
  s_num_label = text_layer_create(GRect(0, 105, 144, 30)); // Create number label
  s_month_label = text_layer_create(GRect(0, 120, 144, 30)); // Create month label
  
  text_layer_set_text(s_day_label, s_day_buffer); // Set buffer to label
  text_layer_set_text(s_num_label, s_num_buffer); // Set buffer to label
  text_layer_set_text(s_month_label, s_month_buffer); // Set buffer to label
  
  layer_set_update_proc(s_solid_bg_layer, bg_update_proc); // Update solid color background layer
  layer_set_update_proc(s_date_layer, date_update_proc); // Update date layer
  layer_set_update_proc(s_hands_layer, hands_update_proc); // Update hands layer
  
  bitmap_layer_set_bitmap(s_background_layer, s_background_bitmap); // Apply bitmap to face layer
  
  #if PBL_PLATFORM_BASALT // Only set this for 3.0 +
    bitmap_layer_set_compositing_mode(s_background_layer, GCompOpSet); // Set png to have transparency
  #endif
    
  #ifdef PBL_COLOR // For color screens
    text_layer_set_text_color(s_num_label, GColorYellow); // Set color for date number label
  #else // For Black and White screens
    text_layer_set_text_color(s_num_label, GColorWhite); // Set black and white for date number label
  #endif
  text_layer_set_text_color(s_day_label, GColorWhite); // Set color for day label
  text_layer_set_text_color(s_month_label, GColorWhite); // Set color for month label
  
  text_layer_set_background_color(s_day_label, GColorClear); // Set background to transparent
  text_layer_set_background_color(s_num_label, GColorClear); // Set background to transparent
  text_layer_set_background_color(s_month_label, GColorClear); // Set background to transparent
  
  text_layer_set_font(s_day_label, s_date_font); // Set day font
  text_layer_set_font(s_num_label, s_num_font); // Set date number font
  text_layer_set_font(s_month_label, s_date_font); // Set month font
  
  text_layer_set_text_alignment(s_day_label, GTextAlignmentCenter); // Center day label
  text_layer_set_text_alignment(s_num_label, GTextAlignmentCenter); // Center date number label
  text_layer_set_text_alignment(s_month_label, GTextAlignmentCenter); // Center month label
  
  layer_add_child(window_layer, s_solid_bg_layer); // Add to main layer
  layer_add_child(window_layer, bitmap_layer_get_layer(s_background_layer)); // Add to main layer
  layer_add_child(window_layer, s_date_layer); // Add to main layer
  layer_add_child(s_date_layer, text_layer_get_layer(s_day_label)); // Add to date layer
  layer_add_child(s_date_layer, text_layer_get_layer(s_num_label)); // Add to date layer
  layer_add_child(s_date_layer, text_layer_get_layer(s_month_label)); // Add to date layer
  layer_add_child(window_layer, s_hands_layer); // Add to main layer
}

// Unload the layers from the main window
static void window_unload(Window *s_main_window) {
  
  layer_destroy(s_solid_bg_layer); // Destroy solid color background layer
  layer_destroy(s_hands_layer); // Destroy hands layer
  layer_destroy(s_date_layer); // Destroy date layer
  
  gbitmap_destroy(s_background_bitmap); // Destroy face bitmap
  bitmap_layer_destroy(s_background_layer); // Destroy face layer
  
  text_layer_destroy(s_day_label); // Destroy day label 
  text_layer_destroy(s_num_label); // Destroy date number label
  text_layer_destroy(s_month_label); // Destroy month label
  
  fonts_unload_custom_font(s_date_font); // Unload date font
  fonts_unload_custom_font(s_num_font); // Unload date number font
}

// Initialize the main window
static void init() {
  s_main_window = window_create(); // Create main window
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = window_load, // Allow window_load to manage window
    .unload = window_unload, // Allow window_unload to manage window
  });
  window_stack_push(s_main_window, true); // Show the window. Animated = true

  s_minute_arrow = gpath_create(&MINUTE_HAND_POINTS); // Draw minute hand
  s_minute_tip = gpath_create(&MINUTE_HAND_TIP); // Draw minute tip
  s_hour_arrow = gpath_create(&HOUR_HAND_POINTS); // Draw hour hand
  s_hour_tip = gpath_create(&HOUR_HAND_TIP); // Draw hour tip
  s_second_arrow = gpath_create(&SECOND_HAND_POINTS); // Draw second hand
  s_second_tip = gpath_create(&SECOND_HAND_TIP); // Draw second tip
  s_center_outer = gpath_create(&CENTER_POINT_OUTER); // Draw outer center point
  s_center_inner = gpath_create(&CENTER_POINT_INNER); // Draw inner center point

  Layer *window_layer = window_get_root_layer(s_main_window); // Create main layer
  GRect bounds = layer_get_bounds(window_layer); // Set bounds for main layer
  GPoint center = grect_center_point(&bounds); // Create center
  gpath_move_to(s_minute_arrow, center); // Move minute hand to center
  gpath_move_to(s_hour_arrow, center); // Move hour hand to center
  gpath_move_to(s_second_arrow, center); // Move second hand to center
  gpath_move_to(s_minute_tip, center); // Move minute tip to center
  gpath_move_to(s_hour_tip, center); // Movie hour tip to center
  gpath_move_to(s_second_tip, center); // Move second tip to center
  gpath_move_to(s_center_outer, center); // Move outer center point to center
  gpath_move_to(s_center_inner, center); // Move inner center point to center
  
  s_day_buffer[0] = '\0'; // Reset day buffer
  s_num_buffer[0] = '\0'; // Reset date number buffer
  s_month_buffer[0] = '\0'; // Reset month buffer

// Call to the handle_second_tick to update the watch every second
  tick_timer_service_subscribe(SECOND_UNIT, handle_second_tick);
}

// Deinitialize the main window
static void deinit() {
  gpath_destroy(s_minute_arrow); // Destroy the minute hand
  gpath_destroy(s_hour_arrow); // Destroy the hour hand
  gpath_destroy(s_second_arrow); // Destroy second hand
  gpath_destroy(s_minute_tip); // Destroy minute tip
  gpath_destroy(s_hour_tip); // Destroy hour tip
  gpath_destroy(s_second_tip); // Destroy second tip
  gpath_destroy(s_center_outer); // Destroy outer center point
  gpath_destroy(s_center_inner); // Destroy inner center point
  
  tick_timer_service_unsubscribe(); // Unsubscribe from the tick timer

  window_destroy(s_main_window); // Destroy the main window
}

int main() {
  init();
  app_event_loop();
  deinit();
}
