#! /bin/sh
die () {
    echo >&2 "$@"
    exit 1
}
#./update_database.sh ${1}

# back to build root
[ "$#" -ge 2 ] || die "2 argument required, $# provided, usage: ./flashTIRemote.sh kappaNode2530.bin root@192.168.1.1"
basefile=${1##*/}
#echo $basefile
scp ${1} ${2}:/tmp/ 
ssh ${2} flashTI.sh /tmp/$basefile



