#include "leapmotion_device.hpp"
#include <Leap.h>

#include <ossia/network/oscquery/oscquery_server.hpp>
#include <ossia/network/common/complex_type.hpp>
#include <ossia/network/base/protocol.hpp>
#include <ossia/detail/logger.hpp>
namespace ossia
{
class leapmotion_protocol::leap_listener final
    : public Leap::Listener
{
    using Controller = Leap::Controller;
    using Vector = Leap::Vector;
    ossia::net::device_base& dev;
  public:
    leap_listener(ossia::net::device_base& device)
      : dev{device}
    {

    }

    ossia::vec3f to_value(Vector v)
    {
      return ossia::vec3f{v.x, v.y, v.z};
    }

    void onInit(const Controller&) override { }
    void onConnect(const Controller&) override { }
    void onDisconnect(const Controller&) override { }
    void onExit(const Controller&) override { }
    void onFrame(const Controller& controller) override
    {
      using namespace Leap;
      // Get the most recent frame and report some basic information
      const Frame frame = controller.frame();
      bool left_active = false, right_active = false;
      const auto& hands = frame.hands();
      for (const auto& hand : hands)
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

        handType.pitch.push_value(hand.direction().pitch() * RAD_TO_DEG);
        handType.roll.push_value(hand.palmNormal().roll() * RAD_TO_DEG);
        handType.yaw.push_value(hand.direction().yaw() * RAD_TO_DEG);

        auto arm = hand.arm();
        handType.arm_direction.push_value(to_value(arm.direction()));
        handType.elbow_position.push_value(to_value(arm.elbowPosition()));
        handType.wrist_position.push_value(to_value(arm.wristPosition()));

        // Get fingers
        for (const auto& finger : hand.fingers())
        {
          switch(finger.type())
          {
            case Finger::TYPE_THUMB:
            {
              auto bone = finger.bone(Bone::Type::TYPE_DISTAL);
              handType.thumb_distal_begin.push_value(to_value(bone.prevJoint()));
              handType.thumb_distal_end.push_value(to_value(bone.nextJoint()));
            }
            case Finger::TYPE_INDEX:
            {
              auto bone = finger.bone(Bone::Type::TYPE_DISTAL);
              handType.index_distal_begin.push_value(to_value(bone.prevJoint()));
              handType.index_distal_end.push_value(to_value(bone.nextJoint()));
            }
            case Finger::TYPE_MIDDLE:
            {
              auto bone = finger.bone(Bone::Type::TYPE_DISTAL);
              handType.middle_distal_begin.push_value(to_value(bone.prevJoint()));
              handType.middle_distal_end.push_value(to_value(bone.nextJoint()));
            }
            case Finger::TYPE_RING:
            {
              auto bone = finger.bone(Bone::Type::TYPE_DISTAL);
              handType.ring_distal_begin.push_value(to_value(bone.prevJoint()));
              handType.ring_distal_end.push_value(to_value(bone.nextJoint()));
            }
            case Finger::TYPE_PINKY:
            {
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

    void onFocusGained(const Controller&) override { }
    void onFocusLost(const Controller&) override { }
    void onDeviceChange(const Controller&) override { }
    void onServiceConnect(const Controller&) override { }
    void onServiceDisconnect(const Controller&) override { }
    void onServiceChange(const Controller&) override { }
    void onDeviceFailure(const Controller&) override { }
    void onLogMessage(const Controller&, Leap::MessageSeverity severity, int64_t t, const char* msg) override
    {
      switch (severity)
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

    struct hand {
        ossia::net::node_base& root;
        std::string kind;
        ossia::net::parameter_base& active             {*ossia::create_parameter(root, "/" + kind + "/active", "bool")};

        ossia::net::parameter_base& thumb_distal_begin {*ossia::create_parameter(root, "/" + kind + "/thumb/distal/begin", "xyz")};
        ossia::net::parameter_base& index_distal_begin {*ossia::create_parameter(root, "/" + kind + "/index/distal/begin", "xyz")};
        ossia::net::parameter_base& middle_distal_begin{*ossia::create_parameter(root, "/" + kind + "/middle/distal/begin", "xyz")};
        ossia::net::parameter_base& ring_distal_begin  {*ossia::create_parameter(root, "/" + kind + "/ring/distal/begin", "xyz")};
        ossia::net::parameter_base& pinky_distal_begin {*ossia::create_parameter(root, "/" + kind + "/pinky/distal/begin", "xyz")};
        ossia::net::parameter_base& thumb_distal_end   {*ossia::create_parameter(root, "/" + kind + "/thumb/distal/end", "xyz")};
        ossia::net::parameter_base& index_distal_end   {*ossia::create_parameter(root, "/" + kind + "/index/distal/end", "xyz")};
        ossia::net::parameter_base& middle_distal_end  {*ossia::create_parameter(root, "/" + kind + "/middle/distal/end", "xyz")};
        ossia::net::parameter_base& ring_distal_end    {*ossia::create_parameter(root, "/" + kind + "/ring/distal/end", "xyz")};
        ossia::net::parameter_base& pinky_distal_end   {*ossia::create_parameter(root, "/" + kind + "/pinky/distal/end", "xyz")};

        ossia::net::parameter_base& palm_velocity      {*ossia::create_parameter(root, "/" + kind + "/palm/velocity", "xyz")};
        ossia::net::parameter_base& palm_position      {*ossia::create_parameter(root, "/" + kind + "/palm/position", "xyz")};
        ossia::net::parameter_base& palm_normal        {*ossia::create_parameter(root, "/" + kind + "/palm/normal", "xyz")};
        ossia::net::parameter_base& palm_direction     {*ossia::create_parameter(root, "/" + kind + "/palm/direction", "xyz")};

        ossia::net::parameter_base& grab_strength      {*ossia::create_parameter(root, "/" + kind + "/grab_strength", "float")};

        ossia::net::parameter_base& wrist_position     {*ossia::create_parameter(root, "/" + kind + "/wrist", "xyz")};
        ossia::net::parameter_base& elbow_position     {*ossia::create_parameter(root, "/" + kind + "/elbow", "xyz")};
        ossia::net::parameter_base& arm_direction      {*ossia::create_parameter(root, "/" + kind + "/arm/direction", "xyz")};

        ossia::net::parameter_base& roll               {*ossia::create_parameter(root, "/" + kind + "/roll", "float")};
        ossia::net::parameter_base& pitch              {*ossia::create_parameter(root, "/" + kind + "/pitch", "float")};
        ossia::net::parameter_base& yaw                {*ossia::create_parameter(root, "/" + kind + "/yaw", "float")};
    };

    hand left{dev.get_root_node(), "left"};
    hand right{dev.get_root_node(), "right"};
    ossia::net::parameter_base& num_hands{*ossia::create_parameter(dev.get_root_node(), "/num_hands", "int")};
};

leapmotion_protocol::leapmotion_protocol()
{
}

void leapmotion_protocol::set_device(net::device_base& dev)
{
  listener = std::make_unique<leap_listener>(dev);
  controller = std::make_unique<Leap::Controller>();
  controller->addListener(*listener);
  controller->setPolicy(Leap::Controller::POLICY_ALLOW_PAUSE_RESUME);
}

leapmotion_protocol::~leapmotion_protocol()
{
  if(controller)
    controller->removeListener(*listener);
}

}


bool ossia::leapmotion_protocol::pull(ossia::net::parameter_base&)
{
  return false;
}

bool ossia::leapmotion_protocol::push(const ossia::net::parameter_base&)
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
