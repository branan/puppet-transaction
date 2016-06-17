#pragma once

#include "value_wrapper.hpp"

#include <puppet-transaction/resource.hpp>

namespace puppet_transaction { namespace ruby {

    class rb_resource : public resource, public value_wrapper {
    public:
        rb_resource(leatherman::ruby::VALUE self, value_registry*);
        ~rb_resource();

        std::vector<std::string> get_before() const override;
        std::vector<std::string> get_require() const override;
        std::vector<std::string> get_notify() const override;
        std::vector<std::string> get_subscribe() const override;

        std::shared_ptr<parameter> get_parameter(std::string) const override;
        void each_property(std::function<void(std::shared_ptr<parameter>)>) const override;

        std::shared_ptr<value> retrieve() const override;
        virtual bool is_present(value*) const override;
    };
}}
