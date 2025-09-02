#include<stdio.h>
#include<math.h>
#include"portaudio.h"

/*#define FRAMES_PER_BUFFER  (64)*/

#ifndef M_PI
#define M_PI  (3.14159265)
#endif

#define NUM_SECONDS (4)
#define FREQUENCY (440.0)

typedef struct
{
  int sample_rate;
  unsigned long current_sample_index;
}my_data;

static int my_Callback( const void *inputBuffer, void *outputBuffer,
			unsigned long framesPerBuffer,
			const PaStreamCallbackTimeInfo* timeInfo,
			PaStreamCallbackFlags statusFlags,
			void *userData )
{

  my_data *data = (my_data *)userData;
  float *output = (float *) outputBuffer;
  float *new_output = (float *) outputBuffer;
  float left_phase;
  float right_phase;
  float *new_phase_output =  (float *) outputBuffer;

  (void) timeInfo; 
  (void) statusFlags;
  (void) inputBuffer;

  unsigned long i;

  for (i = 0; i < framesPerBuffer; i++) {

    float time_in_seconds =  (float)(data -> current_sample_index)/(float)(data -> sample_rate);

    /* *output++ = (float)(i) * sin(2 * M_PI * FREQUENCY * time_in_seconds + 0.0);   */
    
    /* left_phase = 1.0 * sin(2 * M_PI * 440.0 * time_in_seconds + 0.0); */
    /* right_phase = 1.0 * sin(2 * M_PI * 440.0 * time_in_seconds - (M_PI)); */
    /* *new_phase_output++ = left_phase; */
    /* *new_phase_output++ = right_phase; */
    
    /* *new_output++ = (*output) + (float)(i + 100 * sin(2 * M_PI * (FREQUENCY * 2) * time_in_seconds + M_PI)); */

    *output++ = 1.0 * sin(2 * M_PI * FREQUENCY * time_in_seconds + 0.0) + 2.0 * sin(2 * M_PI * (FREQUENCY * 2) * time_in_seconds + (M_PI/2)) + 50.0 * sin(2 * M_PI * (FREQUENCY * 3) * time_in_seconds - (M_PI/4));

    data -> current_sample_index++;

  }

  return paContinue;
  
}

int main()
{

  PaError error;
  PaStreamParameters outputParameters;
  PaDeviceIndex number_of_devices;
  const PaDeviceInfo *my_device;

  my_data my_data_in_main =
    {
      .sample_rate = 44100,
      .current_sample_index = 0,
    };
  
  error = Pa_Initialize();
  if (error != paNoError) {

    fprintf(stderr, "ERROR: Failed To Initialize PortAudio (%s)\n", Pa_GetErrorText(error));
    return -1;
  }
  
  number_of_devices = Pa_GetDeviceCount();
  printf("The Number of Detected devices are: %d\n", number_of_devices);
  outputParameters.device = Pa_GetDefaultOutputDevice();
  printf("The default outputDevice is: %d\n",  outputParameters.device);
  int my_default_input_device = Pa_GetDefaultInputDevice();
  printf("My Default Input Device is: %d\n", my_default_input_device);
    
  for (size_t i = 0; i < number_of_devices; i++) {

    my_device = Pa_GetDeviceInfo(i);
    printf("The Name of the device is: %s\n", my_device -> name);
    
  }
  my_device = Pa_GetDeviceInfo(1);
  printf("The number of Input Channels is: %d\n", my_device -> maxInputChannels);
  my_device =  Pa_GetDeviceInfo(3);
  printf("The number of Output Channels is: %d\n", my_device -> maxOutputChannels);

  PaStream *Stream;

  error = Pa_OpenDefaultStream(&Stream,
			       2,
			       2,
			       paFloat32,
			       my_data_in_main.sample_rate,
			       paFramesPerBufferUnspecified,
			       my_Callback,
			       &my_data_in_main);
  
  if (error != paNoError) {

    fprintf(stderr, "ERROR: Failed To Open Default Audio Stream: (%s)\n", Pa_GetErrorText(error));
    return -1;
  }

  error = Pa_StartStream( Stream );
  if (error != paNoError) {
    fprintf(stderr, "ERROR: Failed To Start The Stream: (%s)\n", Pa_GetErrorText(error));
    return -1;
  }

  Pa_Sleep(NUM_SECONDS*1000);
  
  error = Pa_StopStream( Stream );
  if( error != paNoError ) {

    fprintf(stderr, "ERROR: Failed To Stop The Stream: (%s)\n", Pa_GetErrorText(error));
    return -1;
  }

  error = Pa_CloseStream( Stream );
  if( error != paNoError ) {

    fprintf(stderr, "ERROR: Failed To Close The Stream: (%s)\n", Pa_GetErrorText(error));
    return -1;
  }

  Pa_Terminate();
  puts("Tune Played\n");
}
