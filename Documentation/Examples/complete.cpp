/*!
 * \file complete.cpp
 *
 * \author Clément Bossut
 * \author Théo de la Hogue
 * \author Jean-Michaël Celerier
 *
 * This code is licensed under the terms of the "CeCILL-C"
 * http://www.cecill.info
 */

int main()
{/*
  // Peut aussi être {addr, port} ou addr, port...
  Device<OSC> player{"127.0.0.1:1234",
    "player"};
  Device<Minuit> video{"127.0.0.1:1235",
    "video"};

  Device<MIDI> keyboard{"127.0.0.1:1236",
    "keyboard"};

  // Contrainte "originelle"
  Event firstEvent;
  Constraint c0{firstEvent}; // Par défaut, il y a un lastEvent "vide", p-ê ?
  c0.setDuration(10_s); // Utilise std::literals::chrono_literals

  // Elle a un processus scenario
  Scenario s;
  s.setDuration(4500_ms); // dans i-score, le reste (jusqu'à 10s) serait affiché en grisé
  c0.addProcess(s);

  // Dans le scenario, une contrainte
  Constraint c1;
  c1.setDuration(pos_infin); // Utilise boost::posix_time ?

  // Dans la contrainte, une courbe
  LinearAutomationProcess curve{
    player.addr("volume"), // /player/volume
	2_s, // Durée du process
	{{0.0, 0.0}, // Points
	    {0.5, 0.8},
	    {1.0, 0.0}}}; // Note : ctor peut utiliser std::initializer_list ou std::some_container<std::pair<T(double),U(double)>>
  c1.addProcess(curve);

  // États au début et à la fin de la contrainte / du scénario
  State s1{player.addr("start")}; // /player/start
  State s2{player.addr("stop"), // /player/stop
    video.addr("flash", 255, 255, 255)}; // /player/flash <255,255,255> (le Tuple dont on parle dans la spec)

  Event e1, e2;
  e1.addState(s1);
  e2.addState(s2);
  e2.setCondition(keyboard.condition("channel.1/note_on == 64"));
  c1.setStartEvent(e1);
  c1.setEndEvent(e2);

  Node n1 = s.getStartNode();
  n1.addEvent(e1);

  Node n2;
  n2.addEvent(e2);

  s.addConstraint(c1, n1, n2); // c1 et n2 sont ajoutés.

  firstEvent.play();

  return 0; // Bière si on arrive ici sans crash :)*/
}
