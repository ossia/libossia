package io.ossia;

public class OscQueryMirror extends Protocol
{
  public OscQueryMirror(String ws)
  {
    super(Ossia.INSTANCE.ossia_protocol_oscquery_mirror_create(ws));
  }
}
