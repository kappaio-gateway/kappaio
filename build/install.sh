#! /bin/sh
die () {
    echo >&2 "$@"
    exit 1
}
#./update_database.sh ${1}

# back to build root
[ "$#" -ge 2 ] || die "2 argument required, $# provided, usage: ./rsintall root@192.168.1.24 ramips"
export processor_family=$(echo ${2} | tr '[:lower:]' '[:upper:]')
cd ../../../ 
make package/rsserial/clean V=s 
make package/rsserial/compile V=s 
scp bin/${2}/packages/rsserial*.ipk ${1}:/tmp 
ssh ${1} opkg install --force-overwrite /tmp/rsserial*.ipk 
ssh ${1} /etc/init.d/rsserial-watch restart


