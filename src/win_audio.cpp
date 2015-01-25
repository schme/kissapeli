#include "win_audio.h"


int32 AudioEngine::init() {

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


//TODO(Kasper): Destroy previous voice if id overwrites
int32 AudioEngine::loadAudio(const char *filename, bool32 loop, int32 id ) {

    WAVEFORMATEXTENSIBLE wfx = {};
    XAUDIO2_BUFFER buffer = {};

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
        //return 0;
    }

    if( INVALID_SET_FILE_POINTER == SetFilePointer( file, 0, NULL, FILE_BEGIN ) ) {
        OutputDebugStringA( "FAILED: Audio file not opened\n");
        //return 0;
    }

    DWORD dwChunkSize;
    DWORD dwChunkPosition;

    //check the file type, should be fourccWAVE or 'XWMA'
    FindChunk( file, fourccRIFF, dwChunkSize, dwChunkPosition );
    DWORD filetype;
    ReadChunkData( file, &filetype, sizeof(DWORD), dwChunkPosition);
    if (filetype != fourccWAVE) {
        OutputDebugStringA( "FAILED: Audio file not recognized as WAVE\n");
        //return 0;
    }

    FindChunk( file, fourccFMT, dwChunkSize, dwChunkPosition);
    ReadChunkData( file, &wfx, dwChunkSize, dwChunkPosition);

    //fill out the audio data buffer with the contents of the fourccDATA chunk
    FindChunk(file, fourccDATA, dwChunkSize, dwChunkPosition );
    BYTE *pDataBuffer = new BYTE[dwChunkSize];
    ReadChunkData(file, pDataBuffer, dwChunkSize, dwChunkPosition);

    buffer.AudioBytes = dwChunkSize;        //size of the audio buffer in bytes
    buffer.pAudioData = pDataBuffer;        //buffer containing audio data

    if( loop ) {
        //buffer.LoopBegin = 0;
        buffer.LoopCount = XAUDIO2_LOOP_INFINITE;
    } else {
        buffer.LoopCount = 0;
    }
    buffer.Flags = XAUDIO2_END_OF_STREAM;   // tell the source voice not to expect any data after this buffer

    if( FAILED( XAudioInstance->CreateSourceVoice( &sounds[id], (WAVEFORMATEX*)&wfx))) {
        OutputDebugStringA( "FAILED: CreateSourceVoice\n");
        return 0;
    }

    if( FAILED( sounds[id]->SubmitSourceBuffer( &buffer))) {
        OutputDebugStringA( "FAILED: SubmitSourceBuffer\n");
        return 0;
    }

    return 1;
}

void AudioEngine::playAudio( int32 id) {
    if( sounds[id] != NULL)  {
        sounds[id]->Start(0);
    } else {
        WIN_OUTPUTDEBUG_I( "FAILED: Played empty sound id: ", id);
    }
}


IXAudio2 *AudioEngine::getInstance() {
    return XAudioInstance;
}

AudioEngine::~AudioEngine() {
    //NOTE(Kasper): Do I even have to implement this?
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
