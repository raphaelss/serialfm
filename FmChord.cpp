#include "FmChord.h"
#include "boost/math/special_functions/bessel.hpp"
#include "pitchtab.h"
#include <cmath>
#include <utility>

int FmChord::roundGetIndex(double freq)
{
  double current;
  double last = std::abs(freq - PITCH_FREQ_TAB[0].freq);
  int i = 1;
  while (i <= PITCH_FREQ_TAB_LENGTH )
  {
    current = std::abs(freq - PITCH_FREQ_TAB[i].freq);
    if (current > last)
      return i-1;
    else
    {
      last = current;
      ++i;
    }
  }
  return i-1;
}

double FmChord::getFreq(int i)
{
  return PITCH_FREQ_TAB[freqindex[i]].freq;
}

double FmChord::getPClass(int i)
{
  return PITCH_FREQ_TAB[freqindex[i]].pc;
}

int FmChord::getOctave(int i)
{
  return PITCH_FREQ_TAB[freqindex[i]].octave;
}

double FmChord::getAmp(int i)
{
  return amps[i];
}

int FmChord::getNum()
{
  return n;
}

double FmChord::getDur()
{
  return dur;
}

void FmChord::pitchly(int i, std::ostream& out)
{
  double pcarg = getPClass(i);
  int pcarg_i = (int) pcarg;
  switch (pcarg_i)
  {
  case 0:
    out << "c";
    break;
  case 1:
  case 2:
    out << "d";
    break;
  case 3:
  case 4:
    out << "e";
    break;
  case 5:
    out << "f";
    break;
  case 6:
  case 7:
    out << "g";
    break;
  case 8:
  case 9:
    out << "a";
    break;
  case 10:
  case 11:
    out << "b";
    break;
  }
  if ((double) pcarg_i != pcarg)
  {
    switch (pcarg_i)
    {
    case 1:
    case 3:
    case 5:
    case 6:
    case 8:
    case 10:
      out << "eh";
      break;
    default:
      out << "ih";
      break;
    }
  }
  else
  {
    switch (pcarg_i)
    {
    case 1:
    case 3:
    case 6:
    case 8:
    case 10:
      out << "es";
    }
  }
  int octave = getOctave(i);
  if (octave > 3)
    for(int ii = 0; ii < octave - 3; ++ii)
    {
      out << "'";
    }
  else if (octave < 3)
    for(int ii = octave - 3; ii < 0; ++ii)
    {
      out << ",";
    }
}

void FmChord::dynly(int i, std::ostream& out)
{
  double val = getAmp(i);
  out << "\\tweak #'color #";
  if (val < 0.13)
    out << "darkblue";
  else if (val < 0.25)
    out << "darkred";
  else if (val < 0.37)
    out << "darkgreen";
  else if (val < 0.49)
    out << "cyan";
  else if (val < 0.61)
    out << "green";
  else if (val < 0.73)
    out << "blue";
  else if (val < 0.85)
    out << "magenta";
  else
    out << "red";
}

FmChord::FmChord(double freq, double harm, int ind, int durarg, double min, double max)
{
  n = 0;
  dur = durarg;
  int numsbpair;
  bool negative;
  if (ind == 0)
    numsbpair = 0;
  else
    numsbpair = ind+1;
  int numpar = numsbpair*2+1;
  freqindex = new int [numpar];
  amps = new double [numpar];
  for (int i = -numsbpair; i <= numsbpair; ++i)
  {
    double parfreq = i*(harm*freq)+freq;
    if (parfreq < 0)
    {
      negative = true;
      parfreq = std::abs(parfreq);
    }
    else
      negative = false;
    if (parfreq < min || parfreq > max)
      continue;
    int index = roundGetIndex(parfreq);
    double paramp = boost::math::cyl_bessel_j<int,int>(std::abs(i),ind);
    if(negative)
    {
      paramp = -paramp;
    }
    int ind_ismember = -1;
    for (int ii = 0; ii < n; ++ii)
    {
      if (index == freqindex[ii])
      {
        ind_ismember = ii;
        break;
      }
    }
    if (ind_ismember != -1)
      amps[ind_ismember] = amps[ind_ismember]+paramp;
    else
    {
      freqindex[n] = index;
      amps[n] = paramp;
      ++n;
    }
  }
  for(int i = 0; i < n; ++i)
    amps[i] = std::abs(amps[i]);
}

FmChord::~FmChord()
{
  delete[] freqindex;
  delete[] amps;
}

void FmChord::writeCs(std::ostream & out, double start, double durMin)
{
  for (int i = 0; i < n; ++i)
  {
    out << "i1 " << start << " ";
    out << dur*durMin << " " << getFreq(i);
    out << " " << getAmp(i) << std::endl;
  }
}

void FmChord::writeLy(std::ostream & out, std::stringstream sss [])
{
  std::stringstream ssarray [4][4] = {
  std::move(std::stringstream(std::ios_base::in | std::ios_base::out)),
  std::move(std::stringstream(std::ios_base::in | std::ios_base::out)),
  std::move(std::stringstream(std::ios_base::in | std::ios_base::out)),
  std::move(std::stringstream(std::ios_base::in | std::ios_base::out)),
  std::move(std::stringstream(std::ios_base::in | std::ios_base::out)),
  std::move(std::stringstream(std::ios_base::in | std::ios_base::out)),
  std::move(std::stringstream(std::ios_base::in | std::ios_base::out)),
  std::move(std::stringstream(std::ios_base::in | std::ios_base::out)),
  std::move(std::stringstream(std::ios_base::in | std::ios_base::out)),
  std::move(std::stringstream(std::ios_base::in | std::ios_base::out)),
  std::move(std::stringstream(std::ios_base::in | std::ios_base::out)),
  std::move(std::stringstream(std::ios_base::in | std::ios_base::out)),
  std::move(std::stringstream(std::ios_base::in | std::ios_base::out)),
  std::move(std::stringstream(std::ios_base::in | std::ios_base::out)),
  std::move(std::stringstream(std::ios_base::in | std::ios_base::out)),
  std::move(std::stringstream(std::ios_base::in | std::ios_base::out))};
  int whichone;
  bool changed [4][4] = {false};
  int current [4] = {0};
  for (int i = 0; i < n; ++i)
  {
    if(getOctave(i) < 2)
      whichone = 0;
    else if (getOctave(i) < 4)
      whichone = 1;
    else if (getOctave(i) < 6)
      whichone = 2;
    else
      whichone = 3;
    if (!changed[whichone][current[whichone]])
    {
      ssarray[whichone][current[whichone]] << "\\override Staff.Stem #'transparent = ##t < ";
      changed[whichone][current[whichone]] = true;
    }
    dynly(i,ssarray[whichone][current[whichone]]);
    ssarray[whichone][current[whichone]] << " ";
    pitchly(i, ssarray[whichone][current[whichone]]);
    ssarray[whichone][current[whichone]] << " ";
    switch (current[whichone])
    {
    case 0:
      current[whichone] = 1;
      break;
    case 1:
      current[whichone] = 2;
      break;
    case 2:
      current[whichone] = 3;
      break;
    case 3:
      current[whichone] = 0;
      break;
    }
  }
  for(int i = 0; i < 4; ++i)
    for (int ii = 0; ii < 4; ++ii)
    {
      if (!changed[i][ii])
        ssarray[i][ii] << "s4";
      else
        ssarray[i][ii] << ">4";
      if(i == 3 && ii == 0)
        ssarray[i][ii] << "^\\markup {\\bold \\large " << getDur() << "}";
      ssarray[i][ii] << std::endl;
      sss[i] << ssarray[i][ii].str();
    }
}
