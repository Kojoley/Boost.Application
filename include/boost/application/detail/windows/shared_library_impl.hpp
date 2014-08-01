// shared_library_impl.hpp ---------------------------------------------------//
// -----------------------------------------------------------------------------

// Copyright 2011-2012 Renato Tegon Forti
// Copyright 2014 Renato Tegon Forti, Antony Polukhin.

// Distributed under the Boost Software License, Version 1.0.
// See http://www.boost.org/LICENSE_1_0.txt

// -----------------------------------------------------------------------------

// Revision History
// 05-04-2012 dd-mm-yyyy - Initial Release

// -----------------------------------------------------------------------------

#ifndef BOOST_APPLICATION_SHARED_LIBRARY_IMPL_HPP
#define BOOST_APPLICATION_SHARED_LIBRARY_IMPL_HPP

#include <boost/application/config.hpp>
#include <boost/application/shared_library_initializers.hpp>
#include <boost/application/shared_library_load_mode.hpp>

#include <boost/noncopyable.hpp>
#include <boost/filesystem/path.hpp>

namespace boost { namespace application {

    class shared_library_impl : noncopyable {
    public:
        shared_library_impl()
            : handle_(NULL)
        {}

        ~shared_library_impl() {
            unload();
        }

        static shared_library_load_mode default_mode() {
            return 0;
        }

        void load(const library_path &sh, shared_library_load_mode mode, boost::system::error_code &ec) {
            unload();

            DWORD flags = static_cast<DWORD>(mode);
#if defined(BOOST_APPLICATION_STD_WSTRING)
            handle_ = LoadLibraryEx(path_.wstring().c_str(), 0, flags);
#else
            handle_ = LoadLibraryEx(path_.string().c_str(), 0, flags);
#endif
            if (!handle_) {
                ec = boost::application::last_error_code();
            }
        }

        bool is_loaded() const {
            return (handle_ != 0);
        }

        void unload() {
            if (handle_) {
                FreeLibrary((HMODULE) handle_);
                handle_ = NULL;
            }
        }

        static character_types::string_type suffix() {
#if defined(BOOST_APPLICATION_STD_WSTRING)
            return character_types::string_type(L".dll");
#else
            return character_types::string_type(".dll");
#endif
        }

        void* symbol_addr(const symbol_type &sb, boost::system::error_code &ec) const {
            // Judging by the documentation and
            // at GetProcAddress there is no version for UNICODE.
            // There can be it and is correct, as in executed
            // units names of functions are stored in narrow characters.
         
            if (handle_) {
                return (void*) GetProcAddress((HMODULE) handle_, sb.c_str());
            } else {
                ec = boost::application::last_error_code();
            }

            return NULL;
        }

    private:
        void* handle_;
    };

}} // boost::application

#endif // BOOST_APPLICATION_SHARED_LIBRARY_IMPL_HPP

