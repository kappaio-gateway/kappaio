#! /bin/sh
die () {
    echo >&2 "$@"
    exit 1
}
#./update_database.sh ${1}

# back to build root
#[ "$#" -eq 2 ] || die "\ncompile the program without installing it on the console \n2 argument required, $# provided, usage: ./rsintall root@192.168.1.24 ramips\n"
export processor_family=$(echo ${2} | tr '[:lower:]' '[:upper:]')
cd ../../../ 
make package/rsserial/clean V=s 
make package/rsserial/compile V=s 


