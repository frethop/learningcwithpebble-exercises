//----------------------------------------------------------------------------------
//
// Project 9.4 from "Learning C with Pebble"
// 
// Matthew Reiss with Mike Jipping, April 2016

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

// Work the compas value into a string using snprintf
void compass(CompassHeadingData data){
	snprintf(compass_buffer, sizeof(compass_buffer), "Heading: %d˚", (int)data.true_heading);
	update_compass();
}

// Use snprintf to work the accelerator value into a string
void accel(AccelData *data, uint32_t num_samples){
	snprintf(accel_buffer, sizeof(accel_buffer), "X: %d, Y: %d, Z: %d", data[0].x, data[0].y, data[0].z);
	update_accel();
}

// Derive a string from the light level
void light(){
	static HealthMinuteData *data;
	health_service_get_minute_history(data, 1, (time_t*)(time(NULL)-5*60*1000), (time_t*)(time(NULL)));
	AmbientLightLevel level = data[0].light;
	switch(level){
		case AmbientLightLevelDark: snprintf(light_buffer, sizeof(light_buffer), "Light: Dark");
		case AmbientLightLevelLight: snprintf(light_buffer, sizeof(light_buffer), "Light: Light");
		case AmbientLightLevelUnknown: snprintf(light_buffer, sizeof(light_buffer), "Light: Unknown");
		case AmbientLightLevelVeryDark: snprintf(light_buffer, sizeof(light_buffer), "Light: Very Dark");
		case AmbientLightLevelVeryLight: snprintf(light_buffer, sizeof(light_buffer), "Light: Very Light");
	}
	update_light();
}

void deinit_helper_functions(){
	compass_service_unsubscribe();
	accel_data_service_unsubscribe();
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