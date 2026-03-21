# UselessImGui
## Overview
All functions mirror Dear ImGui, but prefixed with `im_`, so C++ ImGui should return the same in GML.

## Quick Nav
* \ref api.cpp "API Function List"

## Added functions that are not in ImGui
* im_sprite
* im_draw_property_real
* im_draw_property_text
* im_draw_property_color
* im_table_next_row_and_column

## Missing functions
* ImGui::Image - use im_sprite instead
* ImGui::ImageButton
