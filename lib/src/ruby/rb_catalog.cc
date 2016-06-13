#include "internal/ruby/rb_catalog.hpp"
#include "internal/ruby/rb_resource.hpp"

using namespace leatherman::ruby;
using namespace std;

namespace puppet_transaction { namespace ruby {

    rb_catalog::rb_catalog(VALUE self)
         : value_wrapper(self) {}

    rb_catalog::~rb_catalog() {}

    bool rb_catalog::has_version() const
    {
        auto& ruby = api::instance();
        auto ver_fun = ruby.rb_intern("version");
        auto version = ruby.rb_funcall(self(), ver_fun, 0);
        return ! ruby.is_nil(version);
    }

    string rb_catalog::version() const
    {
        auto& ruby = api::instance();
        auto ver_fun = ruby.rb_intern("version");
        auto version = ruby.rb_funcall(self(), ver_fun, 0);
        return ruby.to_string(version);
    }

    void rb_catalog::each_key(std::function<void(const string&)> cb) const {
        auto& ruby = api::instance();
        auto ref_fun = ruby.rb_intern("ref");
        auto resource_table = ruby.rb_ivar_get(self(), ruby.rb_intern("@resource_table"));
        auto resources = ruby.rb_ivar_get(self(), ruby.rb_intern("@resources"));
        ruby.array_for_each(resources, [&](VALUE key) {
            auto resource = ruby.rb_hash_lookup(resource_table, key);
            auto v = ruby.rb_funcall(resource, ref_fun, 0);
            cb(ruby.to_string(v));
            return true;
        });
    }

    unique_ptr<resource> rb_catalog::get_resource(const string& key) const {
        auto& ruby = api::instance();
        auto resource_ref = ruby.utf8_value(key);
        auto res_fun = ruby.rb_intern("resource");
        auto the_resource = ruby.rb_funcall(self(), res_fun, 1, resource_ref);
        if (the_resource == ruby.nil_value()) {
            return {};
        } else  {
             return unique_ptr<resource>(new rb_resource(the_resource));
        }
    }
}}
