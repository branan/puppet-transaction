#pragma once

#include "value_wrapper.hpp"

#include <puppet-transaction/catalog.hpp>

namespace puppet_transaction { namespace ruby {

    class rb_catalog : public value_wrapper, public catalog {
    public:
        rb_catalog(leatherman::ruby::VALUE self);
        ~rb_catalog();
    };
}}
