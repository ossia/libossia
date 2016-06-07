/*!
 * \file JamomaMIDI.h
 *
 * \brief
 *
 * \details
 *
 * \author Théo de la Hogue
 *
 * \copyright This code is licensed under the terms of the "CeCILL-C"
 * http://www.cecill.info
 */

#pragma once

#include "Network/Protocol/MIDI.h"

#include "Network/JamomaProtocol.h"
#include "Editor/Domain.h"
#include "Editor/Value.h"
#include "Network/AddressProperties.h"
#include "Network/Address.h"
using namespace OSSIA;
using namespace std;

class JamomaMIDI final : public MIDI, public JamomaProtocol
{

private:

# pragma mark -
# pragma mark Implementation specific

public:

# pragma mark -
# pragma mark Life cycle

  JamomaMIDI();

  ~JamomaMIDI();

# pragma mark -
# pragma mark Operation

  bool pullAddressValue(Address&) const override;

  bool pushAddressValue(const Address&) const override;

  bool observeAddressValue(shared_ptr<Address>, bool) const override;

  bool updateChildren(Node& node) const override;

  /*! to see IPs of connected Midi devices
   \todo add options */
  vector<MidiInfo> scan() override;
};


class MIDINodeImpl :
        public virtual OSSIA::Node,
        public enable_shared_from_this<MIDINodeImpl>
{
    protected:
        weak_ptr<Device>      mDevice;
        weak_ptr<OSSIA::Node> mParent;
        shared_ptr<Address>   mAddress;

    public:
        using iterator = OSSIA::Container<OSSIA::Node>::iterator;
        using const_iterator = OSSIA::Container<OSSIA::Node>::const_iterator;
        MIDINodeImpl() = default;
        MIDINodeImpl(
                weak_ptr<Device> aDevice,
                weak_ptr<OSSIA::Node> aParent):
            mDevice{std::move(aDevice)},
            mParent{std::move(aParent)}
        {

        }

        std::shared_ptr<Device> getDevice() const final override
        {
            return mDevice.lock();
        }

        std::shared_ptr<Node> getParent() const final override
        {
            return mParent.lock();
        }

        std::shared_ptr<Node> getThis() final override
        {
            return shared_from_this();
        }

        Node& setName(string) override
        {
            return *this;
        }

        std::shared_ptr<Address> getAddress() const final override
        {
            return mAddress;
        }

        std::shared_ptr<Address> createAddress(Value::Type) final override
        {
            return mAddress;
        }

        bool removeAddress() final override
        {
            return false;
        }

        iterator emplace(const_iterator, string) final override
        {
            return {};
        }

        iterator emplace(const_iterator,
                         const string&,
                         Value::Type,
                         AccessMode,
                         const std::shared_ptr<Domain>&,
                         BoundingMode,
                         bool repetitionFilter) final override
        {
            return {};
        }

        iterator insert(const_iterator, std::shared_ptr<Node>, string) final override
        {
            return {};
        }

        iterator erase(const_iterator) final override
        {
            return {};
        }
};

struct MIDIAddressInfo
{
        enum class Type
        {
            NoteOn, // /note/on 64 127 -> C3 at 127 velocity
            NoteOff, // /note/off 64 127
            NoteOn_N, // /note/on/64 127 -> C3 at 127 velocity
            NoteOff_N, // /note/off/64 127
            CC, // /CC 64 123
            CC_N // /CC/64 123
        };

        MIDIAddressInfo(Type t):
            type{t}
        {

        }

        MIDIAddressInfo(Type t, int n):
            type{t},
            note{n}
        {

        }

        Type type{};
        int note{};
};

class MIDIAddress final : public OSSIA::Address
{
        MIDIAddressInfo mInfo;
        weak_ptr<OSSIA::Node> mParent;
        std::shared_ptr<OSSIA::Domain> mDomain;

    public:
        MIDIAddress(MIDIAddressInfo info, std::weak_ptr<OSSIA::Node> parent):
            mInfo{info},
            mParent{parent},
            mDomain{OSSIA::Domain::create(new OSSIA::Int{0}, new OSSIA::Int{127})}
        {

        }

        const std::shared_ptr<Node> getNode() const override
        {
            return mParent.lock();
        }

        const OSSIA::Value* pullValue() override
        {
            return nullptr;
        }

        Address& pushValue(const Value*) override
        {
            return *this;
        }


        const OSSIA::Value* getValue() const override
        {
            return nullptr;
        }

        OSSIA::Value* cloneValue(std::vector<char>) const override
        {
            return nullptr;
        }

        Address&setValue(const Value*) override
        {
            return *this;
        }


        OSSIA::Value::Type getValueType() const override
        {
            return OSSIA::Value::Type::INT;
        }

        Address& setValueType(OSSIA::Value::Type) override
        {
            return *this;
        }


        OSSIA::AccessMode getAccessMode() const override
        {
            return OSSIA::AccessMode::BI;
        }

        Address& setAccessMode(OSSIA::AccessMode) override
        {
            return *this;
        }


        const std::shared_ptr<OSSIA::Domain>& getDomain() const override
        {
            return mDomain;
        }

        Address& setDomain(std::shared_ptr<OSSIA::Domain>) override
        {
            return *this;
        }


        OSSIA::BoundingMode getBoundingMode() const override
        {
            return OSSIA::BoundingMode::CLIP;
        }

        Address&setBoundingMode(OSSIA::BoundingMode) override
        {
            return *this;
        }

        bool getRepetitionFilter() const override
        {
            return false;
        }

        Address&setRepetitionFilter(bool) override
        {
            return *this;
        }
};

class MIDINoteOn_N final : public MIDINodeImpl
{
        const std::string mName;
    public:
        MIDINoteOn_N(
                int note,
                weak_ptr<Device> aDevice,
                weak_ptr<OSSIA::Node> aParent):
            MIDINodeImpl(std::move(aDevice), std::move(aParent)),
            mName{std::to_string(note)}
        {
        }

        std::string getName() const final override
        {
            return mName;
        }

        void init(int note)
        {
            mAddress = std::make_shared<MIDIAddress>(
                        MIDIAddressInfo{MIDIAddressInfo::Type::NoteOn_N, note},
                        getThis());
        }
};
class MIDINoteOff_N final : public MIDINodeImpl
{
        const std::string mName;
    public:
        MIDINoteOff_N(
                int note,
                weak_ptr<Device> aDevice,
                weak_ptr<OSSIA::Node> aParent):
            MIDINodeImpl(std::move(aDevice), std::move(aParent)),
            mName{std::to_string(note)}
        {
        }

        std::string getName() const final override
        {
            return mName;
        }

        void init(int note)
        {
            mAddress = std::make_shared<MIDIAddress>(
                        MIDIAddressInfo{MIDIAddressInfo::Type::NoteOff_N, note},
                        getThis());
        }
};
class MIDI_CC_N final : public MIDINodeImpl
{
        const std::string mName;
    public:
        MIDI_CC_N(
                int param,
                weak_ptr<Device> aDevice,
                weak_ptr<OSSIA::Node> aParent):
            MIDINodeImpl(std::move(aDevice), std::move(aParent)),
            mName{std::to_string(param)}
        {
        }

        std::string getName() const final override
        {
            return mName;
        }

        void init(int param)
        {
            mAddress = std::make_shared<MIDIAddress>(
                        MIDIAddressInfo{MIDIAddressInfo::Type::CC_N, param},
                        getThis());
        }
};

class MIDINoteOn final : public MIDINodeImpl
{
    public:
        MIDINoteOn(weak_ptr<Device> aDevice):
            MIDINodeImpl(aDevice, aDevice)
        {
        }

        std::string getName() const final override
        {
            return "on";
        }

        void init()
        {
            mAddress = std::make_shared<MIDIAddress>(
                        MIDIAddressInfo{MIDIAddressInfo::Type::NoteOn},
                        getThis());

            for(int i = 0; i < 127; i++)
            {
                auto ptr = std::make_shared<MIDINoteOn_N>(i, mDevice, this->getThis());
                ptr->init(i);
                m_children.push_back(std::move(ptr));
            }
        }
};


class MIDINoteOff final : public MIDINodeImpl
{
    public:
        MIDINoteOff(weak_ptr<Device> aDevice):
            MIDINodeImpl(aDevice, aDevice)
        {
        }

        std::string getName() const final override
        {
            return "off";
        }

        void init()
        {
            mAddress = std::make_shared<MIDIAddress>(
                        MIDIAddressInfo{MIDIAddressInfo::Type::NoteOff},
                        getThis());

            for(int i = 0; i < 127; i++)
            {
                auto ptr = std::make_shared<MIDINoteOff_N>(i, mDevice, this->getThis());
                ptr->init(i);
                m_children.push_back(std::move(ptr));
            }
        }
};


class MIDI_CC final : public MIDINodeImpl
{
    public:
        MIDI_CC(weak_ptr<Device> aDevice):
            MIDINodeImpl(aDevice, aDevice)
        {
        }

        std::string getName() const final override
        {
            return "CC";
        }

        void init()
        {
            mAddress = std::make_shared<MIDIAddress>(
                        MIDIAddressInfo{MIDIAddressInfo::Type::CC},
                        getThis());

            for(int i = 0; i < 127; i++)
            {
                auto ptr = std::make_shared<MIDI_CC_N>(i, mDevice, this->getThis());
                ptr->init(i);
                m_children.push_back(std::move(ptr));
            }
        }
};
class MIDIDeviceImpl :
        public OSSIA::Device,
        public MIDINodeImpl
{
        std::string mName;
        std::shared_ptr<Protocol> mProtocol;
    public:
        MIDIDeviceImpl(std::shared_ptr<Protocol> prot):
            mProtocol{prot}
        {
        }

        string getName() const override
        { return mName; }

        Node & setName(std::string n) override
        {
            mName = n;
            return *this;
        }


        std::shared_ptr<Protocol> getProtocol() const override
        {
            return mProtocol;
        }

        bool updateNamespace() override
        {
            m_children.clear();

            try {
                auto ptr = shared_from_this();
                auto dev_ptr = std::dynamic_pointer_cast<OSSIA::Device>(ptr);

                {
                    auto non = std::make_shared<MIDINoteOn>(dev_ptr);
                    non->init();
                    m_children.push_back(std::move(non));
                }
                {
                    auto noff = std::make_shared<MIDINoteOff>(dev_ptr);
                    noff->init();
                    m_children.push_back(std::move(noff));
                }
                {
                    auto cc = std::make_shared<MIDI_CC>(dev_ptr);
                    cc->init();
                    m_children.push_back(std::move(cc));
                }
            }
            catch(...)
            {
                std::cerr << "refresh failed\n";
                return false;
            }
            return true;
        }
};
