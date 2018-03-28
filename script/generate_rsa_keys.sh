#!/usr/bin/env bash

KEY_NAME=key
KEY_LENGTH=2048

while getopts ":l:" option
do
    case $option in
        l)
            KEY_LENGTH=$OPTARG
        ;;
    esac
done
shift $((OPTIND-1))

if [ "$#" -gt 0 ]; then
    KEY_NAME="$1"
fi

openssl genrsa $KEY_LENGTH > "$KEY_NAME.pem"
openssl pkcs8 -in "$KEY_NAME.pem" -out "priv$KEY_NAME.p8" -topk8 -nocrypt -outform der
openssl rsa -in "$KEY_NAME.pem" -pubout -out "pub$KEY_NAME.der" -outform der