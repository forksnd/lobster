// Constants for use with ImGui (https://github.com/ocornut/imgui) im functions.

// From: https://github.com/ocornut/imgui/blob/master/imgui.h

namespace im

// To be passed to im.init()
enum_flags config:
    config_none = 0
    // Warning: turing this on stops the game from receiving keys.
    config_nav_enable_keyboard = 1
    config_nav_enable_gamepad
    config_nav_enable_set_mouse_pos
    config_nav_no_capture_keyboard
    config_no_mouse
    config_no_mouse_cursor_change
    config_no_keyboard
    config_docking_enable
    config_viewport_enable = 1 << 10
    config_dpi_enable_scale_viewports = 1 << 14
    config_dpi_enable_scale_fonts
    config_is_srgb = 1 << 20
    config_is_touch_screen

// To be passed to im.window()
enum_flags window:
    window_none = 0
    window_no_titlebar = 1             // Disable title-bar
    window_no_resize                   // Disable user resizing with the lower-right grip
    window_no_move                     // Disable user moving the window
    window_no_scrollbar                // Disable scrollbars (window can still scroll with mouse or programatically)
    window_no_scroll_with_mouse        // Disable user vertically scrolling with mouse wheel. On child window, mouse wheel will be forwarded to the parent unless NoScrollbar is also set.
    window_no_collapse                 // Disable user collapsing window by double-clicking on it
    window_always_autoresize           // Resize every window to its content every frame
    window_no_background               // Disable drawing background color (WindowBg, etc.) and outside border. Similar as using SetNextWindowBgAlpha(0.0f).
    window_no_saved_settings           // Never load/save settings in .ini file
    window_no_mouse_inputs             // Disable catching mouse, hovering test with pass through.
    window_menubar                     // Has a menu-bar
    window_horizontal_scrollbar        // Allow horizontal scrollbar to appear (off by default). You may use SetNextWindowContentSize(ImVec2(width,0.0f)); prior to calling Begin() to specify width. Read code in imgui_demo in the "Horizontal Scrolling" section.
    window_no_focus_on_appearing       // Disable taking focus when transitioning from hidden to visible state
    window_no_bringtofront_on_focus    // Disable bringing window to front when taking focus (e.g. clicking on it or programatically giving it focus)
    window_always_vertical_scrollbar   // Always show vertical scrollbar (even if ContentSize.y < Size.y)
    window_always_horizontal_scrollbar // Always show horizontal scrollbar (even if ContentSize.x < Size.x)
    window_always_use_window_padding   // Ensure child windows without border uses style.WindowPadding (ignored by default for non-bordered child windows, because more convenient)
    window_UNUSED1
    window_no_nav_inputs               // No gamepad/keyboard navigation within the window
    window_no_nav_focus                // No focusing toward this window with gamepad/keyboard navigation (e.g. skipped by CTRL+TAB)
    window_unsaved_document            // Append '*' to title without affecting the ID, as a convenience to avoid using the ### operator. When used in a tab/docking context, tab is selected on closure and closure is deferred by one frame to allow code to cancel the closure (with a confirmation popup, etc.) without flicker.
    window_no_docking                  // Disable docking of this window

enum_flags treenode:
    treenode_none = 0
    treenode_selected = 1         // Draw as selected
    treenode_framed               // Draw frame with background (e.g. for CollapsingHeader)
    treenode_allow_item_overlap   // Hit testing to allow subsequent widgets to overlap this one
    treenode_no_tree_push_on_open // Don't do a TreePush() when open (e.g. for CollapsingHeader) = no extra indent nor pushing on ID stack
    treenode_no_auto_open_on_log  // Don't automatically and temporarily open node when Logging is active (by default logging will automatically open tree nodes)
    treenode_default_open         // Default node to be open
    treenode_open_on_double_click // Need double-click to open node
    treenode_open_on_arrow        // Only open when clicking on the arrow part. If ImGuiTreeNodeFlags_OpenOnDoubleClick is also set, single-click arrow or double-click all box to open.
    treenode_leaf                 // No collapsing, no arrow (use as a convenience for leaf nodes).
    treenode_bullet               // Display a bullet instead of arrow
    treenode_frame_padding        // Use FramePadding (even for an unframed text node) to vertically align text baseline to regular widget height. Equivalent to calling AlignTextToFramePadding().
    treenode_span_avail_width     // Extend hit box to the right-most edge, even if not framed. This is not the default in order to allow adding other items on the same line. In the future we may refactor the hit system to be front-to-back, allowing natural overlaps and then this can become the default.
    treenode_span_full_width      // Extend hit box to the left-most and right-most edges (bypass the indented area).

    treenode_half_indent = 1 << 30  // NOTE: this is NOT a Dear ImGUI flag, but a Lobster specific one!

enum_flags tabitem:
    tabitem_none = 0
    tabitem_unsaved_document = 1               // Display a dot next to the title + tab is selected when clicking the X + closure is not assumed (will wait for user to stop submitting the tab). Otherwise closure is assumed when pressing the X, so if you keep submitting the tab may reappear at end of tab bar.
    tabitem_set_selected                       // Trigger flag to programmatically make the tab selected when calling BeginTabItem()
    tabitem_no_close_with_middle_mouse_button  // Disable behavior of closing tabs (that are submitted with p_open != NULL) with middle mouse button. You can still repro this behavior on user's side with if (IsItemHovered() && IsMouseClicked(2)) *p_open = false.
    tabitem_no_push_id                         // Don't call PushID(tab->ID)/PopID() on BeginTabItem()/EndTabItem()
    tabitem_no_tooltip                         // Disable tooltip for the given tab
    tabitem_no_reorder                         // Disable reordering this tab or having another tab cross over this tab
    tabitem_leading                            // Enforce the tab position to the left of the tab bar (after the tab list popup button)
    tabitem_trailing                           // Enforce the tab position to the right of the tab bar (before the scrolling buttons)

enum_flags table:
    table_none = 0
    table_resizable = 1                   // Enable resizing columns.
    table_reorderable                     // Enable reordering columns in header row (need calling TableSetupColumn() + TableHeadersRow() to display headers)
    table_hideable                        // Enable hiding/disabling columns in context menu.
    table_sortable                        // Enable sorting. Call TableGetSortSpecs() to obtain sort specs. Also see ImGuiTableFlags_SortMulti and ImGuiTableFlags_SortTristate.
    table_no_saved_settings               // Disable persisting columns order, width and sort settings in the .ini file.
    table_context_menu_in_body            // Right-click on columns body/contents will display table context menu. By default it is available in TableHeadersRow().
    table_row_bg                          // Set each RowBg color with ImGuiCol_TableRowBg or ImGuiCol_TableRowBgAlt (equivalent of calling TableSetBgColor with ImGuiTableBgFlags_RowBg0 on each row manually)
    table_borders_inner_h                 // Draw horizontal borders between rows.
    table_borders_outer_h                 // Draw horizontal borders at the top and bottom.
    table_borders_inner_v                 // Draw vertical borders between columns.
    table_borders_outer_v                 // Draw vertical borders on the left and right sides.
    table_no_borders_in_body              // [ALPHA] Disable vertical borders in columns Body (borders will always appear in Headers). -> May move to style
    table_no_borders_in_body_until_resize // [ALPHA] Disable vertical borders in columns Body until hovered for resize (borders will always appear in Headers). -> May move to style
    table_sizing_fixed_fit    = 1 << 13   // Columns default to _WidthFixed or _WidthAuto (if resizable or not resizable), matching contents width.
    table_sizing_fixed_same   = 2 << 13   // Columns default to _WidthFixed or _WidthAuto (if resizable or not resizable), matching the maximum contents width of all columns. Implicitly enable ImGuiTableFlags_NoKeepColumnsVisible.
    table_sizing_stretch_prop = 3 << 13   // Columns default to _WidthStretch with default weights all equal, unless overridden by TableSetupColumn().
    table_sizing_stretch_same = 4 << 13   // Columns default to _WidthStretch with default weights all equal, unless overridden by TableSetupColumn().
    table_no_host_extend_x    = 1 << 16   // Make outer width auto-fit to columns, overriding outer_size.x value. Only available when ScrollX/ScrollY are disabled and Stretch columns are not used.
    table_no_host_extend_y                // Make outer height stop exactly at outer_size.y (prevent auto-extending table past the limit). Only available when ScrollX/ScrollY are disabled. Data below the limit will be clipped and not visible.
    table_no_keep_columns_visible         // Disable keeping column always minimally visible when ScrollX is off and table gets too small. Not recommended if columns are resizable.
    table_precise_widths                  // Disable distributing remainder width to stretched columns (width allocation on a 100-wide table with 3 columns: Without this flag: 33,33,34. With this flag: 33,33,33). With larger number of columns, resizing will appear to be less smooth.
    table_no_clip                         // Disable clipping rectangle for every individual columns (reduce draw command count, items will be able to overflow into other columns). Generally incompatible with TableSetupScrollFreeze().
    table_pad_outer_x                     // Default if BordersOuterV is on. Enable outermost padding. Generally desirable if you have headers.
    table_no_pad_outer_x                  // Default if BordersOuterV is off. Disable outermost padding.
    table_no_pad_inner_x                  // Disable inner padding between columns (double inner padding if BordersOuterV is on, single inner padding if BordersOuterV is off).
    table_scroll_x                        // Enable horizontal scrolling. Require 'outer_size' parameter of BeginTable() to specify the container size. Changes default sizing policy. Because this creates a child window, ScrollY is currently generally recommended when using ScrollX.
    table_scroll_y                        // Enable vertical scrolling. Require 'outer_size' parameter of BeginTable() to specify the container size.
    table_sort_multi                      // Hold shift when clicking headers to sort on multiple column. TableGetSortSpecs() may return specs where (SpecsCount > 1).
    table_sort_tristate                   // Allow no sorting, disable default sorting. TableGetSortSpecs() may return specs where (SpecsCount == 0).

let table_sizing_stretch_prop = table_sizing_fixed_fit | table_sizing_fixed_same // Columns default to _WidthStretch with default weights proportional to each columns contents widths.
let table_borders_h           = table_borders_inner_h  | table_borders_outer_h   // Draw horizontal borders.
let table_borders_v           = table_borders_inner_v  | table_borders_outer_v   // Draw vertical borders.
let table_borders_inner       = table_borders_inner_v  | table_borders_inner_h   // Draw inner borders.
let table_borders_outer       = table_borders_outer_v  | table_borders_outer_h   // Draw outer borders.
let table_borders             = table_borders_inner    | table_borders_outer     // Draw all borders.

enum_flags table_column:
    table_column_none = 0,
    table_column_disabled = 1           // Overriding/master disable flag: hide column, won't show in context menu (unlike calling TableSetColumnEnabled() which manipulates the user accessible state)
    table_column_default_hide           // Default as a hidden/disabled column.
    table_column_default_sort           // Default as a sorting column.
    table_column_width_stretch          // Column will stretch. Preferable with horizontal scrolling disabled (default if table sizing policy is _SizingStretchSame or _SizingStretchProp).
    table_column_width_fixed            // Column will not stretch. Preferable with horizontal scrolling enabled (default if table sizing policy is _SizingFixedFit and table is resizable).
    table_column_no_resize              // Disable manual resizing.
    table_column_no_reorder             // Disable manual reordering this column, this will also prevent other columns from crossing over this column.
    table_column_no_hide                // Disable ability to hide/disable this column.
    table_column_no_clip                // Disable clipping for this column (all NoClip columns will render in a same draw command).
    table_column_no_sort                // Disable ability to sort on this field (even if ImGuiTableFlags_Sortable is set on the table).
    table_column_no_sort_ascending      // Disable ability to sort in the ascending direction.
    table_column_no_sort_descending     // Disable ability to sort in the descending direction.
    table_column_no_header_label        // TableHeadersRow() will not submit label for this column. Convenient for some small columns. Name will still appear in context menu.
    table_column_no_header_width        // Disable header text width contribution to automatic column width.
    table_column_prefer_sort_ascending  // Make the initial sort direction Ascending when first sorting on this column (default).
    table_column_prefer_sort_descending // Make the initial sort direction Descending when first sorting on this column.
    table_column_indent_enable          // Use current Indent value when entering cell (default for column 0).
    table_column_indent_disable         // Ignore current Indent value when entering cell (default for columns > 0). Indentation changes _within_ the cell will still be honored.

enum_flags drag_drop:
    drag_drop_none = 0,
    drag_drop_source_no_preview_tooltip = 1   // Disable preview tooltip. By default, a successful call to BeginDragDropSource opens a tooltip so you can display a preview or description of the source contents. This flag disables this behavior.
    drag_drop_source_no_disable_hover         // By default, when dragging we clear data so that IsItemHovered() will return false, to avoid subsequent user code submitting tooltips. This flag disables this behavior so you can still call IsItemHovered() on the source item.
    drag_drop_source_no_hold_to_open_others   // Disable the behavior that allows to open tree nodes and collapsing header by holding over them while dragging a source item.
    drag_drop_source_allow_null_id            // Allow items such as Text(), Image() that have no unique identifier to be used as drag source, by manufacturing a temporary identifier based on their window-relative position. This is extremely unusual within the dear imgui ecosystem and so we made it explicit.
    drag_drop_source_extern                   // External source (from outside of dear imgui), won't attempt to read current item/window info. Will always return true. Only one Extern source can be active simultaneously.
    drag_drop_source_auto_expire_payload      // Automatically expire the payload if the source cease to be submitted (otherwise payloads are persisting while being dragged)
    drag_drop_accept_before_delivery          // AcceptDragDropPayload() will returns true even before the mouse button is released. You can then call IsDelivery() to test if the payload needs to be delivered.
    drag_drop_accept_no_draw_default_rect     // Do not draw the default highlight rectangle when hovering over target.
    drag_drop_accept_no_preview_tooltip       // Request hiding the BeginDragDropSource tooltip from the BeginDragDropTarget site.

enum_flags input_text:
    input_text_none = 0
    input_text_chars_decimal = 1               // Allow 0123456789.+-*/
    input_text_chars_hexadecimal               // Allow 0123456789ABCDEFabcdef
    input_text_chars_scientific                // Allow 0123456789.+-*/eE (Scientific notation input)
    input_text_chars_uppercase                 // Turn a..z into A..Z
    input_text_chars_no_blank                  // Filter out spaces, tabs
    input_text_allow_tab_input                 // Pressing TAB input a '\t' character into the text field
    input_text_enter_returns_true              // Return 'true' when Enter is pressed (as opposed to every time the value was modified). Consider looking at the IsItemDeactivatedAfterEdit() function.
    input_text_escape_clears_all               // Escape key clears content if not empty, and deactivate otherwise (contrast to default behavior of Escape to revert)
    input_text_ctrl_enter_for_new_line         // In multi-line mode, validate with Enter, add new line with Ctrl+Enter (default is opposite: validate with Ctrl+Enter, add line with Enter).
    input_text_read_only                       // Read-only mode
    input_text_password                        // Password mode, display all characters as '*', disable copy
    input_text_always_overwrite                // Overwrite mode
    input_text_auto_select_all                 // Select entire text when first taking mouse focus
    input_text_no_horizontal_scroll = 1 << 15  // Disable following the cursor horizontally
    input_text_no_undo_redo                    // Disable undo/redo. Note that input text owns the text data while active, if you want to provide your own undo/redo stack you need e.g. to call ClearActiveID().

enum cond:
    cond_always          = 1   // No condition (always set the variable), same as _None
    cond_once            = 2   // Set the variable once per runtime session (only the first call will succeed)
    cond_first_use_ever  = 4   // Set the variable if the object/window has no persistently saved data (no entry in .ini file)
    cond_appearing       = 8   // Set the variable if the object/window is appearing after being hidden/inactive (or the first time)

enum color_idx:
    col_text
    col_text_disabled
    col_window_bg              // Background of normal windows
    col_child_bg               // Background of child windows
    col_popup_bg               // Background of popups, menus, tooltips windows
    col_border
    col_border_shadow
    col_frame_bg               // Background of checkbox, radio button, plot, slider, text input
    col_frame_bg_hovered
    col_frame_bg_active
    col_title_bg
    col_title_bg_active
    col_title_bg_collapsed
    col_menubar_bg
    col_scrollbar_bg
    col_scrollbar_grab
    col_scrollbar_grab_hovered
    col_scrollbar_grab_active
    col_check_mark
    col_slider_grab
    col_slider_grab_active
    col_button
    col_button_hovered
    col_button_active
    col_header                 // Header* colors are used for CollapsingHeader, TreeNode, Selectable, MenuItem
    col_header_hovered
    col_header_active
    col_separator
    col_separator_hovered
    col_separator_active
    col_resize_grip            // Resize grip in lower-right and lower-left corners of windows.
    col_resize_grip_hovered
    col_resize_grip_active
    col_input_text_cursor
    col_tab_hovered            // Tab background, when hovered
    col_tab                    // Tab background, when tab-bar is focused & tab is unselected
    col_tab_selected           // Tab background, when tab-bar is focused & tab is selected
    col_tab_selected_overline  // Tab horizontal overline, when tab-bar is focused & tab is selected
    col_tab_dimmed             // Tab background, when tab-bar is unfocused & tab is unselected
    col_tab_dimmed_selected    // Tab background, when tab-bar is unfocused & tab is selected
    col_tab_dimmed_selected_overline//..horizontal overline, when tab-bar is unfocused & tab is selected
    col_docking_preview        // Preview overlay color when about to docking something
    col_docking_empty_bg       // Background color for empty node (e.g. CentralNode with no window docked into it)
    col_plot_lines
    col_plot_lines_hovered
    col_plot_histogram
    col_plot_histogram_hovered
    col_table_header_bg        // Table header background
    col_table_border_strong    // Table outer and header borders (prefer using Alpha=1.0 here)
    col_table_border_light     // Table inner borders (prefer using Alpha=1.0 here)
    col_table_row_bg           // Table row background (even rows)
    col_table_row_bg_alt       // Table row background (odd rows)
    col_text_link
    col_text_selected_bg
    col_tree_lines
    col_drag_drop_target       // Rectangle highlighting a drop target
    col_nav_highlight          // Gamepad/keyboard: current highlighted item
    col_nav_windowing_highlight// Highlight window when using CTRL+TAB
    col_nav_windowing_dim_bg   // Darken/colorize entire screen behind the CTRL+TAB window list, when active
    col_modal_window_dim_bg    // Darken/colorize entire screen behind a modal window, when one is active
    col_COUNT


// Utility functions that are more convenient than calling start/end yourself.

def frame(body):
    frame_start()
    body()
    frame_end()

def window(title, flags, body):
    let open = window_start(title, flags, false)
    if open:
        body()
    window_end()  // Must always be called
    return open

def window_dock(title, flags, body):
    let open = window_start(title, flags, true)
    if open:
        body()
    window_end()  // Must always be called
    return open

def treenode(label, body):
    let open = treenode_start(label, 0)
    if open:
        body()
        treenode_end(0)
    return open

def treenode(label, flags, body):
    let open = treenode_start(label, flags)
    if open:
        body()
        treenode_end(flags)
    return open

def treenode_open(label, body):
    let open = treenode_start(label, treenode_default_open)
    if open:
        body()
        treenode_end(0)
    return open

def treenode_open_if(label, cond, body):
    let open = treenode_start(label, if cond: treenode_default_open else: 0)
    if open:
        body()
        treenode_end(0)
    return open

def tooltip_multi(body):
    let open = tooltip_multi_start()
    if open:
        body()
        tooltip_multi_end()
    return open

def tab_bar(label, body):
    let open = tab_bar_start(label)
    if open:
        body()
        tab_bar_end()
    return open

def tab(label, body):
    let open = tab_start(label, tabitem_none)
    if open:
        body()
        tab_end()
    return open

def tab(label, flags, body):
    let open = tab_start(label, flags)
    if open:
        body()
        tab_end()
    return open

def tab(label, font_idx, flags, body):
    font_start(font_idx)
    let open = tab_start(label, flags)
    if open:
        font_start(0)
        body()
        font_end()
        tab_end()
    font_end()
    return open

def menu_bar(main, body):
    let open = menu_bar_start(main)
    if open:
        body()
        menu_bar_end(main)
    return open

def menu(label, disabled, body):
    let open = menu_start(label, disabled)
    if open:
        body()
        menu_end()
    return open

def menu(label, body):
    let open = menu_start(label, false)
    if open:
        body()
        menu_end()
    return open

def id(label, body):
    id_start(label)
    body()
    id_end()

def table(id, num_columsn, flags, body):
    let visible = table_start(id, num_columsn, flags)
    if visible:
        body()
        table_end()
    return visible

// Call im.popup_open (from e.g. im.button) with same label as this, followed by im.popup() with body.
def popup(label, body):
    let open = popup_start(label, 0, false)
    if open:
        body()
        popup_end()
    return open

// Call this right after any item for which you want RMB to generate a popup.
def popup_rmb_prev_item(label, body):
    let open = popup_start(label, 0, true)
    if open:
        body()
        popup_end()
    return open

// Call im.popup_modal_open (from e.g. im.button) with same label as this, followed by im.popup_modal() with body.
def popup_modal(label, flags, body):
    let open = popup_modal_start(label, flags)
    if open:
        body()
        popup_end()
    return open

def disabled(disabled, body):
    disabled_start(disabled)
    body()
    disabled_end()

def disabled_because(disabled, reason, body):
    disabled_start(disabled)
    body()
    disabled_end()
    if disabled:
        tooltip(reason)

def button_repeat(repeat, body):
    button_repeat_start(repeat)
    body()
    button_repeat_end()

def drag_drop_source(flags, body):
    let open = drag_drop_source_start(flags)
    if open:
        body()
        drag_drop_source_end()
    return open

def drag_drop_target(body):
    let open = drag_drop_target_start()
    if open:
        body()
        drag_drop_target_end()
    return open

def width(width, body):
    width_start(width)
    body()
    width_end()

def font(font_idx, body):
    font_start(font_idx)
    body()
    font_end()

def color(col, body):
    color_start(col)
    body()
    color_end()

def child(title, size, body):
    let open = child_start(title, size, 0)
    if open:
        body()
    child_end()  // Must always be called
    return open

def child(title, size, flags, body):
    let open = child_start(title, size, flags)
    if open:
        body()
    child_end()  // Must always be called
    return open

def group(body):
    group_start()
    body()
    group_end()

def indent(amount, body):
    indent_start(amount)
    body()
    indent_end()

private let stack_traces = []
// Collect stack traces for the location this function is called from,
// as a rudimentary form of debugging.
// Pass positive n to collect the first n.
// Pass negative n to colledt the last n.
// Pass 0 to collect all (warning: may be slow if many!)
def stack_trace(n):
    if n > 0:
        if stack_traces.length >= n:
            return
    elif n < 0:
        if stack_traces.length >= -n: stack_traces.remove(0)
    let st = get_stack_trace()
    if st.length: stack_traces.push(st)

// Show the stack traces collected last frame, and clear them afterwards.
def show_stack_traces():
    treenode("Stack Traces"):
        for(stack_traces) st, i:
            treenode("{i}"):
                text(st)
    while stack_traces.length: stack_traces.pop()

// Convenience.

def text_font(font_idx, msg):
    font(font_idx):
        text(msg)

def spaced_separator():
    im.spacing()
    im.separator()
    im.spacing()
