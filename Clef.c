#include<stdio.h>
#include<math.h>
#include"portaudio.h"

/*#define FRAMES_PER_BUFFER  (64)*/

#ifndef M_PI
#define M_PI  (3.14159265)
#endif

#define NUM_SECONDS (4)
#define FREQUENCY (440.0)
#define TABLE_SIZE (200)
#define SAMPLE_RATE (44100)

typedef struct
{
  float sine_table[TABLE_SIZE];
  int left_phase;
  int right_phase;
}my_data;

static int my_Callback( const void *inputBuffer, void *outputBuffer,
			unsigned long framesPerBuffer,
			const PaStreamCallbackTimeInfo* timeInfo,
			PaStreamCallbackFlags statusFlags,
			void *userData )
{

  my_data *data = (my_data *)userData;
  float *output = (float *) outputBuffer;
  /* float *new_output = (float *) outputBuffer; */
  /* float left_phase; */
  /* float right_phase; */
  /* float *new_phase_output =  (float *) outputBuffer; */

  (void) timeInfo; 
  (void) statusFlags;
  (void) inputBuffer;

  unsigned long i;

  for (i = 0; i < framesPerBuffer; i++)
    {
      float left_sample = 0.0f;
      left_sample += 0.5f * data->sine_table[data->left_phase];           
      left_sample += 0.3f * data->sine_table[(data->left_phase * 2) % TABLE_SIZE];  
      left_sample += 0.2f * data->sine_table[(data->left_phase * 3) % TABLE_SIZE];  

      float right_sample = 0.0f;
      right_sample += 0.4f * data->sine_table[data->right_phase];          
      right_sample += 0.4f * data->sine_table[(data->right_phase * 3) % TABLE_SIZE];
      right_sample += 0.2f * data->sine_table[(data->right_phase * 5) % TABLE_SIZE]; 
      
      /* *output++ = data->sine_table[data->left_phase];  */
      /* *output++ = data->sine_table[data->right_phase]; */
      *output++ = left_sample;
      *output++ = right_sample;
      data->left_phase += 1;
      if( data->left_phase >= TABLE_SIZE ) data->left_phase -= TABLE_SIZE;
        data->right_phase += 3; /* higher pitch so we can distinguish left and right. */
        if( data->right_phase >= TABLE_SIZE ) data->right_phase -= TABLE_SIZE;

    }

  return paContinue;
  
}

int main()
{

  my_data data;

  PaError error;
  PaStreamParameters outputParameters;
  PaDeviceIndex number_of_devices;
  const PaDeviceInfo *my_device;

  int i;
  int j;

  for (i = 0; i < TABLE_SIZE; i++)
    {

      data.sine_table[i] = (float) sin(((float) (i)/ (float) (TABLE_SIZE)) * M_PI * 2.0);
    }

  for (i = 0; i < TABLE_SIZE/2; i++)
    {

      for (j = 0; j < TABLE_SIZE/2; j++)
	{

	  data.sine_table[i] += data.sine_table[j];
        }
    }
  
  data.left_phase = data.right_phase = 0;

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
			       SAMPLE_RATE,
			       paFramesPerBufferUnspecified,
			       my_Callback,
			       &data);
  
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
