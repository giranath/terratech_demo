#!/usr/bin/env bash

for path in "$@"; do
    python -mjson.tool "$path" > "$path.bak"
    rm "$path"
    mv "$path.bak" "$path"
done