#include "internal/ruby/value_registry.hpp"

using namespace leatherman::ruby;
using namespace std;

namespace puppet_transaction { namespace ruby {
    void value_registry::add(VALUE value, weak_ptr<value_wrapper> wrapper) {
        auto res = _values.emplace(std::make_pair(value, wrapper));
        if (!res.second) {
            if (_values[value].expired()) {
                _values[value] = wrapper;
            } else {
                throw std::logic_error("Cannot have more than one C++ wrapper for a single ruby VALUE");
            }
        }
    }

    void value_registry::del(VALUE value) {
        _values.erase(value);
    }

    shared_ptr<value_wrapper> value_registry::get(VALUE value) const {
        auto it = _values.find(value);
        if (it != _values.end()) {
            auto ptr = it->second;
            if (ptr.expired()) {
                return nullptr;
            } else {
                return ptr.lock();
            }
        } else {
            return nullptr;
        }
    }

    void value_registry::mark() const {
        auto& ruby = api::instance();
        for (const auto& pair : _values) {
            ruby.rb_gc_mark(pair.first);
        }
    }
}}
