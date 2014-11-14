
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>

typedef unsigned long UInt32;
typedef unsigned short UInt16;

typedef struct
{
    UInt32	mID;		// big
    UInt32	mSize;		// little
    UInt32	mFmt;		// big
} RIFF_Header;

typedef struct
{
    UInt32	mID;			// big
    UInt32	mSize;			// little
    UInt16	mAudioFmt;		// little
    UInt16	mNumChannels;	// little
    UInt32	mSampleRate;	// little
    UInt32	mByteRate;		// little
    UInt16	mBlockAlign;	// little
    UInt16	mBitsPerSample;	// little
} WAVE_Fmt_Chunk;

typedef struct
{
    UInt32	mID;			// big
    UInt32	mSize;			// little
    // data ...
} WAVE_Data_Chunk;

int main (int argc, const char * argv[])
{
    // const used for variables that could be controlled by command line flags
    const unsigned short bitsPerSample = 24;
    const unsigned long samplesPerRamp = 0x01000000;
    const unsigned long sampleRate = 48000;
    const unsigned long silencePadInSeconds = 5;	// add this many seconds of silence to beginning and end
    const char audioFileName[] = "waveout.wav";
    
    unsigned char * audioFileData = NULL;
    unsigned char * sampleData = NULL;
    unsigned char * rampData = NULL;
    RIFF_Header * riffHeader = NULL;
    WAVE_Fmt_Chunk * waveFmtChunk = NULL;
    WAVE_Data_Chunk * waveDataChunk = NULL;
    
    size_t audioFileSize = sizeof(RIFF_Header) + sizeof(WAVE_Fmt_Chunk) + sizeof(WAVE_Data_Chunk) + ((samplesPerRamp + (2 * silencePadInSeconds * sampleRate)) * ( bitsPerSample / 8 ));
    audioFileData = (unsigned char * ) malloc( audioFileSize );
    if ( NULL == audioFileData )
    {
        printf( "Failed to alloc memory!\n" );
        return 1;
    }
    
    riffHeader = (RIFF_Header *) audioFileData;
    waveFmtChunk = (WAVE_Fmt_Chunk *) (audioFileData + sizeof(RIFF_Header));
    waveDataChunk = (WAVE_Data_Chunk *) (audioFileData + sizeof(RIFF_Header) + sizeof(WAVE_Fmt_Chunk));
    sampleData = audioFileData + sizeof(RIFF_Header) + sizeof(WAVE_Fmt_Chunk) + sizeof(WAVE_Data_Chunk);
    rampData = sampleData + ((silencePadInSeconds * sampleRate) * (bitsPerSample / 8));
    
    // fill in headers
    riffHeader->mID					= OSSwapHostToBigInt32( 'RIFF' );
    riffHeader->mSize				= OSSwapHostToLittleInt32( audioFileSize - 8 );
    riffHeader->mFmt				= OSSwapHostToBigInt32( 'WAVE' );
    
    waveFmtChunk->mID				= OSSwapHostToBigInt32( 'fmt ' );
    waveFmtChunk->mSize				= OSSwapHostToLittleInt32( sizeof(WAVE_Fmt_Chunk) - 8 );
    waveFmtChunk->mAudioFmt			= OSSwapHostToLittleInt16( 1 );
    waveFmtChunk->mNumChannels		= OSSwapHostToLittleInt16( 1 );
    waveFmtChunk->mSampleRate		= OSSwapHostToLittleInt32( sampleRate );
    waveFmtChunk->mByteRate			= OSSwapHostToLittleInt32( sampleRate * ( bitsPerSample / 8 ) );
    waveFmtChunk->mBlockAlign		= OSSwapHostToLittleInt16( bitsPerSample / 8 );
    waveFmtChunk->mBitsPerSample	= OSSwapHostToLittleInt16( bitsPerSample );
    
    waveDataChunk->mID				= OSSwapHostToBigInt32( 'data' );
    waveDataChunk->mSize			= OSSwapHostToLittleInt32( ((samplesPerRamp + (2 * silencePadInSeconds * sampleRate)) * ( bitsPerSample / 8 )) );
    
    // leading silence
    bzero( sampleData, ((silencePadInSeconds * sampleRate) * (bitsPerSample / 8)) );
    
    // audio ramp
    unsigned long writeptr = 0;
    long sample = 0xFF800000;
    while ( writeptr < (samplesPerRamp * ( bitsPerSample / 8 )) )
    {
        rampData[writeptr++] = (unsigned char)(((unsigned long)sample) & 0xFF);
        rampData[writeptr++] = (unsigned char)((((unsigned long)sample) >> 8 ) & 0xFF);
        rampData[writeptr++] = (unsigned char)((((unsigned long)sample) >> 16 ) & 0xFF);
        
        sample++;
    }
    
    // trailing silence
    bzero( rampData + (samplesPerRamp * ( bitsPerSample / 8 )), ((silencePadInSeconds * sampleRate) * (bitsPerSample / 8)) );
    
    // dump the waveform to a file
    int fd = open( audioFileName, O_WRONLY | O_CREAT, 755 );
    if ( fd == -1 ) return 1;
    
    int n = write( fd, audioFileData, audioFileSize );
    printf( "n: %ld expected: %ld\n", n, audioFileSize );
    
    close( fd );
    free( audioFileData );
    
    return 0;
}

