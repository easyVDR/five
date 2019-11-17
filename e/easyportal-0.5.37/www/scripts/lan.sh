range="192.168.1."; 
port=2004; 
for host in $(seq 1 255); 
do multi_task=$(result=$(nc -zv $range$host $port 2>&1 | grep succeeded); 
if [ -n "$result" ]; then echo $range$host":"$port >> "/tmp/pscan"; fi;) & done
cat /tmp/pscan