#include "internal/ruby/value_wrapper.hpp"

using namespace leatherman::ruby;

namespace puppet_transaction { namespace ruby {
    value_wrapper::value_wrapper(VALUE self)
         : _self(self) {}

    void value_wrapper::mark() const
    {
        auto& ruby = api::instance();
        ruby.rb_gc_mark(_self);
    }
}}
