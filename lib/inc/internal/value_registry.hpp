#pragma once

#include <leatherman/ruby/api.hpp>

namespace puppet_transaction { namespace ruby {
    class value_wrapper
    class value_registry {
    public:
        void register(leatherman::ruby::VALUE, std::weak_ptr<value_wrapper>);
        void unregister(leatherman::ruby::VALUE);
        std::shared_ptr<value_wrapper> get() const;
    };
}}
