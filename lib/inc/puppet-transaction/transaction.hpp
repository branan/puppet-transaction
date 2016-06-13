#pragma once

#include <list>
#include <map>
#include <memory>
#include <string>

namespace puppet_transaction {
    class catalog;
    class parameter;
    class report;
    class resource;
    class value;

    class transaction
    {
    public:
        transaction(std::unique_ptr<report>, std::unique_ptr<catalog>);

        report* get_report() const { return _report.get(); }
        catalog* get_catalog() const { return _catalog.get(); }

        void evaluate();

    private:
        void expand_relationships(const std::string& key);
        void evaluate_resource(resource*);
        void sync_property(parameter*, value*);

        std::unique_ptr<report> _report;
        std::unique_ptr<catalog> _catalog;

        std::list<std::string> _resources;
        std::map<std::string, std::list<std::string>> _blockers;
        std::map<std::string, std::list<std::string>> _blockees;
    };
}
