#ifndef WIN_AUDIO_H_
#define WIN_AUDIO_H_

#include "kms.h"
#include <xaudio2.h>
#include <vector>

#define SOUNDS_MAX 6


// Little-Endian
#define fourccRIFF 'FFIR'
#define fourccDATA 'atad'
#define fourccFMT ' tmf'
#define fourccWAVE 'EVAW'
#define fourccXWMA 'AMWX'
#define fourccDPDS 'sdpd'


struct AudioCue {
    XAUDIO2_BUFFER buffer;
    const char *filename;
};


HRESULT FindChunk(HANDLE hFile, DWORD fourcc, DWORD & dwChunkSize, DWORD & dwChunkDataPosition);
HRESULT ReadChunkData(HANDLE hFile, void * buffer, DWORD buffersize, DWORD bufferoffset);


class AudioEngine
{
public:
    int32 init();
    /**
     * loadAudio()
     * Loads the audio file and creates a source voice.
     * Overwrites the id if it's already in use.
     */
    int32 loadAudio(const char *filename, bool32 loop, int32 id);

    int32 playAudio( int32 id);

    ~AudioEngine();
    AudioEngine(){};
private:
    IXAudio2 *XAudioInstance;
    IXAudio2MasteringVoice *AudioMasterVoice;
    IXAudio2SourceVoice* voices[SOUNDS_MAX];
    std::vector<AudioCue> sounds;
    /** use the same format for every voice */
    WAVEFORMATEXTENSIBLE wfx;

    uint32 voiceIndex;

    void advanceVoiceIndex();
};


#endif // WIN_AUDIO_H_
