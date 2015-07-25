#include <pebble.h>
#include <main.h>
static Window *s_main_window;
static TextLayer *s_battery_layer;
static BitmapLayer *s_connection_layer;
static TextLayer *s_time_layer;
static TextLayer *s_date_layer;
static BitmapLayer *s_background_layer;
static GBitmap *s_background_bitmap;
static GBitmap *s_active_connection_bitmap;
static GBitmap *s_inactive_connection_bitmap;

static void handle_battery(BatteryChargeState charge_state) {
  // Battery handler
  static char battery_text[] = "100%";

  if (charge_state.is_charging) {
    snprintf(battery_text, sizeof(battery_text), "%d%%*", charge_state.charge_percent);
  } else {
    snprintf(battery_text, sizeof(battery_text), "%d%%", charge_state.charge_percent);
  }
  text_layer_set_text(s_battery_layer, battery_text);
}

static void handle_bluetooth(bool connected) {  
  if (connected) {
    bitmap_layer_set_bitmap(s_connection_layer, s_active_connection_bitmap);  
  }
  else {
   bitmap_layer_set_bitmap(s_connection_layer, s_inactive_connection_bitmap);  
  }
  vibes_short_pulse();
}

static void update_time() {
  // Get a tm structure
  time_t temp = time(NULL); 
  struct tm *tick_time = localtime(&temp);

  // Create a long-lived buffer
  static char buffer[] = "00:00";
  
  // Write the current hours and minutes into the buffer
  if(clock_is_24h_style() == true) {
    // Use 24 hour format
    strftime(buffer, sizeof("00:00"), "%H:%M", tick_time);
  } else {
    // Use 12 hour format
    strftime(buffer, sizeof("00:00"), "%I:%M", tick_time);
  }
  
  // Display this time on the TextLayer
  text_layer_set_text(s_time_layer, buffer);
}  

static void update_date() {
  // Get a tm structure
  time_t temp = time(NULL); 
  struct tm *tick_time = localtime(&temp);
  
  // Create a long-lived buffer
  static char buffer2[] = "Mon Feb 29";
  static char buffer3[] = "Mon";
  char monday[] = "Mon";
  char tuesday[] = "Tue";
  char wednesday[] = "Wed";
  char thursday[] = "Thu";
  char friday[] = "Fri";
  char saturday[] = "Sat";
  char sunday[] = "Sun";
  
  // Get background acccording to day  
  strftime(buffer3, sizeof(buffer3), "%a", tick_time);
  if (strcmp(buffer3, monday) == 0) {
    s_background_bitmap = gbitmap_create_with_resource(RESOURCE_ID_BG_MONDAY);
    s_active_connection_bitmap = gbitmap_create_with_resource(RESOURCE_ID_B_MONDAY);
  }
  else if (strcmp(buffer3, tuesday) == 0) {
    s_background_bitmap = gbitmap_create_with_resource(RESOURCE_ID_BG_TUESDAY);
    s_active_connection_bitmap = gbitmap_create_with_resource(RESOURCE_ID_B_TUESDAY);
  }
  else if (strcmp(buffer3, wednesday) == 0) {
    s_background_bitmap = gbitmap_create_with_resource(RESOURCE_ID_BG_WEDNESDAY);
    s_active_connection_bitmap = gbitmap_create_with_resource(RESOURCE_ID_B_WEDNESDAY);
  }
  else if (strcmp(buffer3, thursday) == 0) {
    s_background_bitmap = gbitmap_create_with_resource(RESOURCE_ID_BG_THURSDAY);
    s_active_connection_bitmap = gbitmap_create_with_resource(RESOURCE_ID_B_THURSDAY);
  }
  else if (strcmp(buffer3, friday) == 0) {
    s_background_bitmap = gbitmap_create_with_resource(RESOURCE_ID_BG_FRIDAY);
    s_active_connection_bitmap = gbitmap_create_with_resource(RESOURCE_ID_B_FRIDAY);
  }
  else if (strcmp(buffer3, saturday) == 0) {    
    s_background_bitmap = gbitmap_create_with_resource(RESOURCE_ID_BG_SATURDAY);
    s_active_connection_bitmap = gbitmap_create_with_resource(RESOURCE_ID_B_SATURDAY);
  }
  else if (strcmp(buffer3, sunday) == 0) {
    s_background_bitmap = gbitmap_create_with_resource(RESOURCE_ID_BG_SUNDAY);
    s_active_connection_bitmap = gbitmap_create_with_resource(RESOURCE_ID_B_SUNDAY);
  }  
  bitmap_layer_set_bitmap(s_background_layer, s_background_bitmap);
  if (bluetooth_connection_service_peek()) {
    bitmap_layer_set_bitmap(s_connection_layer, s_active_connection_bitmap);  
  }
  else {
   bitmap_layer_set_bitmap(s_connection_layer, s_inactive_connection_bitmap);  
  }

  // Write the current date into the buffer
  strftime(buffer2, sizeof("Mon Feb 29"), "%a %b %e", tick_time);

  // Display date on the TextLayer
  text_layer_set_text(s_date_layer, buffer2);
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  update_time();
}

static void tick_handler2(struct tm *tick_time, TimeUnits units_changed) {
  update_date();
}

static void main_window_load(Window *window) {
  // Create GBitmap, then set to created BitmapLayer
  s_background_bitmap = gbitmap_create_with_resource(RESOURCE_ID_BG_MONDAY);
  s_background_layer = bitmap_layer_create(GRect(0, 54, 144, 114));
  bitmap_layer_set_bitmap(s_background_layer, s_background_bitmap);
  layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(s_background_layer));

  // Create time TextLayer
  s_time_layer = text_layer_create(GRect(0, 10, 144, 44));
  text_layer_set_background_color(s_time_layer, GColorBlack);
  text_layer_set_text_color(s_time_layer, GColorWhite);
  text_layer_set_text(s_time_layer, "00:00");
  
  // Create date TextLayer
  s_date_layer = text_layer_create(GRect(0, 0, 144, 19));
  text_layer_set_background_color(s_date_layer, GColorBlack);
  text_layer_set_text_color(s_date_layer, GColorWhite);
  text_layer_set_font(s_date_layer, fonts_get_system_font(FONT_KEY_GOTHIC_18));
  text_layer_set_text_alignment(s_date_layer, GTextAlignmentCenter);
  text_layer_set_text(s_date_layer, "Mon Jan 1");
  
  // Create battery TextLayer
  s_battery_layer = text_layer_create(GRect(0, 0, 144, 20));
  text_layer_set_text_color(s_battery_layer, GColorWhite);
  text_layer_set_background_color(s_battery_layer, GColorClear);
  text_layer_set_font(s_battery_layer, fonts_get_system_font(FONT_KEY_GOTHIC_18));
  text_layer_set_text_alignment(s_battery_layer, GTextAlignmentRight);
  text_layer_set_text(s_battery_layer, "0%");
  handle_battery(battery_state_service_peek());
  
  // Create connection BitmapLayer
  s_active_connection_bitmap = gbitmap_create_with_resource(RESOURCE_ID_B_MONDAY);
  s_inactive_connection_bitmap = gbitmap_create_with_resource(RESOURCE_ID_B_NONE);
  s_connection_layer = bitmap_layer_create(GRect(0, 5, 19, 19));
  bitmap_layer_set_bitmap(s_connection_layer, s_active_connection_bitmap);  

  // Improve the layout to be more like a watchface
  text_layer_set_font(s_time_layer, fonts_get_system_font(FONT_KEY_BITHAM_42_BOLD));
  text_layer_set_text_alignment(s_time_layer, GTextAlignmentCenter);

  // Add child layers to the Window's root layer  
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_time_layer));  
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_date_layer));
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_battery_layer));
  layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(s_connection_layer));
}
static void main_window_unload(Window *window) {
// Destroy TextLayer
  text_layer_destroy(s_time_layer);
  text_layer_destroy(s_battery_layer);
  text_layer_destroy(s_date_layer);
  battery_state_service_unsubscribe();
  bluetooth_connection_service_unsubscribe();
}

static void init() {
 // Create main Window element and assign to pointer
  s_main_window = window_create();

  // Set handlers to manage the elements inside the Window
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload
  });

  // Show the Window on the watch, with animated=true
  window_stack_push(s_main_window, true);
  
  // Make sure the time is displayed from the start
  update_time();
  update_date();
  
  // Register with TickTimerService
  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
  tick_timer_service_subscribe(DAY_UNIT, tick_handler2);
  battery_state_service_subscribe(handle_battery);
  bluetooth_connection_service_subscribe(handle_bluetooth);
  
}
static void deinit() {
  // Destroy Window
  window_destroy(s_main_window);
  // Destroy GBitmap
  gbitmap_destroy(s_active_connection_bitmap);
  gbitmap_destroy(s_inactive_connection_bitmap);
  gbitmap_destroy(s_background_bitmap);
  // Destroy BitmapLayer
  bitmap_layer_destroy(s_background_layer);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}