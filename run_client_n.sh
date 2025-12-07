#!/bin/bash

# assign a a version based on arguments
if [ "$#" -eq 1 ]; then
    CLIENT_NUM="$1"
# default
else
    CLIENT_NUM=1
fi

PROPERTIES_PATH="src/client/properties.txt"
EXECUTABLE="client.exe"

RUN_GCC="gcc -Wall src/client/main.c src/client/sender_handler.c src/client/receiver_handler.c src/message.c src/network.c src/chat_node.c src/properties.c -o $EXECUTABLE -lpthread"

CLIENT_IP="127.0.0.1"
CLIENT_PORT=$((8080 + CLIENT_NUM))
LOGICAL_NAME="client_$CLIENT_NUM"

SERVER_IP="127.0.0.1"
SERVER_PORT=8080

# logic to write correct data to file
echo "MY_IP = $CLIENT_IP" > "$PROPERTIES_PATH"      # write over previous client
echo "MY_PORT = $CLIENT_PORT" >> "$PROPERTIES_PATH" # append
echo "LOGICAL_NAME = $LOGICAL_NAME" >> "$PROPERTIES_PATH"

echo "SERVER_IP = $SERVER_IP" >> "$PROPERTIES_PATH"
echo "SERVER_PORT = $SERVER_PORT" >> "$PROPERTIES_PATH"

# check for executable already exists
if [ -f "$EXECUTABLE" ]; then
    echo "Found executable $EXECUTABLE"
# compile executable
elif $RUN_GCC; then
    echo "Compiled to $EXECUTABLE"
else
    echo "Compilation error"
    exit 1
fi

# display properties and allow user edits
# vim "$PROPERTIES_PATH"

echo "Program Start"

./"$EXECUTABLE"

# clean
rm "$EXECUTABLE"

echo "" > "$PROPERTIES_PATH"
