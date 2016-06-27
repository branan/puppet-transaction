#include "internal/ruby/rb_resource.hpp"

#include "internal/ruby/rb_parameter.hpp"
#include "internal/ruby/rb_property.hpp"
#include <puppet-transaction/parameter.hpp>
#include <puppet-transaction/value.hpp>

#include <leatherman/logging/logging.hpp>

using namespace leatherman::ruby;
using namespace std;

static vector<string> get_dep_param(VALUE res, string key) {
    auto& ruby = api::instance();
    auto param_fun = ruby.rb_intern("parameter");
    auto value_fun = ruby.rb_intern("value");

    auto param = ruby.rb_funcall(res, param_fun, 1, ruby.utf8_value(key));
    if (param == ruby.nil_value()) {
        // missing dependency params can be interpreted as simply no dependencies
        return {};
    }
    auto value = ruby.rb_funcall(param, value_fun, 0);
    
    vector<string> result;
    ruby.array_for_each(value, [&](VALUE v) {
        
        if (!ruby.is_string(v)) {
            auto ref_fun = ruby.rb_intern("ref");
            v = ruby.rb_funcall(v, ref_fun, 0);
        }
        result.push_back(ruby.to_string(v));
        return true;
    });
    return result;
}

namespace puppet_transaction { namespace ruby {
    rb_resource::rb_resource(VALUE self, value_registry* registry)
         : value_wrapper(self, registry) {}

    rb_resource::~rb_resource() {}

    vector<string> rb_resource::get_before() const {
        return get_dep_param(_self, "before");
    }

    vector<string> rb_resource::get_require() const {
        return get_dep_param(_self, "require");
    }

    vector<string> rb_resource::get_notify() const {
        return get_dep_param(_self, "notify");
    }

    vector<string> rb_resource::get_subscribe() const {
        return get_dep_param(_self, "subscribe");
    }

    shared_ptr<parameter> rb_resource::get_parameter(string name) const {
        auto& ruby = api::instance();
        auto param_fun = ruby.rb_intern("parameter");

        auto param = ruby.rb_funcall(_self, param_fun, 1, ruby.utf8_value(name));
        if (param == ruby.nil_value()) {
            return nullptr;
        }

        auto property_klass = ruby.lookup({"Puppet", "Property"});
        if (ruby.is_a(param, property_klass)) {
            return value_wrapper::wrap<rb_property>(param, _registry);
        } else {
            return value_wrapper::wrap<rb_parameter>(param, _registry);
        }
    }

    void rb_resource::each_property(function<void(shared_ptr<property>)> cb) const {
        auto& ruby = api::instance();
        auto props_fun = ruby.rb_intern("properties");

        auto props = ruby.rb_funcall(_self, props_fun, 0);
        ruby.array_for_each(props, [&](VALUE v) {
            cb(value_wrapper::wrap<rb_property>(v, _registry));
            return true;
        });
    }

    shared_ptr<value> rb_resource::retrieve() const {
        auto& ruby = api::instance();
        auto retrieve_fun = ruby.rb_intern("retrieve");
        auto to_hash_fun = ruby.rb_intern("to_hash");

        auto current = ruby.rb_funcall(_self, retrieve_fun, 0);
        auto result = ruby.rb_funcall(current, to_hash_fun, 0);
        return value_wrapper::wrap<value_wrapper>(result, _registry);
    }

    bool rb_resource::is_present(value*) const {
        return true;
    }
}}
