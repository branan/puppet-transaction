#pragma once

#include "value_wrapper.hpp"

#include <puppet-transaction/report.hpp>
#include <leatherman/ruby/api.hpp>

namespace puppet_transaction { namespace ruby {

    class rb_report : public report, public value_wrapper {
    public:
        rb_report(leatherman::ruby::VALUE self, value_registry*);
        ~rb_report();
    };
}}
