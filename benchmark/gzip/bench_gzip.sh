#!/bin/sh

start=`cat /proc/stat | grep softir | cut -f2 -d" "`
stime=`./gettime`


./gzip 100MB.dat
./gzip -d 100MB.dat.gz


end=`cat /proc/stat | grep softirq | cut -f2 -d" "`
etime=`./gettime`

starr=(${stime// / })
enarr=(${etime// / })
irqs=$((end-start))


stsec=${starr[0]}
stusec=${starr[1]}
ensec=${enarr[0]}
enusec=${enarr[1]}

#echo $stsec
#echo $stusec
#echo $ensec
#echo $enusec

esec=$((ensec-stsec))
eusec=$((enusec-stusec))
if [ "$eusec" -lt 0 ]
    then
    esec=$((esec-1))
    eusec=$((1000000+eusec))
fi

eusec=`printf "%06d" $eusec`
echo $irqs
echo "${esec}.${eusec}"
#echo $eusec





