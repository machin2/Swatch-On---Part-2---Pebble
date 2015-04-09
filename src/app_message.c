#include <pebble.h>

Window *window;	

#define NUM_MENU_SECTIONS 2
#define NUM_FIRST_MENU_ITEMS 5
#define NUM_SECOND_MENU_ITEMS 1  

// MENU LAYER
static SimpleMenuLayer *simple_menu_layer;

// A simple menu layer can have multiple sections
static SimpleMenuSection menu_sections[NUM_MENU_SECTIONS];

// Each section is composed of a number of menu items
static SimpleMenuItem first_menu_items[NUM_FIRST_MENU_ITEMS];

static SimpleMenuItem second_menu_items[NUM_SECOND_MENU_ITEMS];

static bool special_flag = true;

// static TextLayer *bt_layer;



///////// COMMUNICATION APP_MESSAGE ////////////////////////////////////////////////////////////////////////



// Key values for AppMessage Dictionary
enum {
	DATA_KEY = 0x0,	
  MESSAGE_KEY = 0x1,
};


// Write message to buffer & send
void send_message(uint8_t requete){
	DictionaryIterator *iter;
	
	app_message_outbox_begin(&iter);
	dict_write_uint8(iter, DATA_KEY, requete);
	
	dict_write_end(iter);
  	app_message_outbox_send();
}

// Called when a message is received from PebbleKitJS
static void in_received_handler(DictionaryIterator *received, void *context) {
	Tuple *tuple;
	
	tuple = dict_find(received, DATA_KEY);
	if(tuple) {
		APP_LOG(APP_LOG_LEVEL_DEBUG, "Received Status: %d", (int)tuple->value->uint32); 
	}
	
	tuple = dict_find(received, MESSAGE_KEY);
	if(tuple) {
		APP_LOG(APP_LOG_LEVEL_DEBUG, "Received Message: %s", tuple->value->cstring);
	}}

// Called when an incoming message from PebbleKitJS is dropped
static void in_dropped_handler(AppMessageResult reason, void *context) {	
}

// Called when PebbleKitJS does not acknowledge receipt of a message
static void out_failed_handler(DictionaryIterator *failed, AppMessageResult reason, void *context) {
}


///////////////////////////////////////////////////////////




// MENU SELECT CALLBACK


static void menu_select_callback(int index, void *ctx) {
        

        if (special_flag) {
        first_menu_items[index].subtitle = "Allumé";
          send_message(2);
          special_flag = !special_flag;
        

        } else {
        first_menu_items[index].subtitle = "Éteint";
          send_message(3);
          special_flag = !special_flag;
        }

        // Mark the layer to be updated
        layer_mark_dirty(simple_menu_layer_get_layer(simple_menu_layer));
        }



// SPECIAL SELECT CALLBACK
static void special_select_callback(int index, void *ctx) {
        // Of course, you can do more complicated things in a menu item select callback
        // Here, we have a simple toggle
        special_flag = !special_flag;

        SimpleMenuItem *menu_item = &second_menu_items[index];

        if (special_flag) {
        menu_item->subtitle = "Au revoir";
        } else {
        menu_item->subtitle = "Merci de votre visite.";
          window_destroy(window);
        }


        // Mark the layer to be updated
        layer_mark_dirty(simple_menu_layer_get_layer(simple_menu_layer));
        }



////////////////////////////////////////////
// WINDOW LOAD
///////////////////////////////////////////

static void window_load(Window *window) {
   
  /*
  //Setup BT Layer
  bt_layer = text_layer_create(GRect(5, 5, 144, 30));
  text_layer_set_font(bt_layer, fonts_get_system_font(FONT_KEY_GOTHIC_18));
  if(bluetooth_connection_service_peek() == true)
  {
    text_layer_set_text(bt_layer, "BT: CONNECTED");
  }
  else
  {
    text_layer_set_text(bt_layer, "BT: DISCONNECTED");
  }
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(bt_layer));*/

  
  
        // Although we already defined NUM_FIRST_MENU_ITEMS, you can define
        // an int as such to easily change the order of menu items later
        int num_a_items = 0;

        // This is an example of how you'd set a simple menu item
        first_menu_items[num_a_items++] = (SimpleMenuItem){
        // You should give each menu item a title and callback
        .title = "Chambre",
        .subtitle = "Sélection",
        .callback = menu_select_callback,
        };
        // The menu items appear in the order saved in the menu items array
        first_menu_items[num_a_items++] = (SimpleMenuItem){
        .title = "Cuisine",
        // You can also give menu items a subtitle
        .subtitle = "Sélection",
        .callback = menu_select_callback,
        };
        first_menu_items[num_a_items++] = (SimpleMenuItem){
        .title = "Bureau",
        .subtitle = "Sélection",
        .callback = menu_select_callback,
        // This is how you would give a menu item an icon
        //  .icon = menu_icon_image,
        };
        first_menu_items[num_a_items++] = (SimpleMenuItem){
        // You should give each menu item a title and callback
        .title = "Salle de bain",
        .subtitle = "Sélection",
        .callback = menu_select_callback,
        };
        first_menu_items[num_a_items++] = (SimpleMenuItem){
        // You should give each menu item a title and callback
        .title = "TV",
        .subtitle = "Sélection",
        .callback = menu_select_callback,
        };

        // This initializes the second section
        second_menu_items[0] = (SimpleMenuItem){
        .title = "Retour",
        // You can use different callbacks for your menu items
        .callback = special_select_callback,
        };

        // Bind the menu items to the corresponding menu sections
        menu_sections[0] = (SimpleMenuSection){
        .title = "Sélection des pièces",
        .num_items = NUM_FIRST_MENU_ITEMS,
        .items = first_menu_items,
        };
        menu_sections[1] = (SimpleMenuSection){
        // Menu sections can also have titles as well
        .title = "Fin de la sélection",
        .num_items = NUM_SECOND_MENU_ITEMS,
        .items = second_menu_items,
        };

// INITIALISATION WINDOW, MENU LAYER,...



        // Now we prepare to initialize the simple menu layer
        // We need the bounds to specify the simple menu layer's viewport size
        // In this case, it'll be the same as the window's
        Layer *window_layer = window_get_root_layer(window);
        GRect bounds = layer_get_frame(window_layer);

        // Initialize the simple menu layer
        simple_menu_layer = simple_menu_layer_create(bounds, window, menu_sections, NUM_MENU_SECTIONS, NULL);

        // Add it to the window for display
        layer_add_child(window_layer, simple_menu_layer_get_layer(simple_menu_layer));
        }

// Deinitialize resources on window unload that were initialized on window load
        void window_unload(Window *window) {
        simple_menu_layer_destroy(simple_menu_layer);
         // text_layer_destroy(bt_layer);
        }


void init(void) {
	window = window_create();
  
  // Setup the window handlers
        window_set_window_handlers(window, (WindowHandlers) {
        .load = window_load,
        .unload = window_unload,
        });

	window_stack_push(window, true);
  

	
	// Register AppMessage handlers
	app_message_register_inbox_received(in_received_handler); 
	app_message_register_inbox_dropped(in_dropped_handler); 
	app_message_register_outbox_failed(out_failed_handler);
		
	app_message_open(app_message_inbox_size_maximum(), app_message_outbox_size_maximum());

}

void deinit(void) {
	app_message_deregister_callbacks();
	window_destroy(window);
}

int main( void ) {
	init();
	app_event_loop();
	deinit();
}