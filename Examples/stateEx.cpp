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

    shared_ptr<State> s = State::create();
    
    s->insert(s->begin(),State::create());

    cout<<"done!"<<endl;
}
