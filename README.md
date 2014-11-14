WaveGenC
========

Generates a non-audible ramp waveform in 24-bit packed .wav format (with valid headers), useful for diagnosing audio driver issues.

This is a very rudimentary tool I wrote in ~2006 in order to help debug an audio driver. The waveform is a ramp where each consecutive sample increases by one (with a configurable amount of silence at the beginning and end). If you have a digital audio device (and/or driver) that's dropping or otherwise corrupting the audio stream, you can play this waveform through it, record the output and inspect it. I makes it easy to identify and characterize problems. For example, if the driver is configured with a 64 sample buffer-half, a dropped buffer appears as small glitch where two consecutive samples differ by 64. Other types of problems are easy to diagnose since the exact expected value of each sample is known.

I never took the time to extent this to parse command-line options -- when I need to adjust sample rate, etc. I would just change variables, rebuild and generate a new waveform. But it would be easy to make those changes.

This built cleanly on Mac OS X in... 2006. It doesn't anymore. I don't know what happened to the byte swapping routines, I rarely develop on macs anymore.
