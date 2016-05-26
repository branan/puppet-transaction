/**
 * @file
 * Declares the Ruby puppet_transaction module
 */
#pragma once

#include <leatherman/logging/logging.hpp>
#include <leatherman/ruby/api.hpp>

namespace puppet_transaction { namespace ruby {

    /**
     * Represents the Ruby module
     */
    struct module
    {
        module();
        ~module();

        static module* current();

   private:
        leatherman::ruby::VALUE _on_message_block;
        leatherman::ruby::VALUE _self;

        leatherman::ruby::VALUE level_to_symbol(leatherman::logging::log_level);
    };

}}
