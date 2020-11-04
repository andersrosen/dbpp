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

#include <stdexcept>

namespace Dbpp {
    /// Generic exception class for the dbpp library. All exceptions thrown by
    /// the library itself will be Dbpp::Error or a subclass. Other exceptions
    /// might be thrown by the use of the standard library.
    class Error : public std::runtime_error {
        using std::runtime_error::runtime_error;
    };

    /// This class is used when a driver encounters a problem for which there
    /// is a driver specific error code that the client of this library might
    /// want to use when handling the exception
    class ErrorWithCode : public Error {
    public:
        /// The error code from the underlying driver implementation.
        const long long code;

        /// Constructor
        ///
        /// \param code Driver-specific error code
        /// \param message A description of the exception
        ErrorWithCode(long long code, const std::string &message)
        : Error(message), code(code)
        {}
    };

    /// This exception is thrown if the client tries to bind a value to a
    /// placeholder that doesn't exist
    class PlaceholderOutOfRange : public Error {
        using Error::Error;
    };
}
