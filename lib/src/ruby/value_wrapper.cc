#include "internal/ruby/value_wrapper.hpp"

using namespace leatherman::ruby;

namespace puppet_transaction { namespace ruby {
    value_wrapper::value_wrapper(VALUE self, value_registry* registry)
         : _self(self), _registry(registry) { }

    value_wrapper::~value_wrapper()
    {
        _registry->del(_self);
    }
}}
