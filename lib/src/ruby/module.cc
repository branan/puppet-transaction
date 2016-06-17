#include "internal/ruby/module.hpp"
#include "internal/ruby/rb_report.hpp"
#include "internal/ruby/rb_catalog.hpp"
#include "internal/ruby/value_registry.hpp"
#include <puppet-transaction/transaction.hpp>
#include <puppet-transaction/export.h>

#include <leatherman/logging/logging.hpp>

#include <boost/nowide/iostream.hpp>
#include <boost/nowide/convert.hpp>

using namespace leatherman::logging;
using namespace leatherman::ruby;
using namespace std;

using namespace puppet_transaction;
using namespace puppet_transaction::ruby;

namespace puppet_transaction { namespace ruby {

    /**
     * Helper for maintaining context when initialized via a Ruby require.
     */
    struct require_context
    {
        /**
         * Constructs a new require context.
         */
        require_context()
        {
            _module.reset(new module);
            // Ruby doesn't have a proper way of notifying extensions that the VM is shutting down
            // The easiest way to get notified is to have a global data object that never gets collected
            // until the VM shuts down
            auto const& ruby = api::instance();
            _canary = ruby.rb_data_object_alloc(*ruby.rb_cObject, this, nullptr, cleanup);
            ruby.rb_gc_register_address(&_canary);
            ruby.register_data_object(_canary);
        }

        /**
         * Destructs the require context.
         */
         ~require_context()
         {
            _module.reset();

            // Remove the finalizer and let Ruby collect the object
            auto const& ruby = api::instance();
            ruby.rb_gc_unregister_address(&_canary);
            ruby.unregister_data_object(_canary);
        }

        /**
         * Creates the require context.
         */
        static void create()
        {
            // Reset first before allocating a new context
            reset();

            _instance.reset(new require_context());
        }

        /**
         * Resets the require context.
         */
        static void reset()
        {
           _instance.reset();
        }

    private:
        static void cleanup(void* ptr)
        {
            if (ptr == _instance.get()) {
                reset();
            }
        }

        unique_ptr<module> _module;
        VALUE _canary;

        static unique_ptr<require_context> _instance;
    };

    unique_ptr<require_context> require_context::_instance;

    struct ruby_transaction {
        ruby_transaction(shared_ptr<report> _report, shared_ptr<catalog> _catalog, unique_ptr<value_registry> registry)
             : _transaction(_report, _catalog), _registry(move(registry)) {}

        transaction _transaction;
        unique_ptr<value_registry> _registry;
    };
}}


extern "C" {
     void LIBPUPPET_TRANSACTION_EXPORT Init_libpuppet_transaction()
     {
         bool logging_init_failed = false;
         string logging_init_error_msg;
         try {
             // TODO: add some locale-handling magic like facter's logging setup
             setup_logging(boost::nowide::cerr, "", PROJECT_NAME);
             set_level(log_level::warning);
         } catch (exception const& e) {
             logging_init_failed = true;
             logging_init_error_msg = e.what();
         }

         api* ruby = nullptr;
         try {
             ruby = &api::instance();
         } catch (runtime_error& ex) {
             if (!logging_init_failed) {
                 LOG_WARNING("%1%: facts requiring ruby will not be resolved.", ex.what());
             }
             return;
         }

         ruby->initialize();

         if (logging_init_failed) {
            ruby->rb_raise(*ruby->rb_eLoadError, "could not initialize facter due to a locale error: %s", logging_init_error_msg.c_str());
         } else {
             puppet_transaction::ruby::require_context::create();
         }
     }
}

static void transaction_mark(void* value)
{
    ruby_transaction* the_transaction = reinterpret_cast<ruby_transaction*>(value);
    (*the_transaction)._registry->mark();
}

static void transaction_free(void* value)
{
    delete reinterpret_cast<transaction*>(value);
}

static VALUE transaction_new(VALUE klass, VALUE the_catalog, VALUE the_report, VALUE /*prioritizer*/)
{
    auto const& ruby = api::instance();

    if (the_report == ruby.nil_value()) {
        // we don't go through the catalog wrapper to avoid bouncing
        // back and forth between ruby and C++ objects.
        auto ver_fun = ruby.rb_intern("version");
        auto env_fun = ruby.rb_intern("environment");
        auto new_fun = ruby.rb_intern("new");

        auto version = ruby.rb_funcall(the_catalog, ver_fun, 0);
        auto environment = ruby.rb_funcall(the_catalog, env_fun, 0);
        auto apply = ruby.utf8_value("apply");

        auto report_klass = ruby.lookup({"Puppet", "Transaction", "Report"});
        the_report = ruby.rb_funcall(report_klass, new_fun, 3, apply, version, environment);
    }

    std::unique_ptr<value_registry> registry(new value_registry);
    auto catalog = value_wrapper::wrap<rb_catalog>(the_catalog, registry.get());
    auto report = value_wrapper::wrap<rb_report>(the_report, registry.get());
    ruby_transaction* value = new ruby_transaction(report, catalog, move(registry));
    return ruby.rb_data_object_alloc(klass, value, transaction_mark, transaction_free);
}

static VALUE transaction_get_report(VALUE self)
{
    auto& ruby = api::instance();
    auto the_transaction = ruby.to_native<ruby_transaction>(self);
    auto the_report = the_transaction->_transaction.get_report();

    return dynamic_cast<rb_report*>(the_report)->self();
}

static VALUE transaction_evaluate(VALUE self)
{
    auto& ruby = api::instance();
    if (ruby.rb_block_given_p()) {
        ruby.rb_raise(*ruby.rb_eArgError, "Transaction::evaluate no longer accepts a block");
    }
    ruby.to_native<ruby_transaction>(self)->_transaction.evaluate();
    return api::instance().nil_value();
}

static VALUE transaction_any_failed(VALUE self)
{
    return api::instance().false_value();
}

namespace puppet_transaction { namespace ruby {

    module::module()
    {
        auto const& ruby = api::instance();
        if (!ruby.initialized()) {
            throw runtime_error("Ruby API is not initialized.");
        }

        // Register the block for logging callback with the GC
        _on_message_block = ruby.nil_value();
        ruby.rb_gc_register_address(&_on_message_block);

        on_message([this](log_level level, string const& message) {
            auto const& ruby = api::instance();

            if (ruby.is_nil(_on_message_block)) {
                return true;
            }

            // Call the block and don't log messages
            ruby.rescue([&]() {
                ruby.rb_funcall(_on_message_block, ruby.rb_intern("call"), 2, level_to_symbol(level), ruby.utf8_value(message));
                return ruby.nil_value();
            }, [&](VALUE) {
                 // Logging can take place from locations where we do not expect Ruby exceptions to be raised
                 // Therefore, intentionally swallow any exceptions.
                return ruby.nil_value();
            });
            return false;
        });

        _self = ruby.rb_define_module("PuppetTransaction");
        auto klass = ruby.rb_define_class_under(_self, "Transaction", *ruby.rb_cObject);
        ruby.rb_define_singleton_method(klass, "new", RUBY_METHOD_FUNC(transaction_new), 3);
        ruby.rb_define_method(klass, "report", RUBY_METHOD_FUNC(transaction_get_report), 0);
        ruby.rb_define_method(klass, "evaluate", RUBY_METHOD_FUNC(transaction_evaluate), 0);
        ruby.rb_define_method(klass, "any_failed?", RUBY_METHOD_FUNC(transaction_any_failed), 0);
    }

    module::~module()
    {
        try {
            api& ruby = api::instance();

            ruby.rb_gc_unregister_address(&_on_message_block);
            on_message(nullptr);
            ruby.rb_const_remove(*ruby.rb_cObject, ruby.rb_intern("PuppetTransaction"));
        } catch (runtime_error& ex) {
            LOG_WARNING("%1%: Ruby cleanup ended prematurely", ex.what());
            return;
        }
    }

    VALUE module::level_to_symbol(log_level level)
    {
        auto const& ruby = api::instance();

        char const* name = nullptr;

        if (level == log_level::trace) {
            name = "trace";
        } else if (level == log_level::debug) {
            name = "debug";
        } else if (level == log_level::info) {
            name = "info";
        } else if (level == log_level::warning) {
            name = "warn";
        } else if (level == log_level::error) {
            name = "error";
        } else if (level == log_level::fatal) {
            name = "fatal";
        }
        if (!name) {
            ruby.rb_raise(*ruby.rb_eArgError, "invalid log level specified.", 0);
        }
        return ruby.to_symbol(name);
    }

}}
