#include "pitchtab.h"
#include "funs.h"
#include "FmChord.h"
#include <fstream>
#include <sstream>
#include <string>
#include <cstdlib>
#include <cstring>
#include <iostream>

int gmatrix [12][12];
bool is_mat = false;
int which [6][3];
char choices [4][12][5];

Fl_Value_Input* main_oct;
Fl_Value_Input* harm_mult;
Fl_Value_Input* minpc;
Fl_Value_Input* maxpc;
Fl_Value_Input* minoct;
Fl_Value_Input* maxoct;
Fl_Value_Input* uniDur;

double ptof(int pc, int oct)
{
  for(int i = 0; i < PITCH_FREQ_TAB_LENGTH; ++i)
    if(pc == PITCH_FREQ_TAB[i].pc && oct == PITCH_FREQ_TAB[i].octave)
      return PITCH_FREQ_TAB[i].freq;
}

bool parseInput(const char* input, int* series)
{
  int temp_s [12];
  int temp_i;
  int i = 0;
  std::stringstream in (input, std::stringstream::in);
  while (in.good())
  {
    in >> temp_i;
    if (temp_i > 11 || temp_i < 0)
      break;
    for (int j = 0; j < i; ++j)
      if (temp_s[j] == temp_i)
        break;
    temp_s[i] = temp_i;
    ++i;
  }
  if (i == 12)
  {
    for (int j = 0; j < 12; ++j)
      series[j] = temp_s[j];
    return true;
  }
  else
    return false;
}

void create12Matrix(const int series[], int matrix[][12])
{
    int interval;
    matrix[0][0] = series[0];
    for (int i = 1; i < 12; ++i)
    {
        matrix[0][i] = series[i];
        interval = series[i]-series[i-1];
        matrix[i][0] = (matrix[i-1][0]-interval+12)%12;
        for (int ii = 1; ii < 12; ++ii)
        {
            interval = series[ii] - series[0];
            matrix[i][ii] = (matrix[i][0]+interval+12)%12;
        }
    }
}

void matrixToString(std::string & str, int matrix[][12])
{

  std::stringstream ss (std::stringstream::in | std::stringstream::out);
  ss.setf(std::ios::left);
  for (int i = 0; i < 12; ++i)
  {
    for (int ii = 0; ii < 12; ++ii)
    {
      ss.width(4);
      ss << matrix[i][ii];
    }
    ss << std::endl;
  }
  str = ss.str();
}

void seriesCallback(Fl_Widget* wid, void* dat)
{
  Fl_Input * widget = (Fl_Input *) wid;
  Fl_Multiline_Output * data = (Fl_Multiline_Output*) dat;
  std::string txtMatrix;
  int series [12];
  if(parseInput(widget->value(),series))
  {
    is_mat = true;
    create12Matrix(series,gmatrix);
    matrixToString(txtMatrix,gmatrix);
    data->value(txtMatrix.c_str());
  }
}

void choiceCallback(Fl_Widget* wid, void* dat)
{
  Fl_Input_Choice * widget = (Fl_Input_Choice *) wid;
  int * data = (int *) dat;
  for(int i = 0; i < 4; ++i)
    for (int ii=0; ii < 12;++ii)
      if (!std::strcmp(choices[i][ii],widget->value()))
      {
        data[0] = i;
        data[1] = ii;
        data[2] = 1;
        return;
      }
  data[2] = 0;
}

void seriesFromMatrix(int tp, int pc ,int matrix[][12], int series[])
{
  switch (tp)
  {
  case 0:
    for(int i = 0; i < 12; ++i)
      if(matrix[i][0] == pc)
      {
        for(int j = 0; j < 12; ++j)
          series[j] = matrix[i][j];
        return;
      }
    break;
  case 1:
    for(int i = 0; i < 12; ++i)
      if(matrix[i][11] == pc)
      {
        for(int j = 0; j < 12; ++j)
          series[j] = matrix[i][11-j];
        return;
      }
    break;
  case 2:
    for(int i = 0; i < 12; ++i)
      if(matrix[0][i] == pc)
      {
        for(int j = 0; j < 12; ++j)
          series[j] = matrix[j][i];
        return;
      }
    break;
  case 3:
    for(int i = 0; i < 12; ++i)
      if(matrix[11][i] == pc)
      {
        for(int j = 0; j < 12; ++j)
          series[j] = matrix[11-j][i];
        return;
      }
    break;
  }
}

void valueCallbackDur(Fl_Widget* wid)
{
  Fl_Value_Input* widget = (Fl_Value_Input*) wid;
  double val = widget->value();
  if(val < 0.01)
    widget->value(0.01);
  else if(val > 30)
    widget->value(30);
}

void valueCallbackHarm(Fl_Widget* wid)
{
  Fl_Value_Input* widget = (Fl_Value_Input*) wid;
  double val = widget->value();
  if(val < 0.1)
    widget->value(0.1);
  else if(val > 100)
    widget->value(100);
}

void valueCallbackPc(Fl_Widget* wid)
{
  Fl_Value_Input* widget = (Fl_Value_Input*) wid;
  double val = widget->value();
  if(val < 0)
    widget->value(0);
  else if(val > 11)
    widget->value(11);
}

void valueCallbackOct(Fl_Widget* wid)
{
  Fl_Value_Input* widget = (Fl_Value_Input*) wid;
  double val = widget->value();
  if(val < 0)
    widget->value(0);
  else if(val > 8)
    widget->value(8);
}

bool ready()
{
  if(!is_mat)
    return false;
  for(int i = 0; i < 6; ++i)
    if(which[i][2] == 0)
      return false;
  return true;
}

void gen_progression(FmChord** progression, int& used)
{
  int all_series [6][12];
  for(int i = 0; i < 6; ++i)
  {
    seriesFromMatrix(which[i][0],which[i][1],gmatrix,all_series[i]);
    if(i != 0)
      for(int ii = 0; ii < 12; ++ii)
        ++all_series[i][ii];
  }
  int abs_series [3][13];
  for(int i = 0; i < 3; ++i)
  {
    abs_series[i][0] = 0;
    for(int ii = 1; ii < 13; ++ii)
    {
      abs_series[i][ii] = abs_series[i][ii-1] + all_series[1+(2*i)][ii-1];
    }
  }
  int i = 0;
  int ind [3] = {0};
  bool changed = false;
  int val [3];
  int dur = 0;
  used = 0;
  while (i <= 78)
  {
    if(changed)
    {
      for(int j = 0; j < 3; ++j)
      {
        if (i == abs_series[j][ind[j]])
        {
          progression[used] = new FmChord(ptof(val[0],main_oct->value()),
                          val[2]*harm_mult->value()/100,
                          val[1],dur,
                          ptof(minpc->value(),minoct->value()),
                          ptof(maxpc->value(),maxoct->value()));
          dur = 0;
          ++used;
          changed = false;
          break;
        }
        if (j == 2)
        {
          ++dur;
          ++i;
        }
      }
    }
    else
    {
      for(int j = 0; j < 3; ++j)
        if (i == abs_series[j][ind[j]])
        {
          val[j] = all_series[(2*j)][ind[j]];
          changed = true;
          ++ind[j];
        }
    }
  }
}

void csoundCallback(Fl_Widget* wid, void* dat)
{
  if(!ready())
    return;
  Fl_Input * data = (Fl_Input *) dat;
  if(data->value()[0] == '\0')
    return;
  int used;
  FmChord * progression[36];
  gen_progression(progression, used);
  double total_dur = 0;
  std::fstream csout (data->value(), std::ios_base::out);
  if(!csout.is_open())
    return;
  for(int i = 0; i < used; ++i)
  {
    progression[i]->writeCs(csout,total_dur,uniDur->value());
    total_dur += (progression[i]->getDur()*uniDur->value());
  }
  csout.close();
}

void introly(std::ostream& out, const char* name,const char* clef)
{
  out<< name << " = {" << std::endl;
    out<< "#(set-accidental-style 'dodecaphonic)" <<std::endl;
    out<< "\\clef "<< clef <<std::endl;
    out<< "\\time 4/4" <<std::endl;
}

void lilyCallback(Fl_Widget* wid, void* dat)
{
  if(!ready())
    return;
  Fl_Input * data = (Fl_Input *) dat;
  if(data->value()[0] == '\0')
    return;
  int used;
  FmChord * progression[36];
  gen_progression(progression, used);
  double total_dur = 0;
  std::fstream lyout (data->value(), std::ios_base::out);
  if(!lyout.is_open())
    return;
  std::stringstream sss [4] ={std::stringstream(std::ios_base::in | std::ios_base::out),
    std::stringstream(std::ios_base::in | std::ios_base::out),
    std::stringstream(std::ios_base::in | std::ios_base::out),
    std::stringstream(std::ios_base::in | std::ios_base::out)};

  introly(sss[0],"loweri","\"bass_15\"");
  introly(sss[1],"lowerii","bass");
  introly(sss[2],"upperi","treble");
  introly(sss[3],"upperii","\"treble^15\"");
  for(int i = 0; i < used; ++i)
  {
    progression[i]->writeLy(lyout,sss);
    delete progression[i];
  }
  for(int i = 0; i < 4; ++i)
  {
    sss[i] << "}" << std::endl;
    lyout << sss[i].str();
  }

  lyout << std::endl;
  lyout << "\\score {" << std::endl;
  lyout << "\\new PianoStaff <<" <<  std::endl;
  lyout << "\\new Staff = \"upperii\" \\upperii" <<  std::endl;
  lyout << "\\new Staff = \"upperi\" \\upperi" <<  std::endl;
  lyout << "\\new Staff = \"lowerii\" \\lowerii" <<  std::endl;
  lyout << "\\new Staff = \"loweri\" \\loweri" <<  std::endl;
  lyout << ">>" <<  std::endl;
  lyout << "\\layout {" <<  std::endl;
  lyout << "\\context {" <<  std::endl;
  lyout << "\\Staff" <<  std::endl;
  lyout << "\\remove \"Time_signature_engraver\"" <<  std::endl;
  lyout << "}" <<  std::endl;
  lyout << "}" <<  std::endl;
  lyout << "}" <<  std::endl;
  lyout.close();
}

Fl_Double_Window* make_window() {
  for (int i = 0; i < 12; ++i)
  {
    choices[0][i][0] = 'P';
    choices[1][i][0] = 'R';
    choices[2][i][0] = 'I';
    choices[3][i][0] = 'R';
    choices[3][i][1] = 'I';
    switch (i)
    {
    case 10:
      choices[0][i][1] = '1';
      choices[0][i][2] = '0';
      choices[1][i][1] = '1';
      choices[1][i][2] = '0';
      choices[2][i][1] = '1';
      choices[2][i][2] = '0';
      choices[3][i][2] = '1';
      choices[3][i][3] = '0';
      choices[0][i][3] = '\0';
      choices[1][i][3] = '\0';
      choices[2][i][3] = '\0';
      choices[3][i][4] = '\0';
      break;
    case 11:
      choices[0][i][1] = '1';
      choices[0][i][2] = '1';
      choices[1][i][1] = '1';
      choices[1][i][2] = '1';
      choices[2][i][1] = '1';
      choices[2][i][2] = '1';
      choices[3][i][2] = '1';
      choices[3][i][3] = '1';
      choices[0][i][3] = '\0';
      choices[1][i][3] = '\0';
      choices[2][i][3] = '\0';
      choices[3][i][4] = '\0';
      break;
    default:
      choices[0][i][1] = i+48;
      choices[1][i][1] = i+48;
      choices[2][i][1] = i+48;
      choices[3][i][2] = i+48;
      choices[0][i][2] = '\0';
      choices[1][i][2] = '\0';
      choices[2][i][2] = '\0';
      choices[3][i][3] = '\0';
      break;
    }
  }
  Fl_Double_Window* w;
  { Fl_Double_Window* o = new Fl_Double_Window(711, 385, "Serialfm");
    w = o;
  { Fl_Multiline_Output* txtDisplay = new Fl_Multiline_Output(70, 60, 255, 205);
      txtDisplay->textsize(14);
     // Fl_Text_Display* o
      Fl_Input* o = new Fl_Input(70, 20, 220, 25, "S\303\251rie ");
    o->callback(seriesCallback,txtDisplay);
    } // Fl_Input* o
    { Fl_Input_Choice* o0 = new Fl_Input_Choice(451, 56, 80, 25);
      Fl_Input_Choice* o1 = new Fl_Input_Choice(571, 56, 80, 25);
      Fl_Input_Choice* o2 = new Fl_Input_Choice(451, 101, 80, 25);
      Fl_Input_Choice* o3 = new Fl_Input_Choice(571, 101, 80, 25);
      Fl_Input_Choice* o4 = new Fl_Input_Choice(451, 147, 80, 25);
      Fl_Input_Choice* o5 = new Fl_Input_Choice(571, 147, 80, 25);
    o0->callback(choiceCallback,which[0]);
    o1->callback(choiceCallback,which[1]);
    o2->callback(choiceCallback,which[2]);
    o3->callback(choiceCallback,which[3]);
    o4->callback(choiceCallback,which[4]);
    o5->callback(choiceCallback,which[5]);
    for(int i=0; i < 4; ++i)
    for(int ii=0; ii<12; ++ii)
    {
      o0->add(choices[i][ii]);
      o1->add(choices[i][ii]);
      o2->add(choices[i][ii]);
      o3->add(choices[i][ii]);
      o4->add(choices[i][ii]);
      o5->add(choices[i][ii]);
    }
    }
    { new Fl_Box(475, 20, 25, 25, "Principal");
    } // Fl_Box* o
    { new Fl_Box(595, 20, 25, 25, "Dura\303\247\303\265""es");
    } // Fl_Box* o
    { new Fl_Box(380, 55, 25, 25, "Altura");
    } // Fl_Box* o
    { new Fl_Box(380, 100, 25, 25, "\303\215ndice");
    } // Fl_Box* o
    { new Fl_Box(380, 145, 25, 25, "Harmonicidade");
    } // Fl_Box* o
    { new Fl_Box(511, 227, 25, 25, "Classe de Notas");
    } // Fl_Box* o
    { new Fl_Box(614, 226, 25, 25, "Oitava");
    } // Fl_Box* o
  main_oct = new Fl_Value_Input(450, 190, 45, 25, "Oitava   ");
  main_oct->range(0,8);
  main_oct->step(1);
  main_oct->value(4);
  main_oct->callback(valueCallbackOct);
  harm_mult = new Fl_Value_Input(605, 190, 45, 25, "Mult. Harm.   ");
  harm_mult->step(0.1);
  harm_mult->range(0.1, 100);
  harm_mult->value(3);
  harm_mult->callback(valueCallbackHarm);
    minpc = new Fl_Value_Input(499, 260, 45, 25, "M\303\255nimo               ");
  minpc->range(0.0,11.0);
  minpc->step(1);
  minpc->value(9);
  minpc->callback(valueCallbackPc);
    maxpc = new Fl_Value_Input(499, 298, 45, 25, "M\303\241ximo               ");
  maxpc->range(0,11);
  maxpc->step(1);
  maxpc->value(0);
  maxpc->callback(valueCallbackPc);
    minoct = new Fl_Value_Input(604, 259, 45, 25);
  minoct->range(0,8);
  minoct->step(1);
  minoct->value(0);
  minoct->callback(valueCallbackOct);
    maxoct = new Fl_Value_Input(604, 299, 45, 25);
  maxoct->range(0,8);
  maxoct->step(1);
  maxoct->value(8);
  maxoct->callback(valueCallbackOct);
  uniDur = new Fl_Value_Input(605, 338, 45, 25, "Unidade de Dura\303\247\303\243o (csound)   ");
  uniDur->step(0.1);
  uniDur->range(0.1,30);
  uniDur->value(1);
  uniDur->callback(valueCallbackDur);
  Fl_Input * lilyfile = new Fl_Input(70, 291, 155, 25);
    Fl_Input * csfile = new Fl_Input(70, 338, 155, 25);
    Fl_Button* lilybutton = new Fl_Button(240, 291, 95, 25, "Lilypond");
  lilybutton->callback(lilyCallback, lilyfile);
    Fl_Button* csbutton = new Fl_Button(240, 338, 95, 25, "Csound");
  csbutton->callback(csoundCallback,csfile);
    o->end();
  } // Fl_Double_Window* o
  return w;
}
