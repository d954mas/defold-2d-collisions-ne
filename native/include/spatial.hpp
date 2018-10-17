#pragma once
#include <map>
#include <set>
#include <vector>
#include <unordered_set>
#include <dmsdk/dlib/log.h>
#include <algorithm> 
#include <vector> 
#define SPP_USE_SPP_ALLOC 1
#define SPP_MIX_HASH 1
#include <sparsepp/spp.h>
#define CIRCLE 1
#define hash(x, y) (((int)x/cell_size) | (((int)y/cell_size)<<16))
#define SHAPE "Shape"
#define SHAPE_NO_GC "ShapeNoGC"
//x, y point coords. ox, oy - origin point -x1, y1 where save result
static void rotatePoint(double x, double y,double rotation, double ox, double oy, double*x1, double*y1){
	double tempX = x - ox;
	double tempY = y - oy;
	double rotatedX = tempX*cos(rotation) - tempY*sin(rotation);
	double rotatedY = tempX*sin(rotation) + tempY*cos(rotation);
	*x1 = rotatedX + ox;
	*y1 = rotatedY + oy;
}

class Shape
{
	public:
	std::vector<int> ids;
	double x=0,y=0;
	double bboxX1=0, bboxX2=0, bboxY1=0, bboxY2=0;
	double rotation=0;
	double scale=1;
	int group=0;
	int mask=0; //collision mask
	int type;
	bool dirty;
	int l_ref;
	Shape(double x1, double y1, int type1){
		x = x1;
		y = y1;
		scale = 1;
		type = type1;
		dirty = true;
	}

	void updateBboxIfNeeded(){
		if(dirty){
			dirty = false;
			updateBbox();
		}
	}

	virtual void updateBbox(){
		bboxX1 = x;
		bboxX2 = x;
		bboxY1 = y;
		bboxY2 = y;
	}

	virtual void setScale(double newScale){
		scale = newScale;
		dirty = true;
	}

	virtual void setRotation(double newRotation){
		rotation = newRotation;
		dirty = true;
	}

	virtual void setPosition(double newX, double newY){
		x = newX;
		y = newY;
		dirty = true;
	}
};

struct Point{
	double x,y;
};

class RectangleShape: public Shape {
	public:
	double width, height;
	Point points[4];
	RectangleShape(double xx, double yy, double width1, double height1): Shape(xx,yy,1){
		width = width1;
		height = height1;
	}

	void setScale(double newScale){
		width = width * newScale/scale;
		height = height * newScale/scale;
		scale = newScale;
		dirty = true;
	}

	void updateBbox(){
		double x1,y1,x2,y2,x3,y3,x4,y4; //left bot; left top; top right; bot right 
		double h_w= width/2;
		double h_h = height/2;
		x1 = x-h_w;
		x2 = x-h_w;
		x3 = x+h_w;
		x4 = x+h_w;
		y1 = y-h_h;
		y2 = y+h_h;
		y3 = y+h_h;
		y4 = y-h_h;
		rotatePoint(x1,y1,rotation, x,y,&x1, &y1);
		rotatePoint(x2,y2,rotation,x,y,&x2, &y2);
		rotatePoint(x3,y3,rotation,x,y,&x3, &y3);
		rotatePoint(x4,y4,rotation,x,y,&x4, &y4);
		bboxX1 = fmin(fmin(x1,x2),fmin(x3,x4));
		bboxX2 = fmax(fmax(x1,x2),fmax(x3,x4));
		bboxY1 = fmin(fmin(y1,y2),fmin(y3,y4));
		bboxY2 = fmax(fmax(y1,y2),fmax(y3,y4));
		points[0].x = x1; points[0].y = y1;
		points[1].x = x2; points[1].y = y2;
		points[2].x = x3; points[2].y = y3;
		points[3].x = x4; points[3].y = y4;
	}

};

class CircleShape: public Shape {
	public:
	double rad;
	CircleShape(double x1, double y1, double radd): Shape(x1,y1,2){
		rad = radd;
	}

	void setScale(double newScale){
		rad = rad * newScale/scale;
		scale = newScale;
		dirty = true;
	}
	void updateBbox(){
		bboxX1 = x-rad;
		bboxX2 = x+rad;
		bboxY1 = y-rad;
		bboxY2 = y+rad;
	}
};

struct ChildShape{
	Shape* shape;
	double x;
	double y;
	double rotation;
};

class ComplexShape: public Shape {
	public:
	std::vector<ChildShape*> shapes;
	ComplexShape(double x1, double y1): Shape(x1,y1,3){}

	RectangleShape* addRect(double x1,double y1,double width,double height, double rotation){
		RectangleShape* shape= new RectangleShape(x+x1, y+y1, width, height);
		shape->setRotation(rotation);
		ChildShape* child = new ChildShape();
		child->shape = shape;
		child->x = x1;
		child->y = y1;
		child->rotation = rotation;
		shapes.push_back(child);
		return shape;
	}

	CircleShape* addCircle(double x1,double y1,double rad){
		CircleShape* shape= new CircleShape(x+x1, y+y1, rad);
		ChildShape* child = new ChildShape();
		child->shape = shape;
		child->x = x1;
		child->y = y1;
		child->rotation = 0;
		shapes.push_back(child);
		return shape;
	}

	void setScale(double newScale){
		double scaled = newScale/scale;
		scale = newScale;
		dirty = true;
		for(ChildShape* child : shapes) {
			child->x *= scaled;
			child->y *= scaled;
			child->shape->setScale(newScale);
			double pos_x = child->x + x;
			double pos_y = child->y + y;
			child->shape->setPosition(pos_x, pos_y);
		}
		
	}

	void setRotation(double newRotation){
		rotation = newRotation;
		dirty = true;
		for(ChildShape* child : shapes) {
			if (child->shape->type == 2){
				double pos_x = child->x + x;
				double pos_y = child->y + y;
				rotatePoint(pos_x,pos_y,newRotation,x,y,&pos_x, &pos_y);
				child->shape->setPosition(pos_x, pos_y);
			}else if (child->shape->type == 1){
				double pos_x = child->x + x;
				double pos_y = child->y + y;
				rotatePoint(pos_x,pos_y,newRotation,x,y,&pos_x, &pos_y);
				child->shape->setPosition(pos_x, pos_y);
				child->shape->setRotation(rotation + child->rotation);
			}
		}	
	}

	void setPosition(double newX, double newY){
		x = newX;
		y = newY;
		dirty = true;
		for(ChildShape* child : shapes) {
			double pos_x = child->x + x;
			double pos_y = child->y + y;
			child->shape->setPosition(pos_x, pos_y);
		}	
	}
	
	void updateBbox(){
		if (shapes.size()>0){
			Shape* shape = shapes[0]->shape;
			shape->updateBbox();
			bboxX1 = shape->bboxX1;
			bboxX2 = shape->bboxX2;
			bboxY1 = shape->bboxY1;
			bboxY2 = shape->bboxY2;
			for(ChildShape* child : shapes) {
				child->shape->updateBbox();
				Shape* shape = child->shape;
				shape->updateBbox();
				if (shape->bboxX1 < bboxX1){
					bboxX1=shape->bboxX1;
				}
				if (shape->bboxX2 > bboxX2){
					bboxX2=shape->bboxX2;
				}
				if (shape->bboxY1 < bboxY1){
					bboxY1=shape->bboxY1;
				}
				if (shape->bboxY2 > bboxY2){
					bboxY2=shape->bboxY2;
                }
		    }
		}
	}
};

static Shape* checkShape (lua_State *L, int index){
	luaL_checktype(L, index, LUA_TUSERDATA);
	//void* storage = lua_touserdata(L, 1);
	//void *p = lua_touserdata(L, 1);
//	Shape** shape = (Shape**) p;
	//return *shape;
	/*if (p != NULL) {  /* //value is a userdata? 
		if (lua_getmetatable(L, 1)) {  / does it have a metatable? 
				lua_getfield(L, LUA_REGISTRYINDEX, SHAPE);  // get correct mt 
				if (lua_rawequal(L, -1, -2)) {  // does it have the correct mt? 
					lua_pop(L, 2);  // remove both metatables 
					Shape** shape = (Shape**) p;
					return *shape;
				}else{
					lua_pop(L,1);
					lua_getfield(L, LUA_REGISTRYINDEX, SHAPE_NO_GC);  //get correct mt 
						if (lua_rawequal(L, -1, -2)) {  // does it have the correct mt? 
							lua_pop(L, 2);  //remove both metatables
							Shape** shape = (Shape**) p;
							return *shape;
				}
			}
		}
	}*/
	Shape** shape = (Shape**)  lua_touserdata(L, index);
	/*if (shape == NULL) {
		shape = (Shape**) luaL_checkudata(L, index, SHAPE);
		if (shape == NULL) {
			luaL_typerror(L, index, SHAPE);
		}
	}*/
	return *shape;
}

static void pushShape (lua_State *L, Shape *shape){
	Shape **l_shape = (Shape **)lua_newuserdata(L, sizeof(Shape*));
	*l_shape = shape;
	luaL_getmetatable(L, SHAPE);
	lua_setmetatable(L, -2);
}

static void pushShapeNoGC (lua_State *L, Shape *shape){
	Shape **l_shape = (Shape **)lua_newuserdata(L, sizeof(Shape*));
	*l_shape = shape;
	luaL_getmetatable(L, SHAPE_NO_GC);
	lua_setmetatable(L, -2);
}

static int ShapeRect_new (lua_State *L){
	double x = luaL_checknumber(L, 1);
	double y = luaL_checknumber(L, 2);
	double width = luaL_checknumber(L, 3);
	double height = luaL_checknumber(L, 4);
	RectangleShape *rect = new RectangleShape(x, y, width, height);
	pushShape(L, rect);
	return 1;
}

static int ShapeCircle_new (lua_State *L){
	double x = luaL_checknumber(L, 1);
	double y = luaL_checknumber(L, 2);
	double rad = luaL_checknumber(L, 3);
	CircleShape* circle = new CircleShape(x, y, rad);
	pushShape(L, circle);
	return 1;
}

static int ShapeComplex_new (lua_State *L){
	double x = luaL_checknumber(L, 1);
	double y = luaL_checknumber(L, 2);
	ComplexShape* shape = new ComplexShape(x, y);
	pushShape(L, shape);
	return 1;
}

static int ShapeComplex_add_rect(lua_State *L){
	Shape *shape = checkShape(L, 1);
	double x = luaL_checknumber(L, 2);
	double y = luaL_checknumber(L, 3);
	double width = luaL_checknumber(L, 4);
	double height = luaL_checknumber(L, 5);
	double rotation = luaL_checknumber(L, 6);
	if (shape->type == 3){
		ComplexShape *complex = (ComplexShape*)shape;
		RectangleShape *rect = complex->addRect(x, y, width, height,rotation);
	}else{
		dmLogError("not complex shape");
	}
	return 0;
}

static int ShapeComplex_add_circle(lua_State *L){
	Shape *shape = checkShape(L, 1);
	double x = luaL_checknumber(L, 2);
	double y = luaL_checknumber(L, 3);
	double rad = luaL_checknumber(L, 4);
	if (shape->type == 3){
		ComplexShape *complex = (ComplexShape*)shape;
		CircleShape *rect = complex->addCircle(x, y, rad);
	}else{
		dmLogError("not complex shape");
	}
	return 0;
}

static int Shape_get_position (lua_State *L){
	Shape *im = checkShape(L, 1);
	lua_pushnumber(L, im->x);
	lua_pushnumber(L, im->y);
	return 2;
}
static int Shape_set_position (lua_State *L){
	Shape *shape = checkShape(L, 1);
	double x = luaL_checknumber(L, 2);
	double y = luaL_checknumber(L, 3);
	shape->setPosition(x,y);
	return 0;
}

static int Shape_get_scale (lua_State *L){
	Shape *im = checkShape(L, 1);
	lua_pushnumber(L, im->scale);
	return 1;
}
static int Shape_set_scale (lua_State *L){
	Shape *shape = checkShape(L, 1);
	double scale = luaL_checknumber(L, 2);
	shape->setScale(scale);
	return 0;
}



static int Shape_get_bbox (lua_State *L){
	Shape *im = checkShape(L, 1);
	lua_pushnumber(L, im->bboxX1);
	lua_pushnumber(L, im->bboxY1);
	lua_pushnumber(L, im->bboxX2);
	lua_pushnumber(L, im->bboxY2);
	return 4;
}

static int Shape_set_rotation (lua_State *L){
	Shape *shape = checkShape(L, 1);
	double rot = luaL_checknumber(L, 2);
	shape->setRotation(rot);
	return 0;
}

static int Shape_get_rotation (lua_State *L){
	Shape *im = checkShape(L, 1);
	lua_pushnumber(L, im->rotation);
	return 1;
}

static int Shape_set_group(lua_State *L){
	Shape *shape = checkShape(L, 1);
	int group = luaL_checknumber(L, 2);
	shape->group = group;
	return 0;
}

static int Shape_get_group (lua_State *L){
	Shape *im = checkShape(L, 1);
	lua_pushnumber(L, im->group);
	return 1;
}

static int Shape_set_mask(lua_State *L){
	Shape *shape = checkShape(L, 1);
	int mask = luaL_checknumber(L, 2);
	shape->mask = mask;
	return 0;
}

static int Shape_get_mask (lua_State *L){
	Shape *im = checkShape(L, 1);
	lua_pushnumber(L, im->mask);
	return 1;
}

static int Shape_get_type (lua_State *L){
	Shape *im = checkShape(L, 1);
	lua_pushnumber(L, im->type);
	return 1;
}

static int Shape_get_data (lua_State *L){
	Shape *shape = checkShape(L, 1);
	lua_rawgeti(L, LUA_REGISTRYINDEX, shape->l_ref);
	return 1;
}

static int Shape_set_data(lua_State *L){
	Shape *shape = checkShape(L, 1);
	if (lua_istable(L,2)){
		int r = luaL_ref(L, LUA_REGISTRYINDEX);
		shape->l_ref = r;
	}else{
		dmLogError("data should be a table");
	}
	
	return 0;
}


static int Shape_get_size(lua_State *L){
	Shape* shape = checkShape(L, 1);
	if (shape->type == 1){
		RectangleShape* rect = (RectangleShape*)(shape);
		lua_pushnumber(L, rect->width);
		lua_pushnumber(L, rect->height);
		return 2;
	}else if (shape->type == 2){
		CircleShape* circle = (CircleShape*)(shape);
		lua_pushnumber(L, circle->rad);
		return 1;
	}
	return 0;
}


int cell_size=100;
int width;
spp::sparse_hash_map<int, spp::sparse_hash_set<Shape*>> MAP;
spp::sparse_hash_set<Shape*> SHAPES;
void init(int cell_size){
	cell_size = cell_size;
	MAP.clear();
	SHAPES.clear();
}

static int World_remove_from_map(Shape* shape){
	for(int id:shape->ids){
		MAP[id].erase(shape);
	}
	return 0;
}

static int World_add_to_map(Shape* shape){
	int x1 = (int)shape->bboxX1;
	int y1 = (int)shape->bboxY1;
	int x2 = (int)(shape->bboxX2+cell_size);
	int y2 = (int)(shape->bboxY2+cell_size);
	shape->ids.clear();
	for(int y=y1; y<=y2; y+=cell_size){
		for(int x=x1; x<=x2; x+=cell_size){
			int id = hash(x,y);
			MAP[id].insert(shape);
			shape->ids.push_back(id);
		}
	}
	return 0;
}

static void World_add(Shape *shape){
	SHAPES.insert(shape);
}
static void World_remove(Shape *shape){
	SHAPES.erase(shape);
	World_remove_from_map(shape);
}

static int World_add_lua(lua_State *L){
	Shape* shape = checkShape(L, 1);
	World_add(shape);
	return 0;
}
static int World_remove_lua(lua_State *L){
	Shape* shape = checkShape(L, 1);
	World_remove(shape);
	return 0;
}
//0 - not collided
//1 -collided all
//2 - collided left
//3 - collided right
static int collide_shape(Shape* a, Shape* b){
	bool collide_left = (a->mask& b->group) > 0;
	bool collide_right = (b->mask& a->group) > 0;
	if (collide_left || collide_right){
		if(collide_left && collide_right){
			return 1;
		}else if(collide_left){
			return 2;
		}else{
			return 3;
		}
	}
	
	return 0;
}

namespace std
{
	// inject specialization of std::hash for Person into namespace std
	// ----------------------------------------------------------------
	template<> 
	struct hash<std::pair<Shape*, Shape*>>
	{
		std::size_t operator()(std::pair<Shape*, Shape*> const &p) const
		{
			std::size_t seed = 0;
			spp::hash_combine(seed, p.first);
			spp::hash_combine(seed, p.second);
			return seed;
		}
	};
}

struct pair_hash {
	template <typename T>
	std::size_t operator () (const std::pair<T, T> &p) const {
		std::size_t seed = 0;
		spp::hash_combine(seed, p.first);
		spp::hash_combine(seed, p.second);
		return seed;
		//return  std::hash<T>()(p.first) ^ std::hash<T>()(p.second);
	}
	
};

static inline bool circle_circle_collision(Shape *shape, Shape* shape2){
	CircleShape* a = (CircleShape*) shape;
	CircleShape* b = (CircleShape*) shape2;
	double dist_x = abs(a->x - b->x);
	double dist_y = abs(a->y - b->y);
	double size = sqrt(dist_x*dist_x + dist_y*dist_y);
	double rad = a->rad + b->rad;
	return rad > size;
}

static inline void vec2d(Point* a, Point* b, Point* result){
	result->x = b->x - a->x;
	result->y = b->y - a->y;
	double len = sqrt(result->x*result->x + result->y*result->y);
	result->x /= len;
	result->y /= len;
}

static inline double LineDistance_sqr(const double x, const double y, const double p1x, const double p1y, const double p2x, const double p2y)
{
	const double dx = p2x-p1x;
	const double dy = p2y-p1y;      
	double t = (dx*(x - p1x) + dy*(y - p1y)) / (dx*dx + dy*dy);
	t = fmin(fmax(t,0),1);//к отрезку а не к линии
	const double px = dx * t + p1x;
	const double py = dy * t + p1y;
	return sqrt((x-px)*(x-px) + (y-py)*(y-py));
}
//https://gist.github.com/snorpey/8134c248296649433de2
static inline bool circle_rect_collision(Shape *a, Shape* b){
	CircleShape* circle = (CircleShape*) a;
	RectangleShape* rect = (RectangleShape*) b;

	double rectCenterX = rect->x;
	double rectCenterY = rect->y;

	double rectX = rectCenterX - rect->width/2;
	double rectY = rectCenterY - rect->height/2;

	double rectReferenceX = rectX;
	double rectReferenceY = rectY;

	double circleDx =circle->x - rectCenterX;
	double circleDy = circle->y - rectCenterY;
	double cos_rot = cos(-rect->rotation);
	double sin_rot = sin(-rect->rotation);

	// Rotate circle's center point back
	double unrotatedCircleX = cos_rot * circleDx - sin_rot * circleDy + rectCenterX;
	double unrotatedCircleY = sin_rot * circleDx + cos_rot * circleDy + rectCenterY;

	// Closest point in the rectangle to the center of circle rotated backwards(unrotated)
	double closestX=0, closestY=0;

	// Find the unrotated closest x point from center of unrotated circle
	if ( unrotatedCircleX < rectReferenceX ) {
		closestX = rectReferenceX;
	} else if ( unrotatedCircleX > rectReferenceX + rect->width ) {
		closestX = rectReferenceX + rect->width;
	} else {
		closestX = unrotatedCircleX;
	}
	// Find the unrotated closest y point from center of unrotated circle
	if ( unrotatedCircleY < rectReferenceY ) {
		closestY = rectReferenceY;
	} else if ( unrotatedCircleY > rectReferenceY + rect->height ) {
		closestY = rectReferenceY + rect->height;
	} else {
		closestY = unrotatedCircleY;
	}
	// Determine collision
	double dX = abs( unrotatedCircleX - closestX );
	double dY = abs( unrotatedCircleY - closestY );
	double distance = sqrt( ( dX * dX ) + ( dY * dY ) );
	return distance <= circle->rad;
}


static inline double scalar(Point* a, Point* b){
	return a->x * b->x  + a->y * b->y;
}

static inline bool have_axis(RectangleShape* a, RectangleShape* b, Point* vec){
	double min_a, max_a, min_b, max_b;
	double value = scalar(&a->points[0], vec);
	min_a = value; max_a = value;
	value = scalar(&b->points[0], vec);
	min_b = value; max_b = value;
	for (int i=1; i<4;i++){
		double value = scalar(&a->points[i], vec);
		if (value < min_a){
			min_a = value;
		}else if (value > max_a){
			max_a = value;
		}
		value = scalar(&b->points[i], vec);
		if (value < min_b){
			min_b = value;
		}else if (value > max_b){
			max_b = value;
		}
	}
	return !((min_a >= min_b && min_a <= max_b) || (max_a >= min_b && max_a <= max_b) ||
	(min_b >= min_a && min_b <= max_a) || (max_b >= min_a && max_b <= max_a));
}

static inline bool rect_rect_collision(Shape *shape, Shape* shape2){
	RectangleShape* a = (RectangleShape*) shape;
	RectangleShape* b = (RectangleShape*) shape2;
	Point vector;
	vec2d(&a->points[0], &a->points[1], &vector);
	if (have_axis(a,b, &vector)){ return false;}
	vec2d(&a->points[1], &a->points[2], &vector);
	if (have_axis(a,b, &vector)){ return false;}
	vec2d(&b->points[0], &b->points[1], &vector);
	if (have_axis(a,b, &vector)){ return false;}
	vec2d(&b->points[1], &b->points[2], &vector);
	if (have_axis(a,b, &vector)){ return false;}
	//printf("vec2d: %f %f\n", vector.x, vector.y);
	return true;
}

static bool is_shape_collided(Shape* a, Shape* b){
	bool collide = false;
	switch(a->type){
		case 1:
		switch(b->type){
			case 1:
			collide = rect_rect_collision(a,b);break;
			case 2:
			collide = circle_rect_collision(b,a);break;
			case 3:break;
			collide = is_shape_collided(b,a);break;
		}
		break;
		case 2:
		switch(b->type){
			case 1:
			collide = circle_rect_collision(a,b);break;
			case 2:
			collide = circle_circle_collision(a,b);break;
			case 3:
			collide = is_shape_collided(b,a);break;
		}
		break;
		case 3:
		ComplexShape* complex = (ComplexShape*) a;
		for(ChildShape* shape: complex->shapes){
			if (is_shape_collided(b,shape->shape)) {
				collide = true;
				break;
			}
		}
		break;
	}
	return collide;
}

spp::sparse_hash_set<std::pair<Shape*, Shape*>>collisions(10000);
static int World_collide(lua_State *L){
	//std::unordered_set<std::pair<Shape*, Shape*>, pair_hash>collisions;
	//std::set<std::pair<Shape*,Shape*>>collisions;
	collisions.clear();
	for(Shape* shape : SHAPES) {
		for(int id:shape->ids){
			auto it = MAP.find(id);
			if (it != MAP.end()){
				it->second.erase(shape);
				for(auto const candidat : it->second){
					if((shape->mask& candidat->group) > 0 || (candidat->mask& shape->group) > 0){
						collisions.insert({shape, candidat});
					}
				}
			}
		}
	}
	//printf("possible collisions:%d\n",  collisions.size());
	int n = 0;
	lua_newtable(L);
	for(auto pair:collisions){
		Shape* a = pair.first;
		Shape* b = pair.second;
		bool collide_left = (a->mask& b->group) > 0;
		bool collide_right = (b->mask& a->group) > 0;
		int can_collide=0;
		if (collide_left || collide_right){
			if(collide_left && collide_right){
				can_collide= 1;
			}else if(collide_left){
				can_collide= 2;
			}else{
				can_collide= 3;
			}
		}
		if(can_collide > 0){
			if(!(b->bboxX1 > a->bboxX2 || b->bboxX2 < a->bboxX1
				||b->bboxY2 < a->bboxY1 || b->bboxY1 > a->bboxY2)){
					bool collide = is_shape_collided(a,b);
					if(collide){
						lua_newtable(L);
						pushShapeNoGC(L, a);
						lua_setfield(L, -2, "shape_1");
						pushShapeNoGC(L, b);
						lua_setfield(L, -2, "shape_2");
						lua_pushnumber(L, can_collide);
						lua_setfield(L, -2, "type");
						lua_rawseti(L,-2,n + 1);
						++n;	
					}
					
				}
			}
		}
	/*for(auto iter1 = SHAPES.begin(); iter1 != SHAPES.end(); ++iter1) {
		for(auto iter2 = std::next(iter1); iter2 != SHAPES.end();++iter2){
			Shape* shape = *iter1;
			Shape* shape2 = *iter2;
			n = n +1;
		}
	}*/	
	
	//printf("size:%d\n",  n);
	return 1;
	//return SHAPES.size();
}


static int World_update(lua_State *L){
	for(Shape* shape : SHAPES) {
		World_remove_from_map(shape);
		shape->updateBboxIfNeeded();
		World_add_to_map(shape);
	}
	/*(for(auto iter1 = SHAPES.begin(); iter1 != SHAPES.end(); ++iter1) {
		for(auto iter2 = std::next(iter1); iter2 != SHAPES.end();++iter2){
			Shape* shape = *iter1;
			Shape* shape2 = *iter2;
			if (((shape->group& shape2->mask) > 0) || ((shape2->group & shape->mask) > 0)){
				//can collide
			}
		}
	}	*/
	return 0;
}

static int World_get_shapes_size(lua_State *L){
	lua_pushnumber(L, SHAPES.size());
	return 1;
}

static int Shape_dispose(lua_State *L){
	Shape *shape = checkShape(L, 1);
	lua_pushnil(L); 
	lua_setmetatable(L,1);
	World_remove(shape);
	return 0;
}

static int Shape_gc (lua_State *L){
	Shape *shape = checkShape(L, 1);
	World_remove(shape);
	if(shape->type ==3){
		ComplexShape * complex = (ComplexShape*) shape;
		for (int index = 0; index < complex->shapes.size(); ++index)
		{
			delete complex->shapes[index]->shape;
			delete complex->shapes[index];
		}
		complex->shapes.clear();
		complex->shapes.resize(0);
	}
	delete shape;
	return 0;
}

static int Shape_get_childs(lua_State *L){
	Shape *shape = checkShape(L, 1);
	lua_newtable(L);
	if (shape->type == 3){
		ComplexShape *shape2 = (ComplexShape*) shape;
		for (int index = 0; index < shape2->shapes.size(); ++index)
		{
			pushShapeNoGC(L, shape2->shapes[index]->shape);
			lua_rawseti(L,-2,index + 1);
		}
	}
	return 1;
}

static int Shape_get_shapes(lua_State *L){
	lua_newtable(L);
	int i=1;
	for(Shape* shape : SHAPES) {
			pushShapeNoGC(L,shape);
			lua_rawseti(L,-2,i);
			i++;
	}
	return 1;
}

static int Shape_tostring (lua_State *L){
	Shape *shape = checkShape(L, 1);
	lua_pushfstring(L, "Shape(%f %f)", shape->x, shape->y);
	return 1;
}

static const luaL_reg Shape_methods[] = {
	{"new_rect",           ShapeRect_new},
	{"new_circle",           ShapeCircle_new},
	{"new_complex",           ShapeComplex_new},
	{"get_size",           Shape_get_size},
	{"get_position",           Shape_get_position},
	{"set_position",           Shape_set_position},
	{"get_rotation",           Shape_get_rotation},
	{"set_rotation",           Shape_set_rotation},
	{"get_scale",           Shape_get_scale},
	{"set_scale",           Shape_set_scale},
	{"get_group",           Shape_get_group},
	{"set_group",           Shape_set_group},
	{"get_mask",           Shape_get_mask},
	{"set_mask",           Shape_set_mask},
	{"get_type",           Shape_get_type},
	{"get_bbox",           Shape_get_bbox},
	{"get_data",           Shape_get_data},
	{"set_data",           Shape_set_data},
	{"add_rect",           ShapeComplex_add_rect},
	{"add_circle",           ShapeComplex_add_circle},
	{"get_childs",           Shape_get_childs},
	{"get_shapes",           Shape_get_shapes},
	{"update",World_update},
	{"collide",World_collide},
	{"remove", World_remove_lua},
	{"add", World_add_lua},
	{"dispose",           Shape_dispose},
	{"get_shapes_size",           World_get_shapes_size},
	{0,0}
};

static const luaL_reg Shape_meta[] = {
	{"__gc",       Shape_gc},
	{"__tostring", Shape_tostring},
	{0, 0}
};

static const luaL_reg Shape_meta_no_gc[] = {
	{"__tostring", Shape_tostring},
	{0, 0}
};

int Shape_register (lua_State *L)
{
	luaL_register(L, SHAPE, Shape_methods); // create methods table,
	luaL_newmetatable(L, SHAPE);
	luaL_register(L, NULL, Shape_meta);
	lua_pushliteral(L, "__index");
	lua_pushvalue(L, -3);
	lua_rawset(L, -3);                
	lua_pushliteral(L, "__metatable");
	lua_pushvalue(L, -3);
	lua_rawset(L, -3);
	lua_pop(L, 1);
	lua_pop(L, 1);

	luaL_register(L, SHAPE_NO_GC, Shape_methods); // create methods table,
	luaL_newmetatable(L, SHAPE_NO_GC);
	luaL_register(L, NULL, Shape_meta_no_gc);
	lua_pushliteral(L, "__index");
	lua_pushvalue(L, -3);
	lua_rawset(L, -3);                
	lua_pushliteral(L, "__metatable");
	lua_pushvalue(L, -3);
	lua_rawset(L, -3);
	lua_pop(L, 1);
	lua_pop(L, 1);
	
	
	return 0;
}



