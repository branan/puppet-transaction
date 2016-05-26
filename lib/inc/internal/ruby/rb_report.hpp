#pragma once

#include "value_wrapper.hpp"

#include <puppet-transaction/report.hpp>
#include <leatherman/ruby/api.hpp>

namespace puppet_transaction { namespace ruby {

    class rb_report : public value_wrapper, public report {
    public:
        rb_report(leatherman::ruby::VALUE self);
        ~rb_report();
    };
}}
