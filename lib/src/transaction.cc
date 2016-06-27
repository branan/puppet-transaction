#include <puppet-transaction/catalog.hpp>
#include <puppet-transaction/transaction.hpp>
#include <puppet-transaction/parameter.hpp>
#include <puppet-transaction/property.hpp>
#include <puppet-transaction/report.hpp>
#include <puppet-transaction/resource.hpp>
#include <puppet-transaction/value.hpp>

#include <leatherman/logging/logging.hpp>

using namespace std;
using namespace puppet_transaction;

namespace puppet_transaction {

    transaction::transaction(shared_ptr<report> r, shared_ptr<catalog> c)
         : _report(move(r)), _catalog(move(c)) {}

    void transaction::evaluate()
    {
        // TODO generate resources
        // TODO perform prerun checks

        if (!_catalog) {
            LOG_ERROR("Catalog is null");
        } else {
            if (_catalog->has_version()) {
                LOG_INFO("Applying configuration version '%1%'", _catalog->version());
            }
        }

        // TODO: expand containers into whits

        _catalog->each_key([&](const std::string& key) {
            _resources.push_back(key);
            expand_relationships(key);
        });

        // TODO: add autorelations

        std::list<std::string> ready;
        for (const auto& res : _resources) {
            if (_blockers[res].empty()) {
                ready.push_front(res);
            }
        }

        while (!_resources.empty()) {
            if(ready.empty()) {
                // TODO: handle providerless types etc.
                LOG_ERROR("Could not evaluate - no unblocked resources");
                return;
            }

            auto resource = ready.front();
            ready.pop_front();
            _resources.remove(resource);
            evaluate_resource(_catalog->get_resource(resource).get());
            for(const auto& blockee : _blockees[resource]) {
                _blockers[blockee].remove(resource);
                if (_blockers[blockee].empty()) {
                    ready.push_front(blockee);
                }
            }
            _blockers.erase(resource);
        }
    }

    void transaction::expand_relationships(const string& key) 
    {
        auto resource = _catalog->get_resource(key);
        auto before = resource->get_before();
        auto require = resource->get_require();
        auto notify = resource->get_notify();
        auto subscribe = resource->get_subscribe();

        // initialize empty maps for this resource, so every resource is
        // in the _blockers/_blockees lists
        _blockers[key] = {};
        _blockees[key] = {};

        for (const auto& k : before) {
            _blockees[key].push_back(k);
            _blockers[k].push_back(key);
        }

        for (const auto& k : notify) {
            _blockees[key].push_back(k);
            _blockers[k].push_back(key);
        }

        for (const auto& k : require) {
            _blockees[k].push_back(key);
            _blockers[key].push_back(k);
        }

        for (const auto& k : subscribe) {
            _blockees[k].push_back(key);
            _blockers[key].push_back(k);
        }
    }

    void transaction::evaluate_resource(resource* res)
    {
        auto current = res->retrieve();
        auto ensure_param = dynamic_pointer_cast<property>(res->get_parameter("ensure"));
        if (ensure_param && ensure_param->has_should() && !ensure_param->is_in_sync(current.get())) {
            ensure_param->sync();
        } else {
            if (res->is_present(current.get())) {
                res->each_property([&](shared_ptr<property> prop) {
                    if (prop->has_should() && !prop->is_in_sync(current.get())) {
                        prop->sync();
                    }
                });
            }
        }
    }
}
