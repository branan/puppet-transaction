#pragma once

#include <puppet-transaction/parameter.hpp>

namespace puppet_transaction {
    class value;

    class property : public virtual parameter {
    public:
        virtual ~property();

        virtual void sync() = 0;
        virtual bool is_in_sync(const value*) const = 0;
    };
}
