#include "internal/ruby/rb_report.hpp"

using namespace leatherman::ruby;

namespace puppet_transaction { namespace ruby {

    rb_report::rb_report(VALUE self)
         : value_wrapper(self) {}

    rb_report::~rb_report() {}
}}
