#!/bin/bash

INSTALL_DIR=../../../

export LD_LIBRARY_PATH=${LD_LIBRARY_PATH}:${INSTALL_DIR}/tools/server/php_linux/libxml2/lib/

${INSTALL_DIR}/tools/server/php_linux/bin/php-bin -c ./php.ini -f ./test.php
echo $?
