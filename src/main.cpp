#include <SDL.h>
#include <GL/glew.h>
#include <SDL_opengl.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <limits.h>
#include <string.h>
#include <string>
#include <stdio.h>
#include <vector>
#include <string>
#include <map>

#include "image.h"

#ifdef BIG_SCREEN
int screen_width = 1280, screen_height = 720;
#else
int screen_width = 1366, screen_height = 768;
#endif

#define PI 3.14159265358979
#define PI2 (PI*2)

int quit = 0;

SDL_Window * window = NULL;
SDL_GLContext window_context;

SDL_Event event;

int init_opengl()
{
  glewInit();

  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glEnable(GL_BLEND);
  glEnable(GL_TEXTURE_2D);

  glClearColor(0, 0, 0, 0);

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0, screen_width, screen_height, 0, -1, 1);

  glMatrixMode(GL_MODELVIEW);

  if(glGetError() != GL_NO_ERROR)
    return 1;
  
  return 0;
}

int init(int resizeable)
{
  if(SDL_Init(SDL_INIT_EVERYTHING) == -1)
    return 1;
  
  if(resizeable)
    window = SDL_CreateWindow("SDSG2", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, screen_width, screen_height, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
  else
    window = SDL_CreateWindow("SDSG2", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, screen_width, screen_height, SDL_WINDOW_OPENGL);

  if(window == NULL)
    return 1;

  /* SDL_SetWindowSize(window, screen_width, screen_height); */
  window_context = SDL_GL_CreateContext(window);
  
  if(init_opengl())
    return 1;

  return 0;
}

void update_time(int * ltime, int * ntime, int * dtime)
{
  *ltime = *ntime;

  *ntime = SDL_GetTicks();
  *dtime = *ntime - *ltime;
}

void resize_window(int w, int h)
{
  screen_width = w;
  screen_height = h;

  SDL_SetWindowSize(window, w, h);

  glViewport(0, 0, screen_width, screen_height);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0, screen_width, screen_height, 0, -1, 1);

  glMatrixMode(GL_MODELVIEW);
}

enum
  {
    TYPE_NORMAL,
    TYPE_CIRCLE
  };

class Firework
{
public:
  double x, y;
  double xv, yv;
  int time, type;
  float r, g, b;
  double size;

  bool dead;
  int preset;

  Firework(double x, double y, double xv, double yv, float r, float g, float b, int type, double size, int preset, int time)
  {
    this->x = x;
    this->y = y;
    this->xv = xv;
    this->yv = yv;

    this->r = r;
    this->g = g;
    this->b = b;

    this->type = type;
    this->size = size;

    this->dead = false;
    this->preset = preset;
    this->time = time;
  }

  Firework(double x, double y, double xv, double yv, float r, float g, float b, int type, double size, int preset) :
    Firework(x, y, xv, yv, r, g, b, type, size, preset, 100)
  {
  };

  Firework(double x, double y, double xv, double yv, float r, float g, float b, int type, double size = 3) :
    Firework(x, y, xv, yv, r, g, b, type, 3, 0, 100)
  {
  };

  void update()
  {
    x += xv;
    y += yv;
    yv += 0.2;
    
    time--;
    if(rand() % 4 == 0)
      time -= 2;

    if(time < 0)
      dead = true;
  }

  void draw()
  {
    glLineWidth(size);
    glColor3f(r, g, b);
    glBegin(GL_LINES);
    glVertex3f(x, y, 0);
    glVertex3f(x + xv * size, y + yv * size, 0);
    glEnd();
  }

  void onDeath(std::vector<Firework> &addlist)
  {
    switch(type)
      {
      case TYPE_NORMAL:
	return;
	
      case TYPE_CIRCLE:
	{
	  int i, max = 10;
	  for(i = 0; i < max; i++)
	    {
	      double pxv = cos((i / (double)max) * PI2),
		pyv = sin((i / (double)max) * PI2);
	      addlist.push_back(Firework(x, y, pxv, pyv - 3, 1, 1, 1, TYPE_NORMAL, 2, 0, 75));
	    }
	}
	return;

      default:
	return;
      }
  }
};

class Scene
{
  std::vector<Firework> fireworks;
  std::vector<Firework> addList;

  std::vector<std::vector<Firework> > presets;
  
public:
  Scene()
  {
  }

  void addFirework(Firework firework)
  {
    fireworks.push_back(firework);
  }
  
  void update()
  {
    for(auto &firework : fireworks)
      firework.update();

    for(unsigned int i = 0; i < fireworks.size(); i++)
      {
	if(fireworks[i].dead)
	  {
	    fireworks[i].onDeath(addList);
	    if(fireworks[i].preset != 0)
	      {
		for(auto &pf : presets[fireworks[i].preset - 1])
		  {
		    Firework tmp = pf;
		    tmp.x += fireworks[i].x;
		    tmp.y += fireworks[i].y;
		    addList.push_back(tmp);
		  }
	      }
	    fireworks.erase(fireworks.begin() + i);
	    i--;
	  }
      }

    for(auto &firework : addList)
      {
	addFirework(firework);
      }
    addList.clear();
  }

  void addPreset()
  {
    std::vector<Firework> list;

    int i, max = 10;
    for(i = 0; i < max; i++)
      {
	double pxv = cos((i / (double)max) * PI2),
	  pyv = sin((i / (double)max) * PI2);
	list.push_back(Firework(0, 0, pxv, pyv - 5, 1, .2, .2, TYPE_CIRCLE, 2, 0, 125));
      }

    presets.push_back(list);
  }

  void addPresetFromImage(const char * filename, Uint32 blank)
  {
    std::vector<Firework> list;

    int i, j;
    SDL_Surface * image = load_image(filename);

    for(i = 0; i < image->w; i++)
      for(j = 0; j < image->h; j++)
	{
	  Uint32 pixel = getpixel(image, i, j) & 0xFFFFFF;

	  if(pixel == blank)
	    continue;

	  int r, g, b;
	  r = (pixel >> 0) & 0xFF;
	  g = (pixel >> 8) & 0xFF;
	  b = (pixel >> 16) & 0xFF;

	  double xv, yv;

	  xv = (i - (image->w / 2)) / 10.;
	  yv = (j - (image->h / 2)) / 10. - 2;

	  list.push_back(Firework(0, 0, xv, yv, r / 255., g / 255., b / 255., TYPE_NORMAL, 1, 0, 75));
	}

    SDL_FreeSurface(image);

    presets.push_back(list);
  }

  void draw()
  {
    for(auto &firework : fireworks)
      firework.draw();
  }
};

int fi = 0;
void scene_program(int time, Scene &scene)
{
  if(time < 1000)
    {
      if((time % 10) == 0)
	{
	  double xv, yv;

	  xv = sin(sin(time / 100.)) * 6;
	  yv = -cos(sin(time / 100.)) * 16;
	  scene.addFirework(Firework(screen_width / 2, screen_height, xv, yv,
				     1, 0, 0, TYPE_CIRCLE, 3, 0, 125));
	}
    }
  if(time == 1000)
    {
      scene.addFirework(Firework(screen_width / 2, screen_height, 0, -17,
				 1, 0, 0, TYPE_NORMAL, 3, 1, 125));

      scene.addFirework(Firework(screen_width / 10, screen_height, 0, -15,
				 .1, 1, .1, TYPE_CIRCLE, 3, 0, 100));

      scene.addFirework(Firework(screen_width / 10 * 9, screen_height, 0, -15,
				 .1, 1, .1, TYPE_CIRCLE, 3, 0, 100));
    }
  int delay = 5, steps = 20;
  if(time > 1100 && time < 1100 + screen_width / (steps / delay))
    {
      if(time % delay == 0)
	{
	  scene.addFirework(Firework(fi * steps, screen_height, 0, -15,
				     .1, 1, .1, TYPE_NORMAL, 3, 2, 100));
	  fi++;
	}
    }
  else
    {
      fi = 0;
    }
  if(time == 1100 + screen_width / (steps / delay) + 40)
    {
      int i;
      for(i = 0; i < 20; i++)
	{
	  scene.addFirework(Firework(screen_width / 2, screen_height, cos(i / 20. * PI) * 10, -sin(i / 20. * PI) * 16,
				     .1, 1, .1, TYPE_NORMAL, 3, 2, 100));
	}
    }
  if(time == 1100 + screen_width / (steps / delay) + 50)
    {
      scene.addFirework(Firework(screen_width / 2, screen_height, 0, -16,
				 .1, 1, .1, TYPE_NORMAL, 3, 3, 100));

      scene.addFirework(Firework(screen_width / 2, screen_height, -3, -15,
				 .1, 1, .1, TYPE_NORMAL, 3, 4, 100));
      scene.addFirework(Firework(screen_width / 2, screen_height, 3, -15,
				 .1, 1, .1, TYPE_NORMAL, 3, 4, 100));
    }
}

int main(int argc, char ** argv)
{
  int resizeable = 0;
  int mouse_x, mouse_y;
  int ltime = 0, ntime = 0, dtime = 0;
  int tick = 0, animation = 0;
  double delay = 0;
  
  Scene scene;

  if(init(resizeable))
    return 1;
  srand(time(NULL));

  scene.addPresetFromImage("image.png", 0x0);
  scene.addPreset();
  scene.addPresetFromImage("image2.png", 0x0);
  scene.addPresetFromImage("image3.png", 0x0);
  
  while(!quit)
    {
      const Uint8 * keystate;
      while(SDL_PollEvent(&event))
	{
	  if(event.type == SDL_QUIT)
	    {
	      quit = 1;
	    }
	  else if(event.type == SDL_KEYDOWN)
	    {
	      SDL_Keycode key = event.key.keysym.sym;

	      if(key == SDLK_F1)
		{
		  animation = !animation;
		  if(animation)
		    tick = 0;
		}
	    }
	  else if(event.type == SDL_WINDOWEVENT)
	    {
	      if(event.window.event == SDL_WINDOWEVENT_RESIZED)
		{
		  resize_window(event.window.data1, event.window.data2);
		}
	    }
	}
      update_time(&ltime, &ntime, &dtime);
      keystate = SDL_GetKeyboardState(NULL);
      keystate = keystate;
      Uint32 mousestate = SDL_GetMouseState(&mouse_x, &mouse_y);
      if(mousestate & SDL_BUTTON(1) && delay < 0)
	{
	  double xv, yv;
	  xv = mouse_x - screen_width / 2;
	  yv = mouse_y - screen_height;
	  xv /= 30;
	  yv /= 30;
	  xv += (rand() % 100) / 100. - 0.5;
	  yv += (rand() % 100) / 100. - 0.5;
	  scene.addFirework(Firework(screen_width / 2, screen_height, xv, yv,
				     (rand() % 256) / 255., (rand() % 256) / 255., (rand() % 256) / 255., TYPE_NORMAL, 3, 2));
	  delay = 1;
	}
      delay -= .05;

      if(animation)
	scene_program(tick, scene);

      scene.update();
      
      glClear(GL_COLOR_BUFFER_BIT);
      glLoadIdentity();

      scene.draw();

      SDL_GL_SwapWindow(window);
      if(animation)
	tick++;
      /* SDL_Delay(50); */
    }

  SDL_DestroyWindow(window);
  SDL_Quit();
  return 0;
}
