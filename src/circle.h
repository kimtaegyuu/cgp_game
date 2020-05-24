#pragma once
#ifndef __CIRCLE_H__
#define __CIRCLE_H__

struct circle_t
{
	vec2	center=vec2(0);		// 2D position for translation
	float	radius=2.0f;		// radius
	float	theta=0.0f;			// rotation angle
	vec4	color;				// RGBA color in [0,1]
	mat4	model_matrix;		// modeling transformation
	bool fall = false;
	bool	up = true;
	bool	is_dead = false;	// for hero / monster die
	bool	is_throwing = false;
	float	life = 0.0f; 
	float	monster_jump_distance = 2.5f;
	bool	monster_is_up = true;
	float	monster_jump_speed = 0.0f;
	bool	transparent = false;
	bool is_gone = false;

	// public functions
	void	update( float t );	
	void	rotate(float t);
	void	update_sword();
	void	monster_dead(float t);
};

inline circle_t get_circle()
{
	circle_t c;
	c = { vec2(0, 0), 1.0f, 0.0f, vec4(0.5f, 0.5f, 0.5f, 1.0f) };
	c.radius = 1.0f;
	return c;
}

inline std::vector<circle_t> create_circles()
{
	std::vector<circle_t> circles;
	circle_t c;
	
	c = {vec2(-0.5f,0),1.0f,0.0f,vec4(1.0f,0.5f,0.5f,1.0f)};
	circles.emplace_back(c);

	c = {vec2(+0.5f,0),1.0f,0.0f,vec4(0.5f,1.0f,1.0f,1.0f)};
	circles.emplace_back(c);

	return circles;
}

inline void circle_t::update( float t )
{
	theta	= t;
	float c	= cos(theta), s=sin(theta);

	// these transformations will be explained in later transformation lecture
	mat4 scale_matrix =
	{
		radius, 0, 0, 0,
		0, radius, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1
	};

	mat4 rotation_matrix =
	{
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1
	};

	mat4 translate_matrix =
	{
		1, 0, 0, center.x,
		0, 1, 0, center.y,
		0, 0, 1, 0,
		0, 0, 0, 1
	};
	
	model_matrix = translate_matrix*rotation_matrix*scale_matrix;
}

inline std::vector<circle_t> create_paths(vec2 start, int num)
{
	std::vector<circle_t> path;
	circle_t p;

	for (int i = 0; i < num; i++)
	{
		p = { vec2(start.x + float(i * p.radius),start.y),2.0f,0.0f,vec4(1.0f,0.5f,0.5f,1.0f) };
		path.emplace_back(p);
	}

	return path;
}


inline void circle_t::rotate(float t)
{
	theta = t*15;
	float c = cos(theta), s = sin(theta);

	// these transformations will be explained in later transformation lecture
	mat4 scale_matrix =
	{
		radius, 0, 0, 0,
		0, radius, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1
	};

	mat4 rotation_matrix =
	{
		c, s, 0, 0,
		-s, c, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1
	};

	mat4 translate_matrix =
	{
		1, 0, 0, center.x,
		0, 1, 0, center.y,
		0, 0, 1, 0,
		0, 0, 0, 1
	};

	model_matrix = translate_matrix * rotation_matrix * scale_matrix;
}


inline void circle_t::update_sword()
{
	theta = PI / 4;
	float c = cos(theta), s = sin(theta);

	// these transformations will be explained in later transformation lecture
	mat4 scale_matrix =
	{
		radius, 0, 0, 0,
		0, radius, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1
	};

	mat4 rotation_matrix =
	{
		c, s, 0, 0,
		-s, c, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1
	};

	mat4 translate_matrix =
	{
		1, 0, 0, center.x,
		0, 1, 0, center.y,
		0, 0, 1, 0,
		0, 0, 0, 1
	};

	model_matrix = translate_matrix * rotation_matrix * scale_matrix;
}

inline void circle_t::monster_dead(float t) { // delta
	center.y -= t;
}


#endif
