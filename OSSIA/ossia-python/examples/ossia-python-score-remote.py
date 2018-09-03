import pyossia as ossia
import time

dev = ossia.MinuitDevice("score-remote", "127.0.0.1", 6666, 9999)
dev.update()
globq = ossia.GlobalMessageQueue(dev)
while(True):
  res = globq.pop()
  while(res != None):
    parameter, value = res
    print("globq: Got " +  str(parameter.node) + " => " + str(value))
    res = globq.pop()

  time.sleep(0.1)
