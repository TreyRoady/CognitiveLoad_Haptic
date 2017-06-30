# CognitiveLoad_Haptic
Control code for presenting EAI C2 tactor haptic signals for the Cognitive Load study
This code utilizes data from Validation Study to select the ideal signal sets.

Note: This code is intentionally incomplete, the Tactor DLL has been excluded due to its proprietary nature, and it would be useless without the hardware, anyways.
Any implementation of this code would need to replace that component with your stimulus interface of choice.

Treatments for study:
  - Haptic Encoding Method:
      Syncopated (Iso-frequency haptic icons)
      Melodic (Variable frequency haptic icons)
      Counted (# of signals per timeframe)
      
  - Number of levels:
      3 Levels
      4 Levels
      5 Levels
  
  - Difficulty of Multitasking Task in MATB-II (See CognitiveLoad_MATB for condition files)
      Moderate
      Difficult

9 Treatments, Repeated at alternating MATB-II Difficulty Levels
4 Minutes apiece, 14 samples per trial.
