#include "internal/ruby/rb_parameter.hpp"

using namespace leatherman::ruby;

namespace puppet_transaction { namespace ruby {
    rb_parameter::rb_parameter(VALUE value, value_registry* registry)
         : value_wrapper(value, registry) { }

    bool rb_parameter::has_should() const {
        auto& ruby = api::instance();

        auto should_fun = ruby.rb_intern("should");
        auto should = ruby.rb_funcall(_self, should_fun, 0);
        if (should == ruby.nil_value()) {
            return false;
        } else {
            return true;
        }
    }
}}
