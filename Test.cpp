#include <iostream>
#include <unistd.h>

#include "Synth.h"

using namespace std;

void testOne();
void testTwo();
void testThree();

int main()
{

  cout << "running splay" << endl;

  testTwo();
}


void testOne()
{
  Synth mysynth;

  mysynth.Init();

  mysynth.AddSound(440); //A
  
  mysynth.BeginPlaying();

  sleep(2);
}

void testTwo()
{
  Synth mysynth;

  mysynth.Init();

  mysynth.AddSound(440); //A
  
  mysynth.BeginPlaying();

  sleep(2);
  mysynth.AddSound(329.63);  //E
  
  sleep(2);

  mysynth.RemoveSound(440); //A
  
  sleep(2);

  mysynth.Close();
}

void testThree()
{
  int iloop = 20;
  
  Synth mysynth;
  
  mysynth.Init();
  
  mysynth.AddSound(440); //A
  
  for ( int i = 0; i < iloop; i++ )
    {
      mysynth.AddSound(440); //A
    }
  
  mysynth.BeginPlaying();
  
  sleep(2);
  
  for ( int i = 0; i < iloop; i++ )
    {
      mysynth.RemoveSound(440); //A
    }
    
  sleep(3);
  
  mysynth.Close();
  
}

void testFour()
{
  int iaddloop = 20;
  int iremoveloop = 20;
  
  Synth mysynth;
  
  mysynth.Init();
  
  for ( int i = 0; i < iaddloop; i++ )
    {
      mysynth.AddSound(440); //A
    }
  
  mysynth.BeginPlaying();
  
  sleep(2);

  // Remove all sounds. Plus - Remove more then were added. 
  for ( int i = 0; i < iremoveloop; i++ )
    {
      mysynth.RemoveSound(440); //A
    }
  
  sleep(3);
  
  mysynth.Close();
  
}
