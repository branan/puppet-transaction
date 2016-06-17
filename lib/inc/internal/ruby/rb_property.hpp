#pragma once

#include <puppet-transaction/property.hpp>
#include "internal/ruby/rb_parameter.hpp"

namespace puppet_transaction { namespace ruby {
    class rb_property : public property, public rb_parameter {
    public:
        rb_property(leatherman::ruby::VALUE, value_registry*);
        void sync() override;
        bool is_in_sync(const value*) const override;
    };
}}
