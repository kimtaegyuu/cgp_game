#include "cgmath.h"			// slee's simple math library
#include "cgut.h"			// slee's OpenGL utility
#include "circle.h"			// circle class definition
#include "irrKlang\irrKlang.h"
#pragma comment(lib, "irrKlang.lib")
using namespace irrklang;

//*************************************
// global constants
static const char* window_name = "cgcirc";
static const char* vert_shader_path = "../bin/shaders/particle.vert";
static const char* frag_shader_path = "../bin/shaders/particle.frag";

// hero texture path
static const char* stand_texture_path = "../bin/images/stand.png";
static const char* dash_texture_path = "../bin/images/dash.png";
static const char* captain_texture_path = "../bin/images/captain.png";
static const char* thor_texture_path = "../bin/images/thor.png";
static const char* hurk_texture_path = "../bin/images/hurk.png";
static const char* thanos_texture_path = "../bin/images/thanos.png";
static const char* dead_texture_path = "../bin/images/dead.png";
static const char* td_texture_path = "../bin/images/thanos_dead.png";
static const char* logo_texture_path = "../bin/images/logo.png";
static const char* he_texture_path = "../bin/images/hero_.png";

// texture path
static const char* hero_texture_path = "../bin/images/hero.png";
static const char* purple_texture_path = "../bin/images/purple.png";
static const char* map_texture_path = "../bin/images/block.png";
static const char* die_texture_path = "../bin/images/die.png";
static const char* cloud_texture_path = "../bin/images/cloud.png";
static const char* open_texture_path = "../bin/images/open.png";
static const char* closed_texture_path = "../bin/images/closed.png";
static const char* axe_texture_path = "../bin/images/axe.png";
static const char* sword_texture_path = "../bin/images/sword.png";

// sound path
static const char* gameclear_sound_path = "../bin/sound/clear.wav";
static const char* gameover_sound_path = "../bin/sound/game_over.wav";
static const char* yell_sound_path = "../bin/sound/yell.wav";
static const char* dash_sound_path = "../bin/sound/dash.wav";
static const char* nbgm_path = "../bin/sound/nbgm.wav";
static const char* abgm_path = "../bin/sound/abgm.wav";


//*************************************
// include stb_image with the implementation preprocessor definition
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

//*************************************
// window objects
GLFWwindow* window = nullptr;
ivec2		window_size = ivec2(720, 480);	// initial window size

//*************************************
// OpenGL objects
GLuint	program = 0;	// ID holder for GPU program
GLuint	vertex_buffer = 0;	// ID holder for vertex buffer
GLuint	index_buffer = 0;	// ID holder for index buffer
GLuint TEX = 0;
GLuint texture[20];

// irrKlang objects
irrklang::ISoundEngine* dash_engine;
irrklang::ISoundSource* dash_sound_src = nullptr;

irrklang::ISoundEngine* gameover_engine;
irrklang::ISoundSource* gameover_sound_src = nullptr;

irrklang::ISoundEngine* gameclear_engine;
irrklang::ISoundSource* gameclear_sound_src = nullptr;

irrklang::ISoundEngine* yell_engine;
irrklang::ISoundSource* yell_sound_src = nullptr;

irrklang::ISoundEngine* bgm_engine;
irrklang::ISoundSource* bgm_sound_src = nullptr;

//*************************************
//Random Func
inline const float random_range(float min, float max) { return min + rand() / (RAND_MAX / (max - min)); }

// forward declarations for freetype text
void text_init();
void render_text(std::string text, GLint x, GLint y, GLfloat scale, vec4 color);

//*************************************
// global variables
int		frame = 0;						// index of rendering frames
float	t = 0.0f;						// current simulation parameter
float	delta; 
bool	b_solid_color = false;			// use circle's color?
bool	b_wireframe = false;
uint	MaxParticles = 50; 
auto	circles = std::move(create_circles());
auto	hero = std::move(get_circle());
auto	sword = std::move(get_circle());
auto	king = std::move(get_circle());
std::vector<circle_t> monsters; 
std::vector<circle_t> clouds;
std::vector<circle_t> boxes;
std::vector<circle_t> axes; 
std::vector<std::vector<circle_t>> paths;

// hero moving variable
bool is_dash = false;
float how_dash = 0.0f;
float dash_distance = 4 * hero.radius;
float jump_distance = 2 * hero.radius;
bool is_jump = true;
bool is_up = true;
bool is_side_path = false;
bool camera_up = false;
float camera_y_pos = 0.0f;
int hero_pos = 0;
int box_pos = 0; 
int monster_pos = 0;
float back_distance = 1.0f;
bool go_back = false;

//camera moving variable
bool is_camera = false;
float how_camera = 0.0f;
float jump_speed = 0.0f;
int flag = 0;

// path distance
float len_path = 0.0f; 

// item moving variables
int axe_idx = 0;
int item_num = 8;
bool axe_seen = false;
int sword_idx = 0;
float sword_theta = 0.0f;
bool sword_seen = false;
bool king_seen = false; 
int king_idx = 0;

// text variables
float a = 0.0f;

// game mode
int game_state = 0; // 0 - easy, 1 - hard
int game_playing = 0; // 0 - menu, 1 - playing, 2 - game over
int game_menu = 0;
bool game_over_flag = true; 
bool game_clear_flag = true;

struct camera
{
	vec3   eye = vec3(0, 0, 13);
	vec3   at = vec3(0, 0, 0);
	vec3   up = vec3(0, 1, 0);
	mat4   view_matrix = mat4::look_at(eye, at, up);

	float   fovy = PI / 4.0f; // must be in radian
	float   aspect_ratio;
	float   dNear = 1.0f;
	float   dFar = 1000.0f;
	mat4   projection_matrix;
};
camera cam;

struct particle_t
{
	vec2 pos;
	vec4 color;
	vec2 velocity;
	float scale;
	float life;

	//optional
	float elapsed_time;
	float time_interval;

	mat4 model_matrix;

	particle_t() { reset(); }

	void reset()
	{
		pos = vec2(random_range(-10.0f, 10.0f), random_range(-10.0f, 10.0f));
		color = vec4(random_range(0, 1.0f), random_range(0, 1.0f), random_range(0, 1.0f), 1);
		scale = random_range(0.1f, 0.2f);
		life = random_range(0.01f, 1.0f);
		velocity = vec2(random_range(-1.0f, 1.0f), random_range(-1.0f, 1.0f)) * 0.003f;
		elapsed_time = 0.0f;
		time_interval = random_range(200.0f, 600.0f);
	}

	void get_blood(float x, float y) {
		pos = vec2(x, y); // 네모 center에서 시작
		color = vec4(139.0f / 256.0f, 32.0f / 256.0f, 2.0f / 256.0f , 1);
		scale = random_range(0.1f, 0.3f);
		life = 1.5f; 
		velocity = vec2(random_range(-1.0f, 1.0f), random_range(-1.0f, 1.0f)) * 0.003f;
		elapsed_time = 0.0f;
		time_interval = random_range(200.0f, 600.0f);
	}

	void get_tail(float x, float y) {
		pos = vec2(x, y); // 네모 center에서 시작
		color = vec4(1, 1, 1, 1);
		scale = random_range(0.1f, 0.3f);
		life = 1.5f;
		velocity = vec2(-1, 0);
		elapsed_time = 0.0f;
		time_interval = random_range(200.0f, 600.0f);
	}

	void tail_update(float x, float y)
	{
		const float dwTime = (float)glfwGetTime();
		elapsed_time += dwTime;
		const float theta = random_range(0, 1.0f) * PI * 2.0f;
		
		constexpr float life_factor = 0.01f;
		//life -= life_factor * dwTime;
		//pos += velocity * 0.0001f;

		life -= life_factor; 
		// disappear
		if (life < 0.0f)
		{
			constexpr float alpha_factor = 0.001f;
			color.a -= alpha_factor * delta * 10000;
		}
		// dead
		if (color.a < 0.0f) get_tail(x, y);

		mat4 scale_matrix =
		{
			scale, 0, 0, 0,
			0, scale, 0, 0,
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
			1, 0, 0, pos.x,
			0, 1, 0, pos.y,
			0, 0, 1, 0,
			0, 0, 0, 1
		};

		model_matrix = translate_matrix * rotation_matrix * scale_matrix;
	}

	void update()
	{
		const float dwTime = (float)glfwGetTime();
		elapsed_time += dwTime;
		const float theta = random_range(0, 1.0f) * PI * 2.0f;

		if (elapsed_time > time_interval)
		{
			
			constexpr float velocity_factor = 0.003f;
			//velocity = vec2(cos(theta), sin(theta)) * velocity_factor;

			elapsed_time = 0.0f;
		}

		pos += velocity;

		constexpr float life_factor = 0.0001f;
		life -= life_factor * dwTime;

		// disappear
		if (life < 0.0f)
		{
			constexpr float alpha_factor = 0.001f;
			color.a -= alpha_factor * dwTime;
		}

		// dead
		//if (color.a < 0.0f) reset();

		mat4 scale_matrix =
		{
			scale, 0, 0, 0,
			0, scale, 0, 0,
			0, 0, 1, 0,
			0, 0, 0, 1
		};

		mat4 rotation_matrix =
		{
			cos(theta), -sin(theta), 0, 0,
			sin(theta), cos(theta), 0, 0,
			0, 0, 1, 0,
			0, 0, 0, 1
		};

		mat4 translate_matrix =
		{
			1, 0, 0, pos.x,
			0, 1, 0, pos.y,
			0, 0, 1, 0,
			0, 0, 0, 1
		};

		model_matrix = translate_matrix * rotation_matrix * scale_matrix;
	}
};

//*************************************
// holder of vertices and indices of a unit circle
std::vector<vertex>	unit_circle_vertices;	// host-side vertices
std::vector<particle_t> particles;
std::vector<particle_t> tail_particles;

//*************************************

bool is_collision(circle_t a, circle_t b)
{
	if (abs(b.center.x - a.center.x) < a.radius / 2.0f + b.radius / 2.0f + 0.01f && abs(a.center.y - b.center.y) < a.radius / 2.0f + b.radius / 2.0f-0.01f && b.center.x>a.center.x && b.transparent == false)
		return true;
	else
		return false;
}

bool is_up_collision(circle_t a, circle_t b)
{
	if (abs(b.center.y - a.center.y) < a.radius / 2.0f + b.radius / 2.0f + 0.01f && abs(a.center.x - b.center.x) < a.radius / 2.0f + b.radius / 2.0f -0.01f&& a.center.y < b.center.y && !b.transparent)
		return true;
	else
		return false;
}

bool is_down_collision(circle_t a, circle_t b)
{
	if (abs(a.center.y - b.center.y) <= a.radius / 2.0f + b.radius / 2.0f + 0.01f && abs(a.center.x - b.center.x) < a.radius / 2.0f + b.radius / 2.0f -0.01f&& a.center.y > b.center.y && !b.transparent)
		return true;
	else
		return false;
}

void make_paths(float x, float y, int num)
{
	paths.push_back(std::move(create_paths(vec2(x, y), num)));
}

void init_path()
{
	srand(uint(time(NULL)));
	// 첫 시작 부분
	float x = -4.0f, y = -1.5f;
	paths.push_back(std::move(create_paths(vec2(x, y), 5)));
	len_path = 3 * 2.0f;
	int pn = 5;
	float px = x, py = y;

	// 랜덤 생성
	for (uint i = 0; i < 50; i++) {
		int n = int(random_range(2.0f, 5.5f));
		int bn = int(random_range(1.0f, 2.5f));
		int a;
		
		if (game_state == 0) a = int(random_range(0.0f, 2.2f));
		if (game_state == 1) a = int(random_range(0.0f, 2.3f));
		if (a == 1) a = int(random_range(0.5f, 1.7f));

		len_path += float(n * 2) + float(bn * 2);
		x = px + float(bn) * 2 + float(pn) * 2;
		y = py + random_range(-1.0f, 1.0f);

		// makes random clouds
		circle_t cloud;
		cloud.center = vec2(x + float(n) * 0.5f, y + 5.5f);
		cloud.radius = 4.0f; 
		clouds.push_back(cloud);

		// makes random mosters
		int big = int(random_range(0.6f, 1.9f));
		float mx = random_range(x, x + float(2 * (n - 1) - a));
		if (a > 0 && big == 0) a = 1, mx = random_range(x, x + float(2 * (n - 1)));
		for (uint j = 0; j < uint(a); j++) {
			circle_t m;
			if (big) m.radius = 1.0f;
			else m.radius = 2.0f;
			m.center = vec2(mx + float(j), big ? y + 1.5f : y + 2.0f);
			monsters.push_back(m);
		}

		// makes random boxes
		if (a == 0) { // no monsters
			int box_p = int(random_range(0.6f, 1.3f));
			if (box_p == 1) {
				circle_t box;
				float bx = random_range(x, x + float(2 * (n - 1)));
				box.center = vec2(bx, y + 1.75f);
				box.radius = 1.5f;
				boxes.push_back(box);
			}
		}

		paths.push_back(std::move(create_paths(vec2(x, y), n)));

		pn = n, px = x, py = y;
	}
}


void die(circle_t& c) {
	// initialize particles
	constexpr int max_particle = 20;
	particles.resize(max_particle);
	
	for (auto& p : particles) { p.get_blood(c.center.x, c.center.y); }


	
	c.is_dead = true;
	c.transparent = true; 
}
void monster_die(circle_t& c) {
	// initialize particles
	constexpr int max_particle = 20;
	particles.resize(max_particle);

	yell_engine->play2D(yell_sound_src, false);

	for (auto& p : particles) { p.get_blood(c.center.x, c.center.y); }
	c.fall = true;
	c.monster_jump_speed = 0.0f; 
	c.is_dead = true;
	c.transparent = true;
}

void box_open(circle_t& c) {
	c.is_dead = true;

	int p = int(random_range(0.0f, 2.9f));
	if (p == 0) { // 무기가 도끼일 때
		axe_idx = 0;
		axe_seen = true;
		sword.life = 0;
		for (auto& i : axes) {
			i.life = 1.5f;
		}
		if (sword_seen) sword_seen = false;
		if (king_seen) king_seen = false;
	}
	else if(p == 1) { // 무기가 검일 때
		sword_idx = 0;
		sword_seen = true;
		sword.life = 1.5f;
		for (auto& i : axes) {
			i.life = 0;
		}
		if (axe_seen) axe_seen = false; 
		if (king_seen) king_seen = false;
	}
	else if (p == 2) {
		king_idx = 0;
		king_seen = true;
		for (auto& i : axes) {
			i.life = 0; 
		}
		if (axe_seen) axe_seen = false;
		if (sword_seen) sword_seen = false; 
	}
}


bool is_up_path(circle_t hero) {
	int num = paths[hero_pos].size();
	for (int i = 0; i < num; i++)
	{
		if (hero.center.x <= paths[hero_pos][i].center.x + 1.5f && hero.center.x >= paths[hero_pos][i].center.x - 1.5f)
		{
			if (hero.center.y <= paths[hero_pos][i].center.y + 1.5f && hero.center.y >= paths[hero_pos][i].center.y + 1.4f)
				return true;
		}
	}
	return false;
}

void update()
{
	if (hero.center.x > paths[hero_pos + 1][0].center.x - 1.5f)
	{
		hero_pos++;
	}

	if (hero.center.x > monsters[monster_pos].center.x + hero.radius / 2.0f + monsters[monster_pos].radius / 2.0f)
		monster_pos++;

	if (hero.center.x > boxes[box_pos].center.x + hero.radius / 2.0f + boxes[box_pos].radius / 2.0f)
		box_pos++;
	
	// update projection matrix
	cam.aspect_ratio = window_size.x / float(window_size.y);
	cam.projection_matrix = mat4::perspective(cam.fovy, cam.aspect_ratio, cam.dNear, cam.dFar);
	cam.view_matrix = mat4::look_at(cam.eye, cam.at, cam.up);

	// build the model matrix for oscillating scale
	
	t = float(glfwGetTime());
	// update uniform variables in vertex/fragment shaders

	glUseProgram(program);
	if (vertex_buffer)	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);

	GLint uloc;
	uloc = glGetUniformLocation(program, "view_matrix");		if (uloc > -1) glUniformMatrix4fv(uloc, 1, GL_TRUE, cam.view_matrix);
	uloc = glGetUniformLocation(program, "projection_matrix");	if (uloc > -1) glUniformMatrix4fv(uloc, 1, GL_TRUE, cam.projection_matrix);
	
	a = abs(sin(float(glfwGetTime()) * 2.5f));
}

void render()
{
	// clear screen (with background color) and clear depth buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// notify GL that we use our own program and buffers
	glUseProgram(program);
	if (vertex_buffer)	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);

	// bind vertex attributes to your shader program
	cg_bind_vertex_attributes(program);

	// update per-circle uniforms
	GLint uloc;



	// 피 파티클 렌더링
	b_solid_color = true;
	for (auto& p : particles) { // 피
		p.update();
		//printf("%f %f %f\n", p.color.x, p.color.y, p.color.z);
		// update per-circle uniforms
		//GLint uloc;
		uloc = glGetUniformLocation(program, "b_solid_color");      if (uloc > -1) glUniform1i(uloc, b_solid_color);   // pointer version
		uloc = glGetUniformLocation(program, "solid_color");      if (uloc > -1) glUniform4fv(uloc, 1, p.color);   // pointer version
		uloc = glGetUniformLocation(program, "model_matrix");      if (uloc > -1) glUniformMatrix4fv(uloc, 1, GL_TRUE, p.model_matrix);

		// per-circle draw calls
		if(p.life > 0) glDrawArrays(GL_TRIANGLES, 0, 6);
	}
	b_solid_color = false;




	// 도끼 렌더링
	for (int i = 0; i < item_num; i++) { 
			
		if (axes[i].is_throwing) { // 던지는 중
			axes[i].rotate(t);
			axes[i].center += vec2(delta * 15, 0);
			axes[i].life -= 0.5f * delta;
		}
		else { // 들고 있는 중
			axes[i].update(t);
			axes[i].center = hero.center + vec2(0.55f, 0.2f);
		}

		uloc = glGetUniformLocation(program, "b_solid_color");      if (uloc > -1) glUniform1i(uloc, b_solid_color);   // pointer version
		uloc = glGetUniformLocation(program, "model_matrix");		if (uloc > -1) glUniformMatrix4fv(uloc, 1, GL_TRUE, axes[i].model_matrix);

		// setup texture
		glActiveTexture(GL_TEXTURE0);                        // select the texture slot to bind
		glBindTexture(GL_TEXTURE_2D, texture[7]);
		glUniform1i(glGetUniformLocation(program, "TEX"), 0);    // GL_TEXTURE0


		// per-circle draw calls
		if(axes[i].life > 0) glDrawArrays(GL_TRIANGLES, 0, 6); // NUM_TESS = N
		else {
			axes[i].is_throwing = false;
		}
	}





	// 검 렌더링
	sword.center = hero.center + vec2(1.2f, 0.05f);
	if (sword.is_throwing) {
		sword.center += vec2((t - sword_theta) * 3, 0);
		sword.update_sword();
		//sword.rotate((t - sword_theta) * 0.2f);
		sword.life -= 5 * delta;
	}
	else sword.update_sword();
	
	uloc = glGetUniformLocation(program, "b_solid_color");      if (uloc > -1) glUniform1i(uloc, b_solid_color);   // pointer version
	uloc = glGetUniformLocation(program, "model_matrix");		if (uloc > -1) glUniformMatrix4fv(uloc, 1, GL_TRUE, sword.model_matrix);

	// setup texture
	glActiveTexture(GL_TEXTURE0);                        // select the texture slot to bind
	glBindTexture(GL_TEXTURE_2D, texture[8]);
	glUniform1i(glGetUniformLocation(program, "TEX"), 0);    // GL_TEXTURE0

	// per-circle draw calls
	if (sword_seen) glDrawArrays(GL_TRIANGLES, 0, 6); // NUM_TESS = N
	if (sword.life < 0) sword.is_throwing = false; 


	// 슈퍼모드 렌더링
	if (king_seen) {
		king.center = hero.center;
		king.update(t);

		uloc = glGetUniformLocation(program, "b_solid_color");      if (uloc > -1) glUniform1i(uloc, b_solid_color);   // pointer version
		uloc = glGetUniformLocation(program, "model_matrix");		if (uloc > -1) glUniformMatrix4fv(uloc, 1, GL_TRUE, king.model_matrix);

		// setup texture
		glActiveTexture(GL_TEXTURE0);                        // select the texture slot to bind
		glBindTexture(GL_TEXTURE_2D, texture[8]);
		glUniform1i(glGetUniformLocation(program, "TEX"), 0);    // GL_TEXTURE0

		// per-circle draw calls
		//glDrawArrays(GL_TRIANGLES, 0, 6); // NUM_TESS = N
	}

	


	// 주인공 렌더링
	hero.update(t);
	//********************************************* 대쉬
	if (is_dash)
	{
		if (how_dash >= dash_distance) //대쉬 거리만큼 다 갔을 때
		{
			how_dash = 0.0f;
			is_dash = false;
		}
		else {
			hero.center.x += delta * 20;
			how_dash += delta * 20;
			if (how_dash > hero.radius * 2.0f)
			{
				is_camera = true;
			}

			if (is_collision(hero, paths[hero_pos + 1][0]))
			{
				//hero_pos++;
				flag++;
				if (flag == 1) {
					how_camera = dash_distance - how_dash;
				}
				hero.center.x -= delta * 20;
			}
		}
	}
	//********************************************* 대쉬
	else { // 대쉬하는 도중에는 점프 안함

		if (is_up)
		{
			hero.center.y += delta * (7 - jump_speed);
			jump_distance -= delta * (7 - jump_speed);
			jump_speed += 0.006f * delta;
			if (jump_distance <= 0)
			{
				jump_distance = 2 * hero.radius;
				is_up = !is_up;
				jump_speed = 0.0f;
			}
		}
		else
		{
			hero.center.y -= delta * 7;
			//jump_distance -= delta * 7;
			if (is_up_path(hero))
			{
				jump_distance = 2.0f;
				is_up = true;
				hero.center.y = paths[hero_pos][0].center.y + 1.5f;
				camera_up = true;
				camera_y_pos = hero.center.y;
			}
			if (is_down_collision(hero, monsters[monster_pos]))
			{
				jump_distance = 2.0f;
				is_up = true;
				hero.center.y = monsters[monster_pos].center.y + hero.radius / 2.0f + monsters[monster_pos].radius / 2.0f;
			}
		}
		if (abs(hero.center.x - cam.eye.x) >= 1.0f)
		{
			if (hero.center.x <= cam.eye.x)
			{
				cam.eye.x -= delta * 3;
				cam.at.x -= delta * 3;
			}
			else
			{
				cam.eye.x += delta * 3;
				cam.at.x += delta * 3;
			}
		}
	}
	//****************************************************히어로 몬스터 충돌 
	if (is_collision(hero, monsters[monster_pos]))
	{
		go_back = true;
		how_camera += (dash_distance - how_dash);
		is_dash = false;
		how_dash = 0.0f;
	}
	if (go_back)
	{
		hero.center.x -= delta * 7;
		back_distance -= delta * 7;
		if (back_distance <= 0.0f)
		{
			go_back = false;
			back_distance = 1.0f;
		}
	}
	if (is_up_collision(hero, monsters[monster_pos]) && !king_seen)
	{
		die(hero);
	}


	//****************************************************히어로 몬스터 충돌 

	if (is_collision(hero, boxes[box_pos]) || is_down_collision(hero, boxes[box_pos])) {
		box_open(boxes[box_pos]);
		box_pos++;
	}
	//********************************************* 카메라 이동
	if (is_camera)
	{

		if (how_camera >= dash_distance)
		{

			how_camera = 0.0f;
			is_camera = false;

			is_dash = false;
			how_dash = 0.0f;
			flag = 0;
		}
		else {
			//printf("%f\n", how_camera);
			cam.eye.x += delta * 20;
			cam.at.x += delta * 20;
			how_camera += delta * 20;
		}
	}
	if (camera_up)
	{
		if (camera_y_pos > cam.eye.y)
		{
			cam.eye.y += delta * 2;
			cam.at.y += delta * 2;
			if (camera_y_pos <= cam.eye.y)
			{
				cam.eye.y = camera_y_pos;
				cam.at.y = camera_y_pos;
				camera_up = false;
			}
		}
		else {
			cam.eye.y -= delta * 2;
			cam.at.y -= delta * 2;
			if (camera_y_pos >= cam.eye.y)
			{
				cam.eye.y = camera_y_pos;
				cam.at.y = camera_y_pos;
				camera_up = false;
			}
		}
	}
	//********************************************* 카메라 이동

	//*******************************************  몬스터 점프
	int len = monsters.size();
	for (int i = 0; i < 5; i++)
	{
		if (monster_pos + i < len)
		{
			if (sword_seen)
			{
				if (is_collision(sword, monsters[monster_pos + i]))
					monster_die(monsters[monster_pos + i]);
			}
			else if (axe_seen)
			{
				for (int j = 0; j < item_num; j++) {
					if (axes[j].is_throwing) {
						if (is_collision(axes[j], monsters[monster_pos + i]))
							monster_die(monsters[monster_pos + i]);
					}
				}

			}
			else if (king_seen)
			{
				if (is_collision(sword, monsters[monster_pos + i]) || is_up_collision(sword, monsters[monster_pos + i]) || is_down_collision(sword, monsters[monster_pos + i]))
					monster_die(monsters[monster_pos + i]);
			}
		}
	}
	for (int i = -3; i < 5; i++)
	{
		if (monster_pos + i >= 0 && monster_pos + i < len && !monsters[monster_pos + i].fall)
		{
			if (monsters[monster_pos + i].monster_is_up)
			{
				monsters[monster_pos + i].center.y += delta * (4 - monsters[monster_pos + i].monster_jump_speed);
				monsters[monster_pos + i].monster_jump_distance -= delta * (4 - monsters[monster_pos + i].monster_jump_speed);
				monsters[monster_pos + i].monster_jump_speed += 0.002f * delta;
				if (monsters[monster_pos + i].monster_jump_distance <= 0)
				{
					monsters[monster_pos + i].monster_jump_distance = 2.5f;
					monsters[monster_pos + i].monster_is_up = !monsters[monster_pos + i].monster_is_up;
					monsters[monster_pos + i].monster_jump_speed = 0.0f;
				}
			}
			else
			{
				monsters[monster_pos + i].center.y -= (delta * (4));
				monsters[monster_pos + i].monster_jump_distance -= delta * (4);

				if (monsters[monster_pos + i].monster_jump_distance <= 0)
				{
					monsters[monster_pos + i].monster_jump_distance = 2.5f;
					monsters[monster_pos + i].monster_is_up = !monsters[monster_pos + i].monster_is_up;
					monsters[monster_pos + i].monster_jump_speed = 0.0f;
				}
			}
		}
	}
	//*******************************************  몬스터 점프

	
	uloc = glGetUniformLocation(program, "b_solid_color");      if (uloc > -1) glUniform1i(uloc, b_solid_color);   // pointer version
	uloc = glGetUniformLocation(program, "model_matrix");		if (uloc > -1) glUniformMatrix4fv(uloc, 1, GL_TRUE, hero.model_matrix);

	// setup texture
	glActiveTexture(GL_TEXTURE0);                        // select the texture slot to bind
	if (game_state == 0) {
		if (hero.is_dead) glBindTexture(GL_TEXTURE_2D, texture[3]);
		else {
			if(king_seen) glBindTexture(GL_TEXTURE_2D, texture[18]);
			else glBindTexture(GL_TEXTURE_2D, texture[0]);
		}
	}
	else {
		if (hero.is_dead) glBindTexture(GL_TEXTURE_2D, texture[15]);
		else if (!is_dash) {
			if(axe_seen) glBindTexture(GL_TEXTURE_2D, texture[12]);
			else if(sword_seen) glBindTexture(GL_TEXTURE_2D, texture[11]);
			else if(king_seen) glBindTexture(GL_TEXTURE_2D, texture[13]);
			else glBindTexture(GL_TEXTURE_2D, texture[10]);
		}
		else if (is_dash) {
			if (axe_seen) glBindTexture(GL_TEXTURE_2D, texture[12]);
			else if (sword_seen) glBindTexture(GL_TEXTURE_2D, texture[11]);
			else if (king_seen) glBindTexture(GL_TEXTURE_2D, texture[13]);
			else glBindTexture(GL_TEXTURE_2D, texture[9]);
		}
	}
	
	glUniform1i(glGetUniformLocation(program, "TEX"), 0);    // GL_TEXTURE0

	// per-circle draw calls
	glDrawArrays(GL_TRIANGLES, 0, 6); // NUM_TESS = N



	




	
	// 맵 렌더링 
	for (auto& path : paths)
	{
		for (auto& c : path) {
			// per-circle update
			c.update(t);

			// update per-circle uniforms
			GLint uloc;
			uloc = glGetUniformLocation(program, "b_solid_color");      if (uloc > -1) glUniform1i(uloc, b_solid_color);   // pointer version
			uloc = glGetUniformLocation(program, "model_matrix");      if (uloc > -1) glUniformMatrix4fv(uloc, 1, GL_TRUE, c.model_matrix);

			// setup texture
			glActiveTexture(GL_TEXTURE0);                        // select the texture slot to bind
			glBindTexture(GL_TEXTURE_2D, texture[1]);
			glUniform1i(glGetUniformLocation(program, "TEX"), 0);    // GL_TEXTURE0

			// per-circle draw calls
			glDrawArrays(GL_TRIANGLES, 0, 6); 
		}
	}


	// 몬스터 렌더링
	for (auto& c : monsters) {
		//c.jump(delta * 3);
		if (!c.is_gone) {
			if (c.fall)
			{
				c.center.x += delta * 4;
				c.center.y -= (delta * 0.5f + c.monster_jump_speed);
				c.monster_jump_speed+=0.03f * delta;
			}
			if (c.center.y <= -10.0f)
				c.is_gone = true;
			c.update(t);

			// update per-circle uniforms
			GLint uloc;
			uloc = glGetUniformLocation(program, "b_solid_color");      if (uloc > -1) glUniform1i(uloc, b_solid_color);   // pointer version
			uloc = glGetUniformLocation(program, "model_matrix");      if (uloc > -1) glUniformMatrix4fv(uloc, 1, GL_TRUE, c.model_matrix);

			// setup texture
			glActiveTexture(GL_TEXTURE0);                        // select the texture slot to bind
			if (game_state == 0) {
				if (!c.is_dead) glBindTexture(GL_TEXTURE_2D, texture[2]);
				else glBindTexture(GL_TEXTURE_2D, texture[3]);
			}
			else {
				if (c.is_dead) glBindTexture(GL_TEXTURE_2D, texture[16]);
				else glBindTexture(GL_TEXTURE_2D, texture[14]);
			}
			glUniform1i(glGetUniformLocation(program, "TEX"), 0);    // GL_TEXTURE0

			// per-circle draw calls
			glDrawArrays(GL_TRIANGLES, 0, 6);

		}
	}


	// 꼬리 파티클 렌더링
	b_solid_color = true;
	for (auto& p : tail_particles) { // 꼬리
		p.tail_update(hero.center.x, hero.center.y);
		// update per-circle uniforms
		GLint uloc;
		uloc = glGetUniformLocation(program, "b_solid_color");      if (uloc > -1) glUniform1i(uloc, b_solid_color);   // pointer version
		uloc = glGetUniformLocation(program, "solid_color");      if (uloc > -1) glUniform4fv(uloc, 1, p.color);   // pointer version
		uloc = glGetUniformLocation(program, "model_matrix");      if (uloc > -1) glUniformMatrix4fv(uloc, 1, GL_TRUE, p.model_matrix);

		// per-circle draw calls
		glDrawArrays(GL_TRIANGLES, 0, 6);
	}
	b_solid_color = false; 

	// 구름 렌더링 
	for (auto& c : clouds) {
		// per-circle update
		c.update(t);

		// update per-circle uniforms
		GLint uloc;
		uloc = glGetUniformLocation(program, "b_solid_color");      if (uloc > -1) glUniform1i(uloc, b_solid_color);   // pointer version
		uloc = glGetUniformLocation(program, "model_matrix");      if (uloc > -1) glUniformMatrix4fv(uloc, 1, GL_TRUE, c.model_matrix);

		// setup texture
		glActiveTexture(GL_TEXTURE0);                        // select the texture slot to bind
		glBindTexture(GL_TEXTURE_2D, texture[4]);
		glUniform1i(glGetUniformLocation(program, "TEX"), 0);    // GL_TEXTURE0

		// per-circle draw calls
		glDrawArrays(GL_TRIANGLES, 0, 6);
	}


	// 보물상자 렌더링 
	for (auto& c : boxes) {
		// per-circle update
		c.update(t);

		// update per-circle uniforms
		GLint uloc;
		uloc = glGetUniformLocation(program, "b_solid_color");      if (uloc > -1) glUniform1i(uloc, b_solid_color);   // pointer version
		uloc = glGetUniformLocation(program, "model_matrix");      if (uloc > -1) glUniformMatrix4fv(uloc, 1, GL_TRUE, c.model_matrix);

		// setup texture
		glActiveTexture(GL_TEXTURE0);                        // select the texture slot to bind
		if (game_state == 0) {
			if (c.is_dead) glBindTexture(GL_TEXTURE_2D, texture[5]);
			else glBindTexture(GL_TEXTURE_2D, texture[6]);
		}
		else {
			if(!c.is_dead) glBindTexture(GL_TEXTURE_2D, texture[17]);
		}
		glUniform1i(glGetUniformLocation(program, "TEX"), 0);    // GL_TEXTURE0

		// per-circle draw calls
		glDrawArrays(GL_TRIANGLES, 0, 6);
	}



	if (hero.center.y < paths[hero_pos][0].center.y - 3.0f) hero.is_dead = true; 
	if (game_over_flag && hero.is_dead) {
		game_playing = 2;
		game_over_flag = false; 
		// play sound file
		gameover_engine->play2D(gameover_sound_src, false);
		
	}
	if (game_clear_flag && hero_pos == 51) {
		game_playing = 3;
		game_clear_flag = false; 
		gameclear_engine->play2D(gameclear_sound_src, false);
	}

	// 텍스트 렌더링
	GLuint x = GLuint(window_size.x / 2.0f);
	GLuint y = GLuint(window_size.y / 2.0f);
	float s = 1.2f * window_size.x / 720.0f;
	float title_s = 2.0f * s;

	if (game_playing == 0) { // menu
		render_text("Help - H", 80, 125, s * 0.7f , vec4(0, 0, 0, 1));
		render_text("Quit - esc", 80, 175, s * 0.7f , vec4(0, 0, 0, 1));
		render_text("press Enter", 80, 225, s * 0.7f, vec4(0, 0, 0, 1));
		if (game_state == 0) { // easy mode
			render_text("NeMo", x - 200, y - 250, title_s, vec4(0, 0, 0, 1));
			render_text("Nemo stage", x - 220, y + 300, s, vec4(0, 1, 0, 1));
			render_text("Avengers stage", x - 320, y + 400, s, vec4(0, 0, 0, 1));
		}
		else { // hard mode
			render_text("Avengers", x - 300, y - 250, title_s, vec4(0, 0, 0, 1));
			render_text("Nemo stage", x - 220, y + 300, s, vec4(0, 0, 0, 1));
			render_text("Avengers stage", x - 320, y + 400, s, vec4(0, 1, 0, 1));
		}
	}
	else if(game_playing == 1){ // playing
		render_text(std::to_string(hero_pos), 100, 125, s, vec4(0, 0, 0, 1));
	}
	else if (game_playing == 3) { // clear
		render_text("Help - H", 80, 125, s * 0.7f, vec4(0, 0, 0, 1));
		render_text("Quit - esc", 80, 175, s * 0.7f, vec4(0, 0, 0, 1));
		render_text("press Enter", 80, 225, s * 0.7f, vec4(0, 0, 0, 1));
		render_text("CLEAR!!", x - 300, y - 250, s * 1.5f, vec4(0, 0, 0, 1));
		if (game_menu == 0) { // restart
			render_text("Restart", x - 210, y + 300, s, vec4(0, 1, 0, 1));
			render_text("  Menu ", x - 200, y + 400, s, vec4(0, 0, 0, 1));
		}
		else if (game_menu == 1) { // menu
			render_text("Restart", x - 210, y + 300, s, vec4(0, 0, 0, 1));
			render_text("  Menu ", x - 200, y + 400, s, vec4(0, 1, 0, 1));
		}
	}
	else { // game over
		render_text("Help - H", 80, 125, s * 0.7f, vec4(0, 0, 0, 1));
		render_text("Quit - esc", 80, 175, s * 0.7f, vec4(0, 0, 0, 1));
		render_text("press Enter", 80, 225, s * 0.7f, vec4(0, 0, 0, 1));
		render_text("GAME OVER", x - 300, y - 250, s * 1.5f, vec4(0, 0, 0, 1));
		if (game_menu == 0) { // restart
			render_text("Restart", x - 210, y + 300, s, vec4(0, 1, 0, 1));
			render_text("  Menu ", x - 200, y + 400, s, vec4(0, 0, 0, 1));
		}
		else if (game_menu == 1) { // menu
			render_text("Restart", x - 210, y + 300, s, vec4(0, 0, 0, 1));
			render_text("  Menu ", x - 200, y + 400, s, vec4(0, 1, 0, 1));
		}
	}

	
	
	// swap front and back buffers, and display to screen
	glfwSwapBuffers(window);
}

void reshape(GLFWwindow* window, int width, int height)
{
	// set current viewport in pixels (win_x, win_y, win_width, win_height)
	// viewport: the window area that are affected by rendering 
	window_size = ivec2(width, height);
	glViewport(0, 0, width, height);
}

void print_help()
{
	printf("[help]\n");
	printf("- press ESC or 'q' to terminate the program\n");
	printf("- press F1 or 'h' to see help\n");
	printf("- press 'd' to toggle between solid color and texture coordinates\n");
	printf("- press 'i' to toggle between index buffering and simple vertex buffering\n");
	printf("- press 'w' to toggle wireframe\n");
	printf("\n");
}

std::vector<vertex> create_circle_vertices()
{
	std::vector<vertex> v; // origin - position, normal, texcoord

	v.push_back({ vec3(0.5f, 0.5f, 0), vec3(0.5f, 0.5f, 0), vec2(1, 1) });
	v.push_back({ vec3(0.5f, -0.5f, 0), vec3(0.5f, -0.5f, 0), vec2(1, 0) });
	v.push_back({ vec3(-0.5f, -0.5f, 0), vec3(-0.5f, -0.5f, 0), vec2(0, 0) });
	v.push_back({ vec3(-0.5f, 0.5f, 0), vec3(-0.5f, 0.5f, 0), vec2(0, 1) });

	return v;
}

void update_vertex_buffer(const std::vector<vertex>& vertices)
{
	// clear and create new buffers
	if (vertex_buffer)	glDeleteBuffers(1, &vertex_buffer);	vertex_buffer = 0;

	// check exceptions
	if (vertices.empty()) { printf("[error] vertices is empty.\n"); return; }

	std::vector<vertex> v; // triangle vertices
	v.push_back(vertices[0]);
	v.push_back(vertices[3]);
	v.push_back(vertices[1]);

	v.push_back(vertices[3]);
	v.push_back(vertices[2]);
	v.push_back(vertices[1]);



	// generation of vertex buffer: use triangle_vertices instead of vertices
	glGenBuffers(1, &vertex_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertex) * v.size(), &v[0], GL_STATIC_DRAW);
}


void loadTexture(int idx, const char* path) {
	// load and flip an image
	int width, height, comp;
	unsigned char* pimage0 = stbi_load(path, &width, &height, &comp, 4);
	int stride0 = width * comp, stride1 = (stride0 + 4) & (~4);   // 4-byte aligned stride
	unsigned char* pimage = (unsigned char*)malloc(sizeof(unsigned char) * stride1 * height);
	for (int y = 0; y < height; y++) memcpy(pimage + (height - 1 - y) * stride1, pimage0 + y * stride0, stride0); // vertical flip
	stbi_image_free(pimage0); // release the original image

	// create a particle texture
	//glGenTextures(1, &TEX);
	glBindTexture(GL_TEXTURE_2D, texture[idx]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pimage);

	// configure texture parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	// release the new image
	free(pimage);
}

void variable_init()
{
	// hero moving variable
	is_dash = false;
	how_dash = 0.0f;
	dash_distance = 4 * hero.radius;
	jump_distance = 2 * hero.radius;
	is_jump = true;
	is_up = true;
	is_side_path = false;
	camera_up = false;
	camera_y_pos = 0.0f;
	hero_pos = 0;
	box_pos = 0;
	monster_pos = 0;
	back_distance = 1.0f;
	go_back = false;

	//camera moving variable
	is_camera = false;
	how_camera = 0.0f;
	jump_speed = 0.0f;
	flag = 0;

	// path distance
	len_path = 0.0f;

	// item moving variables
	axe_idx = 0;
	item_num = 8;
	axe_seen = false;
	sword_idx = 0;
	sword_theta = 0.0f;
	sword_seen = false;
	king_seen = false;
	king_idx = 0;

	hero.center = vec2(0, 0);
	hero.is_dead = false;
	hero.up = true; 
	sword.is_throwing = false;
	king.is_throwing = false;

	monsters.clear();
	clouds.clear();
	boxes.clear();
	axes.clear();

	for (uint i = 0; i < paths.size(); i++) {
		paths[i].clear();
	}
	paths.clear();

	cam.eye = vec3(0, 0, 13);
	cam.at = vec3(0, 0, 0);
	cam.view_matrix = mat4::look_at(cam.eye, cam.at, cam.up);

	game_over_flag = true;
	game_clear_flag = true; 
}

bool user_init()
{


	// log hotkeys
	print_help();

	// init GL states
	glLineWidth(1.0f);
	//glClearColor( 39/255.0f, 40/255.0f, 34/255.0f, 1.0f );	// set clear color
	if (game_state == 0) glClearColor(107 / 255.0f, 198 / 255.0f, 255 / 255.0f, 1.0f);	// set clear color
	else glClearColor(213 / 256.0f, 84 / 256.0f, 48 / 256.0f, 1.0f);
	glEnable(GL_CULL_FACE);								// turn on backface culling
	glEnable(GL_DEPTH_TEST);								// turn on depth tests

	// define the position of four corner vertices
	unit_circle_vertices = std::move(create_circle_vertices());

	// create vertex buffer; called again when index buffering mode is toggled
	update_vertex_buffer(unit_circle_vertices);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_TEXTURE_2D);
	glActiveTexture(GL_TEXTURE0);

	glGenTextures(19, texture);
	loadTexture(0, hero_texture_path);
	loadTexture(1, map_texture_path);
	loadTexture(2, purple_texture_path);
	loadTexture(3, die_texture_path);
	loadTexture(4, cloud_texture_path);
	loadTexture(5, open_texture_path);
	loadTexture(6, closed_texture_path);
	loadTexture(7, axe_texture_path);
	loadTexture(8, sword_texture_path);
	loadTexture(9, dash_texture_path);
	loadTexture(10, stand_texture_path);
	loadTexture(11, captain_texture_path);
	loadTexture(12, thor_texture_path);
	loadTexture(13, hurk_texture_path);
	loadTexture(14, thanos_texture_path);
	loadTexture(15, dead_texture_path);
	loadTexture(16, td_texture_path);
	loadTexture(17, logo_texture_path);
	loadTexture(18, he_texture_path);

	variable_init();

	init_path();


	// initialize particles
	constexpr int max_particle = 100;
	tail_particles.resize(max_particle);
	for (auto& p : tail_particles) { p.get_tail(0, 0); }

	axes.resize(item_num);
	for (auto& i : axes) { i.radius = 1.0f; }
	sword.radius = 1.5f;

	text_init();
	
	
	// create engine
	if (!bgm_engine) bgm_engine = irrklang::createIrrKlangDevice();

	if (!bgm_engine) return false;
	//add sound source from the sound file
	bgm_engine->removeAllSoundSources();
	if(game_state == 0) bgm_sound_src = bgm_engine->addSoundSourceFromFile(nbgm_path);
	else bgm_sound_src = bgm_engine->addSoundSourceFromFile(abgm_path);

	bgm_engine->play2D(bgm_sound_src, true);

	return true;
}


void keyboard(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (action == GLFW_PRESS)
	{
		if (key == GLFW_KEY_ESCAPE || key == GLFW_KEY_Q)	glfwSetWindowShouldClose(window, GL_TRUE);
		else if (key == GLFW_KEY_H || key == GLFW_KEY_F1)	print_help();
		else if (key == GLFW_KEY_SPACE)
		{
			if (game_playing == 1) {
				dash_engine->play2D(dash_sound_src, false);

				if (is_dash == false)
					is_dash = true;

				// 도끼 던질 수 있는 상태면
				if (axe_seen == true) {
					// 던지는 중이라고 표시하고 아이템 개수 줄임
					axes[axe_idx].is_throwing = true;
					axe_idx++;
					// 5개 다 던지면 아이템 숨김
					if (axe_idx == item_num) {
						axe_seen = false;
						axe_idx = 0;
					}
				}
				// 검 찌를 수 있는 상태면
				if (sword_seen == true) {
					sword.is_throwing = true;
					sword.life = 1.0f;
					sword_idx++;
					sword_theta = float(glfwGetTime());
					if (sword_idx == item_num) {
						sword_seen = false;
						sword_idx = 0;
					}
				}
				// king 모드
				if (king_seen == true) {
					king.is_throwing = true;
					king_idx++;
					if (king_idx == item_num) {
						king_seen = false;
						king_idx = 0;
					}
				}
			}

		}
		else if (key == GLFW_KEY_UP) {
			if (game_playing == 0) { // 메인 메뉴
				game_state = (game_state + 1) % 2;
				user_init();
			}
			else if (game_playing == 2 || game_playing == 3) { // 게임 오버/클리어
				game_menu = (game_menu + 1) % 2; 
			}
		}
		else if (key == GLFW_KEY_DOWN) { 
			if (game_playing == 0) { // 메인 메뉴
				game_state = (game_state + 2 - 1) % 2; 
				user_init();
			}
			else if (game_playing == 2 || game_playing == 3) { // 게임 오버/클리어
				game_menu = (game_menu + 2 - 1) % 2; 
			}
		}
		else if (key == GLFW_KEY_ENTER) {
			if (game_playing == 0) { // 메인 메뉴
				game_playing = 1;
			}
			else if (game_playing == 2 || game_playing == 3) { // 게임 오버/클리어
				if (game_menu == 0) { // restart
					game_playing = 1;
					user_init();
				}
				else if (game_menu == 1) { // menu
					game_menu = 0; 
					game_playing = 0; // menu 이동
					user_init();
				}
			}
		}
	}
}

void mouse(GLFWwindow* window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
	{
		dvec2 pos; glfwGetCursorPos(window, &pos.x, &pos.y);
		printf("> Left mouse button pressed at (%d, %d)\n", int(pos.x), int(pos.y));
	}
}

void motion(GLFWwindow* window, double x, double y)
{
}

void user_finalize()
{
	// close the engine
	gameclear_engine->drop();
	dash_engine->drop();
	gameover_engine->drop();
	yell_engine->drop();
	bgm_engine->drop();

}

int main(int argc, char* argv[])
{
	// initialization
	if (!glfwInit()) { printf("[error] failed in glfwInit()\n"); return 1; }

	// create window and initialize OpenGL extensions
	if (!(window = cg_create_window(window_name, window_size.x, window_size.y))) { glfwTerminate(); return 1; }
	if (!cg_init_extensions(window)) { glfwTerminate(); return 1; }	// init OpenGL extensions

	// initializations and validations of GLSL program
	if (!(program = cg_create_program(vert_shader_path, frag_shader_path))) { glfwTerminate(); return 1; }	// create and compile shaders/program
	if (!user_init()) { printf("Failed to user_init()\n"); glfwTerminate(); return 1; }					// user initialization

	// register event callbacks
	glfwSetWindowSizeCallback(window, reshape);	// callback for window resizing events
	glfwSetKeyCallback(window, keyboard);			// callback for keyboard events
	glfwSetMouseButtonCallback(window, mouse);	// callback for mouse click inputs
	glfwSetCursorPosCallback(window, motion);		// callback for mouse movements


	if (!gameover_engine) gameover_engine = irrklang::createIrrKlangDevice();
	if (!gameclear_engine) gameclear_engine = irrklang::createIrrKlangDevice();
	if (!yell_engine) yell_engine = irrklang::createIrrKlangDevice();
	if (!dash_engine) dash_engine = irrklang::createIrrKlangDevice();

	gameover_sound_src = gameover_engine->addSoundSourceFromFile(gameover_sound_path);
	gameclear_sound_src = gameclear_engine->addSoundSourceFromFile(gameclear_sound_path);
	dash_sound_src = dash_engine->addSoundSourceFromFile(dash_sound_path);
	yell_sound_src = yell_engine->addSoundSourceFromFile(yell_sound_path);

	
	float t0 = float(glfwGetTime());
	// enters rendering/event loop
	for (frame = 0; !glfwWindowShouldClose(window); frame++)
	{
		float t1 = float(glfwGetTime());
		delta = t1 - t0; 
		t0 = t1;
		glfwPollEvents();	// polling and processing of events
		update();			// per-frame update
		render();			// per-frame render
	}

	// normal termination
	user_finalize();
	cg_destroy_window(window);

	return 0;
}
