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


#define im_draw_property_dss_map
/// im_draw_property_dss_map(name, map)
var _name, _map, _isOpen;
_name = argument0;
_map = argument1;

im_table_next_row_and_column();
_isOpen = im_tree_node(string(_name) + "##" + string(_map));
im_table_next_column();
im_text("Map (" + string(ds_map_size(_map)) + ")");

if (_isOpen) {
    var _key, _size, _val;
    _size = ds_map_size(_map);
    _key = ds_map_find_first(_map);
    repeat (_size) {
        _val = ds_map_find_value(_map, _key);
        if (is_real(_val) && frac(_val) == 0.125) {
            im_draw_property_dss_list(string(_key), _val);
        } else if (is_real(_val) && frac(_val) == 0.0625) {
            im_draw_property_dss_map(string(_key), _val);
        } else {
            im_table_next_row_and_column();
            im_text(string(_key));
            im_table_next_column();
            im_push_item_width(-1);
            if (is_string(_val)) {
                var _new_val;
                _new_val = im_input_text("##map_"+string(_map)+"_key_"+string(_key), _val);
                if (_new_val != _val) ds_map_replace(_map, _key, _new_val);
            } else {
                var _new_val;
                _new_val = im_input_real("##map_"+string(_map)+"_key_"+string(_key), _val);
                if (_new_val != _val) ds_map_replace(_map, _key, _new_val);
            }
            im_pop_item_width();
        }
        _key = ds_map_find_next(_map, _key);
    }
    im_tree_pop();
}
return _map;


#define im_draw_property_dss_list
/// im_draw_property_dss_list(name, list)
var _name, _list, _isOpen;
_name = argument0;
_list = argument1;

im_table_next_row_and_column();
_isOpen = im_tree_node(string(_name) + "##" + string(_list));
im_table_next_column();
im_text("List (" + string(ds_list_size(_list)) + ")");

if (_isOpen) {
    var _i, _size, _val;
    _size = ds_list_size(_list);
    for (_i = 0; _i < _size; _i += 1) {
        _val = ds_list_find_value(_list, _i);
        if (is_real(_val) && frac(_val) == 0.125) {
            im_draw_property_dss_list(string(_i), _val);
        } else if (is_real(_val) && frac(_val) == 0.0625) {
            im_draw_property_dss_map(string(_i), _val);
        } else {
            im_table_next_row_and_column();
            im_text(string(_i));
            im_table_next_column();
            im_push_item_width(-1);
            if (is_string(_val)) {
                var _new_val;
                _new_val = im_input_text("##list_"+string(_list)+"_idx_"+string(_i), _val);
                if (_new_val != _val) ds_list_replace(_list, _i, _new_val);
            } else {
                var _new_val;
                _new_val = im_input_real("##list_"+string(_list)+"_idx_"+string(_i), _val);
                if (_new_val != _val) ds_list_replace(_list, _i, _new_val);
            }
            im_pop_item_width();
        }
    }
    im_tree_pop();
}
return _list;


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


#define im_window_begin
var _f; 
_f = 0; 
if (argument_count > 1) _f = argument1;
return _im_window_begin(argument0, _f);


#define im_window_begin_closable
var _f; 
_f = 0; 
if (argument_count > 2) _f = argument2;
return _im_window_begin_closable(argument0, argument1, _f);

