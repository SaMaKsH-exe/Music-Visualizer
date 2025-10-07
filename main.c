#include <stdio.h>
#include "raylib.h"
#include <unistd.h>
#include "stdint.h"
#include "string.h"
#include <complex.h>
#include <math.h>
#include <assert.h>
#if defined(PLATFORM_WEB)
#include <emscripten/emscripten.h>
#endif
#define pi 3.14159265358979323846


#define N 256
float in[N];
float complex out[N];
float max_amplitude;

typedef struct {
  float left_channel;
  float right_channel;
} Frame;


void fft(float in[],size_t stride, float complex out[],size_t step) {

  assert(step > 0);

  if(step == 1){
    out[0] = in[0];
    return;
  }
  fft(in,stride,out,step/2);
  fft(in+stride,stride,out+step/2,step/2);
  for(size_t i = 0; i < step/2; i++){
    float t = (float)i/step;
    float complex v = cexp(-2*I*pi*t)*out[i+step/2];
    float complex e = out[i];
    out[i] = e + v;
    out[i+step/2] = e - v;
   
}

}




float amp(float complex z){
  float a = fabsf(crealf(z));
  float b = fabsf(cimagf(z));
  if(a < b) return b;
  return a;


}


void callback(void *bufferData, unsigned int frames) {
  Frame *fs = (Frame *)bufferData;
  size_t to_copy = frames < N ? frames : N;

  for(size_t i = 0; i < to_copy; i++){
    in[i] = fs[i].left_channel;
  }
  for(size_t i = to_copy; i < N; i++){
    in[i] = 0.0f;
  }

  fft(in, 1, out, N);
  max_amplitude = 0.0f;
  for(size_t i = 0; i < N; i++){
    float a = amp(out[i]);
    if(a > max_amplitude) max_amplitude = a;
  }
}

int main(void) {
  InitWindow(800, 600, "Hanuman");
  SetWindowState(FLAG_WINDOW_RESIZABLE);
  SetTargetFPS(60);

  InitAudioDevice();
  Music music = LoadMusicStream("hanuman.wav");
  PlayMusicStream(music);
  SetMusicVolume(music, 0.75f);
  AttachAudioStreamProcessor(music.stream, callback);

  while (!WindowShouldClose()) {
    UpdateMusicStream(music);


    if (IsKeyPressed(KEY_SPACE)) {
        if (IsMusicStreamPlaying(music)) {
            PauseMusicStream(music);
        }
        else {
            PlayMusicStream(music);
        }
    }

    int width = GetRenderWidth();
    int height = GetRenderHeight();

    BeginDrawing();
    ClearBackground(CLITERAL(Color){0x18,0x18,0x18,0xFF});
    //DrawText("Hanuman", width/2-strlen("Hanuman"), 10, 20, WHITE);
    float cell_width = (float)width/(float)N;
    for(size_t i = 0; i < N; i++){
      float t = amp(out[i])
      ;
   
      DrawRectangle((int)(i*cell_width), height/2-height/2*t, cell_width, height/2*t, RED);
    }

    EndDrawing();
  }

  return 0;
}




