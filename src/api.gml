#define im_sprite
_im_sprite(sprite_get_texture(argument0, argument1));


#define im_menu_item
var _s; 
_s = ""; 
if (argument_count > 1) _s = argument1;
return _im_menu_item(argument0, _s);


#define im_input_text
var _f; 
_f = 0; 
if (argument_count > 2) _f = argument2;
return _im_input_text(argument0, argument1, _f);


#define im_input_text_multiline
var _f; 
_f = 0; 
if (argument_count > 4) _f = argument4;
return _im_input_text_multiline(argument0, argument1, argument2, argument3, _f);


#define im_input_real
var _f; 
_f = 0; 
if (argument_count > 2) _f = argument2;
return _im_input_real(argument0, argument1, _f);


#define im_input_color
var _f; 
_f = 0; 
if (argument_count > 2) _f = argument2;
return _im_input_color(argument0, argument1, _f);


#define im_draw_property_real
var _f; 
_f = 0; 
if (argument_count > 2) _f = argument2;
return _im_draw_property_real(argument0, argument1, _f);


#define im_draw_property_color
var _f; 
_f = 0; 
if (argument_count > 2) _f = argument2;
return _im_draw_property_color(argument0, argument1, _f);


#define im_draw_property_text
var _f; 
_f = 0; 
if (argument_count > 2) _f = argument2;
return _im_draw_property_text(argument0, argument1, _f);

