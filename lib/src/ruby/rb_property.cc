#include "internal/ruby/rb_property.hpp"

using namespace leatherman::ruby;

namespace puppet_transaction { namespace ruby {
    rb_property::rb_property(VALUE value, value_registry* registry)
         : rb_parameter(value, registry) {}

    void rb_property::sync() {
        auto& ruby = api::instance();
        auto sync_fun = ruby.rb_intern("sync");
        ruby.rb_funcall(_self, sync_fun, 0);
    }

    bool rb_property::is_in_sync(const value* current) const {
        auto& ruby = api::instance();
        auto cur = dynamic_cast<const value_wrapper*>(current);
        if (!cur) {
            return false;
        }

        auto name_fun = ruby.rb_intern("name");
        auto key = ruby.rb_funcall(_self, name_fun, 0);
        auto the_hash = cur->self();
        auto value = ruby.rb_hash_lookup(the_hash, key);

        auto insync_fun = ruby.rb_intern("insync?");
        auto result = ruby.rb_funcall(_self, insync_fun, 1, value);
        return result == ruby.true_value();
    }
}}
