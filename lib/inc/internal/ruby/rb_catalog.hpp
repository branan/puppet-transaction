#pragma once

#include "value_wrapper.hpp"

#include <puppet-transaction/catalog.hpp>

namespace puppet_transaction { namespace ruby {

    class rb_catalog : public catalog, public value_wrapper {
    public:
        rb_catalog(leatherman::ruby::VALUE self, value_registry*);
        ~rb_catalog();

        bool has_version() const override;
        std::string version() const override;
        void each_key(std::function<void(const std::string&)> cb) const override;
        std::shared_ptr<resource> get_resource(const std::string&) const override;
    };
}}
