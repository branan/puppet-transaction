include(leatherman)
get_commit_string(PROJECT_COMMIT)

# Generate a file containing the above version numbers.
configure_file("version.h.in" "${CMAKE_CURRENT_LIST_DIR}/inc/${PROJECT_NAME}/version.h")

# Generate files for Doxygen
configure_file("Doxyfile.in"       "${CMAKE_CURRENT_LIST_DIR}/Doxyfile")
configure_file("namespaces.dox.in" "${CMAKE_CURRENT_LIST_DIR}/docs/namespaces.dox")

# Set C++ compile flags based on project settings.
set(CMAKE_CXX_FLAGS ${${PROJECT_NAME_UPPER}_CXX_FLAGS})

# Set prefix for logging messages.
leatherman_logging_namespace("puppetlabs.puppet-transaction")

# Setup compiling the library.
include_directories(inc ${Boost_INCLUDE_DIRS} ${LEATHERMAN_INCLUDE_DIRS})

set(PROJECT_SOURCES
    "src/transaction.cc"
    "src/catalog.cc"
    "src/parameter.cc"
    "src/property.cc"
    "src/report.cc"
    "src/resource.cc"
    "src/value.cc"
    "src/ruby/value_registry.cc"
    "src/ruby/value_wrapper.cc"
    "src/ruby/rb_catalog.cc"
    "src/ruby/rb_parameter.cc"
    "src/ruby/rb_property.cc"
    "src/ruby/rb_report.cc"
    "src/ruby/rb_resource.cc"
    "src/ruby/module.cc")

## An object target is generated that can be used by both the library and test executable targets.
## Without the intermediate target, unexported symbols can't be tested.
add_library(libpuppet-transactionsrc OBJECT ${PROJECT_SOURCES})
set_target_properties(libpuppet-transactionsrc PROPERTIES POSITION_INDEPENDENT_CODE true)

add_library(libpuppet_transaction SHARED $<TARGET_OBJECTS:libpuppet-transactionsrc>)
set_target_properties(libpuppet-transactionsrc PROPERTIES POSITION_INDEPENDENT_CODE true)
set_target_properties(libpuppet_transaction PROPERTIES VERSION "${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}" PREFIX "" SUFFIX ".so" IMPORT_PREFIX "" IMPORT_SUFFIX ".so.a")

target_link_libraries(libpuppet_transaction
    ${LEATHERMAN_LIBRARIES}
    ${Boost_LIBRARIES}
)

symbol_exports(libpuppet_transaction "${CMAKE_CURRENT_LIST_DIR}/inc/${PROJECT_NAME}/export.h")

leatherman_install(libpuppet_transaction)

if (WIN32)
    set(LIBTRANSACTION_INSTALL_DESTINATION bin)
else()
    # TODO: lib64 for certain operating systems?
    set(LIBTRANSACTION_INSTALL_DESTINATION lib)
endif()


set(RUBY_LIB_INSTALL "" CACHE PATH "Specify the location to install facter.rb")
if(RUBY_LIB_INSTALL)
    set(RUBY_VENDORDIR ${RUBY_LIB_INSTALL}) 
else()
    # Disable actually installing facter.rb if Ruby is not present and RUBY_LIB_INSTALL not specified.
    if (RUBY_FOUND)
        message(STATUS "Ruby ${RUBY_VERSION} found.")

        execute_process(COMMAND ${RUBY_EXECUTABLE} -rrbconfig -e "puts RbConfig::CONFIG['vendordir']" OUTPUT_VARIABLE RUBY_VENDORDIR_OUT)
        string(STRIP ${RUBY_VENDORDIR_OUT} RUBY_VENDORDIR)
    endif()
endif()

if(RUBY_VENDORDIR)
    file(RELATIVE_PATH LIBTRANSACTION_INSTALL_RELATIVE ${RUBY_VENDORDIR} ${CMAKE_INSTALL_PREFIX})
    configure_file (
        "puppet-transaction.rb.in"
        "${CMAKE_BINARY_DIR}/lib/puppet-transaction.rb"
    )

    message(STATUS "\"make install\" will install puppet-transaction.rb to ${RUBY_VENDORDIR}")
    install(FILES ${CMAKE_BINARY_DIR}/lib/puppet-transaction.rb DESTINATION ${RUBY_VENDORDIR})
endif()


add_subdirectory(tests)
