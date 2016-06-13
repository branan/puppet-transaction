#pragma once

#include <functional>
#include <string>
#include <memory>

namespace puppet_transaction {

    class resource;

    class catalog {
    public:
        virtual ~catalog();

        virtual bool has_version() const = 0;
        virtual std::string version() const = 0;
        virtual void each_key(std::function<void(const std::string&)>) const = 0;
        virtual std::unique_ptr<resource> get_resource(const std::string&) const = 0;
    };

}
