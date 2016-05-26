#include <puppet-transaction/transaction.hpp>

using namespace std;

namespace puppet_transaction {

    transaction::transaction(unique_ptr<report> r, unique_ptr<catalog> c)
         : _report(move(r)), _catalog(move(c)) {}
}
