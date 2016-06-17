#pragma once

#include <puppet-transaction/parameter.hpp>
#include "internal/ruby/value_wrapper.hpp"

namespace puppet_transaction { namespace ruby {
    class rb_parameter : public virtual parameter, public value_wrapper {
    public:
        rb_parameter(leatherman::ruby::VALUE, value_registry*);
        bool has_should() const override;
    };
}}
