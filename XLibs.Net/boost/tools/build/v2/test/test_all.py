#!/usr/bin/python
import os, sys, string

# clear environment for testing
#
for s in (
    'BOOST_ROOT','BOOST_BUILD_PATH','JAM_TOOLSET','BCCROOT',
    'MSVCDir','MSVC','MSVCNT','MINGW','watcom'
    ):
    
    try:
        del os.environ[s]
    except:
        pass

def run_tests(critical_tests, other_tests):
    """Runs first critical tests and then other_tests.

       Stops on first error, and write the name of failed test to
       test_results.txt. Critical tests are run in the specified order,
       other tests are run starting with the one that failed the last time.
    """
    last_failed = last_failed_test()
    other_tests = reorder_tests(other_tests, last_failed)
    all_tests = critical_tests + other_tests

    invocation_dir = os.getcwd()

    failures_count = 0
    for i in all_tests:
        print ("%-25s : " %(i)),
        try:
            __import__(i)
        except:
            print "FAILED"
            if failures_count == 0:
                f = open(os.path.join(invocation_dir, 'test_results.txt'), 'w')
                f.write(i)
                f.close()
            failures_count = failures_count + 1
            # Restore the current directory, which might be changed by the
            # test
            os.chdir(invocation_dir)
            continue
        print "PASSED"
        sys.stdout.flush()  # makes testing under emacs more entertaining.
        
    # Erase the file on success
    if failures_count == 0:
        open('test_results.txt', 'w')
        

def last_failed_test():
    "Returns the name of last failed test or None"
    try:
        f = open("test_results.txt")
        s = string.strip(f.read())
        return s
    except:
        return None

def reorder_tests(tests, first_test):
    try:
        n = tests.index(first_test)
        return [first_test] + tests[:n] + tests[n+1:]
    except ValueError:
        return tests

            
critical_tests = ["unit_tests", "module_actions", "startup_v1", "startup_v2"]

critical_tests += ["core_d12", "core_typecheck", "core_delete_module",
                   "core_varnames", "core_import_module"]

tests = [ "project_test1",
          "project_test3",
          "project_test4",
          "generators_test",
          "dependency_test",
          "path_features",
          "relative_sources",
          "no_type",
          "chain",
          "default_build",
          "use_requirements",
          "conditionals",
          "stage",
          "prebuilt",
          "project_dependencies",
          "build_dir",
          "searched_lib",
          "make_rule",
          "alias",
          "alternatives",
          "unused",
          "default_features",
          "print",
          "ndebug",
          "explicit",
          "absolute_sources",
          "dependency_property",
          "custom_generator",
          "bad_dirname",
          "c_file",
          "inline",
          "conditionals2",
          "property_expansion",
          "loop",
          "conditionals3",
          "tag",
          "suffix",
          "inherit_toolset",
          "skipping",
          "project_root",
          "glob",
          "project_root_constants",
          "double_loading",
          "dll_path",
          "regression",
          "composite",
          "library_chain",
          "unit_test",
          "standalone",
          "library_order",
          "expansion",
          "wrapper",
          #"ordered_properties",
          ]

if os.name == 'posix':
    tests.append("symlink")

if os.environ.has_key('QTDIR'):
    tests.append("railsys")
else:
    print 'skipping railsys test since QTDIR environment variable is unset'

run_tests(critical_tests, tests)
