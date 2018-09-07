package io.ossia;
import com.sun.jna.Pointer;

public abstract class Protocol implements AutoCloseable
{
  Protocol(Pointer p)
  {
    impl = p;
  }

  @Override
  public void close() {
    if(impl != null)
      Ossia.INSTANCE.ossia_protocol_free(impl);
  }

  Pointer impl;
}
