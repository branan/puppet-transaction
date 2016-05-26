#pragma once

#include <memory>

#include <puppet-transaction/report.hpp>
#include <puppet-transaction/catalog.hpp>

namespace puppet_transaction {

    class transaction
    {
    public:
        transaction(std::unique_ptr<report>, std::unique_ptr<catalog>);

        report* get_report() const { return _report.get(); }
        catalog* get_catalog() const { return _catalog.get(); }

    private:
        std::unique_ptr<report> _report;
        std::unique_ptr<catalog> _catalog;
    };
}
