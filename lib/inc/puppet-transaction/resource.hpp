#pragma once

#include <functional>
#include <memory>
#include <string>
#include <vector>

namespace puppet_transaction {

    class parameter;
    class property;
    class value;

    class resource {
    public:
        virtual ~resource();

        virtual std::vector<std::string> get_before() const = 0;
        virtual std::vector<std::string> get_require() const = 0;
        virtual std::vector<std::string> get_notify() const = 0;
        virtual std::vector<std::string> get_subscribe() const = 0;

        virtual std::shared_ptr<parameter> get_parameter(std::string) const = 0;
        virtual void each_property(std::function<void(std::shared_ptr<property>)>) const = 0;

        virtual std::shared_ptr<value> retrieve() const = 0;
        virtual bool is_present(value*) const = 0;
    };
}
