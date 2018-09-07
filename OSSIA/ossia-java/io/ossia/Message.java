package io.ossia;
import com.sun.jna.Pointer;

public class Message
{
  Message(Parameter p, Value v)
  {
    parameter = p;
    value = v;
  }

  public final Parameter parameter;
  public final Value value;
}
