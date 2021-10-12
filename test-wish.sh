#! /bin/bash

if ! [[ -x wish ]]; then
    echo "wish executable does not exist"
    exit 1
fi

cp -rf /p/course/cs537-remzi/tests/p2a/tests .
/p/course/cs537-remzi/tests/tester/run-tests.sh $*

