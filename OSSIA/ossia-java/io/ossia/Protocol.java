package io.ossia;
import com.sun.jna.Pointer;

public abstract class Protocol
{
  Protocol(Pointer p)
  {
    impl = p;
  }

  Pointer impl;
}
