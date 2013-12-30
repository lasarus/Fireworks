#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <SDL2/SDL_image.h>

#include "image.h"

Uint32 getpixel(SDL_Surface * surface, int x, int y)
{
  int bpp = surface->format->BytesPerPixel;
  Uint8 * p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;

  switch(bpp)
    {
    case 1:
      return *p;
      break;

    case 2:
      return *(Uint16 *)p;
      break;

    case 3:
      if(SDL_BYTEORDER == SDL_BIG_ENDIAN)
	return p[0] << 16 | p[1] << 8 | p[2];
      else
	return p[0] | p[1] << 8 | p[2] << 16;
      break;

    case 4:
      return *(Uint32 *)p;
      break;

    default:
      return 0;
    }
}

SDL_Surface * load_image(const char * filename)
{
  SDL_Surface * loadedImage = NULL;
  loadedImage = IMG_Load(filename);
  return loadedImage;
}
