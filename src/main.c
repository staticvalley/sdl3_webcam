#define SDL_MAIN_USE_CALLBACKS 1

#include <SDL3/SDL.h>
#include <SDL3/SDL_camera.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_main.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_surface.h>
#include <stdio.h>
#include <stdlib.h>

#include "../include/config.h"

// application content
static SDL_Window *window = NULL;
static SDL_Renderer *renderer = NULL;
static SDL_Camera *camera = NULL;
static SDL_Texture *frame_texture = NULL;

// program initialization
SDL_AppResult SDL_AppInit(void **appstate, int argc, char **argv) {

  if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_CAMERA | SDL_INIT_EVENTS)) {
    SDL_Log("sdl3 subsystem could not be initialized: %s", SDL_GetError());
    return SDL_APP_FAILURE;
  }

  if (!SDL_CreateWindowAndRenderer("webcam test ^-^", WINDOW_WIDTH,
                                   WINDOW_HEIGHT, 0, &window, &renderer)) {
    SDL_Log("sdl3 window/renderer pair could not be initialized: %s",
            SDL_GetError());
    return SDL_APP_FAILURE;
  }

  int device_count = 0;
  SDL_CameraID *devices = SDL_GetCameras(&device_count);
  if (devices == NULL) {
    SDL_Log("sdl3 could not list camera devices: %s", SDL_GetError());
    return SDL_APP_FAILURE;
  }
  if (device_count == 0) {
    SDL_Log("sdl3 could not find any camera devices to connect");
    return SDL_APP_FAILURE;
  }

  // SDL_CameraSpec **specs = NULL;
  // int spec_count = 0;
  // specs = SDL_GetCameraSupportedFormats(devices[0], &spec_count);
  // for (int i = 0; i < spec_count; i++) {
  //   printf("framerate: %d/%d\n", specs[i]->framerate_numerator,
  //          specs[i]->framerate_denominator);
  // }

  // grab first camera off of list with default spec
  SDL_CameraSpec spec = {
      .format = SDL_PIXELFORMAT_RGBA32,
      .colorspace = SDL_COLORSPACE_RGB_DEFAULT,
      .width = 32,
      .height = 32,
      .framerate_numerator = 30,
      .framerate_denominator = 1,
  };
  camera = SDL_OpenCamera(devices[0], &spec);
  SDL_free(devices);
  if (camera == NULL) {
    SDL_Log("sdl3 could not connect to camera device: %s", SDL_GetError());
    return SDL_APP_FAILURE;
  }

  return SDL_APP_CONTINUE;
}

// main program loop
SDL_AppResult SDL_AppIterate(void *appstate) {

  Uint64 timestamp = 0;
  SDL_Surface *current_frame = SDL_AcquireCameraFrame(camera, &timestamp);

  if (current_frame) {
    if (!frame_texture) {
      frame_texture = SDL_CreateTexture(renderer, current_frame->format,
                                        SDL_TEXTUREACCESS_STREAMING,
                                        current_frame->w, current_frame->h);
    }
    if (frame_texture) {
      SDL_UpdateTexture(frame_texture, NULL, current_frame->pixels,
                        current_frame->pitch);
    }
    SDL_ReleaseCameraFrame(camera, current_frame);
  }

  SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
  SDL_RenderClear(renderer);

  if (frame_texture)
    SDL_RenderTexture(renderer, frame_texture, NULL, NULL);

  SDL_RenderPresent(renderer);

  return SDL_APP_CONTINUE;
}

// event handler
SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event) {
  switch (event->type) {
  case SDL_EVENT_QUIT:
    return SDL_APP_SUCCESS;
  default:
    break;
  }
  return SDL_APP_CONTINUE;
}

// no cleanup needed, sdl cleans up its own entities
void SDL_AppQuit(void *appstate, SDL_AppResult result) {
  SDL_CloseCamera(camera);
  SDL_DestroyTexture(frame_texture);
}
