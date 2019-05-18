package io.ossia;

public class OscQueryServer extends Protocol
{
  public OscQueryServer()
  {
    super(Ossia.INSTANCE.ossia_protocol_oscquery_server_create(1234, 5678));
  }
  public OscQueryServer(int osc, int ws)
  {
    super(Ossia.INSTANCE.ossia_protocol_oscquery_server_create(osc, ws));
  }
}
