#include <alsa/asoundlib.h>
#include <stdio.h>
#include <stdlib.h>

void capture_system_audio(float *buffer, int samples)
{
    static snd_pcm_t *handle = NULL;
    static int initialized = 0;
    int rc;

    if (!initialized)
    {
        /* DEVICE SELECTION:
           - using "default", should work for both PulseAudio and pipewire backends
           - also maybe route your system audio to "monitor" source (in pavucontrol ??) 
        */
        rc = snd_pcm_open(&handle, "default", SND_PCM_STREAM_CAPTURE, 0);
        if (rc < 0) {
            fprintf(stderr, "ALSA: Cannot open device: %s\n", snd_strerror(rc));
            return;
        }

        // Set Hardware Parameters
        snd_pcm_hw_params_t *params;
        snd_pcm_hw_params_alloca(&params);
        snd_pcm_hw_params_any(handle, params);

        snd_pcm_hw_params_set_access(handle, params, SND_PCM_ACCESS_RW_INTERLEAVED);
        
        snd_pcm_hw_params_set_format(handle, params, SND_PCM_FORMAT_S16_LE);
        
        snd_pcm_hw_params_set_channels(handle, params, 2);

        unsigned int val = 44100;
        int dir;
        snd_pcm_hw_params_set_rate_near(handle, params, &val, &dir);

        rc = snd_pcm_hw_params(handle, params);
        if (rc < 0) {
            fprintf(stderr, "ALSA: Cannot set parameters: %s\n", snd_strerror(rc));
            return;
        }

        initialized = 1;
    }

    // 1 frame = 1 sample Left + 1 sample Right
    short *temp_buffer = (short *)malloc(samples * 2 * sizeof(short)); 
    
    snd_pcm_sframes_t frames_read = snd_pcm_readi(handle, temp_buffer, samples);

    if (frames_read == -EPIPE) {
        // Overrun occurred, restart the stream
        snd_pcm_prepare(handle);
        for (int i = 0; i < samples; i++) buffer[i] = 0.0f;
    } else if (frames_read < 0) {
        // Other errors
        for (int i = 0; i < samples; i++) buffer[i] = 0.0f;
    } else {
        // Convert S16_LE to float in -1.0 to 1.0
        for (int i = 0; i < (int)frames_read; i++) {
            buffer[i] = temp_buffer[i * 2] / 32768.0f; // Left channel
        }
        
        // Fill remaining buffer with silence if we didn't get enough frames
        for (int i = (int)frames_read; i < samples; i++) {
            buffer[i] = 0.0f;
        }
    }

    free(temp_buffer);
}
