/************************************************************************

        IMPORTANT NOTE : this file contains two clearly delimited sections :
        the ARCHITECTURE section (in two parts) and the USER section. Each section
        is governed by its own copyright and license. Please check individually
        each section for license and copyright information.
*************************************************************************/

/*******************BEGIN ARCHITECTURE SECTION (part 1/2)****************/

/************************************************************************
    FAUST Architecture File
    Copyright (C) 2003-2016 GRAME, Centre National de Creation Musicale
    ---------------------------------------------------------------------
    This Architecture section is free software; you can redistribute it
    and/or modify it under the terms of the GNU General Public License
    as published by the Free Software Foundation; either version 3 of
    the License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; If not, see <http://www.gnu.org/licenses/>.

    EXCEPTION : As a special exception, you may create a larger work
    that contains this FAUST architecture section and distribute
    that work under terms of your choice, so long as this FAUST
    architecture section is not modified.

 ************************************************************************
 ************************************************************************/

#include "OssiaUI.h"
#include "faust/audio/portaudio-dsp.h"
#include "faust/dsp/llvm-dsp.h"
#include "faust/gui/FUI.h"
#include "faust/gui/GUI.h"
#include "faust/gui/console.h"
#include "faust/misc.h"

#include <cmath>
#include <libgen.h>
#include <stdlib.h>

#include <iostream>
#include <list>

std::list<GUI*> GUI::fGuiList;
ztimedmap GUI::gTimedZoneMap;

//-------------------------------------------------------------------------
// 									MAIN
//-------------------------------------------------------------------------
int main(int argc, char* argv[])
{
  cout << "Libfaust version : " << getCLibFaustVersion() << endl;

  string error_msg;
  dsp_factory* factory
      = createDSPFactoryFromFile(argv[argc - 1], 0, nullptr, "", error_msg, -1);
  if(factory == 0)
  {
    std::cerr << "Unable to create Faust DSP factory" << std::endl;
    exit(1);
  }

  dsp* DSP = factory->createDSPInstance();
  if(DSP == 0)
  {
    std::cerr << "Unable to allocate Faust DSP object" << std::endl;
    exit(1);
  }

  OssiaUI ossia{1234, 5678};
  DSP->buildUserInterface(&ossia);

  portaudio audio(44100, 256);
  audio.init("FaustDSP", DSP);
  audio.start();

  ossia.run(50);
  audio.stop();

  return 0;
}
