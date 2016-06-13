#include "internal/ruby/rb_resource.hpp"

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
    rb_resource::rb_resource(VALUE self)
         : value_wrapper(self) {}

    rb_resource::~rb_resource() {}

    vector<string> rb_resource::get_before() const {
        return get_dep_param(self(), "before");
    }

    vector<string> rb_resource::get_require() const {
        return get_dep_param(self(), "require");
    }

    vector<string> rb_resource::get_notify() const {
        return get_dep_param(self(), "notify");
    }

    vector<string> rb_resource::get_subscribe() const {
        return get_dep_param(self(), "subscribe");
    }

    unique_ptr<parameter> rb_resource::get_parameter(string) const {
        return nullptr;
    }

    void rb_resource::each_property(function<void(unique_ptr<parameter>)>) const {
    }

    unique_ptr<value> rb_resource::retrieve() const {
        return nullptr;
    }

    bool rb_resource::is_present(value*) const {
        return false;
    }
}}
