#pragma once

namespace puppet_transaction {

    class parameter {
    public:
        virtual ~parameter();

        virtual bool has_should() const = 0;
    };
}
