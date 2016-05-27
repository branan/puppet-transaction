# Puppet-Transaction

Puppet-Transaction is a C++ implementation of the Puppet `Transaction`
class, responsible for walking the catalog graph and interfacing with
Providers to resolve system state.

## Required packages

You will need to install [Boost](http://boost.org) for program_options
and to use many of the libraries in
[Leatherman](https://github.com/puppetlabs/leatherman).

In order to use this you will need the
[`native_trans` branch of Puppet](https://github.com/branan/puppet/tree/native_trans)

## Build the library

This template is a fully functional example, and can be built with

```
mkdir build && cd build
cmake ..
make -j
```

CMake will generate source files from template files based on the
project declared in CMakeLists.txt. For cpp-project-template, git
ignores the generated files. For your own project, you can check those
files in, and eventually remove the template files and generation
logic from CMake as you develop your project (look for the REMOVE
string in CMakeLists.txt files).

Tests can be run with `make test`, and the example tool can be run
with `bin/cpp_project_template`.


## Using the native transaction layer

To use the native transaction, you will need to install it, then run
Puppet with the appropriate flags

```
make install
cd /path/to/puppet
bundle exec puppet apply --transaction=native -e 'notify { foo: }'
```

## Maintenance

Maintainers: Branan Riley <b@puppet.com>

Tickets: Can be opened in Github Issues.
