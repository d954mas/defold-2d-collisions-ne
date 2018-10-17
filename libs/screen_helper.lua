local CONSTANTS = require "libs.constants"
local DEBUG_DRAWER = require "libs.debug_drawer.debug_drawer"
local HASHES = require "libs.hashes"
local M = {}
M.SCALE = 1
M.dx = 0

function M.set_scale(scale)
    M.SCALE = scale
    msg.post("@render:", HASHES.MSG_RENDER_WINDOW_RESIZED,{width = M.real_width, height = M.real_height})
end

function M.init_count()
    local width = CONSTANTS.GAME_WIDTH
    local x1,x2,y1,y2 = M.count_projrection_box(width, width*3/4)
    M.MAX_HEIGHT = y2
    M.MIN_HEIGHT = y1
    x1,x2,y1,y2 = M.count_projrection_box(width, width*1125 /2436)
    M.MAX_WIDTH = x2
    M.MIN_WIDTH = x1
end

function M.set_screen_size(width, height)
    M.screen_width = width
    M.screen_height = height
    M.screen_aspect_w = width / height
    M.screen_aspect_h = height / width
end


function M.count_projrection_box(width, height)
    local screen_aspect_w = width/height
    local screen_aspect_h = height/width
    local d_aspect_w = screen_aspect_w - CONSTANTS.GAME_ASPECT_W
    local d_aspect_h = screen_aspect_h - CONSTANTS.GAME_ASPECT_H
    local new_width = CONSTANTS.GAME_WIDTH + CONSTANTS.GAME_WIDTH * d_aspect_w * CONSTANTS.GAME_SCALE * CONSTANTS.GAME_ASPECT_H
    local new_height = CONSTANTS.GAME_HEIGHT + CONSTANTS.GAME_HEIGHT * d_aspect_h * (1 - CONSTANTS.GAME_SCALE) * CONSTANTS.GAME_ASPECT_W
    local d_w = (new_width - CONSTANTS.GAME_WIDTH) * CONSTANTS.GAME_MOVE_X+M.dx
    local game_x_1 = -d_w
    local game_x_2 = new_width - d_w
    local game_y_1 = -new_height / 2
    local game_y_2 = new_height / 2
    return game_x_1, game_x_2, game_y_1, game_y_2
end   

function M.count_projection()
    local game_x_1, game_x_2, game_y_1, game_y_2 = M.count_projrection_box(M.screen_width, M.screen_height)
    M.game_x_1 = game_x_1
    M.game_x_2 = game_x_2
    M.game_y_1 = game_y_1
    M.game_y_2 = game_y_2
    M.game_width = M.game_x_2-M.game_x_1
    M.game_height = M.game_y_2-M.game_y_1
    DEBUG_DRAWER.set_screen_size(M.game_x_1*M.SCALE, M.game_x_2*M.SCALE, M.game_y_1*M.SCALE, M.game_y_2*M.SCALE)
    return vmath.matrix4_orthographic(M.game_x_1*M.SCALE, M.game_x_2*M.SCALE, M.game_y_1*M.SCALE, M.game_y_2*M.SCALE, -1, 1)
end

function M.screen_to_world(x, y)
    local world_x = (M.game_x_1 + x * M.game_width/M.screen_width) * M.SCALE
    local world_y = (M.game_y_1 + y * M.game_height/M.real_height) * M.SCALE * M.real_height/M.screen_height
    return world_x, world_y
end

function M.world_to_screen(x,y)
    local screen_x = (x/M.SCALE-M.game_x_1)/(M.game_width/M.screen_width)
    local screen_y = (y/M.SCALE/(M.real_height/M.screen_height)-M.game_y_1)/(M.game_height/M.real_height)
    return screen_x, screen_y
end

return M