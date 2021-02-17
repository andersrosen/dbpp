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
#include <dbpp/exports.h>
#include <dbpp/util.h>

#include <stdexcept>
#include <string>

namespace Dbpp {
    /// \brief Generic exception class for the dbpp library
    ///
    /// All exceptions thrown by the library itself will be Dbpp::Error or a
    /// subclass of it. Other exceptions might be thrown by the use of the
    /// standard library, or the underlying database driver.
    ///
    /// \since v1.0.0
    class DBPP_EXPORT Error : public std::runtime_error {
        using std::runtime_error::runtime_error;
    };

    /// \brief Exception class with a driver-specific numeric code
    ///
    /// This class is used when a driver encounters a problem for which there
    /// is a driver specific error code that the client of this library might
    /// want to use when handling the exception
    ///
    /// \since v1.0.0
    class DBPP_EXPORT ErrorWithCode : public Error {
    public:
        /// The error code from the underlying driver implementation.
        ///
        /// \since v1.0.0
        const long long code;

        /// Constructor
        ///
        /// \param errorCode Driver-specific error code
        /// \param message A description of the exception
        ///
        /// \since v1.0.0
        ErrorWithCode(long long errorCode, const std::string &message)
        : Error(message), code(errorCode)
        {}
    };

    /// \brief Thrown if the client tries to bind too few parameters to a statement
    ///
    /// \since v1.0.0
    class DBPP_EXPORT TooFewParametersProvided : public Error {
        using Error::Error;

        [[nodiscard]]
        const char* what() const noexcept override  {
            static std::string msg;
            msg = std::string(Error::what()) + ": Too few parameters were provided";
            return msg.c_str();
        }
    };

    /// \brief Thrown if the client tries to bind too many parameters to a statement
    ///
    /// \since v1.0.0
    class DBPP_EXPORT TooManyParametersProvided : public Error {
        using Error::Error;

        [[nodiscard]]
        const char* what() const noexcept override  {
            static std::string msg;
            msg = std::string(Error::what()) + ": Too many parameters were provided";
            return msg.c_str();
        }
    };

    /// \brief Thrown if the client tries to bind a value that's not supported by the database
    ///
    /// \since v1.0.0
    class DBPP_EXPORT UnsupportedDataToBind : public Error {
        using Error::Error;

        [[nodiscard]]
        const char* what() const noexcept override  {
            static std::string msg;
            msg = std::string(std::string{"Could not bind the provided value as a statement parameter: "} + Error::what());
            return msg.c_str();
        }
    };

} // namespace Dbpp
