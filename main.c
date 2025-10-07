#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <complex.h>
#include <assert.h>

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#define Rectangle RB_Rectangle
#define DrawText RB_DrawText
#define DrawTextEx RB_DrawTextEx
#define CloseWindow RB_CloseWindow
#define ShowCursor RB_ShowCursor
#define LoadImage RB_LoadImage
#define PlaySound RB_PlaySound

#include "raylib.h"

#define rb_InitWindow InitWindow
#define rb_SetTargetFPS SetTargetFPS
#define rb_WindowShouldClose WindowShouldClose
#define rb_BeginDrawing BeginDrawing
#define rb_EndDrawing EndDrawing
#define rb_ClearBackground ClearBackground
#define rb_DrawRectangle DrawRectangle
#define rb_GetRenderWidth GetRenderWidth
#define rb_GetRenderHeight GetRenderHeight
#define rb_CloseWindow RB_CloseWindow
#define rb_Color Color

#define N 256

float in[N];
float complex out[N];
float max_amplitude;

void capture_system_audio(float *buffer, int samples);

// literally the original function and it doesnt work the same
void fft(float in[], size_t stride, float complex out[], size_t step)
{

  assert(step > 0);

  if (step == 1)
  {
    out[0] = in[0];
    return;
  }
  fft(in, stride, out, step / 2);
  fft(in + stride, stride, out + step / 2, step / 2);
  for (size_t i = 0; i < step / 2; i++)
  {
    float t = (float)i / step;
    float complex v = cexp(-2 * I * PI * t) * out[i + step / 2];
    float complex e = out[i];
    out[i] = e + v;
    out[i + step / 2] = e - v;
  }
}

float amp(float complex z)
{
  float a = fabsf(crealf(z));
  float b = fabsf(cimagf(z));
  if(a < b) return b;
  return a;
}

int main(void)
{
  rb_InitWindow(800, 400, "Sickass Audio Visualizer");
  rb_SetTargetFPS(60);

  while (!rb_WindowShouldClose())
  {
    capture_system_audio(in, N);

    fft(in, 1, out, N);

    max_amplitude = 0.0f;
    for (int i = 0; i < N / 2; i++)
    {
      float a = amp(out[i]);
      if (a > max_amplitude)
        max_amplitude = a;
    }


    int width = rb_GetRenderWidth();
    int height = rb_GetRenderHeight();
    float cell_width = (float)width / (N / 2);

    rb_BeginDrawing();
    rb_ClearBackground((rb_Color){0x18, 0x18, 0x18, 0xFF});

    for (int i = 0; i < N / 2; i++)
    {
      float t = amp(out[i]) / max_amplitude;
      if (t > 1.0f)
        t = 1.0f;

      int x = (int)(i * cell_width);
      int y = (int)(height - (height / 2.0f * t));
      int w = (int)ceilf(cell_width);
      int h = (int)(height/2 * t);

      rb_Color red = {255, 0, 0, 255};
      rb_DrawRectangle(x, y, w, h, red);
    }

    rb_EndDrawing();
  }

  // rb_CloseWindow();
  return 0;
}
