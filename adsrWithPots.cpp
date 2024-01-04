// adsr envelope example

#include "daisysp.h"
#include "daisy_seed.h"

// Shortening long macro for sample rate
#ifndef sample_rate

#endif

// Interleaved audio definitions
#define LEFT (i)
#define RIGHT (i + 1)

using namespace daisysp;
using namespace daisy;

static DaisySeed  hw;
static Adsr       env;
static Oscillator osc;
static Metro      tick;
bool              gate;

static void AudioCallback(AudioHandle::InterleavingInputBuffer  in,
                          AudioHandle::InterleavingOutputBuffer out,
                          size_t                                size)
{
    float osc_out, env_out;
    for(size_t i = 0; i < size; i += 2)
    {
        // When the metro ticks, trigger the envelope to start.
        if(tick.Process())
        {
            gate = !gate;
        }

        // Use envelope to control the amplitude of the oscillator.
        env_out = env.Process(gate);
        osc.SetAmp(env_out);
        osc_out = osc.Process();

        out[LEFT]  = osc_out;
        out[RIGHT] = osc_out;
    }
}

int main(void)
{
    // initialize seed hardware and daisysp modules
    float sample_rate;
    hw.Configure();
    hw.Init();
    hw.SetAudioBlockSize(4);
    sample_rate = hw.AudioSampleRate();
    env.Init(sample_rate);
    osc.Init(sample_rate);

    // // set up ADC's
    AdcChannelConfig adc_cfg[3];
    adc_cfg[0].InitSingle(seed::A0);
    adc_cfg[1].InitSingle(seed::A1);
    adc_cfg[2].InitSingle(seed::A2);
    adc_cfg[3].InitSingle(seed::A3);

    //Initialize the ADC peripheral with that configuration 4 channels 
    hw.adc.Init(adc_cfg, 4);

    /** Start the ADC conversions in the background */
    hw.adc.Start();

    // Set up metro to pulse every second
    tick.Init(1.0f, sample_rate);

    //Set envelope parameters
    env.SetTime(ADSR_SEG_ATTACK, 1);
    env.SetTime(ADSR_SEG_DECAY, .1);
    env.SetTime(ADSR_SEG_RELEASE, .01);

    env.SetSustainLevel(.25);

    // Set parameters for oscillator
    osc.SetWaveform(osc.WAVE_TRI);
    osc.SetFreq(220);
    osc.SetAmp(0.25);

    // Start logging for printing over serial
    hw.StartLog(true); 
    
    // start callback
    hw.StartAudio(AudioCallback);

    while(1) 
    {
        hw.PrintLine("input 1: %d",hw.adc.Get(0));
        hw.PrintLine("input 2: %d",hw.adc.Get(1));
        hw.PrintLine("input 3: %d",hw.adc.Get(2));
        hw.PrintLine("input 4: %d",hw.adc.Get(3));
        hw.DelayMs(250);
    }
}
