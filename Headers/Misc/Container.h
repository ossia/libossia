/*!
 * \file Container.h
 *
 * \author Clément Bossut
 * \author Théo de la Hogue
 *
 * This code is licensed under the terms of the "CeCILL-C"
 * http://www.cecill.info
 */

#pragma once

#include <vector>
#include <memory>

namespace OSSIA {

template <typename T>
using Container = std::vector<std::shared_ptr<T>>;

}
