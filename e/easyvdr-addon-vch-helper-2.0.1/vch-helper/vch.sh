#!/bin/bash

cd `dirname $0`

java -Djava.net.preferIPv4Stack=true \
     -Djava.util.logging.config.file=conf/logging.properties \
     -cp bundles/logger.jar:bin/org.apache.felix.main-4.6.0.jar \
     org.apache.felix.main.Main
     
#java -Xdebug -Xnoagent -Xrunjdwp:transport=dt_socket,server=y,suspend=n,address=8000 \
#     -Djava.net.preferIPv4Stack=true \
#     -Djava.util.logging.config.file=conf/logging.properties \
#     -cp bundles/logger.jar:bin/org.apache.felix.main-4.6.0.jar \
#     org.apache.felix.main.Main
