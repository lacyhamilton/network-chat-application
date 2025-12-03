#!/bin/bash

PROPERTIES_PATH="src/server/properties.txt"
EXECUTABLE="server.exe"
RUN_GCC="gcc -Wall src/server/main.c src/server/client_handler.c src/server/client_list.c src/message.c src/chat_node.c src/properties.c -o $EXECUTABLE -lpthread" 

if [ -f "$PROPERTIES_PATH" ]; then
    if eval "$RUN_GCC"; then
        echo "Server logic compiled to $EXECUTABLE"
    else
        echo "Compile error"
        exit 1
    fi

    # open editor for properties file
    vim "$PROPERTIES_PATH"
    
    echo "Running server with properties at $PROPERTIES_PATH"

    # run executable
    ./"$EXECUTABLE"
else
    echo "No properties path resolved"
fi
