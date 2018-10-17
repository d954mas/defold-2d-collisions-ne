local M = {}

M.INPUT_TOUCH = hash("touch")
M.INPUT_ACQUIRE_FOCUS = hash("acquire_input_focus")
M.INPUT_RELEASE_FOCUS = hash("release_input_focus")

M.INPUT_BACK = hash("back")
M.INPUT_TOGGLE_PROFILER = hash("toggle_profile")
M.INPUT_TOGGLE_PHYSICS = hash("toggle_physics")
M.INPUT_TOGGLE_RECT = hash("toggle_rect")
M.INPUT_SAVE_LEVEL = hash("save_level")
M.INPUT_LEFT = hash("left")
M.INPUT_RIGHT = hash("right")
M.INPUT_DOWN = hash("down")
M.INPUT_ROT_LEFT = hash("rot_left")
M.INPUT_ROT_RIGHT = hash("rot_right")


M.MSG_SYSTEM_TOGGLE_PROFILER = hash("toggle_profile")

M.MSG_PHYSICS_CONTACT = hash("contact_point_response")
M.MSG_PHYSICS_COLLISION = hash("collision_response")
M.MSG_PHYSICS_TRIGGER = hash("trigger_response")
M.MSG_PHYSICS_RAY_CAST= hash("ray_cast_response")

M.MSG_RENDER_CLEAR_COLOR = hash("clear_color")
M.MSG_RENDER_SET_VIEW_PROJECTION = hash("set_view_projection")
M.MSG_RENDER_WINDOW_RESIZED = hash("window_resized")
M.MSG_RENDER_DRAW_LINE = hash("draw_line")
M.MSG_PLAY_SOUND = hash("play_sound")
M.MSG_ENABLE = hash("enable")
M.MSG_DISABLE = hash("disable")
M.MSG_PLAY_ANIMATION = hash("play_animation")
M.MSG_ACQUIRE_CAMERA_FOCUS = hash("acquire_camera_focus")
M.MSG_SET_PARENT = hash("set_parent")
M.RGB = hash("rgb")


M.MSG_LEVEL_EDITOR_GUI_BTN_OPEN = hash("level_editor_btn_open_clicked")
M.MSG_LEVEL_EDITOR_GUI_BTN_SAVE = hash("level_editor_btn_save_clicked")
M.MSG_LEVEL_EDITOR_GUI_BTN_DELETE = hash("level_editor_btn_delete_clicked")
M.MSG_LEVEL_EDITOR_GUI_BTN_CLONE = hash("level_editor_btn_clone_clicked")
M.MSG_LEVEL_EDITOR_GUI_SET_TARGET_POS = hash("level_editor_btn_save_clicked")

M.MSG_GAME_PLAYER_LOSE_HEALTH = hash("game_player_lose_health")
M.MSG_GAME_BONUS_AMOUNT_CHANGED = hash("game_bonus_amount_changed")
M.MSG_GAME_GUI_POWER_BTN_CLICKED = hash("game_gui_power_btn_clicked")



M.MSG_PLAYER_GUI_ATTACK_ANIMATION = hash("player_gui_attack_animation")
M.MSG_GUI_CUTSCENE_BUBBLE_SHOW = hash("cutscene_gui_bubble_show")
M.MSG_GUI_CUTSCENE_BUBBLE_SET_TEXT = hash("cutscene_gui_bubble_set_text")
M.MSG_GUI_CUTSCENE_BUBBLE_SET_AVATAR = hash("cutscene_gui_bubble_set_avatar")



function M.ensure_hash(string_or_hash)
	return type(string_or_hash) == "string" and hash(string_or_hash) or string_or_hash
end

return M
