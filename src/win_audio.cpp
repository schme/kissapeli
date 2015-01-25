#include "win_audio.h"


int AudioEngine::Init() {

    if( FAILED( XAudio2Create( &XAudioInstance, 0 ))) {
        OutputDebugStringA( "FAILED: Creating XAudio2 instance\n");
        return 0;
    }

    if( FAILED( XAudioInstance->CreateMasteringVoice( &AudioMasterVoice))) {
        OutputDebugStringA( "FAILED: CreateMasteringVoice\n");
        return 0;
    }
    return 1;
}

IXAudio2 *AudioEngine::getInstance() {
    return XAudioInstance;
}

AudioEngine::~AudioEngine() {
    //NOTE(Kasper): Do I even have to implement this?
}
