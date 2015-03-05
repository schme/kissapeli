#include "win_audio.h"

/** We use this files WFX format for all files */
#define EXAMPLE_WFX_FILE "assets/audio/song.wav"


int32 AudioEngine::init() {
    voiceIndex = 0;

    if( FAILED( XAudio2Create( &XAudioInstance, 0 ))) {
        OutputDebugStringA( "FAILED: Creating XAudio2 instance\n");
        return 0;
    }

    if( FAILED( XAudioInstance->CreateMasteringVoice( &AudioMasterVoice))) {
        OutputDebugStringA( "FAILED: CreateMasteringVoice\n");
        return 0;
    }

    HANDLE example = CreateFile(
        EXAMPLE_WFX_FILE,
        GENERIC_READ,
        FILE_SHARE_READ,
        NULL,
        OPEN_EXISTING,
        0,
        NULL );
    DWORD chunkSize;
    DWORD chunkPosition;
    FindChunk( example, fourccFMT, chunkSize, chunkPosition);
    ReadChunkData( example, &wfx, chunkSize, chunkPosition);

    for (int i = 0; i < SOUNDS_MAX; ++i) {
        if( FAILED( XAudioInstance->CreateSourceVoice( &voices[i], (WAVEFORMATEX*)&wfx))) {
            XAudioInstance->Release();
            CoUninitialize();
            return -(SOUNDS_MAX + i);
        } else {
            voices[i]->Start(0);
        }
    }
    return 1;
}


//TODO(Kasper): Destroy previous voice if id overwrites
int32 AudioEngine::loadAudio(const char *filename, bool32 loop, int32 id ) {

    AudioCue audio = {};

    /* Open the audio file */
    HANDLE file = CreateFile(
        filename,
        GENERIC_READ,
        FILE_SHARE_READ,
        NULL,
        OPEN_EXISTING,
        0,
        NULL );

    if( INVALID_HANDLE_VALUE == file ) {
        OutputDebugStringA( "FAILED: Audio file not opened\n");
        return 0;
    }

    if( INVALID_SET_FILE_POINTER == SetFilePointer( file, 0, NULL, FILE_BEGIN ) ) {
        OutputDebugStringA( "FAILED: Audio file not opened\n");
        return 0;
    }

    DWORD dwChunkSize;
    DWORD dwChunkPosition;

    //fill out the audio data buffer with the contents of the fourccDATA chunk
    FindChunk(file, fourccDATA, dwChunkSize, dwChunkPosition );
    BYTE *pDataBuffer = new BYTE[dwChunkSize];
    ReadChunkData(file, pDataBuffer, dwChunkSize, dwChunkPosition);

    audio.buffer.AudioBytes = dwChunkSize;        //size of the audio buffer in bytes
    audio.buffer.pAudioData = pDataBuffer;        //buffer containing audio data

    if( loop ) {
        //buffer.LoopBegin = 0;
        audio.buffer.LoopCount = XAUDIO2_LOOP_INFINITE;
    } else {
        audio.buffer.LoopCount = 0;
    }
    audio.buffer.Flags = XAUDIO2_END_OF_STREAM;   // tell the source voice not to expect any data after this buffer

    audio.filename = filename;
    sounds.push_back(audio);

    return 1;
}


void AudioEngine::advanceVoiceIndex() {
    if( voiceIndex >= SOUNDS_MAX - 1) {
        voiceIndex = 1;
    } else {
        ++voiceIndex;
    }
}


int32 AudioEngine::playAudio( int32 id) {
    if( sounds.size() > id) {
        //voices[voiceIndex]->Stop(0);
        //voices[voiceIndex]->FlushSourceBuffers();
        voices[voiceIndex]->SubmitSourceBuffer( &sounds[id].buffer);
        //voices[voiceIndex]->Start(0);
        advanceVoiceIndex();
    }
    return 1;
}


AudioEngine::~AudioEngine() {
    //NOTE(Kasper): Do I even have to?
}



HRESULT FindChunk(HANDLE hFile, DWORD fourcc, DWORD & dwChunkSize, DWORD & dwChunkDataPosition)
{
    HRESULT hr = S_OK;
    if( INVALID_SET_FILE_POINTER == SetFilePointer( hFile, 0, NULL, FILE_BEGIN ) )
        return HRESULT_FROM_WIN32( GetLastError() );

    DWORD dwChunkType;
    DWORD dwChunkDataSize;
    DWORD dwRIFFDataSize = 0;
    DWORD dwFileType;
    DWORD bytesRead = 0;
    DWORD dwOffset = 0;

    while (hr == S_OK)
    {
        DWORD dwRead;
        if( 0 == ReadFile( hFile, &dwChunkType, sizeof(DWORD), &dwRead, NULL ) )
            hr = HRESULT_FROM_WIN32( GetLastError() );

        if( 0 == ReadFile( hFile, &dwChunkDataSize, sizeof(DWORD), &dwRead, NULL ) )
            hr = HRESULT_FROM_WIN32( GetLastError() );

        switch (dwChunkType)
        {
        case fourccRIFF:
            dwRIFFDataSize = dwChunkDataSize;
            dwChunkDataSize = 4;
            if( 0 == ReadFile( hFile, &dwFileType, sizeof(DWORD), &dwRead, NULL ) )
                hr = HRESULT_FROM_WIN32( GetLastError() );
            break;

        default:
            if( INVALID_SET_FILE_POINTER == SetFilePointer( hFile, dwChunkDataSize, NULL, FILE_CURRENT ) )
            return HRESULT_FROM_WIN32( GetLastError() );
        }

        dwOffset += sizeof(DWORD) * 2;

        if (dwChunkType == fourcc)
        {
            dwChunkSize = dwChunkDataSize;
            dwChunkDataPosition = dwOffset;
            return S_OK;
        }
        dwOffset += dwChunkDataSize;
        if (bytesRead >= dwRIFFDataSize) return S_FALSE;

    }
    return S_OK;

}


HRESULT ReadChunkData(HANDLE hFile, void * buffer, DWORD buffersize, DWORD bufferoffset)
{
    HRESULT hr = S_OK;
    if( INVALID_SET_FILE_POINTER == SetFilePointer( hFile, bufferoffset, NULL, FILE_BEGIN ) )
        return HRESULT_FROM_WIN32( GetLastError() );
    DWORD dwRead;
    if( 0 == ReadFile( hFile, buffer, buffersize, &dwRead, NULL ) )
        hr = HRESULT_FROM_WIN32( GetLastError() );
    return hr;
}
