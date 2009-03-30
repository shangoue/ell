// This file is part of Ell library.
//
// Ell library is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Ell library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with Ell library.  If not, see <http://www.gnu.org/licenses/>.

#ifndef __SIMPLE_SYSTEM_SMARTPTR_H__
#define __SIMPLE_SYSTEM_SMARTPTR_H__

#include "System/Exception.h"

namespace System
{
    /// Smart pointer exception
    DeclareError(SmartPtrError);

    /// GC Monitor interface
    struct GCMonitor
    {
        virtual ~GCMonitor() {}
        virtual void collect() = 0;
    };

    /// Reference counter
    class RefCount
    {
    public:

        /// Constructor
        RefCount()
            : count_(0) , monitor_(NULL)
        {
        }

        /// Destructor
        virtual ~RefCount()
        {
            if(monitor_) monitor_->collect();
        }

        /// Add a reference
        void addRef()
        {
            count_++;
        }

        /// Release a reference, destroy the object if no reference left
        void release()
        {
            count_--;
            if(count_ == 0)
                delete this;
        }

        /// Get reference counter value
        uint refCount() const
        { return count_; }

        /// Set GC function
        void setMonitor(GCMonitor *monitor)
        { monitor_ = monitor; }

    protected:
        // counter
        mutable uint count_;

    private:
        // monitor
        GCMonitor *monitor_;
    };

    /// Smart pointer class
    template <typename T>
    class SmartPtr
    {
    public:

        /// Constructor
        SmartPtr()
        {
            ptr_ = NULL;
        }

        //@{
        /// Copy constructor
        SmartPtr(const SmartPtr &p)
        {
            ptr_ = p.ptr_;
            if(ptr_)
                ptr_->addRef();
        }

        template <typename Y>
        SmartPtr(const SmartPtr<Y> &other)
        {
            ptr_ = other.ptr_;
            if(ptr_)
                ptr_->addRef();
        }

        SmartPtr(const T *p)
        {
            ptr_ = (T*)p;
            if(ptr_)
                ptr_->addRef();
        }
        //@}

        /// Destructor
        ~SmartPtr()
        {
            reset();
        }

        //@{
        /// Copy operator
        SmartPtr &operator=(const SmartPtr &p)
        {
            reset();
            ptr_ = p.ptr_;
            if(ptr_)
                ptr_->addRef();
            return *this;
        }

        template <typename Y>
        SmartPtr &operator=(const SmartPtr<Y> &other)
        {
            reset();
            ptr_ = other.ptr_;
            if(ptr_)
                ptr_->addRef();
            return *this;
        }

        SmartPtr &operator=(const T *p)
        {
            reset();
            ptr_ = (T*)p;
            if(ptr_)
                ptr_->addRef();
            return *this;
        }
        //@}

        /// Dynamic cast
        template <typename Y>
        SmartPtr &dynamicCast(const SmartPtr<Y> &other)
        {
            reset();
            ptr_ = dynamic_cast<T*>(other.ptr_);
            if(ptr_ == NULL)
                RaiseError(SmartPtrError, "Dynamic cast failed");
            ptr_->addRef();
            return *this;
        }

        /// Member access
        T *operator->()
        {
            Assert(ptr_ != 0);
            return ptr_;
        }

        /// Member access (const)
        T *operator->() const
        {
            Assert(ptr_ != NULL);
            return ptr_;
        }

        /* Dangerous!
        /// Raw pointer conversion
        operator T*()
        {
            return ptr_;
        }
        */

        /// Raw pointer conversion (const)
        operator const T*() const
        {
            return ptr_;
        }

        /// Returns internal pointer
        T *ptr()
        {
            return ptr_;
        }

        /// Returns internal pointer (const)
        const T *ptr() const
        {
            return ptr_;
        }

        /// Reset
        void reset()
        {
            if(ptr_)
            {
                ptr_->release();
                ptr_ = NULL;
            }
        }

        //@{
        /// Boolean operator
        operator bool() const
        {
            return ptr_ != NULL;
        }

        bool operator!() const
        {
            return ptr_ == NULL;
        }

        bool operator==(const SmartPtr &p) const
        {
            return ptr_ == p.ptr_;
        }

        bool operator!=(const SmartPtr &p) const
        {
            return ptr_ != p.ptr_;
        }
        //@}

    private:

        // internal pointer
        T *ptr_;

        // Allow access to other smart pointer types
        template <typename Other> friend class SmartPtr;
    };

} // namespace System

#endif // #ifndef __SIMPLE_SYSTEM_SMARTPTR_H__
