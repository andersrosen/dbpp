// Copyright (C) 2020 Anders Rosén (panrosen@gmail.com)
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301
// USA

#pragma once

#include <dbpp/config.h>
#include <dbpp/util.h>

#include <utility>

namespace Dbpp::Detail {

template <typename T, typename... Rest>
struct First { using type = T; }; // NOLINT

// Gets the first of the types provided
template <typename... Ts>
using FirstTypeT = typename First<Ts...>::type;

//////////////////////////////////////////////////////////////////////////////

// True if the type pack contains a single type
template <typename... Ts>
inline constexpr bool IsScalarV = (sizeof...(Ts) == 1);

//////////////////////////////////////////////////////////////////////////////

template <bool IsScalar, typename... Ts>
struct ScalarOrTuple;

template <typename... Ts>
struct ScalarOrTuple<true, Ts...>
{
    using type = FirstTypeT<Ts...>; // NOLINT
};

template <typename... Ts>
struct ScalarOrTuple<false, Ts...>
{
    using type = std::tuple<Ts...>; // NOLINT
};

// Expands to T if the type pack contains a single type T
// Expands to std::tuple<Ts...> otherwise
template <typename... Ts>
using ScalarOrTupleT = typename ScalarOrTuple<sizeof...(Ts) == 1, Ts...>::type;

//////////////////////////////////////////////////////////////////////////////

template <typename T1, typename... Ts>
struct IsOneOf {
    const static bool Value = (std::is_same_v<T1, Ts> || ...);
};

// True if T is one of the types in Ts
template <typename T, typename... Ts>
inline constexpr bool IsOneOfV = IsOneOf<T, Ts...>::Value;

} // namespace Dbpp::Detail
