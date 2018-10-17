local ProxyScene = require "Jester.proxy_scene"
local Scene = ProxyScene:subclass("LogoScene")
local JESTER = require "Jester.jester"
local STENCIL = require "Jester.stencil.stencil_render"
local DEBUG_DRAWER = require "libs.debug_drawer.debug_drawer"
--- Constructor
-- @param name Name of scene.Must be unique
function Scene:initialize()
    ProxyScene.initialize(self, "TestCollisionsScene", "/test_collisions#proxy", "test_collisions:/scene_controller")
    self.shapes = {}

end

function Scene:on_show(input)

    for i=1, 30 do
        local shape = Shape.new_rect(math.random(100,1000),math.random(-300,300),math.random(30,40),math.random(30,40))
        shape:set_group(1)
        shape:set_mask(1)
        shape:set_data({velocity = i})
        Shape.add(shape)
        table.insert(self.shapes, shape)
        local shape = Shape.new_circle(math.random(100,1000),math.random(-300,300),math.random(30,40))
        shape:set_group(1)
        shape:set_mask(1)
        shape:set_data({i = i*10})
        Shape.add(shape)
        table.insert(self.shapes, shape)
       --[[ local complex_shape = Shape.new_complex(math.random(100,1000),math.random(-300,300))
        Shape.add(complex_shape)
        complex_shape:set_group(1)
        complex_shape:set_mask(1)
        complex_shape:add_circle(0,0,22,50)
        complex_shape:add_rect(0,75,222,50,0)
        complex_shape:add_rect(0,-75,50,50,0.78)
        complex_shape:add_circle(22,22,22,50)
        table.insert(self.shapes, complex_shape)--]]
    end
end

function Scene:final(go_self)
end

local function draw_shape(shape, color)
    color = color or vmath.vector4(0,255,0,0)
    local x,y = shape:get_position()
    local width, height = shape:get_size()
    local type = shape:get_type()
    local rotation = shape:get_rotation()
    -- print(rotation)
    local x1,y1,x2,y2 = shape:get_bbox()
    local bbox_width = x2 - x1
    local bbox_height = y2 - y1
    if type == 1 then
        DEBUG_DRAWER.filled_rect(x,y,width, height,color, math.deg(rotation))
    elseif type ==2 then
        DEBUG_DRAWER.circle(x, y, width*2,color)
    elseif type ==3 then
       -- pprint(#shape:get_childs())
        for _, child in ipairs(shape:get_childs())do
           draw_shape(child, color)
        end
    end    
end    
local rot = 0
function Scene:update(go_self, dt)
    rot = rot + 0.5*dt
    print("total_shapes:" .. Shape.get_shapes_size())
    DEBUG_DRAWER.clear()
    Shape.update(0)
    local collisions = Shape.collide()
    for _, shape in ipairs(self.shapes) do
       shape:set_rotation(rot)
       local x,y = shape:get_position()
       local x1,y1,x2,y2 = shape:get_bbox()
       local bbox_width = x2 - x1
       local bbox_height = y2 - y1
        draw_shape(shape)
    end
    
    for _, pair in ipairs(collisions)do
        draw_shape(pair.shape_1, vmath.vector4(0,0,255,0))
        draw_shape(pair.shape_2,vmath.vector4(0,0,255,0))
    end    
end

function Scene:show_out(co)
end
return Scene