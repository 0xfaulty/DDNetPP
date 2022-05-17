#!/bin/bash

function test_tournament() {
	log "test tournaments"

	sleep 1
	echo "rcon start_block_tournament" > client1.fifo

	sleep 1
	echo "say /join" > client1.fifo

	sleep 1
	echo "say /join" > client2.fifo

	# 5 seconds of delay until round starts
	sleep 5
	echo "say /buy shit" > client1.fifo

	# 10 seconds until selfkill is allowed
	sleep 11
	echo "kill" > client1.fifo
}
