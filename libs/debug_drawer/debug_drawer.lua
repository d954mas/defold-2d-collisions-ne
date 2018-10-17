local M = {}
local DEFAULT_COLOR = vmath.vector4(255, 255, 255, 20)
local max_width = 800
local CONSTANTS = require "libs.constants"
M.draw = sys.get_sys_info().system_name == "Windows"

function M.create_buffer()
    M.width = max_width
    M.aspect = 4 / 4
    M.height = M.width * M.aspect
    M.buffer_info = {
        buffer = buffer.create(M.width * M.height, -- size of the buffer width*height
                { {
                      name = hash("debug_physics"),
                      type = buffer.VALUE_TYPE_UINT8,
                      count = 4
                  } }),
        width = M.width,
        height = M.height,
        channels = 4
    }
    M.camera_y = 0
end

function M.set_screen_size(x1, x2, y1, y2)
    if not M.draw then
        return
    end
    if not M.buffer_info then
        M.create_buffer()
    end
    M.x = x1

    local width = x2-x1
    local height = y2-y1
    M.scale = M.width /width
    M.y = -width/2
    M.screen_width = width
    M.screen_height = height
    M.scale = M.width /width
end


--ignore x
function M.set_camera_move(x,y)
    M.camera_y = y
end

function M.update_go()
    if not M.draw then return end
    msg.post("main:/debug_drawer", hash("update_go"))
end

function M.clear()
    if not M.draw then
        return
    end
    local i = M.buffer_info
    drawpixels.fill(M.buffer_info, 0, 0, 0, 0)
end

function M.circle(pos_x, pos_y, diameter, color)
    if not M.draw then
        return
    end
    color = color or DEFAULT_COLOR
    drawpixels.filled_circle(M.buffer_info, (pos_x - M.x) * M.scale, (pos_y - M.y) * M.scale, diameter * M.scale, color.x, color.y, color.z, color.w)
end

function M.filled_rect(pos_x, pos_y, width, height, color, angle)
    if not M.draw then
        return
    end
    color = color or DEFAULT_COLOR
    drawpixels.filled_rect(M.buffer_info, (pos_x - M.x) * M.scale, (pos_y - M.y) * M.scale, width * M.scale, height * M.scale, color.x, color.y, color.z, color.w, angle)
end

function M.line(x1, y1, x2, y2, size, color)
    if not M.draw then
        return
    end
    color = color or DEFAULT_COLOR
    local dx, dy = (x2 - x1), (y2 - y1)
    local cx = x1 + dx / 2
    local cy = y1 + dy / 2
    local width = math.sqrt(dx ^ 2 + dy ^ 2)
    math.min(1, width)
    local cosx = dx / width
    local angle = math.acos(cosx) * 57.29580406904963 + 180
    M.filled_rect(cx, cy, width, size , color, angle)
end

function M.toggle()
    M.clear()
    M.draw = not M.draw
end


return M