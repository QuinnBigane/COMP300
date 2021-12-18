# Hi students! 
upload: hardware.bin 
	iceprog hardware.bin

hardware.bin: hardware.asc
#	icetime -d up5k -c 12 -mtr hardware.rpt hardware.asc
	icepack hardware.asc hardware.bin

hardware.asc: upduino3.pcf hardware.json
	nextpnr-ice40 -q --freq 13 --timing-allow-fail --up5k --package sg48 \
	              --asc hardware.asc --pcf upduino3.pcf --json hardware.json

hardware.json: top.v 
	yosys -ql hardware.log -p 'synth_ice40 -json hardware.json' $^

clean:
	rm -f hardware.json hardware.log hardware.asc hardware.rpt hardware.bin

