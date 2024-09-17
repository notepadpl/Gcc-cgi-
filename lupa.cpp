#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <EGL/egl.h>
#include <GLES/gl.h>
#define WIDTH 800
#define HEIGHT 600
/*path to jpeg fille */
#define IMG_PATH "img.jpg"
static int SHAPE_SIZE=1;
//sprite in picture
 SDL_Rect SrcR;
 //sprite in program
  SDL_Rect DestR;
 SDL_Renderer *renderer = NULL;
  SDL_Texture *img = NULL;
int lupa() {
   SrcR.x = 0;
  SrcR.y = 0;
  SrcR.w = SHAPE_SIZE;
  SrcR.h = SHAPE_SIZE;
  DestR.x = 800 / 2 - SHAPE_SIZE /2;
  DestR.y = 600 / 2 - SHAPE_SIZE /2;
  DestR.w = SHAPE_SIZE;
  DestR.h = SHAPE_SIZE;
 }
//load JPEG
int JPEG(){
  // load our image
 img = IMG_LoadTexture(renderer, IMG_PATH);
//SDL_QueryTexture(img, * pixelFormat, NULL, 600 ,800);
//img= SDL_CreateTexture(renderer, 0 ,SDL_TEXTUREACCESS_TARGET, WIDTH, HEIGHT);
SDL_SetRenderTarget(renderer, img);
  // get the width and height of the texture
 // put the location where we want the texture to be drawn into a rectangle
 // I'm also scaling the texture 2x simply by setting the width and height
// SDL_Rect texr; texr.x = WIDTH/2; texr.y = HEIGHT/2; texr.w = w*2; texr.h = h*2;
// return img;
 }
int main (int argc, char *argv[]) {
 // variable declarations
 SDL_Window *win = NULL;

 int w, h; // texture width & height
 Uint32 *pixelFormat;


 // Initialize SDL.
 if (SDL_Init(SDL_INIT_VIDEO) < 0)
   return 1;
  // create the window and renderer
 // note that the renderer is accelerated
 win = SDL_CreateWindow("Image Loading", 100, 100, WIDTH, HEIGHT ,SDL_WINDOW_OPENGL);
 renderer = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);
 JPEG();
 // main loop
 while (1) {
   // event handling
  SDL_Event e;
  if ( SDL_PollEvent(&e) ) {
   if (e.type == SDL_QUIT)
    break;
   else if (e.type == SDL_KEYUP && e.key.keysym.sym == SDLK_ESCAPE)
    break;
  }
    // clear the screen
  SDL_RenderClear(renderer);
  lupa() ;
  // copy the texture to the rendering context
  SDL_RenderCopy(renderer, img, NULL, NULL);
 SDL_RenderCopy(renderer, img, &SrcR, &DestR);
  // this means that everything that we prepared behind the screens is actually shown
  SDL_RenderPresent(renderer);
       glClear(GL_COLOR_BUFFER_BIT);
 SHAPE_SIZE++;
 if(SHAPE_SIZE>3000){
   SHAPE_SIZE=0;
 }
                glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
                    SDL_GL_SwapWindow(win);
// turn on anim
 }
  SDL_DestroyTexture(img);
 SDL_DestroyRenderer(renderer);
 SDL_DestroyWindow(win);
  return 0;
}