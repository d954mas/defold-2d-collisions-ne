# defold-2d-collisions-ne
I made this native extension for my sidescroller game in [Defold game engine](https://www.defold.com).

This extinsion work only with rects and circles.

Circles and rect can be rotated

This extinsion only find fact of collision. It not found normal, overlapped areas and etc.

## Installation
You can use the extension in your own project by adding this project as a [Defold library dependency](http://www.defold.com/manuals/libraries/). Open your game.project file and in the dependencies field under project add:

https://github.com/d954mas/defold-2d-collisions-ne/archive/master.zip

## API - functions
The pivot point of shape is in center
### Shape.new_rect(x, y, width, height)
### Shape.new_circle(x,y,rad)

Complex shape is a container for other shapes
### Shape.new_complex(x,y) 

Return width and height for rect or rad for circle.
### Shape.get_size() 

### Shape.get_position(shape) 
### Shape.set_position(shape,x,y) 

### Shape.get_rotation(shape) 
### Shape.set_rotation(shape,x,y) 

### Shape.get_scale(shape) 
### Shape.set_scale(shape,scale) 

Bit masks and groups
```lua
M.PLAYER_GROUP = bit.tobit(1)
M.ENEMY_GROUP = bit.tobit(2)
M.COLLECTABLE_GROUP = bit.tobit(4)
M.PLAYER_BULLET_GROUP = bit.tobit(8)
M.PLAYER_LAZER_GROUP = bit.tobit(16)
M.ENEMY_BULLET_GROUP = bit.tobit(32)

--player only collide with enemy, collectable, and enemy_bullets
M.PLAYER_MASK = bit.bor(M.ENEMY_GROUP, M.COLLECTABLE_GROUP, M.ENEMY_BULLET_GROUP)
```

### Shape.get_group(shape) 
### Shape.set_group(shape,group)

### Shape.get_mask(shape) 
### Shape.set_mask(shape,mask)
	
Return 1 for rect, 2 for circle 3 for complex  
### Shape.get_type(shape) 
Return x1,y1,x2,y2 (left bottom right top)
### Shape.get_bbox(shape)

You can attach table as data for shape.
### Shape.get_data(shape,data)
### Shape.set_data(shape,data)

--remove from world and destroy
### Shape.dispose(shape)


Only for complex shape
### Shape.add_rect(shape,x,y,w,h,rotation)
### Shape.add_circle(shape,x,y,rad)
--return array of shapes
### Shape.get_childs(shape)
  
WORLD FUNCTIONS
update all shapes(position, rotations and etc).

### Shape.update() 
Return all collisions.
every collision object have fields

shape_1

shape_2

type(0-no collision, 1-left, 2-right, 3-all)It depends on mask and groups
### Shape.collide() 

### Shape.remove(shape) 
### Shape.add(shape)

Return total shapes in world
### Shape.get_shapes_size()