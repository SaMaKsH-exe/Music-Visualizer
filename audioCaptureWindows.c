#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#include <mmdeviceapi.h>
#include <audioclient.h>
#include <audiopolicy.h>
#include <endpointvolume.h>
#include <objbase.h>

// COM GUIDs for WASAPI
const CLSID CLSID_MMDeviceEnumerator = {0xbcde0395, 0xe52f, 0x467c, {0x8e, 0x3d, 0xc4, 0x57, 0x92, 0x91, 0x69, 0x2e}};
const IID IID_IMMDeviceEnumerator = {0xa95664d2, 0x9614, 0x4f35, {0xa7, 0x46, 0xde, 0x8d, 0xb6, 0x36, 0x17, 0xe6}};
const IID IID_IAudioClient = {0x1cb9ad4c, 0xdbfa, 0x4c32, {0xb1, 0x78, 0xc2, 0xf5, 0x68, 0xa7, 0x03, 0xb2}};
const IID IID_IAudioCaptureClient = {0xc8adbd64, 0xe71e, 0x48a0, {0xa4, 0xde, 0x18, 0x5c, 0x39, 0x5c, 0xd3, 0x17}};

// Simple WASAPI loopback capture that fills `buffer` with `samples` floats in [-1,1]
void capture_system_audio(float *buffer, int samples)
{
    static IMMDeviceEnumerator *pEnumerator = NULL;
    static IMMDevice *pDevice = NULL;
    static IAudioClient *pAudioClient = NULL;
    static IAudioCaptureClient *pCaptureClient = NULL;
    static WAVEFORMATEX *pwfx = NULL;
    static int initialized = 0;

    if (!initialized)
    {
        CoInitialize(NULL);

        CoCreateInstance(&CLSID_MMDeviceEnumerator, NULL, CLSCTX_ALL, &IID_IMMDeviceEnumerator, (void **)&pEnumerator);
        pEnumerator->lpVtbl->GetDefaultAudioEndpoint(pEnumerator, eRender, eConsole, &pDevice);
        pDevice->lpVtbl->Activate(pDevice, &IID_IAudioClient, CLSCTX_ALL, NULL, (void **)&pAudioClient);

        pAudioClient->lpVtbl->GetMixFormat(pAudioClient, &pwfx);
        pAudioClient->lpVtbl->Initialize(pAudioClient, AUDCLNT_SHAREMODE_SHARED, AUDCLNT_STREAMFLAGS_LOOPBACK, 0, 0, pwfx, NULL);

        pAudioClient->lpVtbl->GetService(pAudioClient, &IID_IAudioCaptureClient, (void **)&pCaptureClient);
        pAudioClient->lpVtbl->Start(pAudioClient);

        initialized = 1;
    }

    UINT32 packetLength = 0;
    pCaptureClient->lpVtbl->GetNextPacketSize(pCaptureClient, &packetLength);

    if (packetLength > 0)
    {
        BYTE *pData = NULL;
        UINT32 numFrames = 0;
        DWORD flags = 0;

        pCaptureClient->lpVtbl->GetBuffer(pCaptureClient, &pData, &numFrames, &flags, NULL, NULL);

        if (pData)
        {
            // assume 16-bit stereo PCM (typical for mix format)
            short *samplesData = (short *)pData;
            int framesToCopy = (int)numFrames < samples ? (int)numFrames : samples;
            for (int i = 0; i < framesToCopy; i++)
            {
                buffer[i] = samplesData[i * 2] / 32768.0f; // left channel
            }
            for (int i = framesToCopy; i < samples; i++)
            {
                buffer[i] = 0.0f;
            }
        }
        pCaptureClient->lpVtbl->ReleaseBuffer(pCaptureClient, numFrames);
    }
    else
    {
        // no packet, return zeroed buffer
        for (int i = 0; i < samples; i++)
            buffer[i] = 0.0f;
    }
}
