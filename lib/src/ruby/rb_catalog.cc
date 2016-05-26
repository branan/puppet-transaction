#include "internal/ruby/rb_catalog.hpp"

using namespace leatherman::ruby;

namespace puppet_transaction { namespace ruby {

    rb_catalog::rb_catalog(VALUE self)
         : value_wrapper(self) {}

    rb_catalog::~rb_catalog() {}
}}
