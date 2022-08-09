#include <ossia/detail/config.hpp>
#if defined(OSSIA_PROTOCOL_LEAPMOTION)
#include "leapmotion_device.hpp"

#include <ossia/detail/logger.hpp>
#include <ossia/network/base/protocol.hpp>
#include <ossia/network/common/complex_type.hpp>
#include <ossia/network/context.hpp>
#include <ossia/network/oscquery/oscquery_server.hpp>

#include <Leap.h>
namespace ossia
{
class leapmotion_protocol::leap_listener final : public Leap::Listener
{
  using Controller = Leap::Controller;
  using Vector = Leap::Vector;
  boost::asio::io_context& ctx;
  ossia::net::device_base& dev;

public:
  leap_listener(boost::asio::io_context& ctx, ossia::net::device_base& device)
      : ctx{ctx}
      , dev{device}
  {
    left.init();
    right.init();
  }

  ossia::vec3f to_value(Vector v)
  {
    return ossia::vec3f{v.x, v.y, v.z};
  }

  void onInit(const Controller&) override
  {
    ossia::logger().error("OnInit");
  }
  void onConnect(const Controller&) override
  {
    ossia::logger().error("OnConnect");
  }
  void onDisconnect(const Controller&) override
  {
    ossia::logger().error("onDisconnect");
  }
  void onExit(const Controller&) override
  {
    ossia::logger().error("onExit");
  }

  void processFrame(const Leap::Frame& frame)
  {
    using namespace Leap;
    bool left_active = false, right_active = false;
    const auto& hands = frame.hands();
    for(const auto& hand : hands)
    {
      auto& handType = hand.isLeft() ? this->left : this->right;
      if(hand.isLeft())
        left_active = true;
      else
        right_active = true;

      handType.palm_velocity.push_value(to_value(hand.palmVelocity()));
      handType.palm_position.push_value(to_value(hand.palmPosition()));
      handType.palm_direction.push_value(to_value(hand.direction()));
      handType.palm_normal.push_value(to_value(hand.palmNormal()));

      handType.grab_strength.push_value(hand.grabStrength());

      {
        float p = ossia::wrap(hand.direction().pitch() * RAD_TO_DEG, 0.f, 360.f);
        float r = ossia::wrap(hand.palmNormal().roll() * RAD_TO_DEG, 0.f, 360.f);
        float y = ossia::wrap(hand.direction().yaw() * RAD_TO_DEG, 0.f, 360.f);
        handType.pitch.push_value(p);
        handType.roll.push_value(r);
        handType.yaw.push_value(y);

        handType.orientation.push_value(ossia::vec3f{y, p, r});
      }

      auto arm = hand.arm();
      handType.arm_direction.push_value(to_value(arm.direction()));
      handType.elbow_position.push_value(to_value(arm.elbowPosition()));
      handType.wrist_position.push_value(to_value(arm.wristPosition()));

      // Get fingers
      for(const auto& finger : hand.fingers())
      {
        switch(finger.type())
        {
          case Finger::TYPE_THUMB: {
            auto bone = finger.bone(Bone::Type::TYPE_DISTAL);
            handType.thumb_distal_begin.push_value(to_value(bone.prevJoint()));
            handType.thumb_distal_end.push_value(to_value(bone.nextJoint()));
          }
          case Finger::TYPE_INDEX: {
            auto bone = finger.bone(Bone::Type::TYPE_DISTAL);
            handType.index_distal_begin.push_value(to_value(bone.prevJoint()));
            handType.index_distal_end.push_value(to_value(bone.nextJoint()));
          }
          case Finger::TYPE_MIDDLE: {
            auto bone = finger.bone(Bone::Type::TYPE_DISTAL);
            handType.middle_distal_begin.push_value(to_value(bone.prevJoint()));
            handType.middle_distal_end.push_value(to_value(bone.nextJoint()));
          }
          case Finger::TYPE_RING: {
            auto bone = finger.bone(Bone::Type::TYPE_DISTAL);
            handType.ring_distal_begin.push_value(to_value(bone.prevJoint()));
            handType.ring_distal_end.push_value(to_value(bone.nextJoint()));
          }
          case Finger::TYPE_PINKY: {
            auto bone = finger.bone(Bone::Type::TYPE_DISTAL);
            handType.pinky_distal_begin.push_value(to_value(bone.prevJoint()));
            handType.pinky_distal_end.push_value(to_value(bone.nextJoint()));
          }
        }
      }
    }

    this->num_hands.push_value(hands.count());
    this->left.active.push_value(left_active);
    this->right.active.push_value(right_active);
  }

  void onFrame(const Controller& controller) override
  {
    // Get the most recent frame and report some basic information
    ctx.post([this, frame = controller.frame()] { processFrame(frame); });
  }

  void onFocusGained(const Controller&) override
  {
    ossia::logger().error("onFocusGained");
  }
  void onFocusLost(const Controller&) override
  {
    ossia::logger().error("onFocusLost");
  }
  void onDeviceChange(const Controller&) override
  {
    ossia::logger().error("onDeviceChange");
  }
  void onServiceConnect(const Controller&) override
  {
    ossia::logger().error("onServiceConnect");
  }
  void onServiceDisconnect(const Controller&) override
  {
    ossia::logger().error("onServiceDisconnect");
  }

#if defined(_WIN32)
  void onServiceChange(const Controller&) override
  {
  }
  void onDeviceFailure(const Controller&) override
  {
  }

  void onLogMessage(
      const Controller&, Leap::MessageSeverity severity, int64_t t,
      const char* msg) override
  {

    switch(severity)
    {
      case Leap::MESSAGE_CRITICAL:
        ossia::logger().error("[LeapMotion Critical] {} : {}", t, msg);
        break;
      case Leap::MESSAGE_WARNING:
        ossia::logger().error("[LeapMotion Warning] {} : {}", t, msg);
        break;
      case Leap::MESSAGE_INFORMATION:
        ossia::logger().error("[LeapMotion Info] {} : {}", t, msg);
        break;
      case Leap::MESSAGE_UNKNOWN:
        ossia::logger().error("[LeapMotion Error] {} : {}", t, msg);
    }
  }
#endif

  struct hand
  {
    ossia::net::node_base& root;
    std::string kind;
    ossia::net::parameter_base& active{
        *ossia::create_parameter(root, "/" + kind + "/active", "bool")};

    ossia::net::parameter_base& thumb_distal_begin{
        *ossia::create_parameter(root, "/" + kind + "/thumb/distal/begin", "xyz")};
    ossia::net::parameter_base& index_distal_begin{
        *ossia::create_parameter(root, "/" + kind + "/index/distal/begin", "xyz")};
    ossia::net::parameter_base& middle_distal_begin{
        *ossia::create_parameter(root, "/" + kind + "/middle/distal/begin", "xyz")};
    ossia::net::parameter_base& ring_distal_begin{
        *ossia::create_parameter(root, "/" + kind + "/ring/distal/begin", "xyz")};
    ossia::net::parameter_base& pinky_distal_begin{
        *ossia::create_parameter(root, "/" + kind + "/pinky/distal/begin", "xyz")};
    ossia::net::parameter_base& thumb_distal_end{
        *ossia::create_parameter(root, "/" + kind + "/thumb/distal/end", "xyz")};
    ossia::net::parameter_base& index_distal_end{
        *ossia::create_parameter(root, "/" + kind + "/index/distal/end", "xyz")};
    ossia::net::parameter_base& middle_distal_end{
        *ossia::create_parameter(root, "/" + kind + "/middle/distal/end", "xyz")};
    ossia::net::parameter_base& ring_distal_end{
        *ossia::create_parameter(root, "/" + kind + "/ring/distal/end", "xyz")};
    ossia::net::parameter_base& pinky_distal_end{
        *ossia::create_parameter(root, "/" + kind + "/pinky/distal/end", "xyz")};

    ossia::net::parameter_base& palm_velocity{
        *ossia::create_parameter(root, "/" + kind + "/palm/velocity", "xyz")};
    ossia::net::parameter_base& palm_position{
        *ossia::create_parameter(root, "/" + kind + "/palm/position", "xyz")};
    ossia::net::parameter_base& palm_normal{
        *ossia::create_parameter(root, "/" + kind + "/palm/normal", "xyz")};
    ossia::net::parameter_base& palm_direction{
        *ossia::create_parameter(root, "/" + kind + "/palm/direction", "xyz")};

    ossia::net::parameter_base& grab_strength{
        *ossia::create_parameter(root, "/" + kind + "/grab_strength", "float")};

    ossia::net::parameter_base& wrist_position{
        *ossia::create_parameter(root, "/" + kind + "/wrist", "xyz")};
    ossia::net::parameter_base& elbow_position{
        *ossia::create_parameter(root, "/" + kind + "/elbow", "xyz")};
    ossia::net::parameter_base& arm_direction{
        *ossia::create_parameter(root, "/" + kind + "/arm/direction", "xyz")};

    ossia::net::parameter_base& roll{
        *ossia::create_parameter(root, "/" + kind + "/roll", "float")};
    ossia::net::parameter_base& pitch{
        *ossia::create_parameter(root, "/" + kind + "/pitch", "float")};
    ossia::net::parameter_base& yaw{
        *ossia::create_parameter(root, "/" + kind + "/yaw", "float")};

    ossia::net::parameter_base& orientation{
        *ossia::create_parameter(root, "/" + kind + "/orientation", "euler")};

    void init()
    {
      ossia::vecf_domain<3> box_domain;
      box_domain.min[0] = -200;
      box_domain.min[1] = 0;
      box_domain.min[2] = -200;
      box_domain.max[0] = 200;
      box_domain.max[1] = 500;
      box_domain.max[2] = 200;

      thumb_distal_begin.set_domain(box_domain);
      thumb_distal_end.set_domain(box_domain);
      index_distal_begin.set_domain(box_domain);
      index_distal_end.set_domain(box_domain);
      middle_distal_begin.set_domain(box_domain);
      middle_distal_end.set_domain(box_domain);
      ring_distal_begin.set_domain(box_domain);
      ring_distal_end.set_domain(box_domain);
      pinky_distal_begin.set_domain(box_domain);
      pinky_distal_end.set_domain(box_domain);
      pinky_distal_end.set_domain(box_domain);

      ossia::domain domain_speed = ossia::make_domain(-1000., 1000.f);
      ossia::domain domain_01 = ossia::make_domain(0.f, 1.f);
      ossia::domain domain_minus1_1 = ossia::make_domain(-1.f, 1.f);
      palm_velocity.set_domain(domain_speed);
      palm_position.set_domain(box_domain);
      palm_normal.set_domain(domain_minus1_1);
      palm_direction.set_domain(domain_minus1_1);

      grab_strength.set_domain(domain_01);

      wrist_position.set_domain(box_domain);
      elbow_position.set_domain(box_domain);
      arm_direction.set_domain(domain_minus1_1);

      ossia::domain_base<float> angle_domain;
      angle_domain.min = 0;
      angle_domain.max = 360;

      roll.set_domain(angle_domain);
      yaw.set_domain(angle_domain);
      pitch.set_domain(angle_domain);
      orientation.set_domain(angle_domain);
    }
  };

  hand left{dev.get_root_node(), "left"};
  hand right{dev.get_root_node(), "right"};
  ossia::net::parameter_base& num_hands{
      *ossia::create_parameter(dev.get_root_node(), "/num_hands", "int")};
};

leapmotion_protocol::leapmotion_protocol(ossia::net::network_context_ptr ptr)
    : protocol_base{flags{SupportsMultiplex}}
    , m_context{std::move(ptr)}
{
}

void leapmotion_protocol::set_device(net::device_base& dev)
{
  listener = std::make_unique<leap_listener>(m_context->context, dev);
  controller = std::make_unique<Leap::Controller>();
  controller->addListener(*listener);
#if defined(_WIN32)
  controller->setPolicy(Leap::Controller::POLICY_ALLOW_PAUSE_RESUME);
#endif
  controller->setPolicy(Leap::Controller::POLICY_BACKGROUND_FRAMES);
}

leapmotion_protocol::~leapmotion_protocol()
{
  if(controller && listener)
    controller->removeListener(*listener);
}
}

bool ossia::leapmotion_protocol::pull(ossia::net::parameter_base&)
{
  return false;
}

bool ossia::leapmotion_protocol::push(
    const ossia::net::parameter_base&, const ossia::value& v)
{
  return false;
}

bool ossia::leapmotion_protocol::push_raw(const ossia::net::full_parameter_data&)
{
  return false;
}

bool ossia::leapmotion_protocol::observe(ossia::net::parameter_base&, bool)
{
  return false;
}

bool ossia::leapmotion_protocol::update(ossia::net::node_base& node_base)
{
  return false;
}
#endif
