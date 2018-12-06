#pragma once

///////////////////////////////////////////////////////////////////////////////
// Filename: soundclass.h
// The SoundClass encapsulates the DirectSound 
// functionality as well as the.wav audio loading and 
// playing capabilities.
///////////////////////////////////////////////////////////////////////////////
#ifndef _SOUNDCLASS_H_
#define _SOUNDCLASS_H_

// The following libraries and headers are required 
// for DirectSound to compile properly.

/////////////
// LINKING //
/////////////
#pragma comment(lib, "dsound.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "winmm.lib")


//////////////
// INCLUDES //
//////////////
#include <windows.h>
#include <mmsystem.h>
#include <dsound.h>
#include <stdio.h>


///////////////////////////////////////////////////////////////////////////////
// Class name: SoundClass
///////////////////////////////////////////////////////////////////////////////
class SoundClass
{
private:
	
	// When loading in files the header is read first
	// to determine the metadata required 

	// This is the .wav file format  header
	struct WaveHeaderType
	{
		char chunkId[4];
		unsigned long chunkSize;
		char format[4];
		char subChunkId[4];
		unsigned long subChunkSize;
		unsigned short audioFormat;
		unsigned short numChannels;
		unsigned long sampleRate;
		unsigned long bytesPerSecond;
		unsigned short blockAlign;
		unsigned short bitsPerSample;
		// My extra slots for i don't even know
		/*char extra1[4];
		unsigned long extra2;
		char extra3[4];
		char extra4[4];
		unsigned long extra5;
		char extra6[4];
		unsigned long extra7;
		unsigned long extra8;
		unsigned short extra9;*/
		// data should be after this many bytes
		char dataChunkId[4];
		unsigned long dataSize;
	};

public:
	SoundClass();
	SoundClass(const SoundClass&);
	~SoundClass();
	
	// Initialize DirectSound and load in the .wav audio file and then play it once.
	bool Initialize(HWND);

	// Shutdown will release the .wav file and shutdown DirectSound.
	void Shutdown();

private:
	bool InitializeDirectSound(HWND);
	void ShutdownDirectSound();

	bool LoadWaveFile(char*, IDirectSoundBuffer8**);
	void ShutdownWaveFile(IDirectSoundBuffer8**);

	bool PlayWaveFile();

private:
	IDirectSound8* m_DirectSound;
	IDirectSoundBuffer* m_primaryBuffer;
	// Note: only one secondary buffer as only one sound is loaded yet
	IDirectSoundBuffer8* m_secondaryBuffer1;

};

#endif