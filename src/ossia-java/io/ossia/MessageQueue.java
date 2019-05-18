package io.ossia;
import com.sun.jna.Pointer;
import com.sun.jna.ptr.PointerByReference;

public class MessageQueue implements AutoCloseable
{
  public MessageQueue(Device d)
  {
    impl = Ossia.INSTANCE.ossia_mq_create(d.impl);
  }

  public void register(Parameter p)
  {
    Ossia.INSTANCE.ossia_mq_register(impl, p.impl);
  }
  public void unregister(Parameter p)
  {
    Ossia.INSTANCE.ossia_mq_unregister(impl, p.impl);
  }

  public Message pop()
  {
    PointerByReference param_res = new PointerByReference();
    PointerByReference value_res = new PointerByReference();

    if(Ossia.INSTANCE.ossia_mq_pop(impl, param_res, value_res) != 0)
    {
      if(param_res.getValue() != null & value_res.getValue() != null)
      {
        return new Message(
          new Parameter(param_res.getValue())
        , new Value(value_res.getValue()));
      }
    }

    return null;
  }

  @Override
  public void close()
  {
    Ossia.INSTANCE.ossia_mq_free(impl);
  }


  Pointer impl;
}
