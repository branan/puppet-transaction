#pragma once

#include <leatherman/ruby/api.hpp>
#include <map>
#include <memory>

namespace puppet_transaction { namespace ruby {
    class value_wrapper;
    class value_registry {
    public:
        void add(leatherman::ruby::VALUE, std::weak_ptr<value_wrapper>);
        void del(leatherman::ruby::VALUE);
        std::shared_ptr<value_wrapper> get(leatherman::ruby::VALUE) const;
        
        void mark() const;
    private:
        std::map<leatherman::ruby::VALUE, std::weak_ptr<value_wrapper>> _values;
    };
}}
