import argparse
import sys
import os
import shutil

def get_options(args=sys.argv[1:]):
    parser = argparse.ArgumentParser(description="configure datetime test.")
    parser.add_argument("--boost_lib_dir", help="path to custom boost lib directory.")
    parser.add_argument("--build", action='store_true', help="configure and build.")
    parser.add_argument("--rebuild", action='store_true', help="clean, configure and build.")
    options = parser.parse_args(args)
    return options

def create_cmd(options):
    boost_lib_dir=""
    if(options.boost_lib_dir != None):
        boost_lib_dir=" -DBOOST_LIBRARY_DIRS=" + options.boost_lib_dir
    return 'cmake ' + '\"-G Unix Makefiles\"' + " -DCMAKE_BUILD_TYPE=Debug" +  boost_lib_dir + " -DCMAKE_EXPORT_COMPILE_COMMANDS=1" + " -DCMAKE_CXX_STANDARD=17 ../"

def configure(options):
    os.system(create_cmd(options))

def make():
    os.system("make -j 10")

def test():
    os.system("./tests")

def main():
    options = get_options();
    build_dir = os.path.dirname(os.path.realpath(__file__)) + os.sep + 'build'
    if os.path.exists(build_dir) :
        if options.rebuild:
            shutil.rmtree(build_dir)
            os.makedirs(build_dir)
            os.chdir(build_dir)
            configure(options)
            make()
            test()
        else:
            os.chdir(build_dir)
            make()
            test()
    else:
        os.makedirs(build_dir)
        os.chdir(build_dir)
        configure(options)
        make()
        test()

if __name__=="__main__":
    main()
