#include <pebble.h>

TextLayer *textlayer1, *textlayer2, *textlayer3;

char compass_buffer[32];
char accel_buffer[32];
char light_buffer[32];

Window *window;

char *get_compass(){
	return compass_buffer;
}

char *get_accelerometer(){
	return accel_buffer;
}

char *get_light(){
	return light_buffer;
}

void update_compass();
void update_accel();
void update_light();

void compass(CompassHeadingData data){
	// Build compass_buffer here from the struct "data" -- use data.true_heading
	update_compass();
}

void accel(AccelData *data, uint32_t num_samples){
	// Build accel_buffer here from the struct data: use x, y, and z components.
	update_accel();
}

void light(){
	static HealthMinuteData *data;
	health_service_get_minute_history(data, 1, (time_t*)(time(NULL)-5*60*1000), (time_t*)(time(NULL)));
	AmbientLightLevel level = data[0].light;

    // build the string light_buffer here from the "level" variable.
    
	update_light();
}

void update_compass(){
	text_layer_set_text(textlayer1, get_compass());
}

void update_accel(){
	text_layer_set_text(textlayer2, get_accelerometer());
}

void update_light(){
	text_layer_set_text(textlayer3, get_light());
}

void init_helper_functions(){
	compass_service_subscribe(compass);
	accel_data_service_subscribe(1, accel);
	light();
}

void deinit_helper_functions(){
	compass_service_unsubscribe();
	accel_data_service_unsubscribe();
}

void init(){
	window = window_create();
	
	Layer *root = window_get_root_layer(window);
	
	GRect bounds = layer_get_bounds(root);
	
	textlayer1 = text_layer_create(GRect(0, 0, bounds.size.w, bounds.size.h/3));
	text_layer_set_text_alignment(textlayer1, GTextAlignmentCenter);
	
	textlayer2 = text_layer_create(GRect(0, bounds.size.h/3, bounds.size.w, bounds.size.h/3));
	text_layer_set_text_alignment(textlayer2, GTextAlignmentCenter);
	
	textlayer3 = text_layer_create(GRect(0, 2*bounds.size.h/3, bounds.size.w, bounds.size.h/3));
	text_layer_set_text_alignment(textlayer3, GTextAlignmentCenter);
	
	layer_add_child(root, text_layer_get_layer(textlayer1));
	layer_add_child(root, text_layer_get_layer(textlayer2));
	layer_add_child(root, text_layer_get_layer(textlayer3));
	
	window_stack_push(window, true);
	
	init_helper_functions();
}

void deinit(){
	text_layer_destroy(textlayer1);
	text_layer_destroy(textlayer2);
	text_layer_destroy(textlayer3);
	window_destroy(window);
	
	deinit_helper_functions();
}

int main(void){
	init();
	app_event_loop();
	deinit();
}