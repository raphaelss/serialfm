#ifndef FMCHORD_HPP_INCLUDED
#define FMCHORD_HPP_INCLUDED
#include <iostream>

class FmChord
{
private:
  int *freqindex;
  double *amps;
  int dur;
  int n;
  int roundGetIndex(double freq);
  void pitchly(int i, std::ostream& out);
  void dynly(int i, std::ostream& out);

public:
  FmChord(double freq, double harm, int ind, int durarg, double min, double max);
  ~FmChord(void);
  double getFreq(int i);
  double getPClass(int i);
  int getOctave(int i);
  double getAmp(int i);
  int getNum();
  double getDur();
  void writeCs(std::ostream &out, double start, double durMin);
  void writeLy(std::ostream &out, std::stringstream ssarray[]);
};

#endif
