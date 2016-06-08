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
#include <cassert>
#include <ModernMIDI/midi_input.h>
#include <ModernMIDI/midi_output.h>

using namespace OSSIA;
using namespace std;

using midi_size_t = uint8_t;
struct Channel
{
        // [ note, vel ]
        std::pair<midi_size_t, midi_size_t> mNoteOn;
        std::pair<midi_size_t, midi_size_t> mNoteOff;

        // [ CC, value ]
        std::pair<midi_size_t, midi_size_t> mCC;

        // velocity or value
        std::array<midi_size_t, 127> mNoteOn_N;
        std::array<midi_size_t, 127> mNoteOff_N;
        std::array<midi_size_t, 127> mCC_N;
};

class JamomaMIDI final : public MIDI, public JamomaProtocol
{
# pragma mark -
# pragma mark Implementation specific
    mutable mm::MidiInput mInput;
    mutable mm::MidiOutput mOutput;

    std::array<Channel, 16> mChannels;

    MidiInfo mInfo;
    bool setInfo(MidiInfo) final override;
    MidiInfo getInfo() const final override;
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
        enum class Type : midi_size_t
        {
            NoteOn, // /12/note/on 64 127 -> C3 at 127 velocity
            NoteOn_N, // /12/note/on/64 127 -> C3 at 127 velocity
            NoteOff, // /12/note/off 64 127
            NoteOff_N, // /12/note/off/64 127
            CC, // /12/CC 64 123
            CC_N // /12/CC/64 123
        };

        OSSIA::Value::Type matchingType()
        {
            switch(type)
            {
                case Type::NoteOn:
                case Type::NoteOff:
                case Type::CC:
                    return OSSIA::Value::Type::TUPLE;
                case Type::NoteOn_N:
                case Type::NoteOff_N:
                case Type::CC_N:
                    return OSSIA::Value::Type::INT;
            }
            return {};
        }

        OSSIA::Value* defaultValue(midi_size_t val)
        {
            switch(type)
            {
                case Type::NoteOn:
                case Type::NoteOff:
                case Type::CC:
                    return new OSSIA::Tuple{new OSSIA::Int{val}, new OSSIA::Int{val}};
                case Type::NoteOn_N:
                case Type::NoteOff_N:
                case Type::CC_N:
                    return new OSSIA::Int{val};
            }
            return {};
        }

        std::shared_ptr<OSSIA::Domain> defaultDomain()
        {
            return OSSIA::Domain::create(
                        defaultValue(0),
                        defaultValue(127)
                        );
        }

        MIDIAddressInfo(Type t):
            type{t}
        {

        }

        MIDIAddressInfo(Type t, midi_size_t n):
            type{t},
            note{n}
        {

        }

        MIDIAddressInfo(midi_size_t chan, Type t, midi_size_t n):
            channel{chan},
            type{t},
            note{n}
        {

        }
        midi_size_t channel{};
        Type type{};
        midi_size_t note{};
};

class MIDIAddress final : public OSSIA::Address
{
        MIDIAddressInfo mInfo;
        weak_ptr<OSSIA::Node> mParent;
        weak_ptr<JamomaMIDI> mProtocol;
        std::shared_ptr<OSSIA::Domain> mDomain;

        OSSIA::Value::Type mType;
        std::unique_ptr<OSSIA::Value> mValue;
    public:
        MIDIAddress(MIDIAddressInfo info, std::shared_ptr<OSSIA::Node> parent):
            mInfo{info},
            mParent{parent},
            mProtocol{dynamic_pointer_cast<JamomaMIDI>(parent->getDevice()->getProtocol())},
            mDomain{mInfo.defaultDomain()},
            mValue{mInfo.defaultValue(64)}
        {
            assert(mProtocol.lock());
        }

        const MIDIAddressInfo& info() const
        { return mInfo; }

        const std::shared_ptr<Node> getNode() const override
        {
            return mParent.lock();
        }

        const OSSIA::Value* pullValue() override
        {
            mProtocol.lock()->pullAddressValue(*this);
            return getValue();
        }

        Address& pushValue(const Value* val) override
        {
            mValue.reset(val->clone());
            mProtocol.lock()->pushAddressValue(*this);
            return *this;
        }


        const OSSIA::Value* getValue() const override
        {
            return mValue.get();
        }

        OSSIA::Value* cloneValue(std::vector<char>) const override
        {
            return mValue->clone();
        }

        Address& setValue(const Value* v) override
        {
            mValue.reset(v->clone());
            return *this;
        }


        OSSIA::Value::Type getValueType() const override
        {
            return mType;
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
                midi_size_t note,
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

        void init(midi_size_t channel, midi_size_t note)
        {
            mAddress = std::make_shared<MIDIAddress>(
                        MIDIAddressInfo{channel, MIDIAddressInfo::Type::NoteOn_N, note},
                        getThis());
        }
};
class MIDINoteOff_N final : public MIDINodeImpl
{
        const std::string mName;
    public:
        MIDINoteOff_N(
                midi_size_t note,
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

        void init(midi_size_t channel, midi_size_t note)
        {
            mAddress = std::make_shared<MIDIAddress>(
                        MIDIAddressInfo{channel, MIDIAddressInfo::Type::NoteOff_N, note},
                        getThis());
        }
};
class MIDI_CC_N final : public MIDINodeImpl
{
        const std::string mName;
    public:
        MIDI_CC_N(
                midi_size_t param,
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

        void init(midi_size_t channel, midi_size_t param)
        {
            mAddress = std::make_shared<MIDIAddress>(
                        MIDIAddressInfo{channel, MIDIAddressInfo::Type::CC_N, param},
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

        void init(midi_size_t channel)
        {
            mAddress = std::make_shared<MIDIAddress>(
                        MIDIAddressInfo{channel, MIDIAddressInfo::Type::NoteOn, 0},
                        getThis());

            for(int i = 0; i < 127; i++)
            {
                auto ptr = std::make_shared<MIDINoteOn_N>(i, mDevice, this->getThis());
                ptr->init(channel, i);
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

        void init(midi_size_t channel)
        {
            mAddress = std::make_shared<MIDIAddress>(
                        MIDIAddressInfo{channel, MIDIAddressInfo::Type::NoteOff, 0},
                        getThis());

            for(int i = 0; i < 127; i++)
            {
                auto ptr = std::make_shared<MIDINoteOff_N>(i, mDevice, this->getThis());
                ptr->init(channel, i);
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

        void init(midi_size_t channel)
        {
            mAddress = std::make_shared<MIDIAddress>(
                        MIDIAddressInfo{channel, MIDIAddressInfo::Type::CC, 0},
                        getThis());

            for(int i = 0; i < 127; i++)
            {
                auto ptr = std::make_shared<MIDI_CC_N>(i, mDevice, this->getThis());
                ptr->init(channel, i);
                m_children.push_back(std::move(ptr));
            }
        }
};

class MIDIChannel final : public MIDINodeImpl
{
        const midi_size_t mChannel;
        const std::string mName;
    public:
        MIDIChannel(midi_size_t channel, weak_ptr<Device> aDevice):
            MIDINodeImpl(aDevice, aDevice),
            mChannel{channel},
            mName(std::to_string(channel))
        {
        }

        std::string getName() const final override
        {
            return mName;
        }

        void init()
        {
            {
                auto non = std::make_shared<MIDINoteOn>(mDevice);
                non->init(mChannel);
                m_children.push_back(std::move(non));
            }
            {
                auto noff = std::make_shared<MIDINoteOff>(mDevice);
                noff->init(mChannel);
                m_children.push_back(std::move(noff));
            }
            {
                auto cc = std::make_shared<MIDI_CC>(mDevice);
                cc->init(mChannel);
                m_children.push_back(std::move(cc));
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

                for(int i = 1; i <= 16; i++)
                {
                    auto ptr = std::make_shared<MIDIChannel>(i, dev_ptr);
                    ptr->init();
                    m_children.push_back(std::move(ptr));
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
