#include <nan.h>
#include <ossia/network/oscquery/oscquery_server.hpp>

template<typename T>
T* CheckedUnWrap(v8::Handle<v8::Object> handle)
{
  if (!handle.IsEmpty() && handle->InternalFieldCount() == 1)
  {
    auto objproto = handle->GetPrototype();
    if (objproto == T::prototype)
    {
      return Nan::ObjectWrap::Unwrap<T>(handle);
    }
  }

  return nullptr;
}

// seriously wtf node.js
auto from_pointer(void* ptr)
{
  char ptr_str[sizeof(void*)];
  std::memcpy(ptr_str, &ptr, sizeof(void*));
  return Nan::New(reinterpret_cast<uint16_t*>(ptr_str), sizeof(void*) / 2);
}

template<typename T>
auto to_pointer(v8::Local<v8::Value> v)
{
  char ptr_str[sizeof(void*)];
  v->ToString()->Write(reinterpret_cast<uint16_t*>(ptr_str), 0, sizeof(void*) / 2);
  return *reinterpret_cast<T**>(ptr_str);
}

class OscQueryServer : public Nan::ObjectWrap
{
public:
    std::unique_ptr<ossia::oscquery::oscquery_server_protocol> m_proto;
    static Nan::Persistent<v8::FunctionTemplate> s_ct;
    static NAN_MODULE_INIT(Init)
    {
        Nan::HandleScope scope;

        v8::Local<v8::FunctionTemplate> t = Nan::New<v8::FunctionTemplate>(OscQueryServer::New);

        s_ct.Reset(t);
        t->SetClassName(Nan::New<v8::String>("OscQueryServer").ToLocalChecked());
        t->InstanceTemplate()->SetInternalFieldCount(1);

        target->Set(Nan::New<v8::String>("OscQueryServer").ToLocalChecked(), t->GetFunction());
    }

    OscQueryServer(uint32_t osc, uint32_t ws)
      : m_proto{std::make_unique<ossia::oscquery::oscquery_server_protocol>(osc, ws)}
    {
    }

    ~OscQueryServer()
    {
    }

    static NAN_METHOD(New)
    {
        Nan::HandleScope scope;

        if (!info.IsConstructCall()) {
            return Nan::ThrowTypeError("Use the new operator to create instances of this object.");
        }

        uint32_t osc_port = 1234;
        uint32_t ws_port = 5678;
        if (info.Length() == 2 && info[0]->IsUint32() && info[1]->IsUint32()) {
          osc_port = info[0]->Uint32Value();
          ws_port = info[1]->Uint32Value();
        }

        auto srv = new OscQueryServer{osc_port, ws_port};
        srv->Wrap(info.This());

        info.GetReturnValue().Set(info.This());
    }
};

class Node : public Nan::ObjectWrap
{
public:
    ossia::net::node_base* m_node{};
    static inline Nan::Persistent<v8::FunctionTemplate> s_ct;
    static NAN_MODULE_INIT(Init)
    {
      Nan::HandleScope scope;

      v8::Local<v8::FunctionTemplate> t = Nan::New<v8::FunctionTemplate>(Node::New);

      s_ct.Reset(t);
      t->SetClassName(Nan::New<v8::String>("Node").ToLocalChecked());
      t->InstanceTemplate()->SetInternalFieldCount(1);

      Nan::SetPrototypeMethod(t, "getName", getName);

      constructor().Reset(Nan::GetFunction(t).ToLocalChecked());


      target->Set(Nan::New<v8::String>("Node").ToLocalChecked(), t->GetFunction());
    }

    Node(ossia::net::node_base* n)
      : m_node{n}
    {
    }

    ~Node()
    {
      std::cerr << "Node destoryed\n";
    }

    static NAN_METHOD(New) {
      Nan::HandleScope scope;
      if (info.IsConstructCall()) {
        auto res = to_pointer<ossia::net::node_base>(info[0]);
        Node * obj = new Node(res);
        obj->Wrap(info.This());
        info.GetReturnValue().Set(info.This());
      } else {
        std::cerr << "normal call?\n";
      }
    }


    static NAN_METHOD(getName) {
      Nan::HandleScope scope;
      auto self = Nan::ObjectWrap::Unwrap<Node>(info.This());
      info.GetReturnValue().Set(Nan::New(self->m_node->get_name()).ToLocalChecked());
    }

    static inline Nan::Persistent<v8::Function> & constructor() {
      static Nan::Persistent<v8::Function> my_constructor;
      return my_constructor;
    }
};

class Device : public Nan::ObjectWrap
{
public:
    ossia::net::generic_device m_dev;
    static Nan::Persistent<v8::FunctionTemplate> s_ct;
    static NAN_MODULE_INIT(Init)
    {
        Nan::HandleScope scope;

        v8::Local<v8::FunctionTemplate> t = Nan::New<v8::FunctionTemplate>(Device::New);

        s_ct.Reset(t);
        t->SetClassName(Nan::New<v8::String>("Device").ToLocalChecked());
        t->InstanceTemplate()->SetInternalFieldCount(1);

        Nan::SetPrototypeMethod(t, "getRootNode", GetRootNode);

        target->Set(Nan::New<v8::String>("Device").ToLocalChecked(), t->GetFunction());
    }

    Device(
        std::unique_ptr<ossia::net::protocol_base> protocol_base,
        std::string name)
      : m_dev{std::move(protocol_base), name}
    {
    }

    ~Device()
    {
      std::cerr << "Device destoryed\n";
    }

    static NAN_METHOD(New)
    {
      Nan::HandleScope scope;

      if (!info.IsConstructCall()) {
        return Nan::ThrowTypeError("Use the new operator to create instances of this object.");
      }
      auto wrapper_tpl = OscQueryServer::s_ct.Get(info.GetIsolate());

      if (info.Length() == 2 && wrapper_tpl->HasInstance(info[0]) && info[1]->IsString())
      {
        OscQueryServer* proto = Nan::ObjectWrap::Unwrap<OscQueryServer>(info[0]->ToObject());
        if(proto)
        {
          Nan::Utf8String utf8_value{info[1]};

          auto srv = new Device{std::move(proto->m_proto), std::string(*utf8_value, utf8_value.length())};
          srv->Wrap(info.This());
          info.GetReturnValue().Set(info.This());
        }
      }
    }

    static NAN_METHOD(GetRootNode)
    {
      Nan::HandleScope scope;
      Device* output = Nan::ObjectWrap::Unwrap<Device>(info.This());

      v8::Local<v8::Function> cons = Nan::New(Node::constructor());
      v8::Local<v8::Value> argv[1] = {from_pointer(&output->m_dev.get_root_node()).ToLocalChecked()};
      info.GetReturnValue().Set(Nan::NewInstance(cons, 1, argv).ToLocalChecked());
    }
};

Nan::Persistent<v8::FunctionTemplate> OscQueryServer::s_ct;
Nan::Persistent<v8::FunctionTemplate> Device::s_ct;

extern "C" {
    void init (v8::Handle<v8::Object> target)
    {
        OscQueryServer::Init(target);
        Node::Init(target);
        Device::Init(target);
    }
    NODE_MODULE(ossia, init)
}
