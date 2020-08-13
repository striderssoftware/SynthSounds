#include <iostream>
#include <sstream>
#include <list>
#include <unistd.h> // TODO VDT - added for Test function sleep call if Test is removed remove this.

#include <SDL2/SDL.h>
#include <SDL2/SDL_audio.h>

#include "Synth.h"

#include "SinusGenerator.h"

using namespace std;

static  std::list<SoundGenerator*> SoundGeneratorList;
static std::mutex m_mtx;

Synth::Synth()
{
  cout << "Synth constructor was called" << endl;
}

Synth::~Synth()
{
 SDL_Quit();
}

bool
Synth::Init()
{
  uint32_t ech = 48000;  //TODO VDT ech - ech  i.e 48000 is a magic number, Figure this out.
  
  m_buf_length = BUF_LENGTH;

  int iError = SDL_Init(SDL_INIT_AUDIO | SDL_INIT_TIMER);
  if (  iError != 0 )
    {
      cout << "SDL init failed here is error:" << SDL_GetError() << endl;
      return false;
    }
  
  SDL_memset(&m_AudioSpecWant, 0, sizeof(m_AudioSpecWant)); /* or SDL_zero(want) */
  m_AudioSpecWant.freq = ech; 
  m_AudioSpecWant.format = AUDIO_S16SYS;
  m_AudioSpecWant.channels = 2;
  m_AudioSpecWant.samples = m_buf_length;
  m_AudioSpecWant.callback = SynthAudioCallback;  //Use NULL callback to push data
  m_device = SDL_OpenAudioDevice(NULL, 0, &m_AudioSpecWant, &m_AudioSpecHave, SDL_AUDIO_ALLOW_FORMAT_CHANGE);
  
  if (m_device == 0) {
    SDL_Log("Failed to open audio: %s", SDL_GetError());
    
    return false;

  } else {

    if (m_AudioSpecHave.format != m_AudioSpecWant.format)  /* this is NOT an error- the format can change */
      SDL_Log("We didn't get Float32 audio format.");
    
    
    if (m_buf_length != BUF_LENGTH)  /* this is NOT an error  */
      cout << "Got buffer length : " << m_AudioSpecHave.samples << " wanted: " << m_buf_length << endl; 

    }

  return true;
}

bool
Synth::BeginPlaying()
{
  SDL_PauseAudioDevice(m_device, 0); /* start audio playing. */

  return true;
}

bool
Synth::Close()
{
  SDL_CloseAudioDevice(m_device);
  SDL_Quit();
  
  return true;
}

bool
Synth::AddSound(double iFrequency)
{
  m_mtx.lock();
  
  cout << "Synth::AddSound was called. here is frequency:" << iFrequency << endl;

  SoundGenerator * psound = new SinusGenerator(1, iFrequency);
  SoundGeneratorList.push_front(psound);

  m_mtx.unlock();
  return true;
}

bool
Synth::RemoveSound(double iFrequency)
{
  cout << "Synth::RemoveSound was called. here is frequency:" << iFrequency << endl;
  
  if ( SoundGeneratorList.size() < 1 )
    return true;

  m_mtx.lock();
  SoundGenerator* removeItem = 0;
  for (auto pSound: SoundGeneratorList)
    {
      if ( pSound->GetFrequency() == iFrequency )
	{
	  removeItem = pSound;
	  break;
	}
    }
  
  SoundGeneratorList.remove(removeItem);

  m_mtx.unlock();
  
  return true;
}

bool
Synth::Test()
{
  Init();
  AddSound(440);
  BeginPlaying();
  sleep(5);  // let the sound play for 5 sec
  Close();

  return true;
}

void
Synth::SynthAudioCallback(void *unused, Uint8 *byteStream, int byteStreamLength)
{
  if ( SoundGeneratorList.size() == 0 )
    return;

  m_mtx.lock();
  
  uint32_t ech = byteStreamLength / sizeof(int16_t);  //TODO VDT ech - here ECH is the number of "steps" in the stream. Figure this out.
  int16_t* stream =  reinterpret_cast<int16_t*>( byteStream );
  uint32_t i;
  
  for (i = 0; i < ech; i+=2)
    {
      float left = 0;
      float right = 0;
      
      for (auto pSound: SoundGeneratorList)
	pSound->next(left, right);
      
      stream[i] = 32767 * left / SoundGeneratorList.size(); //TODO VDT -  32767 is a magic number, what, where, why
      stream[i+1] = 32767 * right / SoundGeneratorList.size(); //TODO VDT -  32767 is a magic number, what, where, why
    }

  m_mtx.unlock();
}


