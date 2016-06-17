#include "internal/ruby/rb_parameter.hpp"

using namespace leatherman::ruby;

namespace puppet_transaction { namespace ruby {
    rb_parameter::rb_parameter(VALUE value, value_registry* registry)
         : value_wrapper(value, registry) { }

    bool rb_parameter::has_should() const {
        // TODO
        return false;
    }
}}
