/*!
 * \file JamomaAddress.h
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
#include "Address.hpp"
#include "BasicProtocol.h"
#include "BasicDevice.h"
#include <Editor/Value/Value.h>
#include <Misc/CallbackContainer.h>
#include <Editor/Domain.h>

#include <thread>
#include <mutex>

namespace impl
{
class BasicAddress final : public OSSIA::v2::Address2
{
private:
  const OSSIA::v2::Node2&      mNode;
  mutable OSSIA::v2::Protocol2*  mProtocolCache{};

  OSSIA::Value               mValue;
  mutable std::mutex  mValueMutex;

  OSSIA::Type                mValueType{};
  OSSIA::AccessMode          mAccessMode{};
  OSSIA::BoundingMode        mBoundingMode{};
  bool                mRepetitionFilter{};

  std::shared_ptr<OSSIA::Domain>  mDomain;

  OSSIA::v2::ValueCallback       mCallback;
  std::string         mTextualAddress;

public:
  BasicAddress(const OSSIA::v2::Node2& node);

  ~BasicAddress();

  const OSSIA::v2::Node2& getNode() const override;

  void pullValue() override;

  OSSIA::v2::Address2 & pushValue(const OSSIA::Value&) override;
  OSSIA::v2::Address2 & pushValue() override;

  const OSSIA::Value& getValue() const;
  OSSIA::Value cloneValue(DestinationIndex = {}) const override;
  Address2 & setValue(const OSSIA::Value&) override;

  OSSIA::Type getValueType() const override;
  OSSIA::v2::Address2 & setValueType(OSSIA::Type) override;

  OSSIA::AccessMode getAccessMode() const override;
  OSSIA::v2::Address2 & setAccessMode(OSSIA::AccessMode) override;

  const std::shared_ptr<OSSIA::Domain> & getDomain() const override;
  OSSIA::v2::Address2 & setDomain(std::shared_ptr<OSSIA::Domain>) override;

  OSSIA::BoundingMode getBoundingMode() const override;
  OSSIA::v2::Address2 & setBoundingMode(OSSIA::BoundingMode) override;

  bool getRepetitionFilter() const override;
  OSSIA::v2::Address2 & setRepetitionFilter(bool) override;

  OSSIA::v2::Address2::iterator addCallback(OSSIA::v2::ValueCallback) override;
  void removeCallback(OSSIA::v2::Address2::iterator) override;

  OSSIA::v2::Protocol2& getProtocol() const;
  const std::string& getTextualAddress() const
  { return mTextualAddress; }

private:
};
}
