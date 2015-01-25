#ifndef WIN_AUDIO_H_
#define WIN_AUDIO_H_

#include <xaudio2.h>

class AudioEngine
{
public:
    int Init();
    IXAudio2 *getInstance();

    ~AudioEngine();
    AudioEngine(){};
private:
    IXAudio2 *XAudioInstance;
    IXAudio2MasteringVoice *AudioMasterVoice;
};

#endif // WIN_AUDIO_H_
