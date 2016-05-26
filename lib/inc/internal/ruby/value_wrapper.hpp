#pragma once

#include <leatherman/ruby/api.hpp>

namespace puppet_transaction { namespace ruby {

    class value_wrapper {
    public:
        value_wrapper(leatherman::ruby::VALUE);

        leatherman::ruby::VALUE self() const { return _self; }
        void mark() const;
    private:
        leatherman::ruby::VALUE _self;
    };
}}
