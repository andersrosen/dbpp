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

#ifndef DBPP_EXPORTED
#   if defined(__WIN32__) || defined(__WINRT__)
#       ifdef BUILDING_DBPP_DLL
#           define DBPP_EXPORTED __declspec(dllexport)
#       else
#           define DBPP_EXPORTED __declspec(dllimport)
#       endif
#   else
#       define DBPP_EXPORTED
#   endif
#endif

/// The version of the dbpp library.
/// Format: ABBCC Where A = Major, BB = Minor, CC = Revision
#define DBPP_VERSION 10000
