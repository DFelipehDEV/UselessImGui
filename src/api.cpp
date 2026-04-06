/** @file */
#define _CRT_SECURE_NO_WARNINGS
#include "../useless.hpp"
#include "imgui.h"
#include "imgui_internal.h"
#include "imgui.h"

extern bool imgui_initialized;
bool g_rebuild_fonts = false;
static char string_buffer[65536];

ImFont* g_im_fonts[256];
int g_im_font_count = 0;

/**
 * @brief Returns the version of UselessImGui.
 * @return Version.
 */
GMREAL im_get_version() {
    return 0.3;
}

/**
 * @brief Add font from file ttf.
 * @param filepath Path to the file.
 * @param size_pixels Font size in pixels.
 * @param antialiasing Antialiasing enabled or disabled.
 * @return Index of the loaded font, or -1 if failed.
 */
GMREAL im_add_font_from_file_ttf(const char* filepath, double size_pixels, double antialiasing) {
    if (g_im_font_count >= 256) return -1.0;
    
    ImFontConfig font_cfg;
    if (antialiasing <= 0.5) {
        font_cfg.OversampleH = 1;
        font_cfg.OversampleV = 1;
        font_cfg.PixelSnapH = true;
    }

    ImFont* font = ImGui::GetIO().Fonts->AddFontFromFileTTF(filepath, static_cast<float>(size_pixels), &font_cfg);
    if (!font) return -1.0;
    
    if (imgui_initialized) g_rebuild_fonts = true;
    
    g_im_fonts[g_im_font_count] = font;
    return static_cast<double>(g_im_font_count++);
}

/**
 * @brief Set default font.
 * @param font_index Index of the loaded font.
 * @return 1 if success, 0 if invalid index.
 */
GMREAL im_set_default_font(double font_index) {
    int idx = static_cast<int>(font_index);
    if (idx >= 0 && idx < g_im_font_count) {
        ImGui::GetIO().FontDefault = g_im_fonts[idx];
        if (imgui_initialized) g_rebuild_fonts = true;
        return 1.0;
    }
    return 0.0;
}

/**
 * @brief Push font.
 * @param font_index Index of the loaded font.
 * @return 1 if success, 0 if invalid index.
 */
GMREAL im_push_font(double font_index) {
    if (!imgui_initialized) return 0.0;
    int idx = static_cast<int>(font_index);
    if (idx >= 0 && idx < g_im_font_count) {
        ImGui::PushFont(g_im_fonts[idx]);
        return 1.0;
    }
    return 0.0;
}

/**
 * @brief Pop font.
 */
GMREAL im_pop_font() {
    if (!imgui_initialized) return 0.0;
    ImGui::PopFont();
    return 1.0;
}

/**
 * @brief Window begin.
 * @param title Window title.
 * @return 1 if success, 0 if imgui is not initialized.
 */
GMREAL im_window_begin(const char* title, double flags) {
    if (!imgui_initialized) return 0.0;
    ImGui::Begin(title, NULL, static_cast<ImGuiWindowFlags>(flags));
    return 1.0;
}

/**
 * @brief Window begin closable.
 * @param title Window title.
 * @param is_open Is the window open or closed.
 * @return 1 if success, 0 if imgui is not initialized.
 */
GMREAL im_window_begin_closable(const char* title, double is_open, double flags) {
    if (!imgui_initialized) return 0.0;
    bool open = (is_open > 0.5);
    ImGui::Begin(title, &open, static_cast<ImGuiWindowFlags>(flags));
    return static_cast<double>(open);
}

/**
 * @brief Window end.
 * @return 1 if success, 0 if imgui is not initialized.
 */
GMREAL im_window_end() {
    if (!imgui_initialized) return 0.0;
    ImGui::End();
    return 1.0;
}

/**
 * @brief Text.
 * @param text Text string.
 * @return 1 if success, 0 if imgui is not initialized.
 */
GMREAL im_text(const char* text) {
    if (!imgui_initialized) return 0.0;
    ImGui::TextUnformatted(text);
    return 1.0;
}

/**
 * @brief Show a button that execute something immediately.
 * @param label Text label of the widget.
 * @return 1 if clicked, 0 if not clicked or imgui is not initialized.
 */
GMREAL im_button(const char* label) {
    if (!imgui_initialized) return 0.0;
    return ImGui::Button(label) ? 1.0 : 0.0;
}

/**
 * @brief Show a toggle(check-box).
 * @param label Text label of the widget.
 * @param checked Checked state (1.0 or 0.0).
 * @return 1 if checked, 0 if not checked or imgui is not initialized.
 */
GMREAL im_checkbox(const char* label, double checked) {
    if (!imgui_initialized) return checked;
    bool b = (checked > 0.5);
    if (ImGui::Checkbox(label, &b)) {
        return b ? 1.0 : 0.0;
    }
    return checked;
}

/**
 * @brief Show a horizontal slider that user can drag to select a value.
 * @param label Text label of the widget.
 * @param value The numeric value.
 * @param v_min Minimum allowed value.
 * @param v_max Maximum allowed value.
 * @return Value if changed, original value if not changed or imgui is not initialized.
 */
GMREAL im_slider_real(const char* label, double value, double v_min, double v_max) {
    if (!imgui_initialized) return value;
    float f = static_cast<float>(value);
    if (ImGui::SliderFloat(label, &f, static_cast<float>(v_min), static_cast<float>(v_max))) {
        return static_cast<double>(f);
    }
    return value;
}

/**
 * @brief Combo.
 * @param label Text label of the widget.
 * @param current_index Currently selected item index.
 * @param options_csv Comma-separated options string.
 * @return Index if changed, original index if not changed or imgui is not initialized.
 */
GMREAL im_combo(const char* label, double current_index, const char* options_csv) {
    if (!imgui_initialized) return current_index;
    if (!options_csv) return current_index;

    static char s_combo_buf[16384];
    size_t len = strlen(options_csv);
    if (len >= sizeof(s_combo_buf)) len = sizeof(s_combo_buf) - 1;
    memcpy(s_combo_buf, options_csv, len);
    s_combo_buf[len] = '\0';
    
    const char* items[256];
    int count = 0;
    
    char* token = strtok(s_combo_buf, ",");
    while (token != NULL && count < 256) {
        items[count++] = token;
        token = strtok(NULL, ",");
    }
    
    if (count == 0) return current_index;

    int idx = static_cast<int>(current_index);
    if (ImGui::Combo(label, &idx, items, count)) {
        return static_cast<double>(idx);
    }
    
    return current_index;
}

/**
 * @brief Separator.
 * @return 1 if success, 0 if imgui is not initialized.
 */
GMREAL im_separator() {
    if (!imgui_initialized) return 0.0;
    ImGui::Separator();
    return 1.0;
}

/**
 * @brief Sameline.
 * @return 1 if success, 0 if imgui is not initialized.
 */
GMREAL im_sameline() {
    if (!imgui_initialized) return 0.0;
    ImGui::SameLine();
    return 1.0;
}

/**
 * @brief Draws a single-line text input field.
 * @param label The text displayed next to the input field.
 * @param text The current text string for the input box.
 * @param flags Optional ImGui flags.
 * @return Modified text string if edited, or original text if not edited or imgui is not initialized.
 */
GMSTRING im_input_text(const char* label, const char* text, double flags) {
    if (!imgui_initialized) return const_cast<char*>(text);
    
    size_t len = strlen(text);
    if (len >= sizeof(string_buffer)) len = sizeof(string_buffer) - 1;
    memcpy(string_buffer, text, len);
    string_buffer[len] = '\0';
    
    ImGui::InputText(label, string_buffer, sizeof(string_buffer), static_cast<ImGuiInputTextFlags>(flags));
    
    return const_cast<char*>(string_buffer);
}

/**
 * @brief Draws a multiline text input area.
 * 
 * @param label The text displayed next to the input area.
 * @param text The current text string.
 * @param w Width of the multiline input area.
 * @param h Height of the multiline input area.
 * @param flags Optional ImGui flags.
 * @return Modified text string if edited, or original text if not edited or imgui is not initialized.
 */
GMSTRING im_input_text_multiline(const char* label, const char* text, double w, double h, double flags) {
    if (!imgui_initialized) return const_cast<char*>(text);
    
    size_t len = strlen(text);
    if (len >= sizeof(string_buffer)) len = sizeof(string_buffer) - 1;
    memcpy(string_buffer, text, len);
    string_buffer[len] = '\0';
    
    ImGui::InputTextMultiline(label, string_buffer, sizeof(string_buffer), ImVec2((float)w, (float)h), static_cast<ImGuiInputTextFlags>(flags));
    
    return const_cast<char*>(string_buffer);
}

/**
 * @brief Draws an input field for a real number.
 * @param label The text displayed next to the input.
 * @param value The current real value.
 * @param flags Optional ImGui flags.
 * @return Newly edited real value if edited, or original value if not edited or imgui is not initialized.
 */
GMREAL im_input_real(const char* label, double value, double flags) {
    if (!imgui_initialized) return value;
    
    double v = value;
    if (ImGui::InputDouble(label, &v, 0.0, 0.0, "%.3f", static_cast<ImGuiInputTextFlags>(flags))) {
        return v;
    }
    
    return value;
}

/**
 * @brief Input color.
 * @param label Text label of the widget.
 * @param color Color.
 * @param flags ImGui flags (as real).
 * @return Color if changed, or original color if not changed or imgui is not initialized.
 */
GMREAL im_input_color(const char* label, double color, double flags) {
    if (!imgui_initialized) return color;
    
    float col[3] = { GMCOLOR_TO_RGB(color) };
    if (ImGui::ColorEdit3(label, col, static_cast<ImGuiColorEditFlags>(flags))) {
        return RGB_TO_GMCOLOR(col[0], col[1], col[2]);
    }
    
    return color;
}

/**
 * @brief Watch.
 * @param value The numeric value.
 * @param label Text label of the widget.
 * @return 1 if success, 0 if imgui is not initialized.
 */
GMREAL im_watch(const char* value, const char* label) {
    if (!imgui_initialized) return 0.0;
    ImGui::LabelText(label, "%s", value);
    return 1.0;
}

struct GMTexture {
    void* texture; // IDirect3DTexture9*
    unsigned int width;
    unsigned int height;
    unsigned int width_pow2;
    unsigned int height_pow2;
    bool exists;
};

GMTexture** gm_textures = (GMTexture**)(0x85b3c4);

inline GMTexture* get_gm_texture(int id) {
    return &(*gm_textures)[id];
}

GMREAL _im_sprite(double tex_id) {
    if (!imgui_initialized) return 0.0;
    
    GMTexture* tex = get_gm_texture(static_cast<int>(tex_id));
    if (tex && tex->exists && tex->texture) {
        float uv_u0 = 0.0f;
        float uv_v0 = 0.0f;
        float uv_u1 = (tex->width > 0 && tex->width_pow2 > 0) ? (float)tex->width / (float)tex->width_pow2 : 1.0f;
        float uv_v1 = (tex->height > 0 && tex->height_pow2 > 0) ? (float)tex->height / (float)tex->height_pow2 : 1.0f;
        
        ImGui::Image(tex->texture, ImVec2(static_cast<float>(tex->width), static_cast<float>(tex->height)), ImVec2(uv_u0, uv_v0), ImVec2(uv_u1, uv_v1));
    } else {
        return 0.0;
    }
    
    return 1.0;
}
/**
 * @func im_sprite
 * @brief Draws sprite.
 * @param sprite Sprite to draw.
 * @param image_index Image index.
 */
GMREAL im_sprite(double sprite, double image_index);

/**
 * @brief Tree begin.
 * @param label Text label of the widget.
 * @return 1 if success, 0 if imgui is not initialized.
 */
GMREAL im_tree_begin(const char* label) {
    if (!imgui_initialized) return 0.0;
    return ImGui::TreeNode(label) ? 1.0 : 0.0;
}

/**
 * @brief Tree end.
 * @return 1 if success, 0 if imgui is not initialized.
 */
GMREAL im_tree_end() {
    if (!imgui_initialized) return 0.0;
    ImGui::TreePop();
    return 1.0;
}

/**
 * @brief Main menu bar begin.
 * @return Real value
 */
GMREAL im_main_menu_bar_begin() {
    if (!imgui_initialized) return 0.0;
    return ImGui::BeginMainMenuBar() ? 1.0 : 0.0;
}

/**
 * @brief Main menu bar end.
 * @return 1 if success, 0 if imgui is not initialized.
 */
GMREAL im_main_menu_bar_end() {
    if (!imgui_initialized) return 0.0;
    ImGui::EndMainMenuBar();
    return 1.0;
}

/**
 * @brief Menu begin.
 * @description Begins a menu, only procceed to draw menu items if it returns 1.
 * @param label Text label of the widget.
 * @return 1 if success, 0 if imgui is not initialized.
 */
GMREAL im_menu_begin(const char* label) {
    if (!imgui_initialized) return 0.0;
    return ImGui::BeginMenu(label) ? 1.0 : 0.0;
}

/**
 * @brief Menu end.
 * @return 1 if success, 0 if imgui is not initialized.
 */
GMREAL im_menu_end() {
    if (!imgui_initialized) return 0.0;
    ImGui::EndMenu();
    return 1.0;
}

/**
 * @brief Menu item.
 * @param label Text label of the widget.
 * @return 1 if clicked, 0 if not clicked or imgui is not initialized.
 */
GMREAL im_menu_item(const char* label, const char* shortcut) {
    if (!imgui_initialized) return 0.0;
    return ImGui::MenuItem(label, shortcut) ? 1.0 : 0.0;
}

/**
 * @brief Radio button.
 * @param label Text label of the widget.
 * @param active Active or selected state (1.0 or 0.0).
 * @return 1 if clicked, 0 if not clicked or imgui is not initialized.
 */
GMREAL im_radio_button(const char* label, double active) {
    if (!imgui_initialized) return 0.0;
    return ImGui::RadioButton(label, active > 0.5) ? 1.0 : 0.0;
}

/**
 * @brief Progress bar.
 * @param fraction Progress fraction (0.0 to 1.0).
 * @param w Width in pixels.
 * @param h Height in pixels.
 * @param overlay Optional overlay text.
 * @return 1 if success, 0 if imgui is not initialized.
 */
GMREAL im_progress_bar(double fraction, double w, double h, const char* overlay) {
    if (!imgui_initialized) return 0.0;
    ImGui::ProgressBar(static_cast<float>(fraction), ImVec2(static_cast<float>(w), static_cast<float>(h)), (overlay && overlay[0] != '\0') ? overlay : NULL);
    return 1.0;
}

/**
 * @brief Tooltip.
 * @param text Text string.
 * @return 1 if hovered, 0 if not hovered or imgui is not initialized.
 */
GMREAL im_tooltip(const char* text) {
    if (!imgui_initialized) return 0.0;
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("%s", text);
        return 1.0;
    }
    return 0.0;
}

/**
 * @brief Begin tooltip.
 * @return 1 if hovered, 0 if not hovered or imgui is not initialized.
 */
GMREAL im_begin_tooltip() {
    if (!imgui_initialized) return 0.0;
    if (ImGui::IsItemHovered()) {
        return ImGui::BeginTooltip() ? 1.0 : 0.0;
    }
    return 0.0;
}

/**
 * @brief End tooltip.
 * @return 1 if success, 0 if imgui is not initialized.
 */
GMREAL im_end_tooltip() {
    if (!imgui_initialized) return 0.0;
    ImGui::EndTooltip();
    return 1.0;
}

/**
 * @brief Collapsing header.
 * @param label Text label of the widget.
 * @return 1 if expanded, 0 if not expanded or imgui is not initialized.
 */
GMREAL im_collapsing_header(const char* label) {
    if (!imgui_initialized) return 0.0;
    return ImGui::CollapsingHeader(label) ? 1.0 : 0.0;
}

/**
 * @brief Drag real.
 * @param label Text label of the widget.
 * @param value The numeric value.
 * @param v_speed Adjustment speed.
 * @param v_min Minimum allowed value.
 * @param v_max Maximum allowed value.
 * @return 1 if dragged, 0 if not dragged or imgui is not initialized.
 */
GMREAL im_drag_real(const char* label, double value, double v_speed, double v_min, double v_max) {
    if (!imgui_initialized) return value;
    float f = static_cast<float>(value);
    if (ImGui::DragFloat(label, &f, static_cast<float>(v_speed), static_cast<float>(v_min), static_cast<float>(v_max))) {
        return static_cast<double>(f);
    }
    return value;
}

/**
 * @brief Slider int.
 * @param label Text label of the widget.
 * @param value The numeric value.
 * @param v_min Minimum allowed value.
 * @param v_max Maximum allowed value.
 * @return 1 if dragged, 0 if not dragged or imgui is not initialized.
 */
GMREAL im_slider_int(const char* label, double value, double v_min, double v_max) {
    if (!imgui_initialized) return value;
    int i = static_cast<int>(value);
    if (ImGui::SliderInt(label, &i, static_cast<int>(v_min), static_cast<int>(v_max))) {
        return static_cast<double>(i);
    }
    return value;
}

/**
 * @brief Selectable.
 * @param label Text label of the widget.
 * @param selected Active or selected state (1.0 or 0.0).
 * @return 1 if selected, 0 if not selected or imgui is not initialized.
 */
GMREAL im_selectable(const char* label, double selected) {
    if (!imgui_initialized) return 0.0;
    bool s = (selected > 0.5);
    return ImGui::Selectable(label, &s) ? 1.0 : 0.0;
}

/**
 * @brief Bullet text.
 * @param text Text string.
 * @return 1 if success, 0 if imgui is not initialized.
 */
GMREAL im_bullet_text(const char* text) {
    if (!imgui_initialized) return 0.0;
    ImGui::BulletText("%s", text);
    return 1.0;
}

/**
 * @brief Dummy.
 * @param w Width in pixels.
 * @param h Height in pixels.
 * @return 1 if success, 0 if imgui is not initialized.
 */
GMREAL im_dummy(double w, double h) {
    if (!imgui_initialized) return 0.0;
    ImGui::Dummy(ImVec2(static_cast<float>(w), static_cast<float>(h)));
    return 1.0;
}

/**
 * @brief Spacing.
 * @return 1 if success, 0 if imgui is not initialized.
 */
GMREAL im_spacing() {
    if (!imgui_initialized) return 0.0;
    ImGui::Spacing();
    return 1.0;
}

/**
 * @brief Indent.
 * @param indent_w Width in pixels.
 * @return 1 if success, 0 if imgui is not initialized.
 */
GMREAL im_indent(double indent_w) {
    if (!imgui_initialized) return 0.0;
    ImGui::Indent(static_cast<float>(indent_w));
    return 1.0;
}

/**
 * @brief Unindent.
 * @param indent_w Width in pixels.
 * @return 1 if success, 0 if imgui is not initialized.
 */
GMREAL im_unindent(double indent_w) {
    if (!imgui_initialized) return 0.0;
    ImGui::Unindent(static_cast<float>(indent_w));
    return 1.0;
}

/**
 * @brief Child begin.
 * @param str_id String ID for ImGui.
 * @param w Width in pixels.
 * @param h Height in pixels.
 * @param border Show border or not.
 * @return 1 if success, 0 if imgui is not initialized.
 */
GMREAL im_child_begin(const char* str_id, double w, double h, double border) {
    if (!imgui_initialized) return 0.0;
    return ImGui::BeginChild(str_id, ImVec2(static_cast<float>(w), static_cast<float>(h)), border > 0.5) ? 1.0 : 0.0;
}

/**
 * @brief Child end.
 * @return 1 if success, 0 if imgui is not initialized.
 */
GMREAL im_child_end() {
    if (!imgui_initialized) return 0.0;
    ImGui::EndChild();
    return 1.0;
}

/**
 * @brief Color button.
 * @param desc_id String ID for ImGui.
 * @param color Color.
 * @return 1 if clicked, 0 if not clicked or imgui is not initialized.
 */
GMREAL im_color_button(const char* desc_id, double color) {
    if (!imgui_initialized) return 0.0;
    int c = static_cast<int>(color);
    float r = (c & 0xFF) / 255.0f;
    float g = ((c >> 8) & 0xFF) / 255.0f;
    float b = ((c >> 16) & 0xFF) / 255.0f;
    return ImGui::ColorButton(desc_id, ImVec4(r, g, b, 1.0f)) ? 1.0 : 0.0;
}

/**
 * @brief Set next window size.
 * @param w Width in pixels.
 * @param h Height in pixels.
 * @return 1 if success, 0 if imgui is not initialized.
 */
GMREAL im_set_next_window_size(double w, double h) {
    if (!imgui_initialized) return 0.0;
    ImGui::SetNextWindowSize(ImVec2(static_cast<float>(w), static_cast<float>(h)));
    return 1.0;
}

/**
 * @brief Set next window pos.
 * @param x X coordinate or value.
 * @param y Y coordinate or value.
 * @return 1 if success, 0 if imgui is not initialized.
 */
GMREAL im_set_next_window_pos(double x, double y) {
    if (!imgui_initialized) return 0.0;
    ImGui::SetNextWindowPos(ImVec2(static_cast<float>(x), static_cast<float>(y)));
    return 1.0;
}

/**
 * @brief Set next window alpha.
 * @param alpha Alpha value.
 * @return 1 if success, 0 if imgui is not initialized.
 */
GMREAL im_set_next_window_bg_alpha(double alpha) {
    if (!imgui_initialized) return 0.0;
    ImGui::SetNextWindowBgAlpha(alpha);
    return 1.0;
}

/**
 * @brief Align text to frame padding.
 * @return 1 if success, 0 if imgui is not initialized.
 */
GMREAL im_align_text_to_frame_padding() {
    if (!imgui_initialized) return 0.0;
    ImGui::AlignTextToFramePadding();
    return 1.0;
}

/**
 * @brief Is window focused.
 * @return 1 if focused, 0 if not focused or imgui is not initialized.
 */
GMREAL im_is_window_focused() {
    if (!imgui_initialized) return 0.0;
    return ImGui::IsWindowFocused() ? 1.0 : 0.0;
}

/**
 * @brief Any window focused.
 * @return 1 if focused, 0 if not focused or imgui is not initialized.
 */
GMREAL im_any_window_focused() {
    if (!imgui_initialized) return 0.0;
    return ImGui::IsWindowFocused(ImGuiFocusedFlags_AnyWindow) ? 1.0 : 0.0;
}

/**
 * @brief Begin popup context item.
 * @param str_id String ID for ImGui.
 * @return 1 if success, 0 if imgui is not initialized.
 */
GMREAL im_begin_popup_context_item(const char* str_id) {
    if (!imgui_initialized) return 0.0;
    return ImGui::BeginPopupContextItem(str_id && str_id[0] ? str_id : NULL) ? 1.0 : 0.0;
}

/**
 * @brief Begin popup context void.
 * @param str_id String ID for ImGui.
 * @return 1 if success, 0 if imgui is not initialized.
 */
GMREAL im_begin_popup_context_void(const char* str_id) {
    if (!imgui_initialized) return 0.0;
    return ImGui::BeginPopupContextVoid(str_id && str_id[0] ? str_id : NULL) ? 1.0 : 0.0;
}

/**
 * @brief Open popup.
 * @param str_id String ID for ImGui.
 * @return 1 if success, 0 if imgui is not initialized.
 */
GMREAL im_open_popup(const char* str_id) {
    if (!imgui_initialized) return 0.0;
    if (str_id && str_id[0]) ImGui::OpenPopup(str_id);
    return 1.0;
}

/**
 * @brief Begin popup.
 * @param str_id String ID for ImGui.
 * @return 1 if success, 0 if imgui is not initialized.
 */
GMREAL im_begin_popup(const char* str_id) {
    if (!imgui_initialized) return 0.0;
    return ImGui::BeginPopup(str_id && str_id[0] ? str_id : NULL) ? 1.0 : 0.0;
}

/**
 * @brief End popup.
 * @return 1 if success, 0 if imgui is not initialized.
 */
GMREAL im_end_popup() {
    if (!imgui_initialized) return 0.0;
    ImGui::EndPopup();
    return 1.0;
}

/**
 * @brief Begin table.
 * @param str_id String ID for ImGui.
 * @param columns_count Number of columns.
 * @param flags ImGui flags (as real).
 * @return 1 if success, 0 if imgui is not initialized.
 */
GMREAL im_begin_table(const char* str_id, double columns_count, double flags) {
    if (!imgui_initialized) return 0.0;
    return ImGui::BeginTable(str_id, static_cast<int>(columns_count), static_cast<int>(flags)) ? 1.0 : 0.0;
}

/**
 * @brief End table.
 * @return 1 if success, 0 if imgui is not initialized.
 */
GMREAL im_end_table() {
    if (!imgui_initialized) return 0.0;
    ImGui::EndTable();
    return 1.0;
}

/**
 * @brief Table next row.
 * @return 1 if success, 0 if imgui is not initialized.
 */
GMREAL im_table_next_row() {
    if (!imgui_initialized) return 0.0;
    ImGui::TableNextRow();
    return 1.0;
}

/**
 * @brief Table next column.
 * @return 1 if success, 0 if imgui is not initialized.
 */
GMREAL im_table_next_column() {
    if (!imgui_initialized) return 0.0;
    return ImGui::TableNextColumn() ? 1.0 : 0.0;
}

/**
 * @brief Table next row and column.
 * @return 1 if success, 0 if imgui is not initialized.
 */
GMREAL im_table_next_row_and_column() {
    if (!imgui_initialized) return 0.0;
    ImGui::TableNextRow();
    return ImGui::TableNextColumn() ? 1.0 : 0.0;
}

/**
 * @brief Table setup column.
 * @param label Text label of the widget.
 * @return 1 if success, 0 if imgui is not initialized.
 */
GMREAL im_table_setup_column(const char* label) {
    if (!imgui_initialized) return 0.0;
    ImGui::TableSetupColumn(label);
    return 1.0;
}

/**
 * @brief Table headers row.
 * @return 1 if success, 0 if imgui is not initialized.
 */
GMREAL im_table_headers_row() {
    if (!imgui_initialized) return 0.0;
    ImGui::TableHeadersRow();
    return 1.0;
}

/**
 * @brief Push item width.
 * @param item_width Item width in pixels.
 * @return 1 if success, 0 if imgui is not initialized.
 */
GMREAL im_push_item_width(double item_width) {
    if (!imgui_initialized) return 0.0;
    ImGui::PushItemWidth(static_cast<float>(item_width));
    return 1.0;
}

/**
 * @brief Pop item width.
 * @return 1 if success, 0 if imgui is not initialized.
 */
GMREAL im_pop_item_width() {
    if (!imgui_initialized) return 0.0;
    ImGui::PopItemWidth();
    return 1.0;
}

/**
 * @brief Draw property real.
 * @param label Text label of the widget.
 * @param value The numeric value.
 * @param flags ImGui flags.
 * @return 1 if success, 0 if imgui is not initialized.
 */
GMREAL im_draw_property_real(const char* label, double value, double flags) {
    if (!imgui_initialized || !label) return value;
    ImGui::TableNextRow();
    ImGui::TableNextColumn();
    ImGui::TextUnformatted(label);
    ImGui::TableNextColumn();
    ImGui::PushItemWidth(-1);
    
    char buf[128];
    ImFormatString(buf, sizeof(buf), "##%s", label);
    float val = (float)value;
    if (ImGui::InputFloat(buf, &val, 0.0f, 0.0f, "%.3f", static_cast<ImGuiInputTextFlags>(flags))) {
        value = (double)val;
    }
    ImGui::PopItemWidth();
    return value;
}

/**
 * @brief Draw property color.
 * @param label Text label of the widget.
 * @param value The numeric value.
 * @param flags ImGui flags.
 * @return 1 if success, 0 if imgui is not initialized.
 */
GMREAL im_draw_property_color(const char* label, double value, double flags) {
    if (!imgui_initialized || !label) return value;
    ImGui::TableNextRow();
    ImGui::TableNextColumn();
    ImGui::TextUnformatted(label);
    ImGui::TableNextColumn();
    ImGui::PushItemWidth(-1);
    
    char buf[128];
    ImFormatString(buf, sizeof(buf), "##%s", label);

    float col[3] = { GMCOLOR_TO_RGB(value) };
    if (ImGui::ColorEdit3(buf, col, static_cast<ImGuiColorEditFlags>(flags))) {
        value = RGB_TO_GMCOLOR(col[0], col[1], col[2]);
    }
    
    ImGui::PopItemWidth();
    return value;
}

/**
 * @brief Draw property text.
 * @param label Text label of the widget.
 * @param text Text string.
 * @param flags ImGui flags.
 * @return Returns a string.
 */
GMSTRING im_draw_property_text(const char* label, const char* text, double flags) {
    if (!imgui_initialized || !label || !text) return const_cast<char*>(text);
    ImGui::TableNextRow();
    ImGui::TableNextColumn();
    ImGui::TextUnformatted(label);
    ImGui::TableNextColumn();
    ImGui::PushItemWidth(-1);
    
    char buf[128];
    ImFormatString(buf, sizeof(buf), "##%s", label);
    
    size_t len = strlen(text);
    if (len >= sizeof(string_buffer)) len = sizeof(string_buffer) - 1;
    memcpy(string_buffer, text, len);
    string_buffer[len] = '\0';
    
    ImGui::InputText(buf, string_buffer, sizeof(string_buffer), static_cast<ImGuiInputTextFlags>(flags));
    
    ImGui::PopItemWidth();
    return const_cast<char*>(string_buffer);
}


/**
 * @brief Begin main menu bar.
 * @return 1 if success, 0 if imgui is not initialized.
 */
GMREAL im_begin_main_menu_bar() {
    if (!imgui_initialized) return 0.0;
    return ImGui::BeginMainMenuBar() ? 1.0 : 0.0;
}

/**
 * @brief End main menu bar.
 * @return 1 if success, 0 if imgui is not initialized.
 */
GMREAL im_end_main_menu_bar() {
    if (!imgui_initialized) return 0.0;
    ImGui::EndMainMenuBar();
    return 1.0;
}

/**
 * @brief Begin menu.
 * @param label Text label of the widget.
 * @return 1 if success, 0 if imgui is not initialized.
 */
GMREAL im_begin_menu(const char* label) {
    if (!imgui_initialized) return 0.0;
    return ImGui::BeginMenu(label) ? 1.0 : 0.0;
}

/**
 * @brief End menu.
 * @return 1 if success, 0 if imgui is not initialized.
 */
GMREAL im_end_menu() {
    if (!imgui_initialized) return 0.0;
    ImGui::EndMenu();
    return 1.0;
}

/**
 * @brief Style light.
 * @return 1 if success, 0 if imgui is not initialized.
 */
GMREAL im_style_light() {
    if (!imgui_initialized) return 0.0;
    ImGui::StyleColorsLight();
    return 1.0;
}

/**
 * @brief Style dark.
 * @return 1 if success, 0 if imgui is not initialized.
 */
GMREAL im_style_dark() {
    if (!imgui_initialized) return 0.0;
    ImGui::StyleColorsDark();
    return 1.0;
}

/**
 * @brief Style set color.
 * @param col_idx Color style index.
 * @param color Color.
 * @return 1 if success, 0 if imgui is not initialized.
 */
GMREAL im_style_set_color(double col_idx, double color) {
    if (!imgui_initialized) return 0.0;
    ImGui::GetStyle().Colors[(int)col_idx] = ImVec4(GMCOLOR_TO_RGB(color), 1.0f);
    return 1.0;
}

/**
 * @brief Style set var real.
 * @param var_idx Variable style index.
 * @param val The numeric value.
 * @return 1 if success, 0 if imgui is not initialized.
 */
GMREAL im_style_set_var_real(double var_idx, double val) {
    if (!imgui_initialized) return 0.0;
    ImGuiStyle& style = ImGui::GetStyle();
    int idx = (int)var_idx;
    switch(idx) {
        case ImGuiStyleVar_Alpha: style.Alpha = (float)val; break;
        case ImGuiStyleVar_DisabledAlpha: style.DisabledAlpha = (float)val; break;
        case ImGuiStyleVar_WindowRounding: style.WindowRounding = (float)val; break;
        case ImGuiStyleVar_WindowBorderSize: style.WindowBorderSize = (float)val; break;
        case ImGuiStyleVar_ChildRounding: style.ChildRounding = (float)val; break;
        case ImGuiStyleVar_ChildBorderSize: style.ChildBorderSize = (float)val; break;
        case ImGuiStyleVar_PopupRounding: style.PopupRounding = (float)val; break;
        case ImGuiStyleVar_PopupBorderSize: style.PopupBorderSize = (float)val; break;
        case ImGuiStyleVar_FrameRounding: style.FrameRounding = (float)val; break;
        case ImGuiStyleVar_FrameBorderSize: style.FrameBorderSize = (float)val; break;
        case ImGuiStyleVar_IndentSpacing: style.IndentSpacing = (float)val; break;
        case ImGuiStyleVar_ScrollbarSize: style.ScrollbarSize = (float)val; break;
        case ImGuiStyleVar_ScrollbarRounding: style.ScrollbarRounding = (float)val; break;
        case ImGuiStyleVar_GrabMinSize: style.GrabMinSize = (float)val; break;
        case ImGuiStyleVar_GrabRounding: style.GrabRounding = (float)val; break;
        case ImGuiStyleVar_TabRounding: style.TabRounding = (float)val; break;
    }
    return 1.0;
}

/**
 * @brief Style set var vec2.
 * @param var_idx Variable style index.
 * @param val_x X coordinate or value.
 * @param val_y Y coordinate or value.
 * @return 1 if success, 0 if imgui is not initialized.
 */
GMREAL im_style_set_var_vec2(double var_idx, double val_x, double val_y) {
    if (!imgui_initialized) return 0.0;
    ImGuiStyle& style = ImGui::GetStyle();
    int idx = (int)var_idx;
    switch(idx) {
        case ImGuiStyleVar_WindowPadding: style.WindowPadding = ImVec2((float)val_x, (float)val_y); break;
        case ImGuiStyleVar_WindowMinSize: style.WindowMinSize = ImVec2((float)val_x, (float)val_y); break;
        case ImGuiStyleVar_WindowTitleAlign: style.WindowTitleAlign = ImVec2((float)val_x, (float)val_y); break;
        case ImGuiStyleVar_FramePadding: style.FramePadding = ImVec2((float)val_x, (float)val_y); break;
        case ImGuiStyleVar_ItemSpacing: style.ItemSpacing = ImVec2((float)val_x, (float)val_y); break;
        case ImGuiStyleVar_ItemInnerSpacing: style.ItemInnerSpacing = ImVec2((float)val_x, (float)val_y); break;
        case ImGuiStyleVar_CellPadding: style.CellPadding = ImVec2((float)val_x, (float)val_y); break;
        case ImGuiStyleVar_ButtonTextAlign: style.ButtonTextAlign = ImVec2((float)val_x, (float)val_y); break;
        case ImGuiStyleVar_SelectableTextAlign: style.SelectableTextAlign = ImVec2((float)val_x, (float)val_y); break;
    }
    return 1.0;
}

/**
 * @brief Push style color.
 * @param col_idx Color style index.
 * @param color Color.
 * @return 1 if success, 0 if imgui is not initialized.
 */
GMREAL im_push_style_color(double col_idx, double color) {
    if (!imgui_initialized) return 0.0;
    ImGui::PushStyleColor((int)col_idx, ImVec4(GMCOLOR_TO_RGB(color), 1.0f));
    return 1.0;
}

/**
 * @brief Pop style color.
 * @param count Number of items.
 * @return 1 if success, 0 if imgui is not initialized.
 */
GMREAL im_pop_style_color(double count) {
    if (!imgui_initialized) return 0.0;
    ImGui::PopStyleColor((int)count);
    return 1.0;
}

/**
 * @brief Push style var real.
 * @param var_idx Variable style index.
 * @param val The numeric value.
 * @return 1 if success, 0 if imgui is not initialized.
 */
GMREAL im_push_style_var_real(double var_idx, double val) {
    if (!imgui_initialized) return 0.0;
    ImGui::PushStyleVar((int)var_idx, (float)val);
    return 1.0;
}

/**
 * @brief Push style var vec2.
 * @param var_idx Variable style index.
 * @param val_x X coordinate or value.
 * @param val_y Y coordinate or value.
 * @return 1 if success, 0 if imgui is not initialized.
 */
GMREAL im_push_style_var_vec2(double var_idx, double val_x, double val_y) {
    if (!imgui_initialized) return 0.0;
    ImGui::PushStyleVar((int)var_idx, ImVec2((float)val_x, (float)val_y));
    return 1.0;
}

/**
 * @brief Pop style var.
 * @param count Number of items.
 * @return 1 if success, 0 if imgui is not initialized.
 */
GMREAL im_pop_style_var(double count) {
    if (!imgui_initialized) return 0.0;
    ImGui::PopStyleVar((int)count);
    return 1.0;
}

/**
 * @brief Dockspace.
 * @param dockspace_id Dockspace ID.
 * @param w Width in pixels.
 * @param h Height in pixels.
 * @return 1 if success, 0 if imgui is not initialized.
 */
GMREAL im_dockspace(double dockspace_id, double w, double h) {
    if (!imgui_initialized) return 0.0;
    ImGuiID id = static_cast<ImGuiID>(dockspace_id);
    if (id == 0) id = ImGui::GetID("MainDockSpace");
    ImGui::DockSpace(id, ImVec2(static_cast<float>(w), static_cast<float>(h)));
    return 1.0;
}
