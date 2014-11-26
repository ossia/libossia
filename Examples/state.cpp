/*!
 * \file state.cpp
 *
 * \author Clément Bossut
 * \author Théo de la Hogue
 *
 * This code is licensed under the terms of the "CeCILL-C"
 * http://www.cecill.info
 */

#include "Editor/State.h"

using namespace OSSIA;

#include <iostream>

using namespace std;

int main() {

    State* s = State::create();
    
    delete s;
}
