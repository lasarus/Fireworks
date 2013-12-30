#ifndef IMAGE_H
#define IMAGE_H

SDL_Surface * load_image(const char * filename);
Uint32 getpixel(SDL_Surface * surface, int x, int y);

#endif
